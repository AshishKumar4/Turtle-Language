#pragma once
#include "operatorHandlers.hpp"
#include "tokens.hpp"
#include "DataTypes.hpp"

#include "library/common.hpp"
#include "tokenTree/tokenTree.hpp"
#include "tokenTree/operator.hpp"

namespace turtle
{
TokenTree*  operator_unaryTemplate(TokenTree* node);
TokenTree*  operator_binaryTemplate(TokenTree* left, TokenTree* right);

void init_typeOperations();
}