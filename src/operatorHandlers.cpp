#include "operatorHandlers.hpp"
#include "parser/operator.hpp"
#include "tokens.hpp"
#include "DataTypes.hpp"

#include "library/common.hpp"

namespace turtle
{
TokenTree *operator_unaryTemplate(TokenTree *node)
{
    errorHandler(NotImplementedError("Unary Operator logic!"));
    return nullptr;
}

TokenTree *operator_binaryTemplate(TokenTree *left, TokenTree *right)
{
    errorHandler(NotImplementedError("Binary Operator logic!"));
    return nullptr;
}

bool isVariableUnknown(VariableTreeNode *left, VariableTreeNode *right)
{
    if (left->getStoreType() == TokenTreeType::UNKNOWN)
    {
        errorHandler(SyntacticError("object " + left->getName() + " Not Defined!"));
        return true;
    }
    if (right->getStoreType() == TokenTreeType::UNKNOWN)
    {
        errorHandler(SyntacticError("object " + left->getName() + " Not Defined!"));
        return true;
    }
    return false;
}

TokenTree *operator_binaryAddition(TokenTree *left, TokenTree *right)
{
    left = solveVariablePlaceHolder(left);
    right = solveVariablePlaceHolder(right);
    if (left->getType() == right->getType())
    {
        switch (left->getType())
        {
        case TokenTreeType::CONSTANT:
        {
            // Only constants can come over here as Variables got solved in the parser stage,
            // and Unknowns aren't allowed to get solved here.
            // Get MemoryWrapper objects
            auto leftobj = ((ConstantTreeNode *)left)->getObject();
            auto rightobj = ((ConstantTreeNode *)right)->getObject();
            auto result = ((*leftobj) + (*rightobj));
            return new ConstantTreeNode(result, result->getResultString());
            break;
        }
        case TokenTreeType::VARIABLE:
            std::cout << "We are here!";
            return nullptr;
        default:
            errorHandler(NotImplementedError("Operation for tokens {" + left->getName() + " " + right->getName() + "}!"));
        }
    }
    else
    {
        errorHandler(SyntacticError("Operator Type Incompatibility!"));
        return nullptr;
    }
    return nullptr;
}

TokenTree *operator_binarySubtraction(TokenTree *left, TokenTree *right)
{
    left = solveVariablePlaceHolder(left);
    right = solveVariablePlaceHolder(right);
    if (left->getType() == right->getType())
    {
        switch (left->getType())
        {
        case TokenTreeType::CONSTANT:
        {
            auto leftobj = ((ConstantTreeNode *)left)->getObject();
            auto rightobj = ((ConstantTreeNode *)right)->getObject();
            auto result = (*leftobj) - ((*rightobj));
            return new ConstantTreeNode(result, result->getResultString());
            break;
        }
        case TokenTreeType::VARIABLE:
            return nullptr;
        default:
            errorHandler(NotImplementedError("Operation for tokens {" + left->getName() + " " + right->getName() + "}!"));
        }
    }
    else
    {
        return nullptr;
    }
    return nullptr;
}

TokenTree *operator_binaryMultiplication(TokenTree *left, TokenTree *right)
{
    left = solveVariablePlaceHolder(left);
    right = solveVariablePlaceHolder(right);
    if (left->getType() == right->getType())
    {
        switch (left->getType())
        {
        case TokenTreeType::CONSTANT:
        {
            auto leftobj = ((ConstantTreeNode *)left)->getObject();
            auto rightobj = ((ConstantTreeNode *)right)->getObject();
            auto result = ((*leftobj) * (*rightobj));
            return new ConstantTreeNode(result, result->getResultString());
            break;
        }
        case TokenTreeType::VARIABLE:
            return nullptr;
        default:
            errorHandler(NotImplementedError("Operation for tokens {" + left->getName() + " " + right->getName() + "}!"));
        }
    }
    else
    {
        return nullptr;
    }
    return nullptr;
}

TokenTree *operator_binaryDivision(TokenTree *left, TokenTree *right)
{
    left = solveVariablePlaceHolder(left);
    right = solveVariablePlaceHolder(right);
    if (left->getType() == right->getType())
    {
        switch (left->getType())
        {
        case TokenTreeType::CONSTANT:
        {
            auto leftobj = ((ConstantTreeNode *)left)->getObject();
            auto rightobj = ((ConstantTreeNode *)right)->getObject();
            auto result = (*leftobj) / ((*rightobj));
            return new ConstantTreeNode(result, result->getResultString());
            break;
        }
        case TokenTreeType::VARIABLE:
            return nullptr;
        default:
            errorHandler(NotImplementedError("Operation for tokens {" + left->getName() + " " + right->getName() + "}!"));
        }
    }
    else
    {
        return nullptr;
    }
    return nullptr;
}

TokenTree *operator_binaryEqual(TokenTree *left, TokenTree *right)
{
    if (left->getType() == TokenTreeType::VARIABLE)
    {
        if (right->getType() == TokenTreeType::VARIABLE)
        {
            if (((VariableTreeNode *)right)->getStoreType() == TokenTreeType::UNKNOWN)
                right = nullptr;
            else
                right = ((VariableTreeNode *)right)->getValue();
        }
        ((VariableTreeNode *)left)->setValue(right);
        return left;
    }
    else
    {
        errorHandler(NotImplementedError("Equivalence for non variable types"));
    }
    return nullptr;
}

// TokenTree *operator_binarySubtraction(TokenTree *left, TokenTree *right)
// {
//     if (left->getType() == right->getType())
//     {
//         switch (left->getType())
//         {
//         case TokenTreeType::PLACEHOLDER:
//         {
//             MemoryWrapper *leftobj = ((ConstantTreeNode *)left)->getObject();
//             MemoryWrapper *rightobj = ((ConstantTreeNode *)right)->getObject();

//             auto result = ((*leftobj) - (*rightobj));
//             return new ConstantTreeNode(result, result->getResultString());
//         }
//         default:
//             errorHandler(NotImplementedError("Operation for tokens {" + left->getName() + " " + right->getName() + "}!"));
//         }
//     }
//     else
//     {
//         return nullptr;
//     }
//     return nullptr;
// }
} // namespace turtle