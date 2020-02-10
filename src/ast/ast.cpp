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
VariableTreeNode *contextSolver(VariableTreeNode *tok, variableContext_t context, bool strict)
{
    // Starting from the end, search for context to the begining
    std::cout << context.size();
    for (int i = context.size() - 1; i >= 0; i--)
    {
        // std::cout << "\nTRYING";
        try
        {
            auto currContext = (*context[i]);

            // auto var = &currContext[tok->getName()];
            if (currContext.find(tok->getName()) != currContext.end())
            {
                VariableTreeNode *var = (currContext[tok->getName()]);
                if (strict && var->getStoreType() == TokenTreeType::UNKNOWN)
                {
                    continue;
                }
                // std::cout << " FOUND " << var->stringRepresentation();
                return var;
            }
        }
        catch (...)
        {
            ;
        }
    }
    std::cout << "making in context: " << tok->getName();
    // currContext[tok->getName()] = tok;
    (*(context.back()))[tok->getName()] = tok;
    // (*(context.back())).insert(std::make_pair(tok->getName(), tok));
    return tok;
}

auto parserLastStage(std::vector<TokenTree *> nodes)
{
    std::list<TokenTree * > treelist(nodes.begin() + 1, nodes.end());
    std::vector<TokenTree * > outQueue = {nodes[0]};
    while(treelist.size())
    {
        auto l = outQueue.back();
        auto r = treelist.front();
        switch(l->getType())
        {
        case TokenTreeType::FUTURE_LITERAL:
        case TokenTreeType::TUPLE:
        case TokenTreeType::LIST:
        case TokenTreeType::DICT:
        case TokenTreeType::VARIABLE:
        case TokenTreeType::TEMP_LITERAL:
        {
            switch(r->getType())
            {
                case TokenTreeType::TUPLE:
                {
                    auto node = new FutureSolutionTreeNode(TokenTreeType::FUNCTIONAL, {l, r}, GLOBAL_LITERAL_TABLE["__function__"]->rules);
                    outQueue.pop_back();
                    outQueue.push_back(node); 
                    // node = new TempLiteralWrapperNode(node, GLOBAL_LITERAL_TABLE["__function__"], TokenTreeType::TEMP_LITERAL_WRAPPER);
                    break;
                }
                case TokenTreeType::LIST:
                {
                    auto node = new FutureSolutionTreeNode(TokenTreeType::LIST, {l, r}, GLOBAL_LITERAL_TABLE["__array__"]->rules);
                    outQueue.pop_back();
                    outQueue.push_back(node); 
                    // node = new TempLiteralWrapperNode(node, GLOBAL_LITERAL_TABLE["__array__"], TokenTreeType::TEMP_LITERAL_WRAPPER);
                    break;
                }
                default:
                outQueue.push_back(r);
            }
            break;
        }
        default:
            outQueue.push_back(r);
        }
        treelist.pop_front();
    }
    return outQueue;
}

auto genAST_Stage1(std::vector<TokenTree *> nodes, variableContext_t context, bool tree_expantion = true)
{
    // std::vector<TokenTree *> newNodes;
    // while(true)
    // {
    //     newNodes = parserLastStage(nodes);
    //     if(newNodes.size() == nodes.size())
    //         break;
    // }
    // nodes = parserLastStage(nodes);//newNodes;
    // std::cout<<"\n";
    // for(auto i : nodes)
    // {
    //     std::cout<<"-->"<<i->stringRepresentation();
    // }
    std::vector<TokenTree *> treelist;
    TokenTree **tlist = &nodes[0];

    for (int i = 0; i < nodes.size();)
    {
        switch (tlist[i]->getType())
        {
        // case TokenTreeType::TEMP_LITERAL_WRAPPER:
        // {
        //     auto tmp = (TempLiteralWrapperNode *)tlist[i];
        //     tlist[i] = tmp->getOriginal();
        //     // If its a temp_literal_wrapper, most probably it would hold only
        //     // Unknown variables. But still, lets just confirm!
        //     if(tlist[i]->getType() == TokenTreeType::VARIABLE)
        //         tlist[i] = contextSolver((VariableTreeNode *)tlist[i], context);
        //     auto [elem, poped] = tmp->getRule()->tokenTreeBuilder(tlist, i, nodes.size(), context, tree_expantion);
        //     i += poped;
        //     treelist.push_back(elem);
        //     break;
        // }
        case TokenTreeType::TEMP_LITERAL:
        {
            auto tmp = (TempLiteralTreeNode *)tlist[i];
            auto [elem, poped] = tmp->getRule()->tokenTreeBuilder(tlist, i, nodes.size(), context, tree_expantion);
            i += poped;
            treelist.push_back(elem);
            break;
        }
        // case TokenTreeType::FUTURE_LITERAL:
        // {
        //     // ((FutureSolutionTreeNode*)tlist[i])->setContext(context);
        //     treelist.push_back(tlist[i]->execute(context));
        //     ++i;
        //     // errorHandler(NotImplementedError("Just testing"));
        //     break;
        // }
        default:
            treelist.push_back(tlist[i]);
            ++i;
        }
    }

    return treelist;
}

