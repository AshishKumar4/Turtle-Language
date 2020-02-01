#pragma once
#include "stdint.h"
#include "stdlib.h"
#include "string"
#include "iostream"
#include "exception"
#include "functional"
#include <map>

#include "DataTypes.hpp"
#include "tokenRules.hpp"
#include "library/common.hpp"

#include "grabs.hpp"

namespace turtle
{

class Token;

class Token
{
    static Grabs<Token> grabsToken;
protected:
public:
    std::string data;
    TokenType type;

    Token(std::string data, TokenType type) : data(data), type(type)
    {
    }

    void* operator new(std::size_t size)
    {
        Token* tok = grabsToken.grab();
        return tok;
    }

    void operator delete(void* ptr)
    {
        std::cout<<"Custom delete!";
        grabsToken.giveBack((Token*)ptr);
    }
};

typedef std::function<Token *(Token *, Token *)> Operator;

#include "library/trie.hpp"

extern TrieNode<MemoryWrapper> *GLOBAL_TRIE_STRUCTURE;
// std::vector<TokenTree *> genTreeNodeList(std::vector<Token *> list);
Token *getToken(TokenType type, char *op, int len);
std::vector<Token *> genTokenList(std::string str);
void init_tokenCreatorTable();
void init_parser();
}