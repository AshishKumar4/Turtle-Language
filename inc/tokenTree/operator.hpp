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

extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_ADDITION_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_SUBTRACTION_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_MULTIPLY_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_DIVIDE_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_MODULO_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_LESS_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_GREATER_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_EQUALS_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_LESSEQUAL_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
extern std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_GREATEREQUAL_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];

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
    // OperatorBinaryLogic logic;
    static Grabs<BinaryOperatorTreeNode> grabsToken;
protected:
    TokenTree *left;
    TokenTree *right;

public:
    BinaryOperatorTreeNode(OperatorInfo type, std::string symbol, OperatorPrecedence precedence = 0, OperatorAssociativity associativity = OperatorAssociativity::LEFT, TokenTree *left = nullptr, TokenTree *right = nullptr) : OperatorTreeNode(type, symbol, precedence, associativity), left(left), right(right)
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

    virtual MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right) 
    {
        return nullptr;
    }

    virtual TokenTree* logic(TokenTree* left, TokenTree* right)
    {
        // if(left->getType() != TokenTreeType::CONSTANT && right->getType() != TokenTreeType::CONSTANT)
        // {
        //     // errorHandler(NotImplementedError("Logic for non constant values"));
        //     nullptr;
        // }
        switch(left->getType())
        {
            case TokenTreeType::CONSTANT:
            {
                switch(right->getType())
                {
                    case TokenTreeType::CONSTANT:
                    {
                        return logic_internal((MemHolderTreeNode*)left, (MemHolderTreeNode*)right);
                    }
                    case TokenTreeType::LIST:
                    {
                        ListTreeNode* r = (ListTreeNode*)right;
                        ListTreeNode* newList = new ListTreeNode({});
                        for(int i = 0; i < r->size(); i++)
                        {
                            newList->push_back(logic_internal((MemHolderTreeNode*)left, (MemHolderTreeNode*)r->get(i)));
                        }
                        return newList;
                    }
                }
            }
            case TokenTreeType::LIST:
            {
                switch(right->getType())
                {
                    case TokenTreeType::CONSTANT:
                    {
                        ListTreeNode* l = (ListTreeNode*)left;
                        ListTreeNode* newList = new ListTreeNode({});
                        for(int i = 0; i < l->size(); i++)
                        {
                            newList->push_back(logic_internal((MemHolderTreeNode*)l->get(i), (MemHolderTreeNode*)right));
                        }
                        return newList;
                    }
                    case TokenTreeType::LIST:
                    {
                        ListTreeNode* l = (ListTreeNode*)left;
                        ListTreeNode* r = (ListTreeNode*)right;
                        if(r->size() > l->size())
                        {
                            errorHandler(SyntacticError("Size of Left side list is smaller than right"));
                            return nullptr;
                        }
                        ListTreeNode* newList = new ListTreeNode({});
                        for(int i = 0; i < r->size(); i++)
                        {
                            newList->push_back(logic_internal((MemHolderTreeNode*)l->get(i), (MemHolderTreeNode*)r->get(i)));
                        }
                        return newList;
                    }
                }
            }
        }
        return nullptr;
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        // std::cout << "\n->r{" << left->stringRepresentation() << "_" << this->getName() << "_" << right->stringRepresentation() << "}";
        fflush(stdout);
        auto l = left;
        auto r = right;

        if (l != nullptr && r != nullptr
            && !checkIfUnknown(l) && !checkIfUnknownVar(r))
        {
            r = r->execute(context);
            if (!(this->opType & (OperatorInfo)OperatorType::CANNOT_SOLVE_VARIABLE))
            {
                l = l->execute(context);
            }

            // std::cout << "\n---->r[" << l->stringRepresentation() << "_" << this->getName() << "_" << r->stringRepresentation() << "]";
            auto val = (this->logic(l, r));
            if (val != nullptr)
            {
                this->result = val;
                return this->result;
            }
        }
        // std::cout << "\n->r[" << left->stringRepresentation() << "_" << this->getName() << "_" << right->stringRepresentation() << "]";
        return this;
    }

    std::string stringRepresentation()
    {
        if (left != nullptr && right != nullptr)
            return "{{" + left->stringRepresentation() + " " + this->getName() + " " + right->stringRepresentation() + "}}";
        return this->getName();
    }
};

class BinaryAdditionOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryAdditionOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), "+", 6, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_ADDITION_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("Addition of Incompatible types"));
        }
        return func(left, right);
    }
};

class BinarySubtractionOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinarySubtractionOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), "-", 6, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_SUBTRACTION_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("Addition of Incompatible types"));
        }
        return func(left, right);
    }
};

class BinaryMultiplicationOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryMultiplicationOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), "*", 5, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_MULTIPLY_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("Multiplication of Incompatible types"));
        }
        return func(left, right);
    }
};

class BinaryDivisionOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryDivisionOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), "/", 5, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_DIVIDE_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("Division of Incompatible types"));
        }
        return func(left, right);
    }
};

class BinaryModuloOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryModuloOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC), "%", 5, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_MODULO_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("Modulo of Incompatible types"));
        }
        return func(left, right);
    }
};

class BinaryEqualOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryEqualOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::ARITHMATIC | (OperatorInfo)OperatorType::CANNOT_SOLVE_VARIABLE), "=", 16, OperatorAssociativity::RIGHT)
    {

    }

    TokenTree* logic(TokenTree* left, TokenTree* right)
    {
        switch(left->getType())
        {
            case TokenTreeType::VARIABLE:
            {
                *((VariableTreeNode *)left) = right;
                return left;
                break;
            }
            case TokenTreeType::TUPLE:
            {
                auto tup = (TupleTreeNode *)left;
                *tup = (TupleTreeNode *)right;
                return tup;
                break;
            }
        }
        errorHandler(NotImplementedError("Support for equation of such types"));
        return nullptr;
    }
};

class BinarySeperatorOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinarySeperatorOperatorTreeNode(std::string symbol, TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY ), symbol, 17, OperatorAssociativity::LEFT)
    {

    }

    TokenTree* logic(TokenTree* left, TokenTree* right)
    {
        return nullptr;
    }
};

class BinaryLessthanOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryLessthanOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), "<", 9, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_LESS_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("LessThan of Incompatible types"));
        }
        return func(left, right);
    }
};

class BinaryGreaterthanOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryGreaterthanOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), ">", 9, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_GREATER_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("GreaterThan of Incompatible types"));
        }
        return func(left, right);
    }
};

class BinaryBooleanEqualOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryBooleanEqualOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), "==", 10, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_EQUALS_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("BooleanEquals of Incompatible types"));
        }
        return func(left, right);
    }
};


class BinaryLessthanEqualOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryLessthanEqualOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), "<=", 9, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_LESSEQUAL_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("LessThanEqual of Incompatible types"));
        }
        return func(left, right);
    }
};


class BinaryGreaterthanEqualOperatorTreeNode : public BinaryOperatorTreeNode
{
public:
    BinaryGreaterthanEqualOperatorTreeNode(TokenTree *left = nullptr, TokenTree *right = nullptr) : 
    BinaryOperatorTreeNode(((OperatorInfo)OperatorType::BINARY | (OperatorInfo)OperatorType::BOOLEAN), ">=", 9, OperatorAssociativity::LEFT)
    {

    }
    
    MemHolderTreeNode* logic_internal(MemHolderTreeNode* left, MemHolderTreeNode* right)
    {
        auto ltype = (int)left->getType();
        auto rtype = (int)right->getType();
        
        auto func = TYPE_GREATEREQUAL_TABLE[ltype][rtype];
        if(func == nullptr)
        {
            errorHandler(SymanticError("GreaterThanEqual of Incompatible types"));
        }
        return func(left, right);
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