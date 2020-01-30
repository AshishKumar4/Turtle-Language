#include "ast/ast.hpp"
#include "library/common.hpp"
#include "tokens.hpp"

#include "parser/parser.hpp"
#include "parser/literal.hpp"
#include "parser/bracket.hpp"
#include "parser/operator.hpp"
#include "parser/quotestr.hpp"

#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <exception>
#include <list>

namespace turtle
{

// typedef std::function<TokenDigesterReturn_t(std::list<TokenTree*>::iterator)> tokenTreeDigester_t;

// std::map<TokenTreeType, tokenTreeDigester_t> TOKENTREE_DIGESTERS;

void init_tokenTreeDigester()
{
    // TOKENTREE_DIGESTERS[TokenTreeType::DEFINE] =
}

auto genAST_Stage1(std::vector<TokenTree *> nodes, variableContext_t &context)
{
    std::vector<TokenTree *> treelist;
    TokenTree **tlist = &nodes[0];

    // std::list<TokenTree*> nodesList(nodes.begin(), nodes.end());
    for (int i = 0; i < nodes.size();)
    {
        switch (tlist[i]->getType())
        {
        case TokenTreeType::TEMP_LITERAL_WRAPPER:
        {
            auto tmp = (TempLiteralWrapperNode *)tlist[i];
            tlist[i] = tmp->getOriginal();
            auto [elem, poped] = tmp->getRule()->tokenTreeBuilder(tlist, i, nodes.size(), context);
            i += poped;
            treelist.push_back(elem);
            break;
        }
        case TokenTreeType::TEMP_LITERAL:
        {
            auto tmp = (TempLiteralTreeNode *)tlist[i];
            auto [elem, poped] = tmp->getRule()->tokenTreeBuilder(tlist, i, nodes.size(), context);
            i += poped;
            treelist.push_back(elem);
            break;
        }
        default:
            treelist.push_back(tlist[i]);
            ++i;
        }
    }

    return treelist;
}

// auto genAST_stage2(std::vector<TokenTree *> nodes, variableContext_t &context)
// {
//     std::vector<TokenTree *> treelist;
//     TokenTree **tlist = &nodes[0];
//     for (int i = 0; i < nodes.size();)
//     {
//         switch (tlist[i]->getType())
//         {
//         case TokenTreeType::FUNCTIONAL:
//         case TokenTreeType::INBUILT_FUNCTION:
//         {
//             // If there is a tuple next to it,
//         }
//         default:
//             treelist.push_back(tlist[i]);
//             ++i;
//         }
//     }

//     return treelist;
// }

TokenTree *genAST(std::vector<TokenTree *> nodes, variableContext_t &context)
{
    //Stich together the tokentree nodes to generate an Abstract Syntax Tree with one single root node
    // This is done in several passes.
    // First Pass includes Pairing together TempLiteralNodes and determining CurlyBracket Types (Dicts/Codeblocks) and lots of other works
    auto stage1Nodes = genAST_Stage1(nodes, context);

    // return stage1Nodes;
    return nullptr;
}

inline auto transferOpToOutQueue(std::vector<OperatorTreeNode *> &opstack, std::vector<TokenTree *> &outQueue)
{
    // Pop elements while There are elements of higher precedence in the stack
    // or equal precedence but the operator is left associative
    auto op = (BinaryOperatorTreeNode *)opstack.back();
    opstack.pop_back();

    // generate Tree out of these operators
    // Pop two elements out of outQueue
    auto elm1 = outQueue.back();
    outQueue.pop_back();
    auto elm2 = outQueue.back();
    outQueue.pop_back();

    // Make them as the two nodes of the Operator
    op->setRight(elm1);
    op->setLeft(elm2);

    auto val = op->execute();

    // Now push this back to the outQueue
    outQueue.push_back(val);
}

TokenTree *simpleASTmaker(std::vector<TokenTree *> nodes, variableContext_t &context)
{
    printf("[In SimpleAstMaker]");
    // Its job is to solve for simple expressions
    // And if its purely arithmatic on constants, Even solve it completely
    // and return a single node at the end, Maybe a constant, maybe a root to an
    // AST. Panic at complex things like TempLiterals etc.
    if (nodes.size() == 0)
    {
        errorHandler(SyntacticError("Some element needs to be provided into the tupple"));
        return nullptr;
    }

    auto stage1Nodes = genAST_Stage1(nodes, context); // For Only Functions and Lists
    // std::cout<<stage1Nodes.size();
    // fflush(stdout);
    // for (auto i : stage1Nodes)
    // {
    //     std::cout << "_" ;
    //     fflush(stdout);
    //     std::cout<< i->getName();
    //     fflush(stdout);
    // }
    // std::cout<<std::endl;
    // fflush(stdout);
    TokenTree *rootNode = stage1Nodes[0];
    TokenTree *currNode = rootNode;

    std::vector<OperatorTreeNode *> opstack;
    std::vector<TokenTree *> outQueue;

    try
    {
        for (auto i : stage1Nodes)
        {
            std::cout << "\t" << i->getName();
            fflush(stdout);
            switch (i->getType())
            {
            case TokenTreeType::OPERATOR:
            {
                OperatorTreeNode *node = (OperatorTreeNode *)i;
                if (node->getOptype() & (uint)OperatorType::BINARY)
                {
                    while (opstack.size() and
                           (opstack.back()->getPrecedence() < node->getPrecedence() or
                            (opstack.back()->getPrecedence() == node->getPrecedence() and node->getAssociativity() == OperatorAssociativity::LEFT)))
                    {
                        transferOpToOutQueue(opstack, outQueue);
                    }
                    opstack.push_back(node);
                }
                else if (node->getOptype() & (uint)OperatorType::UNARY)
                {
                    errorHandler(NotImplementedError("Unary Operator"));
                }
                else
                {
                    errorHandler(NotImplementedError("Ternary Operator"));
                }
                break;
            }
            case TokenTreeType::TUPLE:
            {
                if (((TupleTreeNode *)i)->getSize() == 1)
                {
                    outQueue.push_back(((TupleTreeNode *)i)->get()[0]); // replace solvable tuples with their contents
                    break;
                }
            }
            case TokenTreeType::VARIABLE:
            // {
            //     auto node = solveVariablePlaceHolder(i);
            //     if(node != nullptr)
            //         outQueue.push_back(node);
            //     else 
            //         outQueue.push_back(i);
            //     break;
            // }
            case TokenTreeType::CONSTANT:
            case TokenTreeType::INBUILT_FUNCTION:
            case TokenTreeType::FUNCTIONAL:
            case TokenTreeType::LIST:
            case TokenTreeType::DICT:
            {
                outQueue.push_back(i);
                break;
            }
            default:
                errorHandler(NotImplementedError("SimpleASTMaker Support for " + i->getName() + " tokenType"));
            }

            // We use Shunting yard Algorithm here!
        }

        // If there are still operators on the operator stack, pop them all
        while (opstack.size())
        {
            transferOpToOutQueue(opstack, outQueue);
        }
    }
    catch (...)
    {
        errorHandler(SyntacticError("Wrong Grammer followed!"));
    }

    // fflush(stdout);
    // std::cout<<"\n\nhere-->";
    // for (auto i : outQueue)
    // {
    //     std::cout << "_";
    //     fflush(stdout);
    //     std::cout << i->getName();
    //     fflush(stdout);
    // }
    // std::cout << std::endl;
    // // fflush(stdout);
    if(outQueue.size() != 1)
    {
        errorHandler(SyntacticError("Could not solve the statement for 1 result!"));
    }
    return outQueue[0];
}

std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t &context)
{
    std::vector<TokenTree *> solvedNodes;
    int tmpIndex = 0;
    int i = 0;

    while (tmpIndex < nodes.size())
    {
        if (i >= nodes.size() || (nodes[i]->getType() == TokenTreeType::OPERATOR && nodes[i]->getName() == ","))
        {
            if (i - tmpIndex == 1)
            {
                solvedNodes.push_back(nodes[tmpIndex]);
            }
            else
            {
                solvedNodes.push_back(simpleASTmaker(std::vector<TokenTree *>(&nodes[tmpIndex], &nodes[i]), context));
            }
            tmpIndex = i + 1;
        }
        ++i;
    }
    for (auto i : solvedNodes)
    {
        std::cout << "_" << i->getName();
        fflush(stdout);
    }
    std::cout << std::endl;

    return solvedNodes;
}
} // namespace turtle