#include "grabs.hpp"
#include "library/common.hpp"

#include "tokens.hpp"
#include "tokenTree/tokenTree.hpp"

#include "tokenTree/literal.hpp"
#include "tokenTree/bracket.hpp"
#include "tokenTree/operator.hpp"

// #include "parser/parser.hpp"

#include "DataTypes.hpp"
#include "ast/ast.hpp"

namespace turtle
{

Grabs<Token> Token::grabsToken("tokens");
Grabs<TokenTree> TokenTree::grabsToken("tokenTreeNodes");

Grabs<VariableTreeNode> VariableTreeNode::grabsToken("varTreeNodes");
Grabs<ReturnTreeNode> ReturnTreeNode::grabsToken("returnTreeNodes");
Grabs<FunctionTreeNode> FunctionTreeNode::grabsToken("funcTreeNodes");
Grabs<LambdaTreeNode> LambdaTreeNode::grabsToken("lambdaTreeNodes");
Grabs<ConditionalTreeNode> ConditionalTreeNode::grabsToken("conditionalTreeNodes");
Grabs<LoopTreeNode> LoopTreeNode::grabsToken("loopTreeNodes");
Grabs<TempLiteralTreeNode> TempLiteralTreeNode::grabsToken("tempTreeNodes");
Grabs<TempLiteralWrapperNode> TempLiteralWrapperNode::grabsToken("tempWrapperTreeNodes");

Grabs<OperatorTreeNode> OperatorTreeNode::grabsToken("operatorTreeNodes");
Grabs<UnaryOperatorTreeNode> UnaryOperatorTreeNode::grabsToken("unaryOperatorTreeNodes");
Grabs<BinaryOperatorTreeNode> BinaryOperatorTreeNode::grabsToken("binaryOperatorTreeNodes");

void init_grabs()
{
}

}