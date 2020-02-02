#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"

#include "library/trie.hpp"
#include "library/common.hpp"

#include <exception>
#include <map>

#include "tokenTree/bracket.hpp"
#include "tokenTree/tokenTree.hpp"

namespace turtle
{

std::map<std::string, BracketInfo> TOKEN_BRACKET_TYPE_TABLE;

std::string OPENING_BRACKETS[] = {"{", "(", "["};
std::string CLOSING_BRACKETS[] = {"}", ")", "]"};

std::string ROUND_BRACKETS[] = {"(", ")"};
std::string CURLY_BRACKETS[] = {"}", "{"};
std::string SQUARE_BRACKETS[] = {"[", "]"};

void init_bracketTypeTable()
{
    for(auto i : OPENING_BRACKETS)
    {
        TOKEN_BRACKET_TYPE_TABLE[i] = BRACKET_OPENING;
    }
    for(auto i : CLOSING_BRACKETS)
    {
        TOKEN_BRACKET_TYPE_TABLE[i] = BRACKET_CLOSING;
    }
    
    for(auto i : ROUND_BRACKETS)
    {
        TOKEN_BRACKET_TYPE_TABLE[i] |= BRACKET_ROUND;
    }
    for(auto i : CURLY_BRACKETS)
    {
        TOKEN_BRACKET_TYPE_TABLE[i] |= BRACKET_CURLY;
    }
    for(auto i : SQUARE_BRACKETS)
    {
        TOKEN_BRACKET_TYPE_TABLE[i] |= BRACKET_SQUARE;
    }
}

TokenTree *packageBracketNodes(std::vector<TokenTree *> treelist, BracketInfo btype)//, variableContext_t context)
{
    switch (btype & BRACKET_INFO_FUNCTIONTYPE)
    {
    case BRACKET_ROUND:
        // These are always tuples. 
        // They may have single root ABST or multi root ABSTs
        // In case of Single root ABSTs, they are solvable and their values
        // directly passable to higher nodes, in case of arithmatic nesting
        return new TupleTreeNode(treelist);
        break;
    case BRACKET_CURLY:
        // These can EITHER be DICTs or CODEBLOCKS, thus 
        // We should just wrap these into a temporary 
        // holder and pass on for further stages to determine its type.
        return new CodeBlock(treelist);//CurlyBracketTreeNode(treelist);
        break;
    case BRACKET_SQUARE:
        // Only for lists!
        return new ListTreeNode(treelist);
        break;
    default:
        errorHandler("Bracket Type incorrectly set!");
    }
    return nullptr;
}

inline bool validateBracketEquality(BracketInfo btype, BracketInfo origType)
{
    if ((btype & origType) | BRACKET_INFO_FUNCTIONTYPE)
    {
        return true;
    }
    return false;
}

TokenDigesterReturn_t bracketSolver(Token **list, int index, int size, BracketInfo origBtype, std::function<TokenTree*(std::vector<TokenTree*>, BracketInfo)> packager=packageBracketNodes)
{
    // Recursive Solve, just find out its bracket
    // Find out the closing bracket for this bracket

    int openings = 0;
    for (int i = index; i < size; i++)
    {
        auto tok = list[i];
        // Check if its a bracket!
        if(tok->type != TokenType::BRACKET) continue;
        BracketInfo btype = TOKEN_BRACKET_TYPE_TABLE[tok->data];
        if (validateBracketEquality(btype, origBtype))
        {
            if (btype & BRACKET_OPENING)
            {
                ++openings;
            }
            else
            {
                --openings;
            }
        }
        if (openings == 0)
        {
            // We found our possible pair. If this isn't truely
            // our closing bracket for this opening bracket,
            // We would catch the error somewhere down the road for sure!
            std::vector<Token *> newlist(&list[index+1], &list[i]);

            // auto newContext = new Context_t();
            // variableContext_t ncontext(context);
            // ncontext.push_back(newContext);
            // context.push_back(newContext); // Add a new local context
            // Recursion of the system...
            std::vector<TokenTree *> treelist = genTreeNodeList(newlist);//, ncontext);
            // Now according to the type of bracket, Package the
            // Treelist into a single tree node
            return TokenDigesterReturn_t(packager(treelist, origBtype), newlist.size()+2);
        }
    }

    errorHandler("Couldn't find a closing bracket as expected!");
    return TokenDigesterReturn_t(nullptr, 1);
}

TokenDigesterReturn_t tokenDigester_bracket(Token **list, int index, int size)//, variableContext_t context)
{
    // When we Encounter any bracket type, We recursively
    // solve for internal tokens to generate pseudo-complete
    // ABSTs as TreeNodes

    auto tok = list[index];
    BracketInfo btype = TOKEN_BRACKET_TYPE_TABLE[tok->data];

    if (btype & BRACKET_CLOSING)
    {
        errorHandler("Got Closing bracket when opening expected!");
    }

    auto [node, popped] = bracketSolver(list, index, size, btype);
    return TokenDigesterReturn_t(node, popped);
}

} // namespace turtle