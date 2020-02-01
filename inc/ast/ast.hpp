#pragma once 
#include "library/common.hpp"
#include "tokens.hpp"

#include "tokenTree/literal.hpp"
#include "tokenTree/operator.hpp"
#include "tokenTree/bracket.hpp"
#include "tokenTree/tokenTree.hpp"

#include <map>
#include <iostream>
#include <algorithm>
#include <vector>
#include <exception>

namespace turtle 
{
std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t &context, std::string seperator=",", bool symbolic_execution = false, bool tree_expantion=true);
TokenTree* genAST(std::vector<TokenTree*> nodes, variableContext_t &context);
TokenTree *simpleASTmaker(std::vector<TokenTree *> nodes, variableContext_t &context, bool tree_expantion = true);
TokenTree *symbolicASTexecutor(std::vector<TokenTree *> nodes, variableContext_t &context);
}