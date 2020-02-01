#pragma once
#include "stdint.h"
#include "stdlib.h"
#include "string"
#include "iostream"
#include "exception"
#include "functional"
#include <map>

#include "tokens.hpp"

#include "DataTypes.hpp"
#include "tokenRules.hpp"
#include "library/common.hpp"

#include "grabs.hpp"

namespace turtle
{

enum class TokenTreeType
{
    UNKNOWN,
    DUMMY,
    ABST,

    SCALAR, // Integers, Strings etc.
    OBJECT,
    DICT,
    LIST,
    TUPLE,
    SET,

    CODEBLOCK,
    // PLACEHOLDER, // Of Type MemoryWrapper, Can be wrapped into tokentree
    VARIABLE,
    CONSTANT,
    // STRUCTS,    // Lists, Dicts, Sets
    FUNCTIONAL, // Of Type TokenTree
    INBUILT_FUNCTION,
    CURLY_BRACKET_BLOCK,

    CONDITIONAL, // Of Type TokenTree
    LOOP,
    DEFINE,
    RETURN,
    IMPORT,

    TEMP_LITERAL,
    TEMP_LITERAL_WRAPPER,
    FUTURE_LITERAL,

    BOOLEAN,    // true, false
    RELATIONAL, // and, or, not, xor, in
    OPERATOR,
};

enum class TokenTreeUseType
{
    STATIC, // One time use Tree
    DYNAMIC // For CodeBlocks and stuffs which can be used again and again
};

// Our Abstract Syntax Tree
class TokenTree
{
    Token *result;

    TokenTree *left;
    TokenTree *right;
    Operator operation;
    static Grabs<TokenTree> grabsToken;

protected:
    const std::string name;
    const TokenTreeType type;
    const TokenTreeUseType useType;
    bool determined;

public:
    TokenTree(TokenTreeType type, std::string name) : type(type), determined(false), useType(TokenTreeUseType::STATIC), name(name)
    {
    }

    TokenTree(TokenTreeType type, TokenTreeUseType useType, std::string name) : type(type), determined(false), useType(useType), name(name)
    {
    }

    TokenTree(Token *token) : result(token), left(NULL), right(NULL), operation(NULL), type(TokenTreeType::ABST), determined(false), useType(TokenTreeUseType::STATIC), name(token->data)
    {
    }

    TokenTree(TokenTree *left, TokenTree *right, Operator operation, std::string name) : left(left), right(right), operation(operation), type(TokenTreeType::ABST), determined(false), useType(TokenTreeUseType::STATIC), name(name)
    {
    }

    // void* operator new(std::size_t size)
    // {
    //     TokenTree* tok = grabsToken.grab();
    //     return tok;
    // }

    // void operator delete(void* ptr)
    // {
    //     std::cout<<"Custom delete!";
    //     grabsToken.giveBack((TokenTree*)ptr);
    // }

    virtual TokenTree *execute() = 0;

    virtual std::string stringRepresentation()
    {
        return name;
    }

    TokenTreeType getType()
    {
        return type;
    }

    TokenTreeUseType getUseType()
    {
        return useType;
    }

    std::string getName()
    {
        return name;
    }
};

TokenTree *solveVariablePlaceHolder(TokenTree *node);

class VariableTreeNode : public TokenTree
{
    TokenTree *holder;
    // TokenTreeType storeType;
    static Grabs<VariableTreeNode> grabsToken;

public:
    // These hold data of other nodes
    VariableTreeNode(TokenTree *value, std::string name) : TokenTree(TokenTreeType::VARIABLE, TokenTreeUseType::DYNAMIC, name), holder(value)
    {
    }

    VariableTreeNode(TokenTree *value) : TokenTree(TokenTreeType::VARIABLE, TokenTreeUseType::DYNAMIC, value->getName()), holder(value)
    {
    }

    void *operator new(std::size_t size)
    {
        VariableTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        std::cout << "Custom delete!";
        grabsToken.giveBack((VariableTreeNode *)ptr);
    }

    TokenTree *getValue()
    {
        if (holder == nullptr)
        {
            return this;
        }
        return holder;
    }

    auto getStoreType()
    {
        if (holder == nullptr)
        {
            return TokenTreeType::UNKNOWN;
        }
        return holder->getType();
    }

    void setValue(TokenTree *val)
    {
        holder = val;
    }

    virtual std::string stringRepresentation()
    {
        std::string container;
        if (holder == nullptr)
            container = "undefined";
        else 
            container = holder->stringRepresentation();
        return this->getName() + " = " + container;
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this->getValue();
    }

    VariableTreeNode* operator =(TokenTree *obj)
    {
        this->setValue(obj);
        return this;
    }

    // VariableTreeNode* operator +(VariableTreeNode* obj)
    // {
    //     if(obj->getStoreType() != )
    // }
};

class ReturnTreeNode : public TokenTree
{
    TokenTree *holder;
    // TokenTreeType storeType;
    static Grabs<ReturnTreeNode> grabsToken;

public:
    // These hold data of other nodes
    ReturnTreeNode(TokenTree *value) : TokenTree(TokenTreeType::RETURN, TokenTreeUseType::DYNAMIC, value->getName()), holder(value)
    {
    }

