#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"

#include "library/trie.hpp"
#include "library/common.hpp"

#include "tokenTree/tokenTree.hpp"
#include "tokenTree/bracket.hpp"
#include "tokenTree/literal.hpp"

#include "ast/ast.hpp"

#include <exception>
#include <map>

namespace turtle
{

TokenTree *packageDict(std::vector<TokenTree *> list, BracketInfo btype, variableContext_t context)
{
    return new DictTreeNode(list);
}

TokenTree *packageSet(std::vector<TokenTree *> list, BracketInfo btype, variableContext_t context)
{
    return nullptr;
}

TokenDigesterReturn_t tokenDigester_literal(Token **list, int index, int size) //, variableContext_t context)
{
    // Find out the type of literal
    auto tok = list[index];
    try
    {
        // printf("\nsearching in global literal table..");
        auto obj = GLOBAL_LITERAL_TABLE[tok->data];
        if (obj == nullptr)
        {
            // Lets not solve variables over here!
            VariableTreeNode *newTok = new VariableTreeNode(nullptr, tok->data); //contextSolver(tok, context);
            return TokenDigesterReturn_t(newTok, 1);
        }
        // Must be some keyword
        return TokenDigesterReturn_t(new TempLiteralTreeNode(tok->data, obj), 1);
    }
    catch (...)
    {
        return TokenDigesterReturn_t(new VariableTreeNode(nullptr, tok->data), 1);
    }
}

TokenDigesterReturn_t tokenDigester_number(Token **list, int index, int size) //, variableContext_t context)
{
    auto tok = list[index];
    // Find out the type of number
    auto num = tok->data;

    int base = 10;
    try
    {
        switch (num[1])
        {
        case 'x':
            base = 16;
            break;
        case 'o':
            base = 8;
            break;
        case 'b':
            base = 2;
            break;
        default:
            base = 10;
        }
        int dots = 0;
        for (auto i : num)
        {
            if (i == '.')
            {
                // Its a Float!
                ++dots;
            }
        }
        if (dots == 0)
        {
            turtleInt number = (int64_t)std::stoull(num, nullptr, base);
            ConstantTreeNode<turtleInt> *node = new ConstantTreeNode<turtleInt>(number, num);
            return TokenDigesterReturn_t(node, 1);
        }
        else if (dots == 1)
        {
            turtleFloat number = (float)std::stof(num);
            ConstantTreeNode<turtleFloat> *node = new ConstantTreeNode<turtleFloat>(number, num);
            return TokenDigesterReturn_t(node, 1);
        }
        else
        {
            errorHandler(SyntacticError("Multiple 'dot' operators detected in a single number!"));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return TokenDigesterReturn_t((TokenTree *)NULL, 1);
}
} // namespace turtle