#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"

#include "library/trie.hpp"
#include "library/common.hpp"

#include "tokenTree/bracket.hpp"
#include "tokenTree/operator.hpp"
#include "tokenTree/literal.hpp"
#include "operatorHandlers.hpp"

#include "ast/ast.hpp"

#include <exception>
#include <map>

namespace turtle
{
std::map<TokenType, tokenDigester_t> TOKEN_DIGESTERS;

extern TokenDigesterReturn_t tokenDigester_literal(Token **list, int index, int size);  //, variableContext_t context);
extern TokenDigesterReturn_t tokenDigester_number(Token **list, int index, int size);   //, variableContext_t context);
extern TokenDigesterReturn_t tokenDigester_operator(Token **list, int index, int size); //, variableContext_t context);
extern TokenDigesterReturn_t tokenDigester_bracket(Token **list, int index, int size);  //, variableContext_t context);

void CodeBlock::solve(variableContext_t context)
{
    context.push_back(localContext);
    // std::cout << "\nSOLVING_BLOCK";
    this->statements = sanitizeSequences(this->statements, context, ";", false, false);
    // std::cout << "\nBLOCK_SOLVED";
    is_solved = true;
}

TokenTree *CodeBlock::execute(variableContext_t context)
{
    // Context_t tmpCtx;
    // tmpCtx = *localContext;
    variableContext_t nctx = context;
    nctx.push_back(localContext);
    // std::cout<<"\nEXECUTING CODEBLOCK SIZE "<<context.size()<<"\n";
    return symbolicASTexecutor(this->statements, nctx);
}

void TupleTreeNode::solve(variableContext_t context)
{
    this->elements = sanitizeSequences(this->elements, context, ",", true, true);
    is_solved = true;
}

void ListTreeNode::solve(variableContext_t context)
{
    this->nodes = sanitizeSequences(this->nodes, context, ",", true, true);
    // is_solved = true;
}

void FunctionTreeNode::setParams(TupleTreeNode *paramVals, variableContext_t context)
{
    // std::cout << "\nSET PARAMETERS\n";
    fflush(stdout);
    if (!paramVals->isSolved())
    {
        paramVals = new TupleTreeNode(*paramVals);
        paramVals->solve(context); // in the higher context
    }
    this->tmpParams = paramVals;
    paramsSet = true;
}

TokenDigesterReturn_t tokenDigester_unknown(Token **list, int index, int size) //, variableContext_t context)
{
    errorHandler("Recieved unknown token " + list[index]->data);
    return TokenDigesterReturn_t((TokenTree *)NULL, 1);
}

TokenDigesterReturn_t tokenDigester_quotes(Token **list, int index, int size) //, variableContext_t context)
{
    auto tok = list[index];
    std::cout << "str[" << tok->data << "]";
    // StrConst *str = new StrConst(std::string(tok->data.begin()+1, tok->data.end()-1));
    auto turtlestr = turtleString(std::string(tok->data.begin()+1, tok->data.end()-1));
    ConstantTreeNode<turtleString> *node = new ConstantTreeNode<turtleString>(turtlestr, tok->data);
    return TokenDigesterReturn_t(node, 1);
}

std::vector<TokenTree *> genTreeNodeList(std::vector<Token *> list) //, variableContext_t context)
{
    Token **tlist = &list[0];
    std::vector<TokenTree *> treelist;
    for (int i = 0; i < list.size();)
    {
        // std::cout<< "<" << tlist[i]->data << "," <<i<<">";
        fflush(stdout);
        auto [elem, poped] = TOKEN_DIGESTERS[tlist[i]->type](tlist, i, list.size());
        // for(int j = i; j < i+poped && j < list.size(); j++)
        // {
        //     delete tlist[j];
        // }
        i += poped;
        treelist.push_back(elem);
    }

    return treelist;
}

std::vector<TokenTree *> genTokenStack(std::string str)
{
    try
    {
        std::vector<Token *> list = genTokenList(str);
        auto treelist = genTreeNodeList(list);

        // Lets clear up some memory
        for(auto i : list)
        {
            delete i;
        }
        return treelist;
    }
    catch (std::exception &e)
    {
        errorHandler(ParserError("Grammer Error!, " + std::string(e.what())));
        exit(0);
    }
    return std::vector<TokenTree *>();
}

void init_tokenDigesters()
{
    TOKEN_DIGESTERS[TokenType::NUMBER] = tokenDigester_number;
    TOKEN_DIGESTERS[TokenType::UNKNOWN] = tokenDigester_unknown;
    TOKEN_DIGESTERS[TokenType::LITERAL] = tokenDigester_literal;
    TOKEN_DIGESTERS[TokenType::BRACKET] = tokenDigester_bracket;
    TOKEN_DIGESTERS[TokenType::OPERATOR] = tokenDigester_operator;
    TOKEN_DIGESTERS[TokenType::SINGLE_QUOTES] = tokenDigester_quotes;
    TOKEN_DIGESTERS[TokenType::DOUBLE_QUOTES] = tokenDigester_quotes;

    init_operatorTypeTable();
    init_typeOperations();
}

void init_tokenTrees()
{
    init_tokenDigesters();
    init_bracketTypeTable();
    init_globalLiteralTable();
}
} // namespace turtle