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

std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_ADDITION_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_SUBTRACTION_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_MULTIPLY_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_DIVIDE_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_MODULO_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_LESS_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_GREATER_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_EQUALS_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_LESSEQUAL_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_GREATEREQUAL_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];
// std::function<MemHolderTreeNode*(MemHolderTreeNode*, MemHolderTreeNode*)> TYPE_MODULO_TABLE[(int)turtleObjectType::num][(int)turtleObjectType::num];

/***********************************************************************************************************/
/***************************************** Definitions for addition ****************************************/
/***********************************************************************************************************/

MemHolderTreeNode* addStringToString(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<turtleString>( (std::string)(turtleString)(*static_cast<ConstantTreeNode<turtleString>*>(left)) + (std::string)(turtleString)(*static_cast<ConstantTreeNode<turtleString>*>(right)) );
}

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* addValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) + (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

/***********************************************************************************************************/
/**************************************** Definitions for Subtraction **************************************/
/***********************************************************************************************************/

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* subValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) - (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

/***********************************************************************************************************/
/***************************************** Definitions for Multiplication **********************************/
/***********************************************************************************************************/

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* mulValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) * (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

template<typename Char, typename Traits, typename Allocator>
std::basic_string<Char, Traits, Allocator> operator *
(const std::basic_string<Char, Traits, Allocator> s, size_t n)
{
   std::basic_string<Char, Traits, Allocator> tmp = s;
   for (size_t i = 0; i < n; ++i)
   {
      tmp += s;
   }
   return tmp;
}

template<typename Char, typename Traits, typename Allocator>
std::basic_string<Char, Traits, Allocator> operator *
(size_t n, const std::basic_string<Char, Traits, Allocator>& s)
{
   return s * n;
}

MemHolderTreeNode* mulStringToInt(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<turtleString>( (std::string)(turtleString)(*static_cast<ConstantTreeNode<turtleString>*>(left)) * (turtleInt)(turtleInt)(*static_cast<ConstantTreeNode<turtleInt>*>(right)) );
}

MemHolderTreeNode* mulIntToString(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<turtleString>((turtleInt)(turtleInt)(*static_cast<ConstantTreeNode<turtleInt>*>(left)) * (std::string)(turtleString)(*static_cast<ConstantTreeNode<turtleString>*>(right)) );
}

/***********************************************************************************************************/
/***************************************** Definitions for Division ****************************************/
/***********************************************************************************************************/

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* divValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) / (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

/***********************************************************************************************************/
/***************************************** Definitions for Modulos *****************************************/
/***********************************************************************************************************/

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* modValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) % (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

/***********************************************************************************************************/
/************************************* Definitions for Boolean Operators ***********************************/
/***********************************************************************************************************/

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* lessThanValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) < (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* greaterThanValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) > (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* lessThanEqualValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) <= (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* greaterThanEqualValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) >= (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

template <class targetType, class leftType, class rightType>
MemHolderTreeNode* equalsValToVal(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<targetType>( (targetType)(leftType)(*static_cast<ConstantTreeNode<leftType>*>(left)) == (targetType)(rightType)(*static_cast<ConstantTreeNode<rightType>*>(right)) );
}

MemHolderTreeNode* equalsStringToString(MemHolderTreeNode* left, MemHolderTreeNode* right)
{
    return new ConstantTreeNode<turtleInt>( (std::string)(turtleString)(*static_cast<ConstantTreeNode<turtleString>*>(left)) == (std::string)(turtleString)(*static_cast<ConstantTreeNode<turtleString>*>(right)) );
}

/***********************************************************************************************************/
/***************************************** Initialization Procedures ***************************************/
/***********************************************************************************************************/

void init_typeOperations()
{
    //Addition -->
    TYPE_ADDITION_TABLE[(int)turtleObjectType::STRING][(int)turtleObjectType::STRING] = &addStringToString;

    TYPE_ADDITION_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &addValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_ADDITION_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &addValToVal<turtleFloat, turtleInt, turtleFloat>;

    TYPE_ADDITION_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &addValToVal<turtleFloat, turtleFloat, turtleInt>;
    TYPE_ADDITION_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &addValToVal<turtleFloat, turtleFloat, turtleFloat>;

    // Subtraction -->
    TYPE_SUBTRACTION_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &subValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_SUBTRACTION_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &subValToVal<turtleFloat, turtleInt, turtleFloat>;

    TYPE_SUBTRACTION_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &subValToVal<turtleFloat, turtleFloat, turtleInt>;
    TYPE_SUBTRACTION_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &subValToVal<turtleFloat, turtleFloat, turtleFloat>;

    // Multiply -->
    TYPE_MULTIPLY_TABLE[(int)turtleObjectType::STRING][(int)turtleObjectType::INT] = &mulStringToInt;
    TYPE_MULTIPLY_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::STRING] = &mulIntToString;
    TYPE_MULTIPLY_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &mulValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_MULTIPLY_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &mulValToVal<turtleFloat, turtleInt, turtleFloat>;

    TYPE_MULTIPLY_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &mulValToVal<turtleFloat, turtleFloat, turtleInt>;
    TYPE_MULTIPLY_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &mulValToVal<turtleFloat, turtleFloat, turtleFloat>;

    // Divide -->
    TYPE_DIVIDE_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &divValToVal<turtleFloat, turtleInt, turtleInt>;
    TYPE_DIVIDE_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &divValToVal<turtleFloat, turtleInt, turtleFloat>;

    TYPE_DIVIDE_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &divValToVal<turtleFloat, turtleFloat, turtleInt>;
    TYPE_DIVIDE_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &divValToVal<turtleFloat, turtleFloat, turtleFloat>;
    
    // Mod -->
    TYPE_MODULO_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &modValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_MODULO_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &modValToVal<turtleFloat, turtleInt, turtleFloat>;

    TYPE_MODULO_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &modValToVal<turtleFloat, turtleFloat, turtleInt>;
    TYPE_MODULO_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &modValToVal<turtleFloat, turtleFloat, turtleFloat>;
    
    // Boolean Less Than-->
    TYPE_LESS_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &lessThanValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_LESS_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &lessThanValToVal<turtleInt, turtleInt, turtleFloat>;

    TYPE_LESS_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &lessThanValToVal<turtleInt, turtleFloat, turtleInt>;
    TYPE_LESS_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &lessThanValToVal<turtleInt, turtleFloat, turtleFloat>;
    
    // Boolean Greater Than-->
    TYPE_GREATER_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &greaterThanValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_GREATER_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &greaterThanValToVal<turtleInt, turtleInt, turtleFloat>;

    TYPE_GREATER_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &greaterThanValToVal<turtleInt, turtleFloat, turtleInt>;
    TYPE_GREATER_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &greaterThanValToVal<turtleInt, turtleFloat, turtleFloat>;
    
    // Boolean Equals-->
    TYPE_EQUALS_TABLE[(int)turtleObjectType::STRING][(int)turtleObjectType::STRING] = &equalsStringToString;

    TYPE_EQUALS_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &equalsValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_EQUALS_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &equalsValToVal<turtleInt, turtleInt, turtleFloat>;

    TYPE_EQUALS_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &equalsValToVal<turtleInt, turtleFloat, turtleInt>;
    TYPE_EQUALS_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &equalsValToVal<turtleInt, turtleFloat, turtleFloat>;

    // Boolean Less than Equals-->
    TYPE_LESSEQUAL_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &lessThanEqualValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_LESSEQUAL_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &lessThanEqualValToVal<turtleInt, turtleInt, turtleFloat>;

    TYPE_LESSEQUAL_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &lessThanEqualValToVal<turtleInt, turtleFloat, turtleInt>;
    TYPE_LESSEQUAL_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &lessThanEqualValToVal<turtleInt, turtleFloat, turtleFloat>;

    // Boolean Equals-->
    TYPE_GREATEREQUAL_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::INT] = &greaterThanEqualValToVal<turtleInt, turtleInt, turtleInt>;
    TYPE_GREATEREQUAL_TABLE[(int)turtleObjectType::INT][(int)turtleObjectType::FLOAT] = &greaterThanEqualValToVal<turtleInt, turtleInt, turtleFloat>;

    TYPE_GREATEREQUAL_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::INT] = &greaterThanEqualValToVal<turtleInt, turtleFloat, turtleInt>;
    TYPE_GREATEREQUAL_TABLE[(int)turtleObjectType::FLOAT][(int)turtleObjectType::FLOAT] = &greaterThanEqualValToVal<turtleInt, turtleFloat, turtleFloat>;
}

} // namespace turtle