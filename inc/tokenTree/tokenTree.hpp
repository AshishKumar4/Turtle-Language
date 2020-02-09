#pragma once
#include "stdint.h"
#include "stdlib.h"
#include "string"
#include "iostream"
#include "exception"
#include "functional"
#include <map>
#include <any>

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
    LAMBDA,
    BREAK,
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

class VariableTreeNode;

typedef std::map<std::string, VariableTreeNode *> Context_t;
typedef std::vector<Context_t *> variableContext_t;
extern Context_t GLOBAL_VARIABLE_TABLE;
extern variableContext_t GLOBAL_CONTEXT;

VariableTreeNode *contextSolver(VariableTreeNode *tok, variableContext_t context, bool strict);

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

    virtual TokenTree *execute(variableContext_t context) = 0;

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

// TokenTree *solveVariablePlaceHolder(TokenTree *node);

class VariableTreeNode : public TokenTree
{
    TokenTree *holder;
    // TokenTreeType storeType;
    static Grabs<VariableTreeNode> grabsToken;

public:
    VariableTreeNode() : TokenTree(TokenTreeType::VARIABLE, TokenTreeUseType::DYNAMIC, "tmpvar")
    {
    }
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

        grabsToken.giveBack((VariableTreeNode *)ptr);
    }

    // TokenTree *getValue()
    // {
    //     if (holder == nullptr)
    //     {
    //         return this;
    //     }
    //     return holder->execute();
    // }

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

    TokenTree *execute(variableContext_t context)
    {
        fflush(stdout);
        // Execution logic

        if (holder == nullptr)
        {
            return this;
        }
        return holder->execute(context);
    }

    VariableTreeNode *operator=(TokenTree *obj)
    {
        this->setValue(obj);
        return this;
    }

    // VariableTreeNode* operator +(VariableTreeNode* obj)
    // {
    //     if(obj->getStoreType() != )
    // }
};

class BreakTreeNode : public TokenTree
{
public:
    BreakTreeNode() : TokenTree(TokenTreeType::BREAK, TokenTreeUseType::STATIC, "break")
    {
    }

    TokenTree *execute(variableContext_t context)
    {
        return this;
    }
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

