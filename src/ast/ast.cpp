#include "ast/ast.hpp"
#include "library/common.hpp"
#include "tokens.hpp"

#include "tokenTree/literal.hpp"
#include "tokenTree/bracket.hpp"
#include "tokenTree/operator.hpp"
#include "tokenTree/tokenTree.hpp"

#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <exception>
#include <list>

namespace turtle
{
VariableTreeNode *contextSolver(VariableTreeNode *tok, variableContext_t &context, bool strict)
{
    // Starting from the end, search for context to the begining
    std::cout<<context.size();
    for (int i = context.size() - 1; i >= 0; i--)
    {
        // std::cout<<"TRYING";
        try
        {
            auto currContext = (*context[i]);

            auto var = currContext[tok->getName()];
            if (var != nullptr)
            {
                // printf("<=>");
                if(strict && var->getStoreType() == TokenTreeType::UNKNOWN)
                {
                    // printf("HERE");
                    continue;
                }
                // std::cout<<" FOUND ";
                return var;
            }
        }
        catch (...)
        {
            ;
        }
    }
    std::cout << "making in context: " << tok->getName();
    (*(context.back()))[tok->getName()] = tok;
    return tok;
}

auto parserLastStage(std::vector<TokenTree* > nodes)
{
    // Its purpose is just to compliment original parser stages to resolve things like
    // Function calls (<variable><tuple>) or Array Indexing (<variable><list>)

    std::vector<TokenTree *> treelist;

    for(int i = 0; i < nodes.size();)
    {
        switch(nodes[i]->getType())
        {
            case TokenTreeType::VARIABLE:
            {
                TokenTree* node = nodes[i];
                if(i + 1 < nodes.size())
                {
                    switch(nodes[i+1]->getType())
                    {
                        case TokenTreeType::TUPLE:
                        {
                            
                            node = new TempLiteralWrapperNode((VariableTreeNode*)node, GLOBAL_LITERAL_TABLE["__function__"], TokenTreeType::TEMP_LITERAL_WRAPPER);
                            // ++i;
                            break;
                        }
                        case TokenTreeType::LIST:
                        {
                            node = new TempLiteralWrapperNode((VariableTreeNode*)node, GLOBAL_LITERAL_TABLE["__array__"], TokenTreeType::TEMP_LITERAL_WRAPPER);
                            break;
                        }
                        default:
                        ;
                    }
                }
                treelist.push_back(node);
                ++i;
                break;
            }
            default:
            treelist.push_back(nodes[i]);
            ++i;
        }
    }
    return treelist;
}

auto genAST_Stage1(std::vector<TokenTree *> nodes, variableContext_t &context, bool tree_expantion = true)
{
    nodes = parserLastStage(nodes);

    std::vector<TokenTree *> treelist;
    TokenTree **tlist = &nodes[0];

    // std::list<TokenTree*> nodesList(nodes.begin(), nodes.end());
    for (int i = 0; i < nodes.size();)
    {
        // std::cout << tlist[i]->stringRepresentation() << " <=> ";
        switch (tlist[i]->getType())
        {
        case TokenTreeType::VARIABLE:
        {
            auto tmp = (VariableTreeNode *)tlist[i];
            if (tmp->getStoreType() == TokenTreeType::UNKNOWN)
            {
                tmp = contextSolver(tmp, context);
            }
            else
            {
                errorHandler(SyntacticError("Expected an undefined Variable"));
            }
            treelist.push_back(tmp);
            ++i;
            break;
        }
        case TokenTreeType::TUPLE:
        {
            // A lone tuple, Solve it!
            auto tmp = (TupleTreeNode *)tlist[i];
            tmp->solve(context);
            treelist.push_back(tmp);
            ++i;
            break;
        }
        case TokenTreeType::TEMP_LITERAL_WRAPPER:
        {
            auto tmp = (TempLiteralWrapperNode *)tlist[i];
            tlist[i] = tmp->getOriginal();
            // If its a temp_literal_wrapper, most probably it would hold only
            // Unknown variables. But still, lets just confirm!
            tlist[i] = contextSolver((VariableTreeNode *)tlist[i], context);
            auto [elem, poped] = tmp->getRule()->tokenTreeBuilder(tlist, i, nodes.size(), context, tree_expantion);
            i += poped;
            treelist.push_back(elem);
            break;
        }
        case TokenTreeType::TEMP_LITERAL:
        {
            auto tmp = (TempLiteralTreeNode *)tlist[i];
            auto [elem, poped] = tmp->getRule()->tokenTreeBuilder(tlist, i, nodes.size(), context, tree_expantion);
            i += poped;
            treelist.push_back(elem);
            break;
        }
        case TokenTreeType::FUTURE_LITERAL:
        {
            // tlist[i]->setContext(context);
            treelist.push_back(tlist[i]->execute());
            ++i;
            // errorHandler(NotImplementedError("Just testing"));
            break;
        }
        default:
            treelist.push_back(tlist[i]);
            ++i;
        }
    }

    return treelist;
}

TokenTree *genAST(std::vector<TokenTree *> nodes, variableContext_t &context)
{
    //Stich together the tokentree nodes to generate an Abstract Syntax Tree with one single root node
    // This is done in several passes.
    // First Pass includes Pairing together TempLiteralNodes and determining CurlyBracket Types (Dicts/Codeblocks) and lots of other works
    auto stage1Nodes = genAST_Stage1(nodes, context);

    // return stage1Nodes;
    return nullptr;
}

TokenTree *symbolicASTexecutor(std::vector<TokenTree *> nodes, variableContext_t &context)
{
    // In Symbolic Execution, Basically, all nodes are Tree Roots
    // We solve the tree Depth First from right side
    TokenTree *result, *temp;
    std::cout << "\nSymbolic Execution! " << nodes.size() << " " << context.size() << "\n";

    for (int i = 0; i < nodes.size(); i++)
    {
        auto tok = nodes[i];
        // tok = solveVariablePlaceHolder(tok);
        while (tok != nullptr)
        {
            std::cout<<"==>"<<tok->stringRepresentation();
            fflush(stdout);
            temp = tok->execute();

            if (temp == tok)
                break;
            tok = temp;
        }
        
        if (tok == nullptr)
        {
            errorHandler(SyntacticError("Nullptr returned!"));
            break;
        }
        result = tok;
        if (result->getType() == TokenTreeType::RETURN)
        {
            // (*context.back())["$return"] = new VariableTreeNode(((ReturnTreeNode *)result)->getValue());
            result = ((ReturnTreeNode *)result)->getValue()->execute(); //result->getValue();
            break;
        }
    }
    // errorHandler(NotImplementedError("Symbolic Execution"));
    std::cout << "Symbolic Execution complete!\n";
    if (result == nullptr)
    {
        errorHandler(SyntacticError("Nullptr returned!"));
    }
    // std::cout<<result->stringRepresentation();
    return result;
}

inline auto transferOpToOutQueue(std::vector<OperatorTreeNode *> &opstack, std::vector<TokenTree *> &outQueue)
{
    // Pop elements while There are elements of higher precedence in the stack
    // or equal precedence but the operator is left associative
    TokenTree *val;
    if (opstack.back()->getOptype() & (uint)OperatorType::BINARY)
    {
        auto op = (BinaryOperatorTreeNode *)opstack.back();
        opstack.pop_back();

        // generate Tree out of these operators
        // Pop two elements out of outQueue
        if (outQueue.size() < 2)
        {
            outQueue.push_back(op);
            return;
        }
        auto elm1 = outQueue.back();
        outQueue.pop_back();
        auto elm2 = outQueue.back();
        outQueue.pop_back();

        // Make them as the two nodes of the Operator
        op->setRight(elm1);
        op->setLeft(elm2);

        val = op; //op->execute();
    }
    else
    {
        // errorHandler(NotImplementedError("Unary Operators"));

        auto op = (UnaryOperatorTreeNode *)opstack.back();
        opstack.pop_back();

        // generate Tree out of these operators
        // Pop two elements out of outQueue
        if (outQueue.size() < 1)
        {
            outQueue.push_back(op);
            return;
        }
        auto elm1 = outQueue.back();
        outQueue.pop_back();

        // Make them as the two nodes of the Operator
        op->setChild(elm1);

        val = op; //->execute();
    }
    // Now push this back to the outQueue
    outQueue.push_back(val);
}

TokenTree *simpleASTmaker(std::vector<TokenTree *> nodes, variableContext_t &context, bool tree_expantion)
{
    printf("[In SimpleAstMaker]");
    fflush(stdout);
    // Its job is to solve for simple expressions
    // And if its purely arithmatic on constants, Even solve it completely
    // and return a single node at the end, Maybe a constant, maybe a root to an
    // AST. Panic at complex things like TempLiterals etc.
    if (nodes.size() == 0)
    {
        errorHandler(SyntacticError("Some element needs to be provided into the tupple"));
        return nullptr;
    }

    auto stage1Nodes = genAST_Stage1(nodes, context, tree_expantion); // For Only Functions and Lists

    TokenTree *rootNode = stage1Nodes[0];
    TokenTree *currNode = rootNode;

    std::vector<OperatorTreeNode *> opstack;
    std::vector<TokenTree *> outQueue;

    try
    {
        for (auto i : stage1Nodes)
        {
            std::cout << " >> " << i->getName();
            fflush(stdout);
            switch (i->getType())
            {
            case TokenTreeType::OPERATOR:
            {
                OperatorTreeNode *node = (OperatorTreeNode *)i;

                while (opstack.size() and
                       (opstack.back()->getPrecedence() < node->getPrecedence() or
                        (opstack.back()->getPrecedence() == node->getPrecedence() and node->getAssociativity() == OperatorAssociativity::LEFT)))
                {
                    transferOpToOutQueue(opstack, outQueue);
                }
                opstack.push_back(node);
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
            case TokenTreeType::CONDITIONAL:
            case TokenTreeType::VARIABLE:
            case TokenTreeType::CONSTANT:
            case TokenTreeType::INBUILT_FUNCTION:
            case TokenTreeType::FUNCTIONAL:
            case TokenTreeType::LIST:
            case TokenTreeType::DICT:
            case TokenTreeType::FUTURE_LITERAL:
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
            // std::cout << "We are here!";
            // fflush(stdout);
            std::cout << " -->> " << outQueue.size();
            transferOpToOutQueue(opstack, outQueue);
            // outQueue.push_back(opstack.back());
            // opstack.pop_back();
        }
    }
    catch (...)
    {
        errorHandler(SyntacticError("Wrong Grammer followed!"));
    }

    if (outQueue.size() != 1)
    {
        errorHandler(SyntacticError("Could not solve the statement for 1 result!"));
    }
    return outQueue[0];
}

std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t &context, std::string seperator, bool symbolic_execution, bool tree_expantion)
{
    std::vector<TokenTree *> solvedNodes;
    int tmpIndex = 0;
    int i = 0;

    // if (symbolic_execution)
    // {
    //     solvedNodes.push_back(symbolicASTexecutor(nodes, context));
    // }
    // else
    // {
        while (tmpIndex < nodes.size())
        {
            if (i >= nodes.size() || (nodes[i]->getType() == TokenTreeType::OPERATOR && nodes[i]->getName() == seperator))
            {
                    solvedNodes.push_back(simpleASTmaker(std::vector<TokenTree *>(&nodes[tmpIndex], &nodes[i]), context, tree_expantion));
                tmpIndex = i + 1;
            }
            ++i;
        }
    // }

    for (auto i : solvedNodes)
    {
        std::cout << "_" << i->getName();
        fflush(stdout);
    }
    std::cout << std::endl;

    return solvedNodes;
}
} // namespace turtle