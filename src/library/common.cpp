#include "exception"
#include "iostream"
#include "vector"
#include "string"

#include "library/common.hpp"
#include "tokens.hpp"
#include "tokenTree/tokenTree.hpp"

void errorHandler(std::string errorMsg)
{
    std::cerr << errorMsg << std::endl;
    throw errorMsg;
}

void errorHandler(std::exception exp)
{
    std::cerr << exp.what()<<std::endl;
    fflush(stderr);
    fflush(stdout);
    throw exp;
}

void errorHandler(turtle::TurtleError exp)
{
    std::cerr << std::endl << exp.what()<<std::endl;
    fflush(stderr);
    fflush(stdout);
    throw exp;
}

namespace turtle
{
bool checkIfUnknown(TokenTree* tok)
{
    // if((tok->getType() == TokenTreeType::VARIABLE and 
    //     ((VariableTreeNode*)tok)->getStoreType() == TokenTreeType::UNKNOWN) or
    if(tok->getType() == TokenTreeType::UNKNOWN)
    {
        return true;
    }
    return false;
}   

bool checkIfUnknownVar(TokenTree* tok)
{
    if(tok->getType() == TokenTreeType::VARIABLE and ((VariableTreeNode*)tok)->getStoreType() == TokenTreeType::UNKNOWN)
    {
        return true;
    }
    return false;
}
}