auto genAST_Stage2(std::vector<TokenTree *> nodes, variableContext_t context, bool tree_expantion = true)
{
    nodes = parserLastStage(nodes);
    std::vector<TokenTree *> treelist;
    TokenTree **tlist = &nodes[0];

    for (int i = 0; i < nodes.size();)
    {
        std::cout << tlist[i]->stringRepresentation() << " <=> ";
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
        case TokenTreeType::LIST:
        {
            // A lone tuple, Solve it!
            auto tmp = (ListTreeNode *)tlist[i];
            tmp->solve(context);
            treelist.push_back(tmp);
            ++i;
            break;
        }
        default:
            treelist.push_back(tlist[i]);
            ++i;
        }
    }

    return treelist;
}

TokenTree *symbolicASTexecutor(std::vector<TokenTree *> nodes, variableContext_t context)
{
    // In Symbolic Execution, Basically, all nodes are Tree Roots
    // We solve the tree Depth First from right side
    TokenTree *result, *temp; //, *last = nodes[0];
    std::cout << "\nSymbolic Execution! " << nodes.size() << " " << context.size() << "\n";
    fflush(stdout);
    for (int i = 0; i < nodes.size(); i++)
    {
        auto tok = nodes[i];
        // tok = solveVariablePlaceHolder(tok);
        while (tok != nullptr)
        {
            // std::cout << "==>" << tok->stringRepresentation();
            fflush(stdout);
            temp = tok->execute(context);

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
            // result = ((ReturnTreeNode *)result)->getValue()->execute(context); //result->getValue();
            break;
        }
        else if (result->getType() == TokenTreeType::BREAK)
        {
            // Return the last token
            // result = last;
            break;
        }
        // last = result;
    }
    // errorHandler(NotImplementedError("Symbolic Execution"));
    std::cout << "Symbolic Execution complete!\n";
    if (result == nullptr)
    {
        errorHandler(SyntacticError("Nullptr returned!"));
    }
    std::cout << result->stringRepresentation();
    fflush(stdout);
    return result;
}

inline auto transferOpToOutQueue(std::vector<OperatorTreeNode *> &opstack, std::vector<TokenTree *> &outQueue, variableContext_t context)
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

        val = op;//->execute(context);
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

TokenTree* genFutureAST(std::vector<TokenTree *> nodes)
{
    std::list<TokenTree *> outQueue(nodes.begin(), nodes.end());
    while(outQueue.size() > 1)
    {
        auto l = outQueue.front();
        outQueue.pop_front();
        auto r = outQueue.front();
        outQueue.pop_front();

        if(r->getType() == TokenTreeType::TUPLE)
        {
            std::cout<<"\n{"<<l->stringRepresentation()<<" || "<<r->stringRepresentation()<<std::endl;
            // Must be a Function call!
            outQueue.push_front(new FutureSolutionTreeNode(TokenTreeType::FUNCTIONAL, {l, r}, GLOBAL_LITERAL_TABLE["__function__"]->rules));
        }
        else if(r->getType() == TokenTreeType::LIST)
        {
            // Must be a Function call!
            std::cout<<"\n{"<<l->stringRepresentation()<<" || "<<r->stringRepresentation()<<std::endl;
            outQueue.push_front(new FutureSolutionTreeNode(TokenTreeType::LIST, {l, r}, GLOBAL_LITERAL_TABLE["__array__"]->rules));
        }
    }
    if(outQueue.size() != 1) 
    {
        errorHandler(SyntacticError("Could not solve the statement for 1 result!"));
    }
    return outQueue.front();
}

TokenTree *simpleASTmaker(std::vector<TokenTree *> nodes, variableContext_t context, bool tree_expantion)
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
    auto stage2Nodes = genAST_Stage2(stage1Nodes, context, tree_expantion); // For Only Functions and Lists

    TokenTree *rootNode = stage2Nodes[0];
    TokenTree *currNode = rootNode;

    std::vector<OperatorTreeNode *> opstack;
    std::vector<TokenTree *> outQueue;

    try
    {
        for (auto i : stage2Nodes)
        {
            std::cout << " >> " << i->stringRepresentation();
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
                    transferOpToOutQueue(opstack, outQueue, context);
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
            case TokenTreeType::LOOP:
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
            // std::cout << " -->> " << outQueue.size();
            transferOpToOutQueue(opstack, outQueue, context);
            // outQueue.push_back(opstack.back());
            // opstack.pop_back();
        }
    }
    catch (...)
    {
        errorHandler(SyntacticError("Wrong Grammer followed!"));
    }

    TokenTree* result;

    if (outQueue.size() != 1)
    {
        // Recursively solve AST
        result = genFutureAST(outQueue);
        // result = simpleASTmaker(outQueue, context, true);
        // errorHandler(SyntacticError("Could not solve the statement for 1 result!"));
    }
    else 
    {
        result = outQueue[0];
    }

    std::cout << "\n[AST OUTPUT: " << result->stringRepresentation() << "]";
    return result;
}

std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t context, std::string seperator, bool symbolic_execution, bool tree_expantion)
{
    std::vector<TokenTree *> solvedNodes;
    int tmpIndex = 0;
    int i = 0;

    while (tmpIndex < nodes.size())
    {
        if (i >= nodes.size() || (nodes[i]->getType() == TokenTreeType::OPERATOR && nodes[i]->getName() == seperator))
        {
            solvedNodes.push_back(simpleASTmaker(std::vector<TokenTree *>(&nodes[tmpIndex], &nodes[i]), context, tree_expantion));
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