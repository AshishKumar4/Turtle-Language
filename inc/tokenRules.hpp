#pragma once

#include "iostream"
#include "string"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include <map>

namespace turtle
{

typedef uint32_t DerivativeType;

/*
    Here we define the very basic rules of the grammer of our language
*/

enum class TokenType
{
    UNKNOWN,
    LITERAL,
    NUMBER,
    OPERATOR,
    BRACKET,
    SINGLE_QUOTES,
    DOUBLE_QUOTES,
    INFER,
    IGNORE,
    END_OF_STATEMENT,
    SEPERATOR_OPERATOR,
    count
};

#define DERV_TYPE_WHITESPACE      1 << 1
#define DERV_TYPE_LETTER          1 << 2
#define DERV_TYPE_NUMBER          1 << 3
#define DERV_TYPE_OPERATOR        1 << 4
#define DERV_TYPE_BRACKET         1 << 5
#define DERV_TYPE_SINGLEQUOTE     1 << 6
#define DERV_TYPE_DOUBLEQUOTE     1 << 7
// #define DERV_TYPE_SEPERATOR_OPERATOR    1<<8
#define DERV_TYPE_INFER           1 << 8

#define TOKEN_RULE_DECISION_MAKE        1 << 1
#define TOKEN_RULE_DECISION_BREAK       1 << 2
#define TOKEN_RULE_DECISION_CONTINUE    1 << 3
#define TOKEN_RULE_DECISION_IGNORE      1 << 4

#define TOKEN_RULE_DECISION_BREAK_NOW       0
#define TOKEN_RULE_DECISION_BREAK_LATER     1 << 0

class TokenRules
{
public:
    const TokenType type;
    DerivativeType    startAt;
    int         occuranceLength;    // 1, 2, 3... -1 for infinite
    DerivativeType    makeAfter;
    DerivativeType    breakAt;
    DerivativeType    continueAfter;

    uint32_t breakTime;

    TokenRules(TokenType type, DerivativeType startAt, int occLength, DerivativeType makeAfter, DerivativeType breakAt, DerivativeType continueAfter, uint32_t breakTime=TOKEN_RULE_DECISION_BREAK_NOW) : 
    type(type), startAt(startAt), occuranceLength(occLength), makeAfter(makeAfter), breakAt(breakAt), continueAfter(continueAfter), breakTime(breakTime)
    {
        
    }

    TokenType getType()
    {
        return type;
    }

    DerivativeType decide(DerivativeType lastCharType, DerivativeType newCharType, int win_length, bool verified = true);
};

void init_derivRules();
TokenRules* getTokenRule(TokenType type);
TokenRules* getTokenRule(DerivativeType type);
DerivativeType derivativeClassifier(char derivative);

}