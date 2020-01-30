#pragma once
#include "operatorHandlers.hpp"
#include "parser/operator.hpp"
#include "tokens.hpp"
#include "DataTypes.hpp"

#include "library/common.hpp"

namespace turtle
{
TokenTree*  operator_unaryTemplate(TokenTree* node);
TokenTree*  operator_binaryTemplate(TokenTree* left, TokenTree* right);
TokenTree*  operator_binaryAddition(TokenTree* left, TokenTree* right);
TokenTree *operator_binarySubtraction(TokenTree *left, TokenTree *right);
TokenTree *operator_binaryMultiplication(TokenTree *left, TokenTree *right);
TokenTree *operator_binaryDivision(TokenTree *left, TokenTree *right);
TokenTree *operator_binaryEqual(TokenTree *left, TokenTree *right);
}