    virtual TokenTree *execute(variableContext_t context)
    {
        // Execution logic
        return this;
    }
};

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

    T *get(int index)
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

    virtual TokenTree *execute(variableContext_t context)
    {
        // Execution logic
        if (elements.size() == 1)
        {
            return elements[0];
        }
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
    // friend std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t context, std::string seperator, bool symbolic_execution, bool tree_expantion);

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

    TupleTreeNode *operator=(TupleTreeNode *obj)
    {
        if (obj->getSize() != this->getSize())
        {
            errorHandler(SyntacticError("Unequal size tuple equivalance"));
            return this;
        }
        for (int i = 0; i < this->elements.size(); i++)
        {
            if (this->elements[i]->getType() == TokenTreeType::VARIABLE)
            {
                auto tok = obj->get(i);
                // if(tok->getType() == TokenTreeType::VARIABLE)
                //     tok = ((VariableTreeNode*)tok)->getValue();
                // tok = solveVariablePlaceHolder(tok);
                *((VariableTreeNode *)this->elements[i]) = tok;
            }
        }
        return this;
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        // Execution logic
        if (elements.size() == 1)
        {
            return elements[0]->execute(context);
        }
        else
        {
            // TupleTreeNode *tuple = new TupleTreeNode(this->elements);
            for (int i = 0; i < elements.size(); i++)
            {
                elements[i] = elements[i]->execute(context); //->execute(context);
            }
            // return tuple;
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

    virtual TokenTree *execute(variableContext_t context)
    {
        // Execution logic
        return this;
    }
};

class MemHolderTreeNode : public TokenTree 
{
public:
    MemHolderTreeNode(TokenTreeType type, TokenTreeUseType useType, std::string name) :
    TokenTree(type, useType, name)
    {

    }

    // virtual MemHolderTreeNode* operator + (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator - (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator / (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator * (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator % (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator < (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator > (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator == (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator >= (MemHolderTreeNode* obj) = 0;
    // virtual MemHolderTreeNode* operator <= (MemHolderTreeNode* obj) = 0;

    virtual bool booleanValue() = 0;

    virtual turtleObject* getRawObject() = 0;
    virtual std::any getObject() = 0;

    virtual turtleObjectType getType()= 0;
};

// template <class T>


// std::any TYPE_RECAST_TABLE[3] = { &std::any_cast<turtleString>, &std::any_cast<turtleInt>, &std::any_cast<turtleFloat>};

template <typename memObj>
class ConstantTreeNode : public MemHolderTreeNode
{
    memObj obj;
    static inline Grabs<ConstantTreeNode<memObj>> grabsToken = Grabs<ConstantTreeNode<memObj>>("consTreeNodes");

public:

    ConstantTreeNode(memObj obj, std::string name = "tmp") : MemHolderTreeNode(TokenTreeType::CONSTANT, TokenTreeUseType::STATIC, "const_" + name), obj(obj)
    {
    }

    void *operator new(std::size_t size)
    {
        ConstantTreeNode<memObj> *tok = grabsToken.grab();
        // std::cout<<"\nConstant ADDRESS: ";
        // printf("%x", (int)tok);
        return tok;
    }

    void operator delete(void *ptr)
    {
        grabsToken.giveBack((ConstantTreeNode<memObj> *)ptr);
    }

    turtleObjectType getType()
    {
        return obj.type;
    }

    turtleObject* getRawObject()
    {
        return &obj;
    }

    std::any getObject()
    {
        return obj;
    }

    operator memObj()
    {
        return obj;
    }

    // std::type_info getType()
    // {
    //     return typeid(memObj);
    // }

    bool booleanValue()
    {
        // auto val = obj->getBooleanValue();
        auto val = bool(obj);
        if (val)
            std::cout << "\nTrue\n";
        else
            std::cout << "\nFalse\n";
        return val;
    }

    virtual std::string stringRepresentation()
    {
        return (std::string)(obj);//->getResultString();
    }

    TokenTree *execute(variableContext_t context)
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
        // errorHandler("Lists Not Implemented!");
    }

    void set(std::vector<TokenTree *> target)
    {
        set(&target[0]);
    }

    void set(TokenTree **target)
    {
        // Length of target array SHOULD ALWAYS be equal to length of elements array
        try
        {
            for (int i = 0; i < nodes.size(); i++)
            {
                nodes[i] = target[i];
            }
        }
        catch (...)
        {
            std::cerr << "Error in setting function elements to target!";
            exit(0);
        }
    }

    std::vector<TokenTree *> get()
    {
        return nodes;
    }

    std::vector<TokenTree *> get(std::vector<TokenTree *> indexes)
    {
        std::vector <TokenTree*>    val; 
        for(auto i: indexes)
        {
            // Indexing shouldn't work if every index isn't an Int Type Constant
            if(i->getType() == TokenTreeType::CONSTANT)
            {
                if(static_cast<ConstantTreeNode<turtleInt>*>(i)->getType() == turtleObjectType::INT)
                {
                    val.push_back(nodes[(int)(turtleInt)*static_cast<ConstantTreeNode<turtleInt>*>(i)]);
                    continue;
                }
            }
            errorHandler(SymanticError("Wrong type of elements provided as indexes!"));
        }
        return val;
    }

    TokenTree *get(int index)
    {
        return nodes[index];
    }

    virtual std::string stringRepresentation()
    {
        std::string val = "[";
        for (auto i : nodes)
        {
            val += i->stringRepresentation() + ",";
        }
        return val + "]";
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        // Execution logic
        if (nodes.size() == 1)
        {
            return nodes[0]->execute(context);
        }
        else
        {
            // TupleTreeNode *tuple = new TupleTreeNode(this->elements);
            for (int i = 0; i < nodes.size(); i++)
            {
                nodes[i] = nodes[i]->execute(context); //->execute(context);
            }
            // return tuple;
        }
        return this;
    }

    auto size()
    {
        return nodes.size();
    }

    void push_back(TokenTree* node)
    {
        nodes.push_back(node);
    }

    void solve(variableContext_t context);
};

class CodeBlock : public TokenTree
{
protected:
    std::vector<TokenTree *> statements;
    // variableContext_t context;
    Context_t *localContext;

    bool is_solved;

public:
    // friend std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t context, std::string seperator, bool symbolic_execution, bool tree_expantion);

    CodeBlock(CodeBlock *block) : TokenTree(TokenTreeType::CODEBLOCK, TokenTreeUseType::DYNAMIC, block->name), statements(block->statements), is_solved(block->is_solved)
    {
        // We need to copy the context
        auto octx = *(block->localContext);
        localContext = new Context_t;
        for (auto [i, j] : octx)
        {
            (*localContext)[i] = new VariableTreeNode(*j);
        }
    }

    CodeBlock(std::vector<TokenTree *> &nodelist, TokenTreeUseType useType = TokenTreeUseType::DYNAMIC, std::string name = "_tmp_") : TokenTree(TokenTreeType::CODEBLOCK, TokenTreeUseType::DYNAMIC, "block" + name), statements(nodelist), is_solved(false)
    {
        localContext = new Context_t;
    }

    virtual std::string stringRepresentation()
    {
        std::string val = "{";
        for (auto i : statements)
        {
            val += i->stringRepresentation() + ",";
        }
        return val + "}";
    }

    TokenTree *execute(variableContext_t context);

    Context_t *getContext()
    {
        return localContext;
    }

    bool isSolved()
    {
        return is_solved;
    }

    void solve(variableContext_t context = GLOBAL_CONTEXT);

    // TokenTree* execute(variableContext_t context = GLOBAL_CONTEXT);
};

class FunctionTreeNode : public TokenTree
{
    TupleTreeNode *tmpParams;

protected:
    // A Function is a collection of several TokenTrees
    variableContext_t tmp_context;
    TupleTreeNode params;
    TupleTreeNode *results; // Is set by returning statements present in the codeblock!
    CodeBlock *block;
    static Grabs<FunctionTreeNode> grabsToken;

    bool paramsSet;

public:
    FunctionTreeNode(FunctionTreeNode *func) : TokenTree(TokenTreeType::FUNCTIONAL, TokenTreeUseType::DYNAMIC, func->name), params(func->params), paramsSet(false)
    {
        // create a new block as well as context!
        // block = new CodeBlock(func->block);
        block = func->block;
    }

    FunctionTreeNode(std::string name, TupleTreeNode *params, CodeBlock *block, variableContext_t context) : TokenTree(TokenTreeType::FUNCTIONAL, TokenTreeUseType::DYNAMIC, name), block(block), params(*params), paramsSet(false)
    {
        std::cout << "<func " << name << " " << params->getSize() << "> ";
        if (this->params.isSolved() == false)
        {
            // Now we would make a new context, and solve the tuple using it
            // Solve it in our context
            auto newContext = this->block->getContext();
            this->params.solve({newContext});
            this->block->solve(context);
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

        grabsToken.giveBack((FunctionTreeNode *)ptr);
    }

    void setParams(TupleTreeNode *paramVals, variableContext_t context);

    virtual std::string stringRepresentation()
    {
        return " " + name + "_" + params.stringRepresentation() + "_" + block->stringRepresentation();
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        // auto blockCopy = new CodeBlock(this->block);
        // this->block->solve(this->context);    // Solve it in the current context
        // Solve it in the current context
        if (paramsSet)
        {
            CodeBlock block = *(this->block);
            tmpParams->execute(context);
            this->params = tmpParams;

            auto tok = block.execute(context);
            if (tok->getType() == TokenTreeType::RETURN)
            {
                // Because Only functions solve Return Types!
                tok = ((ReturnTreeNode *)tok)->getValue()->execute(context);
            }
            else if (tok->getType() == TokenTreeType::BREAK)
            {
                errorHandler(SyntacticError("Break statements can't be used to exit Functions!"));
            }
            delete tmpParams;
            paramsSet = false;
            std::cout << "[" << tok->stringRepresentation() << "]";
            return tok;
        }
        else
            return this;
        // return this;
    }
};

class LambdaTreeNode : public FunctionTreeNode
{
protected:
    static Grabs<LambdaTreeNode> grabsToken;

public:
    LambdaTreeNode(TupleTreeNode *params, CodeBlock *block, variableContext_t context) : FunctionTreeNode("lambda", params, block, context)
    {
    }

    void *operator new(std::size_t size)
    {
        LambdaTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {

        grabsToken.giveBack((LambdaTreeNode *)ptr);
    }

    // virtual std::string stringRepresentation()
    // {
    //     return " " + name + "_" + params->stringRepresentation() + "_" + block->stringRepresentation();
    // }
};

typedef std::function<TokenTree *(TupleTreeNode *)> inbuiltFunc_t;

class InbuiltFunctionTreeNode : public TokenTree
{
protected:
    TupleTreeNode *params;
    inbuiltFunc_t func;

public:
    InbuiltFunctionTreeNode(inbuiltFunc_t func, TupleTreeNode *params, std::string name) : TokenTree(TokenTreeType::INBUILT_FUNCTION, TokenTreeUseType::STATIC, name), params(params), func(func)
    {
    }

    TokenTree *execute(variableContext_t context)
    {
        return this;
    }
};

class ConditionalTreeNode : public TokenTree
{
    // Can be used as If/Else as well as Switch()
protected:
    std::vector<TupleTreeNode *> params;
    std::vector<CodeBlock *> blocks;
    static Grabs<ConditionalTreeNode> grabsToken;

public:
    ConditionalTreeNode(std::vector<TupleTreeNode *> params, std::vector<CodeBlock *> blocks) : TokenTree(TokenTreeType::CONDITIONAL, TokenTreeUseType::DYNAMIC, "conditional"), params(params), blocks(blocks)
    {
        // if(params.size() != blocks.size())
        // {
        //     errorHandler(SyntacticError("Number of blocks don't match number of params"));
        // }
        // We don't really need this because its handled by the parser itself
    }

    void *operator new(std::size_t size)
    {
        ConditionalTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {

        grabsToken.giveBack((ConditionalTreeNode *)ptr);
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        // Solve its parameters
        for (int i = 0; i < params.size(); i++)
        {
            params[i]->solve(context);
            int flag = 1;
            for (int j = 0; j < params[i]->getSize(); j++)
            {
                auto tok = params[i]->get(j)->execute(context);
                if (tok->getType() == TokenTreeType::CONSTANT)
                {
                    if (((MemHolderTreeNode *)tok)->booleanValue() == false)
                    {
                        flag = 0;
                        break;
                    }
                }
                else
                {
                    flag = 0;
                    break;
                }
            }
            if (flag)
            {
                // printf("\nGot a block to solve!\n");
                // Execute the First 'True' param's corresponding block
                blocks[i]->solve(context);
                return blocks[i]->execute(context);
            }
            // Its true,
        }
        if (params.size() < blocks.size())
        {
            // There is an else block,
            blocks.back()->solve(context);
            return blocks.back()->execute(context);
        }
        return this;
    }

    // virtual std::string stringRepresentation()
    // {
    //     return
    // }
};

class LoopTreeNode : public TokenTree
{
protected:
    TupleTreeNode params;
    CodeBlock *block;
    static Grabs<LoopTreeNode> grabsToken;

public:
    LoopTreeNode(TupleTreeNode *params, CodeBlock *block) : TokenTree(TokenTreeType::LOOP, TokenTreeUseType::DYNAMIC, "loop"), params(*params), block(block)
    {
    }

    void *operator new(std::size_t size)
    {
        LoopTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {

        grabsToken.giveBack((LoopTreeNode *)ptr);
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        return this;
    }
};

class WhileLoopTreeNode : public LoopTreeNode
{
protected:
public:
    WhileLoopTreeNode(TupleTreeNode *params, CodeBlock *block) : LoopTreeNode(params, block)
    {
    }

    TokenTree *execute(variableContext_t context)
    {
        params.solve(context);
        block->solve(context);
        TokenTree *val = this;
        while (true)
        {
            // If all elements of the parameter tuple are Defined and True, Keep on executing
            int flag = 1;
            for (int j = 0; j < params.getSize(); j++)
            {
                auto tok = params.get(j)->execute(context);
                std::cout << "\n>>>>> " << tok->stringRepresentation() << "=>" << params.get(j)->stringRepresentation() << "\n";
                if (tok->getType() == TokenTreeType::CONSTANT)
                {
                    if (((MemHolderTreeNode *)tok)->booleanValue() == false)
                    {
                        flag = 0;
                        break;
                    }
                }
                else
                {
                    flag = 0;
                    break;
                }
            }
            if (flag)
            {
                // printf("\nGot a block to solve!\n");
                printf("\nSolving loop!");
                // Execute the First 'True' param's corresponding block
                val = block->execute(context);
                if (val->getType() == TokenTreeType::BREAK)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
        return val;
    }
};

class ForLoopTreeNode : public LoopTreeNode
{
};

typedef std::tuple<TokenTree *, int> TokenDigesterReturn_t;

typedef std::function<TokenDigesterReturn_t(Token **, int, int)> tokenDigester_t;

extern std::map<TokenType, tokenDigester_t> TOKEN_DIGESTERS;

std::vector<TokenTree *> genTreeNodeList(std::vector<Token *> list);
std::vector<TokenTree *> genTokenStack(std::string str);
bool checkIfUnknown(TokenTree *tok);
bool checkIfUnknownVar(TokenTree *tok);

void init_tokenTrees();

} // namespace turtle