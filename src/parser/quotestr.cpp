#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"

#include "library/trie.hpp"
#include "library/common.hpp"

#include <exception>
#include <map>

#include "parser/quotestr.hpp"

namespace turtle 
{

TokenDigesterReturn_t tokenDigester_quotes(Token **list, int index, int size, variableContext_t &context)
{
    auto tok = list[index];
    std::cout<<"str["<<tok->data<<"]";
    StrConst* str = new StrConst(tok->data);
    ConstantTreeNode * node = new ConstantTreeNode(str, tok->data);
    return TokenDigesterReturn_t(node, 1);
}
}