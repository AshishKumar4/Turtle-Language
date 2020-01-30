#pragma once

#include "library/common.hpp"
#include "tokens.hpp"
#include "tokenRules.hpp"

namespace turtle
{
enum class OperatorType
{
    UNARY = 1 << 1,
    BINARY = 1 << 2,
    TERNARY = 1 << 3,

    ARITHMATIC = 1 << 4,
    BOOLEAN = ARITHMATIC | 1 << 5,
    BITWISE = ARITHMATIC | 1 << 6,
    SEPERATOR_OPERATORS = 1 << 7,
};

enum class OperatorAssociativity
{
    LEFT,
    RIGHT,
    NONE
};

typedef uint OperatorInfo;
typedef uint OperatorPrecedence;
// typedef std::function<MemoryWrapper*(MemoryWrapper*)> OperatorUnaryLogic;
// typedef std::function<MemoryWrapper*(MemoryWrapper*, MemoryWrapper*)> OperatorBinaryLogic;

typedef std::function<TokenTree *(TokenTree *)> OperatorUnaryLogic;
typedef std::function<TokenTree *(TokenTree *, TokenTree *)> OperatorBinaryLogic;

typedef std::tuple<OperatorUnaryLogic, OperatorInfo, OperatorAssociativity, OperatorPrecedence> OperatorUnaryProfile;
typedef std::tuple<OperatorBinaryLogic, OperatorInfo, OperatorAssociativity, OperatorPrecedence> OperatorBinaryProfile;

class OperatorTreeNode : public TokenTree
{
protected:
    OperatorPrecedence precedence;
    OperatorAssociativity associativity;
    const OperatorInfo opType;
    TokenTree *result;

public:
    OperatorTreeNode(OperatorInfo opType, std::string symbol, OperatorPrecedence precedence = 0, OperatorAssociativity associativity = OperatorAssociativity::LEFT) : opType(opType), TokenTree(TokenTreeType::OPERATOR, TokenTreeUseType::STATIC, symbol), precedence(precedence), associativity(associativity)
    {
    }

    auto getPrecedence()
    {
        return precedence;
    }

    auto getAssociativity()
    {
        return associativity;
    }

    auto getOptype()
    {
        return opType;
    }

    virtual TokenTree *execute()
    {
        return this;
    }
};

class UnaryOperatorTreeNode : public OperatorTreeNode
{
    TokenTree *child;
    OperatorUnaryLogic logic;

public:
    UnaryOperatorTreeNode(OperatorInfo type, std::string symbol, OperatorUnaryLogic logic, OperatorPrecedence precedence = 0, OperatorAssociativity associativity = OperatorAssociativity::LEFT, TokenTree *child = nullptr) : OperatorTreeNode(type, symbol, precedence, associativity), child(child), logic(logic)
    {
    }

    TokenTree *execute()
    {
        std::cout << "Trying to execute!";
        return this;
    }
};

class BinaryOperatorTreeNode : public OperatorTreeNode
{
    TokenTree *left;
    TokenTree *right;
    OperatorBinaryLogic logic;

public:
    BinaryOperatorTreeNode(OperatorInfo type, std::string symbol, OperatorBinaryLogic logic, OperatorPrecedence precedence = 0, OperatorAssociativity associativity = OperatorAssociativity::LEFT, TokenTree *left = nullptr, TokenTree *right = nullptr) : OperatorTreeNode(type, symbol, precedence, associativity), left(left), right(right), logic(logic)
    {
    }

    void setLeft(TokenTree *tok)
    {
        left = tok;
    }

    void setRight(TokenTree *tok)
    {
        right = tok;
    }

    TokenTree *execute()
    {
        // std::cout << "Trying to execute!";
        std::cout << "\n->{" << left->getName() << " " << this->getName() << " " << right->getName() << "}";
        fflush(stdout);
        if (left != nullptr && right != nullptr && !checkIfUnknown(left) && !checkIfUnknown(right))
        {
            auto val = logic(left, right);
            if(val != nullptr)
            {
                this->result = val;
                return val;
            }
        }
        return this;
    }
};

class TertiaryOperatorTreeNode : public OperatorTreeNode
{
    TokenTree *first;
    TokenTree *second;
    TokenTree *third;

public:
    TertiaryOperatorTreeNode(OperatorInfo type, std::string symbol, TokenTree *first, TokenTree *second, TokenTree *third, int precedence = 0) : OperatorTreeNode(type, symbol, precedence), first(first), second(second), third(third)
    {
    }
};

void init_operatorTypeTable();
} // namespace turtle