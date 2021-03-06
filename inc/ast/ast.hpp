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
VariableTreeNode *contextSolver(VariableTreeNode *tok, variableContext_t context, bool strict = false);
std::vector<TokenTree *> sanitizeSequences(std::vector<TokenTree *> &nodes, variableContext_t context, std::string seperator=",", bool symbolic_execution = false, bool tree_expantion=true);
TokenTree *simpleASTmaker(std::vector<TokenTree *> nodes, variableContext_t context, bool tree_expantion = true);
TokenTree *symbolicASTexecutor(std::vector<TokenTree *> nodes, variableContext_t context);
}