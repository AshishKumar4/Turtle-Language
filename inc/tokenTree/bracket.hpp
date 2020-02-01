#pragma once

#include "tokens.hpp"
#include "tokenRules.hpp"
#include "tokenTree/tokenTree.hpp"

namespace turtle
{
    
enum class BracketType
{
    OPENING = 1<<1,
    CLOSING = 1<<2,
    ROUND = 1<<3,
    CURLY = 1<<4,
    SQUARE = 1<<5
};

#define BRACKET_OPENING     (uint32_t)BracketType::OPENING
#define BRACKET_CLOSING     (uint32_t)BracketType::CLOSING

#define BRACKET_ROUND       (uint32_t)BracketType::ROUND
#define BRACKET_CURLY       (uint32_t)BracketType::CURLY
#define BRACKET_SQUARE      (uint32_t)BracketType::SQUARE

#define BRACKET_INFO_FUNCTIONTYPE   ((uint32_t)BracketType::ROUND | (uint32_t)BracketType::CURLY | (uint32_t)BracketType::SQUARE)
#define BRACKET_INFO_BEHAVTYPE      ((uint32_t)BracketType::OPENING | (uint32_t)BracketType::CLOSING)

typedef uint32_t BracketInfo;

// class CurlyBracketTreeNode : public TokenTree 
// {
// public:
//     std::vector<TokenTree* > treelist;
//     CurlyBracketTreeNode(std::vector<TokenTree* > treelist) : TokenTree(TokenTreeType::CURLY_BRACKET_BLOCK, TokenTreeUseType::DYNAMIC, "curlyblock"), treelist(treelist)
//     {
//         // std::cout<<"(curly "<<treelist.size()<<")";
//     }
// };

extern std::map<std::string, BracketInfo> TOKEN_BRACKET_TYPE_TABLE;
TokenDigesterReturn_t bracketSolver(Token **list, int index, int size, BracketInfo origBtype, std::function<TokenTree*(std::vector<TokenTree*>, BracketInfo)> packager);
void init_bracketTypeTable();
}