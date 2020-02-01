#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"

#include "library/trie.hpp"
#include "library/common.hpp"

#include <exception>
#include <map>
#include <list>

#include "tokenTree/tokenTree.hpp"
// #include "tokenTree/operator.hpp"
// #include "parser/bracket.hpp"
// #include "parser/quotestr.hpp"

#include "ast/ast.hpp"

#include "grabs.hpp"

namespace turtle
{

std::map<TokenType, std::function<Token *(char *, int)>> TOKEN_CREATOR_TABLE;

Token *getOperatorToken(char *op, int len)
{
    return new Token(std::string(op, op + len), TokenType::OPERATOR);
}

Token *getLiteralToken(char *op, int len)
{
    return new Token(std::string(op, op + len), TokenType::LITERAL);
}

Token *getNumericalToken(char *op, int len)
{
    return new Token(std::string(op, op + len), TokenType::NUMBER);
}

Token *getBracketToken(char *op, int len)
{
    return new Token(std::string(op, op + len), TokenType::BRACKET);
}

Token *getSingleQuoteToken(char *op, int len)
{
    return new Token(std::string(op, op + len), TokenType::SINGLE_QUOTES);
}

Token *getDoubleQuoteToken(char *op, int len)
{
    return new Token(std::string(op, op + len), TokenType::DOUBLE_QUOTES);
}

Token *getToken(TokenType type, char *op, int len)
{
    return TOKEN_CREATOR_TABLE[type](op, len);
}

void init_tokenCreatorTable()
{
    TOKEN_CREATOR_TABLE[TokenType::LITERAL] = getLiteralToken;
    TOKEN_CREATOR_TABLE[TokenType::NUMBER] = getNumericalToken;
    TOKEN_CREATOR_TABLE[TokenType::OPERATOR] = getOperatorToken;
    TOKEN_CREATOR_TABLE[TokenType::BRACKET] = getBracketToken;
    TOKEN_CREATOR_TABLE[TokenType::SINGLE_QUOTES] = getSingleQuoteToken;
    TOKEN_CREATOR_TABLE[TokenType::DOUBLE_QUOTES] = getDoubleQuoteToken;
    // TOKEN_CREATOR_TABLE[TokenType::INFER] = getInferToken;
}

std::vector<Token *> genTokenList(std::string str)
{
    std::vector<Token *> toks;
    str += " ";
    char *cstr = (char *)str.c_str();
    int hold_index = 0;
    int win_length = 0;
    TokenRules *currRule = NULL;
    DerivativeType lastCharType = DERV_TYPE_WHITESPACE;

    for (int curr_index = 0; curr_index < str.length(); curr_index++)
    {
        auto charType = derivativeClassifier(cstr[curr_index]);
        if (currRule != NULL) // No token is defined
        {
            DerivativeType decision = currRule->decide(lastCharType, charType, win_length);
            switch (decision)
            {
            case (TOKEN_RULE_DECISION_BREAK | TOKEN_RULE_DECISION_BREAK_LATER):
                // We would break it for sure, but in the next iteration!
                // We emulate the next iteration!
                ++curr_index;
                lastCharType = charType;
                ++win_length;
                charType = derivativeClassifier(cstr[curr_index]);
            case TOKEN_RULE_DECISION_BREAK:
                toks.push_back(getToken(currRule->getType(), &cstr[hold_index], win_length));
                currRule = NULL;
                win_length = 0;
                hold_index = curr_index;
                break;
            case TOKEN_RULE_DECISION_CONTINUE:
                ++win_length;
                break;
            case TOKEN_RULE_DECISION_IGNORE:
            case TOKEN_RULE_DECISION_MAKE:
            default:
                std::cerr << decision << "->" << cstr[curr_index] << " ";
                errorHandler("\t\tYou Cannot put " + std::string(&cstr[curr_index], &cstr[curr_index] + 1) + " after " + std::string(&cstr[curr_index] - 1, &cstr[curr_index]) + " !");
            }
        }
        if (currRule == NULL)
        {
            // Either this is the first iteration or there is no token
            // in making. Decide if a token needs to be made.
            if (charType != DERV_TYPE_WHITESPACE)
            {
                // We decide which Token type based on the first character itself, thats the grammer
                // Get the token rules for this char type
                currRule = getTokenRule(charType);
                // Decide if we need to make new token or not, just verification
                if (currRule->decide(lastCharType, charType, win_length, false) & TOKEN_RULE_DECISION_MAKE)
                {
                    // We can safely create new token
                    ++win_length;
                    hold_index = curr_index;
                    // Anything else? don't think so
                }
                else
                {
                    errorHandler("\t\tYou Cannot put " + std::string(&cstr[curr_index], &cstr[curr_index] + 1) + " after " + std::string(&cstr[curr_index] - 1, &cstr[curr_index]) + " !");
                }
            }
        }
        lastCharType = charType;
    }
    return toks;
}


Context_t GLOBAL_VARIABLE_TABLE;
variableContext_t GLOBAL_CONTEXT({&GLOBAL_VARIABLE_TABLE});

void init_parser()
{
    printf("\nInitializing Parser...\n");
    init_derivRules();
    init_tokenCreatorTable();
    init_tokenTrees();
}

} // namespace turtle