    void *operator new(std::size_t size)
    {
        ReturnTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        grabsToken.giveBack((ReturnTreeNode *)ptr);
    }

    TokenTree *getValue()
    {
        if (holder == nullptr)
        {
            return this;
        }
        return holder;
    }

    auto getStoreType()
    {
        if (holder == nullptr)
        {
            return TokenTreeType::UNKNOWN;
        }
        return holder->getType();
    }

    void setValue(TokenTree *val)
    {
        holder = val;
    }

    virtual std::string stringRepresentation()
    {
        std::string container;
        if (holder == nullptr)
            container = "undefined";
        else 
            container = holder->stringRepresentation();
        return this->getName() + " = " + container;
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }

    // VariableTreeNode* operator =(TokenTree *obj)
    // {
    //     this->setValue(obj);
    //     return this;
    // }
};

typedef std::map<std::string, VariableTreeNode *> Context_t;
typedef std::vector<Context_t *> variableContext_t;
extern Context_t GLOBAL_VARIABLE_TABLE;
extern variableContext_t GLOBAL_CONTEXT;

template <typename T>
class TreeTuple : public TokenTree
{
protected:
    std::vector<T *> elements;

public:
    TreeTuple() : TokenTree(TokenTreeType::TUPLE, "tuple" + std::to_string(elements.size()))
    {
    }

    TreeTuple(std::vector<T *> elements, TokenTreeUseType useType) : elements(elements), TokenTree(TokenTreeType::TUPLE, useType, "tuple" + std::to_string(elements.size()))
    {
    }

    void set(std::vector<T *> target)
    {
        set(&target[0]);
    }

    void set(T **target)
    {
        // Length of target array SHOULD ALWAYS be equal to length of elements array
        try
        {
            for (int i = 0; i < elements.size(); i++)
            {
                elements[i] = target[i];
            }
        }
        catch (...)
        {
            std::cerr << "Error in setting function elements to target!";
            exit(0);
        }
    }

    std::vector<T *> get()
    {
        return elements;
    }

    T* get(int index)
    {
        return elements[index];
    }

    virtual std::string stringRepresentation()
    {
        std::string val = "(";
        for (auto i : elements)
        {
            val += i->stringRepresentation() + ",";
        }
        return val + ")";
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }
};

// std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes);

class TupleTreeNode : public TreeTuple<TokenTree>
{
protected:
    // A TupleTreeNode is said to be solvable if it only
    // consists of a single Root ABST
    bool is_dynamic;
    bool is_solved;
    // bool is_deterministic;
public:
    // friend std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t &context, std::string seperator, bool symbolic_execution, bool tree_expantion);

    TupleTreeNode(std::vector<TokenTree *> nodes) : TreeTuple<TokenTree>(nodes, TokenTreeUseType::DYNAMIC), is_dynamic(true), is_solved(false)
    {
    }

    auto getSize()
    {
        return elements.size();
    }

    bool isDynamic()
    {
        return is_dynamic;
    }

    bool isSolvable()
    {
        if (elements.size() == 1)
            return true;
        return false;
    }

    bool isSolved()
    {
        return is_solved;
    }

    void solve(variableContext_t context);

    TupleTreeNode* operator =(TupleTreeNode* obj)
    {
        if(obj->getSize() != this->getSize())
        {
            errorHandler(SyntacticError("Unequal size tuple equivalance"));
            return this;
        }
        for(int i=0; i < this->elements.size(); i++)
        {   
            if(this->elements[i]->getType() == TokenTreeType::VARIABLE)
            {
                auto tok = obj->get(i);
                // if(tok->getType() == TokenTreeType::VARIABLE)
                //     tok = ((VariableTreeNode*)tok)->getValue();
                tok = solveVariablePlaceHolder(tok);
                *((VariableTreeNode*)this->elements[i]) = tok;
            }
        }
        return this;
    }
};

class DictTreeNode : public TokenTree
{
    std::vector<TokenTree *> nodes;

public:
    DictTreeNode(std::vector<TokenTree *> nodes) : TokenTree(TokenTreeType::DICT, TokenTreeUseType::DYNAMIC, "dict" + std::to_string(nodes.size())), nodes(nodes)
    {
        errorHandler("Dicts Not Implemented!");
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }
};

class ListTreeNode : public TokenTree
{
    std::vector<TokenTree *> nodes;

public:
    ListTreeNode(std::vector<TokenTree *> nodes) : TokenTree(TokenTreeType::LIST, TokenTreeUseType::DYNAMIC, "list" + std::to_string(nodes.size())), nodes(nodes)
    {
        errorHandler("Lists Not Implemented!");
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }
};

class ConstantTreeNode : public TokenTree
{
    MemoryWrapper *obj;
    static Grabs<ConstantTreeNode> grabsToken;

public:
    // These hold Raw Scalar Data!
    ConstantTreeNode(MemoryWrapper *obj, std::string name = "tmp") : TokenTree(TokenTreeType::CONSTANT, TokenTreeUseType::STATIC, "const_" + name), obj(obj)
    {
    }

