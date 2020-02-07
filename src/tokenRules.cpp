#include "tokenRules.hpp"
#include "iostream"
#include "vector"

namespace turtle
{

TokenRules*         TOKEN_RULES_TABLE[(int)TokenType::count];
DerivativeType      derivativeMap[255]; // Ascii
std::map<DerivativeType, TokenType>           TOKEN_TYPE_TABLE;

std::string VALID_LETTERS = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_";
std::string VALID_NUMBERS = "1234567890";
std::string OPERATOR_CHARACTERS = "<>!@#$%^&*~+|:?,./;\\`-=";
std::string BRACKETS = "[]{}()";

void init_rulesTable()
{
    auto rule_literal = new TokenRules(TokenType::LITERAL, DERV_TYPE_LETTER,
                            -1, 
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_BRACKET, // | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE,
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_BRACKET | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE,
                            DERV_TYPE_LETTER | DERV_TYPE_NUMBER
                            );

    auto rule_number = new TokenRules(TokenType::NUMBER, DERV_TYPE_NUMBER,
                            -1, 
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_BRACKET, // | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE,
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_BRACKET | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE,
                            DERV_TYPE_LETTER | DERV_TYPE_NUMBER
                            );
    
    auto rule_operator = new TokenRules(TokenType::OPERATOR, DERV_TYPE_OPERATOR,
                            -1, 
                            DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_BRACKET,
                            DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_BRACKET,
                            DERV_TYPE_OPERATOR
                            );
    
    // auto rule_seperator_operator = new TokenRules(TokenType::SEPERATOR_OPERATOR, DERV_TYPE_SEPERATOR_OPERATOR,
    //                         1, 
    //                         DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_BRACKET,
    //                         DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_BRACKET,
    //                         DERV_TYPE_OPERATOR
    //                         );
        
    auto rule_bracket = new TokenRules(TokenType::BRACKET, DERV_TYPE_BRACKET,
                            1, 
                            DERV_TYPE_BRACKET | DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_OPERATOR,
                            DERV_TYPE_BRACKET | DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_OPERATOR,
                            DERV_TYPE_BRACKET
                            );

    // auto rule_quote = new TokenRules(TokenType::QUOTES, DERV_TYPE_QUOTE,
    //                         1, 
    //                         DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_OPERATOR,
    //                         DERV_TYPE_WHITESPACE | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_OPERATOR,
    //                         DERV_TYPE_QUOTE
    //                         );

    // Rule for Quoted Text (Strings)
    auto rule_single_quote = new TokenRules(TokenType::SINGLE_QUOTES, DERV_TYPE_SINGLEQUOTE,
                            -1, 
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_BRACKET,
                            DERV_TYPE_SINGLEQUOTE,
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_LETTER | DERV_TYPE_NUMBER | DERV_TYPE_BRACKET,
                            TOKEN_RULE_DECISION_BREAK_LATER
                            );

    auto rule_double_quote = new TokenRules(TokenType::DOUBLE_QUOTES, DERV_TYPE_DOUBLEQUOTE,
                            -1, 
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_BRACKET,
                            DERV_TYPE_DOUBLEQUOTE,
                            DERV_TYPE_WHITESPACE | DERV_TYPE_OPERATOR | DERV_TYPE_LETTER | DERV_TYPE_NUMBER | DERV_TYPE_BRACKET,
                            TOKEN_RULE_DECISION_BREAK_LATER
                            );

    // auto rule_infer = new TokenRules(TokenType::INFER, DERV_TYPE_INFER,
    //                         1, 
    //                         DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_QUOTE | DERV_TYPE_BRACKET,
    //                         DERV_TYPE_NUMBER | DERV_TYPE_LETTER,
    //                         DERV_TYPE_INFER
    //                         );

    auto rule_ignore = new TokenRules(TokenType::IGNORE, DERV_TYPE_WHITESPACE,
                            -1, 
                            DERV_TYPE_BRACKET | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_OPERATOR,
                            DERV_TYPE_BRACKET | DERV_TYPE_NUMBER | DERV_TYPE_LETTER | DERV_TYPE_SINGLEQUOTE | DERV_TYPE_DOUBLEQUOTE | DERV_TYPE_OPERATOR,
                            DERV_TYPE_WHITESPACE
                            );

    TOKEN_RULES_TABLE[(int)TokenType::LITERAL] = rule_literal;
    TOKEN_RULES_TABLE[(int)TokenType::NUMBER] = rule_number;
    TOKEN_RULES_TABLE[(int)TokenType::OPERATOR] = rule_operator;
    TOKEN_RULES_TABLE[(int)TokenType::BRACKET] = rule_bracket;
    TOKEN_RULES_TABLE[(int)TokenType::SINGLE_QUOTES] = rule_single_quote;
    TOKEN_RULES_TABLE[(int)TokenType::DOUBLE_QUOTES] = rule_double_quote;
    // TOKEN_RULES_TABLE[(int)TokenType::INFER] = rule_infer;
    TOKEN_RULES_TABLE[(int)TokenType::IGNORE] = rule_ignore;
    // TOKEN_RULES_TABLE[(int)TokenType::SEPERATOR_OPERATOR] = rule_seperator_operator;

    TOKEN_TYPE_TABLE[DERV_TYPE_WHITESPACE] = TokenType::IGNORE;
    TOKEN_TYPE_TABLE[DERV_TYPE_LETTER] = TokenType::LITERAL;
    TOKEN_TYPE_TABLE[DERV_TYPE_NUMBER] = TokenType::NUMBER;
    TOKEN_TYPE_TABLE[DERV_TYPE_OPERATOR] = TokenType::OPERATOR;
    TOKEN_TYPE_TABLE[DERV_TYPE_BRACKET] = TokenType::BRACKET;
    TOKEN_TYPE_TABLE[DERV_TYPE_SINGLEQUOTE] = TokenType::SINGLE_QUOTES;
    TOKEN_TYPE_TABLE[DERV_TYPE_DOUBLEQUOTE] = TokenType::DOUBLE_QUOTES;
    // TOKEN_TYPE_TABLE[DERV_TYPE_SEPERATOR_OPERATOR] = TokenType::SEPERATOR_OPERATOR;
    // TOKEN_TYPE_TABLE[DERV_TYPE_INFER] = TokenType::INFER;
}

void init_derivRules()
{
    init_rulesTable();


    for (auto i : VALID_LETTERS)
    {
        derivativeMap[(int)i] = DERV_TYPE_LETTER;
    }
    for (auto i : VALID_NUMBERS)
    {
        derivativeMap[(int)i] = DERV_TYPE_NUMBER;
    }
    for (auto i : OPERATOR_CHARACTERS)
    {
        derivativeMap[(int)i] = DERV_TYPE_OPERATOR;
    }
    for (auto i : BRACKETS)
    {
        derivativeMap[(int)i] = DERV_TYPE_BRACKET;
    }

    derivativeMap[(int)' '] = DERV_TYPE_WHITESPACE;
    derivativeMap[(int)'\t'] = DERV_TYPE_WHITESPACE;
    derivativeMap[(int)'\n'] = DERV_TYPE_WHITESPACE;
    derivativeMap[(int)'\r'] = DERV_TYPE_WHITESPACE;

    derivativeMap[(int)'\''] = DERV_TYPE_SINGLEQUOTE;
    derivativeMap[(int)'\"'] = DERV_TYPE_DOUBLEQUOTE;
}

TokenRules* getTokenRule(TokenType type)
{
    return TOKEN_RULES_TABLE[(int)type];
}

TokenRules* getTokenRule(DerivativeType type)
{
    return TOKEN_RULES_TABLE[(int)TOKEN_TYPE_TABLE[type]];
}

DerivativeType derivativeClassifier(char derivative)
{
    return derivativeMap[(int)derivative];
}

DerivativeType TokenRules::decide(DerivativeType lastCharType, DerivativeType newCharType, int win_length, bool verified)
{
    DerivativeType decision = 0;
    if(win_length == occuranceLength)
    {
        // We need to break!
        decision |= TOKEN_RULE_DECISION_BREAK;
        return decision;
    }
    if(lastCharType & DERV_TYPE_WHITESPACE && newCharType & DERV_TYPE_WHITESPACE)
    {
        // In the most optimal implementation, This block shouldn't exist!
        decision |= TOKEN_RULE_DECISION_IGNORE;
        return decision;
    }

    
    if(!verified && lastCharType & makeAfter && newCharType & startAt)
    {
        decision |= TOKEN_RULE_DECISION_MAKE;
        return decision;
    }

    if(breakAt & newCharType)
    {
        decision |= TOKEN_RULE_DECISION_BREAK;
        decision |= breakTime;
    }
    else 
    {
        decision |= TOKEN_RULE_DECISION_CONTINUE;
    }
    return decision;
}

}