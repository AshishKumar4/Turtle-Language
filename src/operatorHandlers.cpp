#include "operatorHandlers.hpp"
#include "tokenTree/operator.hpp"
#include "tokenTree/tokenTree.hpp"
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

// TokenTree *operator_unaryAddition(TokenTree *node)
// {
//     node = solveVariablePlaceHolder(node);
//     switch (node->getType())
//     {
//     case TokenTreeType::CONSTANT:
//     {
//         // Only constants can come over here as Variables got solved in the parser stage,
//         // and Unknowns aren't allowed to get solved here.
//         // Get MemoryWrapper objects
//         auto leftobj = ((ConstantTreeNode *)node)->getObject();
//         auto result = ((*leftobj));
//         return new ConstantTreeNode(result, result->getResultString());
//         break;
//     }
//     case TokenTreeType::VARIABLE:
//     case TokenTreeType::OPERATOR:
//     case TokenTreeType::FUTURE_LITERAL:
//     default:
//         errorHandler(NotImplementedError("Operation for tokens {" + left->getName() + " " + right->getName() + "}!"));
//     }
//     return nullptr;
// }

// TokenTree *operator_unarySubtraction(TokenTree *node)
// {
//     return nullptr;
// }

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
    // left = solveVariablePlaceHolder(left);
    // right = solveVariablePlaceHolder(right);
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
        case TokenTreeType::OPERATOR:
        case TokenTreeType::FUTURE_LITERAL:
            return nullptr;
        default:
            errorHandler(NotImplementedError("Operation for tokens {" + left->getName() + " " + right->getName() + "}!"));
        }
    }
    else
    {
        // errorHandler(SyntacticError("Operator Type Incompatibility!"));
        return nullptr;
    }
    return nullptr;
}

TokenTree *operator_binarySubtraction(TokenTree *left, TokenTree *right)
{
    // left = solveVariablePlaceHolder(left);
    // right = solveVariablePlaceHolder(right);
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
        case TokenTreeType::OPERATOR:
        case TokenTreeType::FUTURE_LITERAL:
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
    // left = solveVariablePlaceHolder(left);
    // right = solveVariablePlaceHolder(right);
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
        case TokenTreeType::OPERATOR:
        case TokenTreeType::FUTURE_LITERAL:
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
    // left = solveVariablePlaceHolder(left);
    // right = solveVariablePlaceHolder(right);
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
        case TokenTreeType::OPERATOR:
        case TokenTreeType::FUTURE_LITERAL:
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
    switch (left->getType())
    {
    case TokenTreeType::VARIABLE:
    {
        // right = solveVariablePlaceHolder(right);
        if (right == nullptr)
        {
            return nullptr;
        }
        *((VariableTreeNode *)left) = right;
        return left;
    }
    case TokenTreeType::TUPLE:
    {
        // right = solveVariablePlaceHolder(right);
        if (right == nullptr)
        {
            return nullptr;
        }
        if (right->getType() == TokenTreeType::TUPLE)
        {
            auto tup = (TupleTreeNode *)left;
            *tup = (TupleTreeNode *)right;
        }
        else
        {
            errorHandler(SyntacticError("Tuple equated to Non tuple obj"));
        }
        return left;
    }
    default:
    {
        errorHandler(NotImplementedError("Equivalence for non variable types"));
    }
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