    ConstantTreeNode(MemoryWrapper &obj, std::string name = "tmp") : TokenTree(TokenTreeType::CONSTANT, TokenTreeUseType::STATIC, "const_" + name), obj(&obj)
    {
    }

    void *operator new(std::size_t size)
    {
        ConstantTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        std::cout << "Custom delete!";
        grabsToken.giveBack((ConstantTreeNode *)ptr);
    }

    auto getObject()
    {
        return obj;
    }

    virtual std::string stringRepresentation()
    {
        return obj->getResultString();
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }
};

class CodeBlock : public TokenTree
{
protected:
    std::vector<TokenTree *> statements;
    // variableContext_t context;
    Context_t* localContext;

    bool is_solved;

public:
    // friend std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t &context, std::string seperator, bool symbolic_execution, bool tree_expantion);

    CodeBlock(std::vector<TokenTree *> &nodelist, TokenTreeUseType useType = TokenTreeUseType::DYNAMIC, std::string name = "_tmp_") : TokenTree(TokenTreeType::CODEBLOCK, TokenTreeUseType::DYNAMIC, "block" + name), statements(nodelist), is_solved(false)
    {
        localContext = new Context_t;
    }

    // CodeBlock(CodeBlock* blk)
    // {
    //     Context_t ncontext = *(blk->getContext());
    //     localContext = new Context_t(ncontext);
    //     for(auto i :blk->statements)
    //     {
    //         this->statements.push_back();
    //     }
    // }

    virtual std::string stringRepresentation()
    {
        std::string val = "{";
        for (auto i : statements)
        {
            val += i->stringRepresentation() + ",";
        }
        return val + "}";
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }

    Context_t* getContext()
    {
        return localContext;
    }

    bool isSolved()
    {
        return is_solved;
    }

    void solve(variableContext_t context = GLOBAL_CONTEXT);

    TokenTree* execute(variableContext_t context = GLOBAL_CONTEXT);

    // TokenTree* getReturnedValue()
    // {
    //     auto val = (*localContext)["$return"];
    //     TokenTree* result;
    //     if( val != nullptr )
    //     {
    //         result = val->getValue();
    //     }
    //     else 
    //     {
    //         // Pop from the last node
    //         result = statements.back();
    //     }
    //     return result;
    // }
};

class FunctionTreeNode : public TokenTree
{
    // A Function is a collection of several TokenTrees
    variableContext_t tmp_context;
protected:
    TupleTreeNode* params;
    TupleTreeNode *results; // Is set by returning statements present in the codeblock!
    CodeBlock* block;
    static Grabs<FunctionTreeNode> grabsToken;

public:

    FunctionTreeNode(std::string name, TupleTreeNode *params, CodeBlock *block, variableContext_t &context) : 
    TokenTree(TokenTreeType::FUNCTIONAL, TokenTreeUseType::DYNAMIC, name), block(block), params(params)
    {
        std::cout << "<func " << name << " " << params->getSize() << "> ";
        if (this->params->isSolved() == false)
        {
            // Now we would make a new context, and solve the tuple using it
            // Solve it in our context
            auto newContext = this->block->getContext();
            this->params->solve({newContext});
            this->block->solve(context);
            std::cout<<"FUNCTION_MADE!";
            // this->context.push_back(newContext);
        }
        else 
        {
            errorHandler(SyntacticError("Function's parameters already solved!"));
        }
    }

    void *operator new(std::size_t size)
    {
        FunctionTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        std::cout << "Custom delete!";
        grabsToken.giveBack((FunctionTreeNode *)ptr);
    }

    void setParams(TupleTreeNode *paramVals, variableContext_t context);

    virtual std::string stringRepresentation()
    {
        return " " + name + "_" + params->stringRepresentation() + "_" + block->stringRepresentation();
    }

    virtual TokenTree *execute()
    {
        // auto blockCopy = new CodeBlock(this->block);
        // this->block->solve(this->context);    // Solve it in the current context
            // Solve it in the current context
        return this->block->execute(this->tmp_context);
    }
};

class ConditionalTreeNode : public TokenTree
{
    // Can be used as If/Else as well as Switch()
protected:
    TupleTreeNode params;
    TreeTuple<CodeBlock> blocks;

public:
    ConditionalTreeNode(TupleTreeNode params, TreeTuple<CodeBlock> blocks) : TokenTree(TokenTreeType::CONDITIONAL, TokenTreeUseType::DYNAMIC, "conditional"), params(params), blocks(blocks)
    {
    }

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }

    // virtual std::string stringRepresentation()
    // {
    //     return
    // }
};

typedef std::tuple<TokenTree *, int> TokenDigesterReturn_t;

typedef std::function<TokenDigesterReturn_t(Token **, int, int)> tokenDigester_t;

extern std::map<TokenType, tokenDigester_t> TOKEN_DIGESTERS;

std::vector<TokenTree *> genTreeNodeList(std::vector<Token *> list);
std::vector<TokenTree *> genTokenStack(std::string str);
bool checkIfUnknown(TokenTree *tok);
bool checkIfUnknownVar(TokenTree* tok);

void init_tokenTrees();

} // namespace turtle