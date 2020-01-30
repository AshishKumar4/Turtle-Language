#pragma once 
#include "library/common.hpp"
#include "tokens.hpp"

#include "parser/parser.hpp"
#include "parser/literal.hpp"
#include "parser/bracket.hpp"
#include "parser/operator.hpp"
#include "parser/quotestr.hpp"

#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <exception>

namespace turtle 
{
std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t &context);
TokenTree* genAST(std::vector<TokenTree*> nodes, variableContext_t &context);
TokenTree *simpleASTmaker(std::vector<TokenTree *> nodes, variableContext_t &context);
}