#pragma once
#include "stdint.h"
#include "stdlib.h"
#include "string"
#include "iostream"
#include "exception"
#include "functional"
#include <map>

#include "DataTypes.hpp"
#include "tokenRules.hpp"
#include "library/common.hpp"

namespace turtle
{
class Token
{
protected:
public:
    std::string data;
    TokenType type;

    Token(std::string data, TokenType type) : data(data), type(type)
    {
    }
};

typedef std::function<Token *(Token *, Token *)> Operator;

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

    virtual TokenTree *execute()
    {
        // Execution logic
        return this;
    }

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

    virtual std::string stringRepresentation()
    {
        std::string val = "(";
        for(auto i:elements)
        {
            val += i->stringRepresentation() + ",";
        }
        return val + ")";
    }
};

// std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes);

class TupleTreeNode : public TreeTuple<TokenTree>
{
protected:
    // A TupleTreeNode is said to be solvable if it only
    // consists of a single Root ABST
    bool is_dynamic;
    // bool is_deterministic;
public:
    friend std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes);

    TupleTreeNode(std::vector<TokenTree *> nodes, bool sanitize = true);

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
        if(elements.size() == 1) return true;
        return false;
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
};

class ListTreeNode : public TokenTree
{
    std::vector<TokenTree *> nodes;

public:
    ListTreeNode(std::vector<TokenTree *> nodes) : TokenTree(TokenTreeType::LIST, TokenTreeUseType::DYNAMIC, "list" + std::to_string(nodes.size())), nodes(nodes)
    {
        errorHandler("Lists Not Implemented!");
    }
};

class ConstantTreeNode : public TokenTree
{
    MemoryWrapper *obj;
public:
    // These hold Raw Scalar Data!
    ConstantTreeNode(MemoryWrapper *obj, std::string name = "tmp") : 
    TokenTree(TokenTreeType::CONSTANT, TokenTreeUseType::STATIC, "const_"+name), obj(obj)
    {
    }

    ConstantTreeNode(MemoryWrapper &obj, std::string name = "tmp") : 
    TokenTree(TokenTreeType::CONSTANT, TokenTreeUseType::STATIC, "const_"+name), obj(&obj)
    {
    }

    auto getObject()
    {
        return obj;
    }

    virtual std::string stringRepresentation()
    {
        return obj->getResultString();
    }
};

class VariableTreeNode : public TokenTree
{
    TokenTree* holder;
    // TokenTreeType storeType;
public:
    // These hold data of other nodes
    VariableTreeNode(TokenTree* value, std::string name) : 
    TokenTree(TokenTreeType::VARIABLE, TokenTreeUseType::DYNAMIC, name), holder(value)
    {
    }

    TokenTree* getValue()
    {
        if(holder == nullptr)
        {
            return this;
        }
        return holder;
    }
    
    auto getStoreType()
    {
        if(holder == nullptr)
        {
            return TokenTreeType::UNKNOWN;
        }
        return holder->getType();
    }

    void setValue(TokenTree* val)
    {
        holder = val;
    }

    virtual std::string stringRepresentation()
    {
        if(holder == nullptr)
        {
            return "undefined";
        }
        return holder->stringRepresentation();
    }
};

// class UnknownPlaceholderTreeNode : public VariableTreeNode
// {
// public:
//     UnknownPlaceholderTreeNode(std::string name) : 
//     VariableTreeNode(nullptr, name)
//     {
//     }
// };


typedef std::map<std::string, VariableTreeNode *> Context_t;
typedef std::vector<Context_t*> variableContext_t;
extern Context_t GLOBAL_VARIABLE_TABLE;
extern variableContext_t GLOBAL_CONTEXT;

class CodeBlock : public TokenTree
{
protected:
    std::vector<TokenTree *> statements;
    variableContext_t context;

public:
    CodeBlock(std::vector<TokenTree *> &statements, variableContext_t &context, TokenTreeUseType useType = TokenTreeUseType::DYNAMIC, std::string name="_tmp_") : TokenTree(TokenTreeType::CODEBLOCK, TokenTreeUseType::DYNAMIC, "block"+name), context(context), statements(statements)
    {
    }

    virtual std::string stringRepresentation()
    {
        std::string val = "{";
        for(auto i:statements)
        {
            val += i->stringRepresentation() + ",";
        }
        return val + "}";
    }
};

typedef std::tuple<TokenTree *, int> TokenDigesterReturn_t;

typedef std::function<TokenDigesterReturn_t(Token **, int, int, variableContext_t&)> tokenDigester_t;
// typedef std::function<TokenDigesterReturn_t(TokenTree **, int, int, variableContext_t)> tokenTreeDigester_t;

extern std::map<TokenType, tokenDigester_t> TOKEN_DIGESTERS;
// extern std::map<TokenTreeType, tokenTreeDigester_t> TOKENTREE_DIGESTERS;

#include "library/trie.hpp"

extern TrieNode<MemoryWrapper> *GLOBAL_TRIE_STRUCTURE;

std::vector<TokenTree *> genTreeNodeList(std::vector<Token *> list, variableContext_t &context);
std::vector<TokenTree *> genTokenStack(std::string str);
// std::vector<TokenTree *> genTreeNodeList(std::vector<Token *> list);
void init_parser();

bool checkIfUnknown(TokenTree* tok);
TokenTree *solveVariablePlaceHolder(TokenTree *node);

} // namespace turtle