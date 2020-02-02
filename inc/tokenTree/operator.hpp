#pragma once

#include <functional>
#include <tuple>
#include <vector>

#include "library/common.hpp"
#include "tokens.hpp"
#include "tokenRules.hpp"

#include "tokenTree/tokenTree.hpp"

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

    CANNOT_SOLVE_VARIABLE = 1 << 8,
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
    static Grabs<OperatorTreeNode> grabsToken;

protected:
    OperatorPrecedence precedence;
    OperatorAssociativity associativity;
    const OperatorInfo opType;
    TokenTree *result;

public:
    OperatorTreeNode(OperatorInfo opType, std::string symbol, OperatorPrecedence precedence = 0, OperatorAssociativity associativity = OperatorAssociativity::LEFT) : opType(opType), TokenTree(TokenTreeType::OPERATOR, TokenTreeUseType::STATIC, symbol), precedence(precedence), associativity(associativity)
    {
    }

    // void* operator new(std::size_t size)
    // {
    //     std::cout<<"This made!";
    //     OperatorTreeNode* tok = grabsToken.grab();
    //     return tok;
    // }

    // void operator delete(void* ptr)
    // {
    //     std::cout<<"Custom delete!";
    //     grabsToken.giveBack((OperatorTreeNode*)ptr);
    // }

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

    // virtual TokenTree *execute() = 0;

    // virtual TokenTree *executeRecursive() = 0;
};

class UnaryOperatorTreeNode : public OperatorTreeNode
{
    static Grabs<UnaryOperatorTreeNode> grabsToken;

    TokenTree *child;
    OperatorUnaryLogic logic;

public:
    UnaryOperatorTreeNode(OperatorInfo type, std::string symbol, OperatorUnaryLogic logic, OperatorPrecedence precedence = 0, OperatorAssociativity associativity = OperatorAssociativity::LEFT, TokenTree *child = nullptr) : OperatorTreeNode(type, symbol, precedence, associativity), child(child), logic(logic)
    {
    }

    void *operator new(std::size_t size)
    {
        // std::cout<<"OPERATOR MADE!";
        UnaryOperatorTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        // std::cout<<"Custom delete!";
        grabsToken.giveBack((UnaryOperatorTreeNode *)ptr);
    }

    void setChild(TokenTree *child)
    {
        child = child;
    }

    TokenTree *execute(variableContext_t context)
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

    static Grabs<BinaryOperatorTreeNode> grabsToken;

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

    void *operator new(std::size_t size)
    {
        // std::cout<<"OPERATOR MADE!";
        BinaryOperatorTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        // std::cout<<"Custom delete!";
        grabsToken.giveBack((BinaryOperatorTreeNode *)ptr);
    }

    // TokenTree *execute()
    // {
    //     // std::cout << "Trying to execute!";
    //     std::cout << "\n->{" << left->getName() << " " << this->getName() << " " << right->getName() << "}";
    //     fflush(stdout);
    //     if (left != nullptr && right != nullptr && left->getType() != TokenTreeType::OPERATOR && right->getType() != TokenTreeType::OPERATOR && !checkIfUnknown(left) && !checkIfUnknownVar(right))
    //     {
    //         auto val = logic(left, right);
    //         if (val != nullptr)
    //         {
    //             this->result = val;
    //             return val;
    //         }
    //     }
    //     return this;
    // }

    TokenTree *execute(variableContext_t context)
    {
        // std::cout << "Trying to execute!";
        // std::cout << "\n->r{" << left->getName() << " " << this->getName() << " " << right->getName() << "}";
        std::cout << "\n->r{" << left->stringRepresentation() << "_" << this->getName() << "_" << right->stringRepresentation() << "}";
        fflush(stdout);
        auto l = left;
        auto r = right;

        if (l != nullptr && r != nullptr
            //  && l->getType() != TokenTreeType::OPERATOR
            //  && r->getType() != TokenTreeType::OPERATOR
            && !checkIfUnknown(l) && !checkIfUnknownVar(r))
        {
            r = r->execute(context);
            if (!(this->opType & (OperatorInfo)OperatorType::CANNOT_SOLVE_VARIABLE))
            {
                l = l->execute(context);
            }
            std::cout << "\n---->r[" << l->stringRepresentation() << "_" << this->getName() << "_" << r->stringRepresentation() << "]";
            auto val = logic(l, r);
            if (val != nullptr)
            {
                this->result = val;
                return val;
            }
        }
        // std::cout << "\n->r[" << left->stringRepresentation() << "_" << this->getName() << "_" << right->stringRepresentation() << "]";
        return this;
    }

    // TokenTree *executeRecursive()
    // {
    //     std::cout << "\n->r{" << left->stringRepresentation() << "_" << this->getName() << "_" << right->stringRepresentation() << "}";
    //     fflush(stdout);
    //     auto l = left;
    //     auto r = right;
    //     if (l != nullptr && r != nullptr && !checkIfUnknown(l) && !checkIfUnknown(r))
    //     {
    //         if (!(this->opType & (OperatorInfo)OperatorType::CANNOT_SOLVE_VARIABLE))
    //         {
    //             l = solveVariablePlaceHolder(l);
    //             r = solveVariablePlaceHolder(r);

    //             // Now also solve for future literals and other fancy stuffs
    //             l = l->execute();
    //             r = r->execute();
    //         }
    //         if (r->getType() == TokenTreeType::OPERATOR)
    //         {
    //             r = ((OperatorTreeNode *)r)->executeRecursive();
    //         }
    //         if (l->getType() == TokenTreeType::OPERATOR)
    //         {
    //             l = ((OperatorTreeNode *)l)->executeRecursive();
    //         }
    //         auto val = logic(l, r);
    //         if (val != nullptr)
    //         {
    //             std::cout << " [solved]=>" << val->stringRepresentation();
    //             this->result = val;
    //             return val;
    //         }
    //     }
    //     return this;
    // }

    std::string stringRepresentation()
    {
        if (left != nullptr && right != nullptr)
            return "{{" + left->stringRepresentation() + " " + this->getName() + " " + right->stringRepresentation() + "}}";
        return this->getName();
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

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }
};

void init_operatorTypeTable();
} // namespace turtle