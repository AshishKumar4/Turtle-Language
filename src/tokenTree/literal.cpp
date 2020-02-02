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

TokenTree *packageDict(std::vector<TokenTree *> list, BracketInfo btype, variableContext_t &context)
{
    return new DictTreeNode(list);
}

TokenTree *packageSet(std::vector<TokenTree *> list, BracketInfo btype, variableContext_t &context)
{
    return nullptr;
}

TokenDigesterReturn_t tokenDigester_literal(Token **list, int index, int size) //, variableContext_t &context)
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

            // If a literal is followed by a bracket, it means something!
            if (index + 1 < size && list[index + 1]->type == TokenType::BRACKET)
            {
                auto brack = list[index + 1]->data;
                if (brack == "(")
                {
                    // Its a function call!
                    return TokenDigesterReturn_t(new TempLiteralWrapperNode(newTok, GLOBAL_LITERAL_TABLE["__function__"], TokenTreeType::TEMP_LITERAL_WRAPPER), 1);
                }
                else if (brack == "[")
                {
                    // Its Array Indexing
                    return TokenDigesterReturn_t(new TempLiteralWrapperNode(newTok, GLOBAL_LITERAL_TABLE["__array__"], TokenTreeType::TEMP_LITERAL_WRAPPER), 1);
                }
                // else if (brack == "{")
                // {
                //     if (tok->data == "dict" || tok->data == "d")
                //     {
                //         auto [node, popped] = bracketSolver(list, index + 1, size, TOKEN_BRACKET_TYPE_TABLE["{"], context, packageDict);
                //         return TokenDigesterReturn_t(node, popped + 1);
                //     }
                //     else if (tok->data == "set" || tok->data == "s")
                //     {
                //         auto [node, popped] = bracketSolver(list, index + 1, size, TOKEN_BRACKET_TYPE_TABLE["{"], context, packageSet);
                //         return TokenDigesterReturn_t(node, popped + 1);
                //     }
                //     else
                //         errorHandler(ParserError("A Literal cannot be followed by Curly Braces!"));
                // }
            }
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

TokenDigesterReturn_t tokenDigester_number(Token **list, int index, int size) //, variableContext_t &context)
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
        auto number = std::stoull(num, nullptr, base);
        // std::cout<<"num["<<number<<"]";
        Integer *intnum = new Integer((uint64_t)number, DataPassMode::COPY);
        ConstantTreeNode *node = new ConstantTreeNode(intnum, num);
        return TokenDigesterReturn_t(node, 1);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return TokenDigesterReturn_t((TokenTree *)NULL, 1);
}
} // namespace turtle