#pragma once

#include "tokens.hpp"
#include "tokenRules.hpp"

namespace turtle
{

class FunctionTreeNode : public TokenTree
{
    // A Function is a collection of several TokenTrees
protected:
    TupleTreeNode params;
    TupleTreeNode* results; // Is set by returning statements present in the codeblock!
    CodeBlock block;

public:
    FunctionTreeNode(std::string name, TupleTreeNode &params, CodeBlock &block) : 
    TokenTree(TokenTreeType::FUNCTIONAL, TokenTreeUseType::DYNAMIC, name), block(block), params(params)
    {
        std::cout<<"<func "<<name<<" "<<params.getSize()<<"> ";
    }

    void setParams(TupleTreeNode* paramVals)
    {
        errorHandler(NotImplementedError("Function setParam()"));
    }

    virtual std::string stringRepresentation()
    {
        return " " + name + "_" + params.stringRepresentation() + "_" + block.stringRepresentation();
    }
};

class ConditionalTreeNode : public TokenTree
{
    // Can be used as If/Else as well as Switch()
protected:
    TupleTreeNode params;
    TreeTuple<CodeBlock> blocks;

public:
    ConditionalTreeNode(TupleTreeNode params, TreeTuple<CodeBlock> blocks) : 
    TokenTree(TokenTreeType::CONDITIONAL, TokenTreeUseType::DYNAMIC, "conditional"), params(params), blocks(blocks)
    {
    }

    // virtual std::string stringRepresentation()
    // {
    //     return 
    // }
};

/*
    * <Placeholders> *
    * <BOOLEAN> *
    * <Conditional>(tuple conditions){CODEBLOCK cond1}{CODEBLOCK cond2}{CODEBLOCK cond3}...size of conditions tuple
    * <FUNCTION call>(tuple params)
    * <INBUILT_FUNCTION>(tuple params)
    * <LOOP>(tuple solvable condition){CODEBLOCK}
    * <DEFINE><LITERAL>(tuple params){CODEBLOCK}
    * <IMPORT><LITERAL>
    * <RETURN>(tuple)
    * <PLACEHOLDER><RELATIONAL><PLACEHOLDER>
    * 
*/

class LiteralConstraint
{
    // An Abstract class
protected:
    TokenTreeType type;

public:
    LiteralConstraint(TokenTreeType type) :
    type(type)
    {

    }

    auto getType()
    {
        return type;
    }
};

class TupleLiteralConstraint : public LiteralConstraint
{
    int size;
public:
    TupleLiteralConstraint(int size) :
    LiteralConstraint(TokenTreeType::TUPLE), size(size)
    {

    }

    auto getSize()
    {
        return size;
    }
};

class NameLiteralConstraint : public LiteralConstraint
{
    std::string name;
public:
    NameLiteralConstraint(std::string name, TokenTreeType type=TokenTreeType::TEMP_LITERAL) :
    LiteralConstraint(type), name(name)
    {

    }

    auto getName()
    {
        return name;
    }
};


#define REPEAT_ZERO_OR_MANY     0
#define REPEAT_ONE_OR_MANY     -1
#define REPEAT_ZERO_OR_ONE     -2

class LiteralRule
{
    int repeats;    // -1 = 1 or more, 0 = 0 or more, 1 = 1 
public:
    std::vector<LiteralConstraint*> constraints;
    LiteralRule(std::vector<LiteralConstraint*> constraints, int repeats) :
    constraints(constraints), repeats(repeats)
    {
        
    }

    int getRepeats()
    {
        return repeats;
    }

    auto getConstraints()
    {
        return constraints;
    }
};

// template<typename ruleTuple>
class LiteralRulesConstruct
{
protected:
    TokenTreeType type;
    std::string name;
public:
    std::vector<LiteralRule> rules;
    // std::function<TokenDigesterReturn_t(Token**, int, int, std::vector<LiteralRules>)>     tokenTreeBuilder;

    LiteralRulesConstruct(std::string name, TokenTreeType type, std::vector<LiteralRule> rules) ://, std::function<TokenDigesterReturn_t(Token**, int, int, std::vector<LiteralRules>)> tokenTreeBuilder) : 
    name(name), type(type), rules(rules)//, tokenTreeBuilder(tokenTreeBuilder)
    {

    }

    TokenTreeType getType() {return type;}

    TokenDigesterReturn_t tokenTreeBuilder(TokenTree** list, int index, int size, variableContext_t &context);
};

class TempLiteralTreeNode : public TokenTree 
{
protected:
    LiteralRulesConstruct*   rule;
public:
    TempLiteralTreeNode(std::string name, LiteralRulesConstruct* rule, TokenTreeType type = TokenTreeType::TEMP_LITERAL) : 
    TokenTree(type, TokenTreeUseType::DYNAMIC, "key_"+name), rule(rule)
    {
        // std::cout<<"keyword["<<name<<"]";
    }
    
    auto getRule()
    {
        return rule;
    }
};

class TempLiteralWrapperNode : public TokenTree 
{
protected:
    LiteralRulesConstruct*   rule;
    TokenTree*  orig;
public:
    TempLiteralWrapperNode(TokenTree* orig, LiteralRulesConstruct* rule, TokenTreeType type = TokenTreeType::TEMP_LITERAL_WRAPPER) : 
    TokenTree(type, TokenTreeUseType::DYNAMIC, "tmp_"+orig->getName()), rule(rule), orig(orig)
    {
        // std::cout<<"keyword["<<name<<"]";
    }
    
    auto getRule()
    {
        return rule;
    }

    auto getOriginal()
    {
        return orig;
    }
};

extern std::map<std::string, LiteralRulesConstruct *> GLOBAL_LITERAL_TABLE;

void init_globalLiteralTable();
} // namespace turtle