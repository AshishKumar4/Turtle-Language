#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"
// #include "library/trie.hpp"
#include "library/common.hpp"
#include "tokenTree/tokenTree.hpp"
#include "tokenTree/operator.hpp"

#include "operatorHandlers.hpp"

#include <exception>
#include <map>

namespace turtle
{

std::map<std::string, OperatorUnaryProfile> TOKEN_OPERATOR_UNARY_TYPE_TABLE;
std::map<std::string, OperatorBinaryProfile> TOKEN_OPERATOR_BINARY_TYPE_TABLE;

std::string UNARY_OPERATORS[] = {"++", "--", "+", "-", "~", "!"};
std::string BINARY_OPERATORS[] = {"+", "-", "=", "*", "/", "**", "&", "^", "|",
                                  "&&", "||", "<", ">", "<<", ">>", "%", "!", "<=",
                                  ">=", "-=", "+=", "*=", "/=", "|=", "&=", "^=",
                                  "#", ",", ":"};
std::string TERNARY_OPERATOR[] = {"?"};

std::string ARITHMATIC_OPERATORS[] = {"+", "-", "*", "/", "**"};
std::string BOOLEAN_OPERATORS[] = {"&&", "||", "<", ">", "<=", ">="};
std::string BITWISE_OPERATORS[] = {"|", "&", "<<", ">>"};
std::string SEPERATOR_OPERATORS[] = {",", ":"};

std::string ALL_OPERATORS[] = {"++", "--", "+", "-", "!", "=", "*", "/", "**", "&"
                                                                               "^",
                               "|", "&&", "||", "<", ">", "<<", ">>", "%", "<=",
                               ">=", "-=", "+=", "*=", "/=", "|=", "&=", "^=", "#", ",", ":"};

void init_operatorTypeTable()
{
    TOKEN_OPERATOR_UNARY_TYPE_TABLE["++"] = OperatorUnaryProfile(operator_unaryTemplate, ((OperatorInfo)OperatorType::UNARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 2);
    TOKEN_OPERATOR_UNARY_TYPE_TABLE["--"] = OperatorUnaryProfile(operator_unaryTemplate, ((OperatorInfo)OperatorType::UNARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 2);

    TOKEN_OPERATOR_UNARY_TYPE_TABLE["+"] = OperatorUnaryProfile(operator_unaryTemplate, ((OperatorInfo)OperatorType::UNARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 3);
    TOKEN_OPERATOR_UNARY_TYPE_TABLE["-"] = OperatorUnaryProfile(operator_unaryTemplate, ((OperatorInfo)OperatorType::UNARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 3);
    TOKEN_OPERATOR_UNARY_TYPE_TABLE["~"] = OperatorUnaryProfile(operator_unaryTemplate, ((OperatorInfo)OperatorType::UNARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::LEFT, 3);
    TOKEN_OPERATOR_UNARY_TYPE_TABLE["!"] = OperatorUnaryProfile(operator_unaryTemplate, ((OperatorInfo)OperatorType::UNARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 3);

    // Binary Operators -->
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["**"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::RIGHT, 3);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE["*"] = OperatorBinaryProfile(operator_binaryMultiplication, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 5);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["/"] = OperatorBinaryProfile(operator_binaryDivision, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 5);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["%"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 5);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE["+"] = OperatorBinaryProfile(operator_binaryAddition, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 6);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["-"] = OperatorBinaryProfile(operator_binarySubtraction, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 6);
    // TOKEN_OPERATOR_BINARY_TYPE_TABLE["&"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::LEFT, 6);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE["<<"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::LEFT, 7);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE[">>"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::LEFT, 7);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE["<"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 9);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE[">"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 9);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["<="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 9);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE[">="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 9);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE["=="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 10);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["!="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 10);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE["^"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::LEFT, 12);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["|"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::LEFT, 13);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["&&"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 14);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["||"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), OperatorAssociativity::LEFT, 15);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE["="] = OperatorBinaryProfile(operator_binaryEqual, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC | (OperatorInfo)OperatorType::CANNOT_SOLVE_VARIABLE), OperatorAssociativity::RIGHT, 16);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["+="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::RIGHT, 16);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["-="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::RIGHT, 16);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["*="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::RIGHT, 16);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["/="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), OperatorAssociativity::RIGHT, 16);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["|="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::RIGHT, 16);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["&="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::RIGHT, 16);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE["^="] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BITWISE), OperatorAssociativity::RIGHT, 16);

    TOKEN_OPERATOR_BINARY_TYPE_TABLE[","] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY), OperatorAssociativity::LEFT, 17);
    TOKEN_OPERATOR_BINARY_TYPE_TABLE[";"] = OperatorBinaryProfile(operator_binaryTemplate, ((OperatorInfo)OperatorType::BINARY), OperatorAssociativity::LEFT, 17);
}

TokenDigesterReturn_t tokenDigester_operator(Token **list, int index, int size)//, variableContext_t &context)
{
    // Operators can be of many type too
    auto tok = list[index];
    if(index == 0 || list[index-1]->type == TokenType::OPERATOR)
    {
        // Its a Unary Operator!
        if(TOKEN_OPERATOR_UNARY_TYPE_TABLE.find(tok->data) == TOKEN_OPERATOR_UNARY_TYPE_TABLE.end())
        {
            errorHandler(ParserError("No such operator found!"));
            return TokenDigesterReturn_t(nullptr, 1);
        }
        auto opProfile = TOKEN_OPERATOR_UNARY_TYPE_TABLE[tok->data];
        auto [logic, info, asso, pres] = opProfile;
        if(info == 0)
        {
            errorHandler(ParserError("No such operator found!"));
            return TokenDigesterReturn_t(nullptr, 1);
        }
        auto node = new UnaryOperatorTreeNode(info, tok->data, logic, pres, asso);
        return TokenDigesterReturn_t(node, 1);
    }
    else 
    {
        // We assumes its a Binary Operator!
        // Its a Binary Operator!
        if(TOKEN_OPERATOR_BINARY_TYPE_TABLE.find(tok->data) == TOKEN_OPERATOR_BINARY_TYPE_TABLE.end())
        {
            errorHandler(ParserError("No such operator found!"));
            return TokenDigesterReturn_t(nullptr, 1);
        }
        auto opProfile = TOKEN_OPERATOR_BINARY_TYPE_TABLE[tok->data];
        // std::cout<<tok->data;
        // fflush(stdout);
        auto [logic, info, asso, pres] = opProfile;
        if(info == 0)
        {
            errorHandler(ParserError("No such operator found!"));
            return TokenDigesterReturn_t(nullptr, 1);
        }
        auto node = new BinaryOperatorTreeNode(info, tok->data, logic, pres, asso);
        // std::cout<<"GENERATED";
        // fflush(stdout);
        return TokenDigesterReturn_t(node, 1);
    }
}
} // namespace turtle