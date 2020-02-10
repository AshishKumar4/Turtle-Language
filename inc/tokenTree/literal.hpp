#pragma once

#include "tokens.hpp"
#include "tokenRules.hpp"

#include "tokenTree/tokenTree.hpp"
#include "tokens.hpp"

namespace turtle
{

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
    LiteralConstraint(TokenTreeType type) : type(type)
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
    TupleLiteralConstraint(int size) : LiteralConstraint(TokenTreeType::TUPLE), size(size)
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
    NameLiteralConstraint(std::string name, TokenTreeType type = TokenTreeType::TEMP_LITERAL) : LiteralConstraint(type), name(name)
    {
    }

    auto getName()
    {
        return name;
    }
};

#define REPEAT_ZERO_OR_MANY 0
#define REPEAT_ONE_OR_MANY -1
#define REPEAT_ZERO_OR_ONE -2

class LiteralRule
{
    int repeats; // -1 = 1 or more, 0 = 0 or more, 1 = 1
public:
    std::vector<LiteralConstraint *> constraints;
    LiteralRule(std::vector<LiteralConstraint *> constraints, int repeats) : constraints(constraints), repeats(repeats)
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

    LiteralRulesConstruct(std::string name, TokenTreeType type, std::vector<LiteralRule> rules) :                                      //, std::function<TokenDigesterReturn_t(Token**, int, int, std::vector<LiteralRules>)> tokenTreeBuilder) :
                                                                                                  name(name), type(type), rules(rules) //, tokenTreeBuilder(tokenTreeBuilder)
    {
    }

    TokenTreeType getType() { return type; }

    TokenDigesterReturn_t tokenTreeBuilder(TokenTree **list, int index, int size, variableContext_t context, bool tree_expantion = true);
};

class TempLiteralTreeNode : public TokenTree
{
    static Grabs<TempLiteralTreeNode> grabsToken;

protected:
    LiteralRulesConstruct *rule;

public:
    TempLiteralTreeNode(std::string name, LiteralRulesConstruct *rule, TokenTreeType type = TokenTreeType::TEMP_LITERAL) : TokenTree(type, TokenTreeUseType::DYNAMIC, "key_" + name), rule(rule)
    {
        // std::cout<<"keyword["<<name<<"]";
    }

    void *operator new(std::size_t size)
    {
        TempLiteralTreeNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        std::cout << "Custom delete!";
        grabsToken.giveBack((TempLiteralTreeNode *)ptr);
    }

    auto getRule()
    {
        return rule;
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        // Execution logic
        return this;
    }
};

class TempLiteralWrapperNode : public TokenTree
{
    static Grabs<TempLiteralWrapperNode> grabsToken;

protected:
    LiteralRulesConstruct *rule;
    TokenTree *orig;

public:
    TempLiteralWrapperNode(TokenTree *orig, LiteralRulesConstruct *rule, TokenTreeType type = TokenTreeType::TEMP_LITERAL_WRAPPER) : TokenTree(type, TokenTreeUseType::DYNAMIC, "tmp_" + orig->getName()), rule(rule), orig(orig)
    {
        // std::cout<<"keyword["<<name<<"]";
    }

    void *operator new(std::size_t size)
    {
        TempLiteralWrapperNode *tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void *ptr)
    {
        std::cout << "Custom delete!";
        grabsToken.giveBack((TempLiteralWrapperNode *)ptr);
    }

    auto getRule()
    {
        return rule;
    }

    auto getOriginal()
    {
        return orig;
    }

    virtual TokenTree *execute(variableContext_t context)
    {
        // Execution logic
        return this;
    }
};

typedef std::tuple<TokenTreeType, std::vector<TokenTree *>, std::vector<LiteralRule>> futureData_t;

TokenTree *genTokenTreeNodeFromList(TokenTreeType type, std::vector<TokenTree *> &list, std::vector<LiteralRule> &rules, variableContext_t context);

class FutureSolutionTreeNode : public TokenTree
{
    futureData_t data;

public:
    FutureSolutionTreeNode(TokenTreeType type, std::vector<TokenTree *> list, std::vector<LiteralRule> rules) : TokenTree(TokenTreeType::FUTURE_LITERAL, TokenTreeUseType::DYNAMIC, "tmp")
    {
        data = futureData_t(type, list, rules);
    }

    futureData_t getData()
    {
        return data;
    }

    int checkSatisfiability(LiteralConstraint **constraints, int size);

    TokenTree *execute(variableContext_t context)
    {
        auto [type, list, rules] = data;
        for (int i = 0; i < list.size(); i++)
        {
            // if (list[i]->getType() == TokenTreeType::VARIABLE)
            //     list[i] = contextSolver((VariableTreeNode *)list[i], context, false); //list[i]->execute(context);
            if (list[i]->getType() == TokenTreeType::FUTURE_LITERAL)
            {
                list[i] = list[i]->execute(context);
            }
        }
        return genTokenTreeNodeFromList(type, list, rules, context)->execute(context);
    }
};

extern std::map<std::string, LiteralRulesConstruct *> GLOBAL_LITERAL_TABLE;

void init_globalLiteralTable();
} // namespace turtle