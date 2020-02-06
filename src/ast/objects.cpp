#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"

// #include "library/trie.hpp"
#include "library/common.hpp"

#include "tokenTree/tokenTree.hpp"
#include "tokenTree/bracket.hpp"
#include "tokenTree/literal.hpp"

#include "ast/ast.hpp"

#include <exception>
#include <map>

namespace turtle
{

TokenTree *genTokenTreeNodeFromList(TokenTreeType type, std::vector<TokenTree *> &list, std::vector<LiteralRule> &rules, variableContext_t context) //, bool symbolic)
{
    TokenTree *node = nullptr;
    switch (type)
    {
    case TokenTreeType::LAMBDA:
    {
        std::cout << "\nLAMBDA:";
        node = new LambdaTreeNode((TupleTreeNode *)list[1], ((CodeBlock *)list[2]), context);
        break;
    }
    case TokenTreeType::DEFINE:
    {
        std::cout << "\nDEFINE:";
        // We construct a new function!
        node = new FunctionTreeNode(list[1]->getName(), (TupleTreeNode *)list[2], ((CodeBlock *)list[3]), context);
        // Wrap our new node into a PlaceHolder Variable and put it into the context
        auto var = new VariableTreeNode(node, list[1]->getName());
        // (*(context.back())).insert(std::make_pair(list[1]->getName(), var));
        // std::cout<<"\nSAVED IN CONTEXT: "<<list[1]->getName()<<"\n";
        (*(context.back()))[list[1]->getName()] = var;
        break;
    }
    case TokenTreeType::FUNCTIONAL:
    {
        std::cout << "\nFUNCTIONAL:";
        node = list[0];
        std::cout << node->getName();
        if (node->getType() == TokenTreeType::VARIABLE)
        {
            node = contextSolver((VariableTreeNode *)node, context, true);
            node = node->execute(context);
        }
        switch (node->getType())
        {
        case TokenTreeType::FUNCTIONAL:
        {
            auto func = ((FunctionTreeNode *)node);
            auto funccopy = new FunctionTreeNode(func);
            // printf(" {%x, %x}", func, funccopy);
            funccopy->setParams((TupleTreeNode *)list[1], context);
            // node = func->execute(context);
            node = funccopy; // We provide an instance of the function
            std::cout<<"["<<node->stringRepresentation()<<"]";
            break;
        }
        case TokenTreeType::INBUILT_FUNCTION:
        {

            break;
        }
        case TokenTreeType::UNKNOWN:
        {
            // It would be solved in the future.
            errorHandler(NotImplementedError("Future Calling"));
            break;
        }
        default:
            std::cout << (int)node->getType();
            errorHandler(NotImplementedError("Non-Function type to Function conversion"));
        }
        break;
    }
    case TokenTreeType::RETURN:
    {
        std::cout << "\nRETURN ";
        // set a value in the current context
        // Solve it just in case
        auto tok = simpleASTmaker(std::vector<TokenTree *>(list.begin() + 1, list.end()), context);

        auto arg = new ReturnTreeNode(tok);
        // std::cout << tok->stringRepresentation();
        // fflush(stdout);
        node = arg;
        break;
    }
    case TokenTreeType::INBUILT_FUNCTION:
        std::cout << "INBUILD_FUNCTION:";
        break;
    case TokenTreeType::VARIABLE:
        std::cout << "VARIABLE:";
        break;
    case TokenTreeType::CONSTANT:
        std::cout << "CONSTANT:";
        break;
    case TokenTreeType::CONDITIONAL:
    {
        std::cout << "\nCONDITIONAL:";
        std::vector<TupleTreeNode *> params;
        std::vector<CodeBlock *> blocks;
        for (auto i : list)
        {
            std::cout << "->" << i->stringRepresentation();
            if (i->getType() == TokenTreeType::TUPLE)
            {
                params.push_back((TupleTreeNode *)i);
            }
            else if (i->getType() == TokenTreeType::CODEBLOCK)
            {
                blocks.push_back((CodeBlock *)i);
            }
        }

        node = new ConditionalTreeNode(params, blocks);
        break;
    }
    case TokenTreeType::LOOP:
        std::cout << "\nLOOP:";
        if(list[0]->getName() == "key_while")
        {
            node = new WhileLoopTreeNode((TupleTreeNode*)list[1], (CodeBlock*)list[2]);
        }
        else 
        {
            errorHandler(NotImplementedError(list[0]->getName() + " kind of loops!"));
        }
        break;
    case TokenTreeType::DUMMY:
        errorHandler(SyntacticError("You cannot use " + list[0]->getName() + " like this!"));
        break;
    default:
        errorHandler(SyntacticError("Not implemented!"));
    }
    for (auto i : list)
    {
        std::cout << i->getName() << "\t";
    }
    std::cout << std::endl;
    if (node != nullptr)
        return node;
    errorHandler(SyntacticError("Could not process the token " + list[0]->getName()));
    // return new TokenTree(TokenTreeType::UNKNOWN, "__unknown__");
    return nullptr;
}

int checkConstraints(TokenTree **list, std::vector<LiteralConstraint *> &constraints, int finger, int size)
{
    int counter = 0;
    for (auto j : constraints)
    {
        if (finger >= size)
        {
            return 0;
        }
        if (list[finger]->getType() == TokenTreeType::TEMP_LITERAL_WRAPPER)
        {
            // Expand them out!
            list[finger] = ((TempLiteralWrapperNode *)list[finger])->getOriginal();
        }

        switch (j->getType())
        {
        case TokenTreeType::DUMMY:
            // Dummy is basically to take in Anything!
            ++finger;
            ++counter;
            continue;
        case TokenTreeType::TUPLE:
            if (list[finger]->getType() == TokenTreeType::TUPLE && (((TupleLiteralConstraint *)j)->getSize() <= 0 || ((TupleLiteralConstraint *)j)->getSize() == ((TupleTreeNode *)list[finger])->getSize()))
            {
                // Constraint met!
                ++finger;
                ++counter;
                continue;
            }
            // std::cerr<< "[tuple condition not met "<< ((TupleLiteralConstraint *)j)->getSize() << " , " << ((TupleTreeNode *)list[finger])->getSize();
            break;
        case TokenTreeType::TEMP_LITERAL:
            if (((NameLiteralConstraint *)j)->getName() == list[finger]->getName())
            {
                // Constraint met!
                ++finger;
                ++counter;
                continue;
            }
            std::cout << "{" << ((NameLiteralConstraint *)j)->getName() << "}";
            break;
        case TokenTreeType::CODEBLOCK:
            if (list[finger]->getType() == TokenTreeType::CODEBLOCK)
            {
                // Constraint met!
                ++finger;
                ++counter;
                continue;
            }
            break;
        case TokenTreeType::VARIABLE:
            if (list[finger]->getType() == TokenTreeType::VARIABLE)
            {
                // Constraint met!
                ++finger;
                ++counter;
                continue;
            }
            break;
        case TokenTreeType::CONSTANT:
            if (list[finger]->getType() == TokenTreeType::CONSTANT)
            {
                // Constraint met!
                ++finger;
                ++counter;
                continue;
            }
            break;
        case TokenTreeType::FUNCTIONAL:
            if (list[finger]->getType() == TokenTreeType::FUNCTIONAL || list[finger]->getType() == TokenTreeType::INBUILT_FUNCTION)
            {
                // Constraint met!
                ++finger;
                ++counter;
                continue;
            }
            break;
        default:
            std::cerr << (int)j->getType();
            errorHandler(SyntacticError("Incorrect TokenTreeType!"));
            break;
        }
        // std::cerr << list[finger]->getName() << " [condition not met]";
        return 0;
    }
    // Now if all contraints were met correctly, then the value of 'counter' should equal to the
    // Length of the constraints vector!
    if (counter == constraints.size())
    {
        // std::cout<<"[constraint satisfied] ";
        // fflush(stdout);
        // All Constraints met, We found a possible candidate
        return 1;
    }
    // std::cerr << list[finger]->getName() << " [condition not met]";
    return 0;
}

TokenDigesterReturn_t LiteralRulesConstruct::tokenTreeBuilder(TokenTree **list, int index, int size, variableContext_t context, bool tree_expantion)
{
    int finger = index;
    // TokenTree*     root = genTokenTreeNodeFromType(this->getType());
    std::vector<LiteralRule> satisfiedRules;
    for (auto i : rules)
    {
        if (finger >= size)
        {
            break;
        }
        int occ = 0;

        if (i.getRepeats() == REPEAT_ONE_OR_MANY)
        {
            while (checkConstraints(list, i.constraints, finger, size))
            {
                // Whenever a Constraint is met, We should do something
                satisfiedRules.push_back(i);
                finger += i.constraints.size();
                ++occ;
            }
            if (occ == 0)
            {
                // Rule not met!
                errorHandler(SyntacticError("Rule not met for item " + list[finger]->getName()));
            }
        }
        else if (i.getRepeats() == REPEAT_ZERO_OR_MANY)
        {
            while (checkConstraints(list, i.constraints, finger, size))
            {
                satisfiedRules.push_back(i);
                finger += i.constraints.size();
                ++occ;
            }
            // It really dosen't matter if constraint was met or not!
        }
        else if (i.getRepeats() == REPEAT_ZERO_OR_ONE)
        {
            if (checkConstraints(list, i.constraints, finger, size))
            {
                satisfiedRules.push_back(i);
                finger += i.constraints.size();
                ++occ;
            }
        }
        else
        {
            for (int j = 0; j < i.getRepeats() && checkConstraints(list, i.constraints, finger, size); j++)
            {
                satisfiedRules.push_back(i);
                finger += i.constraints.size();
                ++occ;
            }
            if (occ != i.getRepeats())
            {
                std::cerr << occ << "<->" << i.getRepeats();
                errorHandler(SyntacticError("<2>Rule not met for item " + list[finger]->getName()));
            }
        }
    }
    std::vector<TokenTree *> tmpList(&list[index], &list[finger]);
    if (tmpList.size() == 0)
    {
        errorHandler(SyntacticError("You cannot use " + list[index]->getName() + " like this!"));
        return TokenDigesterReturn_t(list[0], 1);
    }
    TokenTree *node;
    if (!tree_expantion)
    {
        // To be executed later
        // Wrap in future object
        node = new FutureSolutionTreeNode(this->getType(), tmpList, satisfiedRules);
    }
    else
        node = genTokenTreeNodeFromList(this->getType(), tmpList, satisfiedRules, context);
    return TokenDigesterReturn_t(node, finger - index);
}

std::map<std::string, LiteralRulesConstruct *> GLOBAL_LITERAL_TABLE;

std::string KEYWORDS_LIST[] = {"if", "else", "while", "for", "and", "or", "not",
                               "define", "return", "using", "import", "list", "dict",
                               "set", "tuple", "print", "input", "switch", "break",
                               "continue", "pass", "true", "false"};

void init_globalLiteralTable()
{
    // TODO : Define this
    GLOBAL_LITERAL_TABLE["if"] = new LiteralRulesConstruct("if", TokenTreeType::CONDITIONAL,
                                                           {
                                                               LiteralRule({new NameLiteralConstraint("key_if"), new TupleLiteralConstraint(REPEAT_ONE_OR_MANY), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
                                                               LiteralRule({new NameLiteralConstraint("key_elif"), new TupleLiteralConstraint(REPEAT_ONE_OR_MANY), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, REPEAT_ZERO_OR_MANY),
                                                               LiteralRule({new NameLiteralConstraint("key_else"), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, REPEAT_ZERO_OR_ONE),
                                                           });

    GLOBAL_LITERAL_TABLE["elif"] = new LiteralRulesConstruct("elif", TokenTreeType::DUMMY, {});

    GLOBAL_LITERAL_TABLE["else"] = new LiteralRulesConstruct("else", TokenTreeType::DUMMY, {});

    GLOBAL_LITERAL_TABLE["define"] = new LiteralRulesConstruct("define", TokenTreeType::DEFINE,
                                                               {
                                                                   LiteralRule({new NameLiteralConstraint("key_define"), new LiteralConstraint(TokenTreeType::DUMMY), new TupleLiteralConstraint(REPEAT_ZERO_OR_MANY), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
                                                               });

    GLOBAL_LITERAL_TABLE["def"] = GLOBAL_LITERAL_TABLE["define"];

    GLOBAL_LITERAL_TABLE["lambda"] = new LiteralRulesConstruct("lambda", TokenTreeType::LAMBDA,
                                                               {
                                                                   LiteralRule({new NameLiteralConstraint("key_lambda"), new TupleLiteralConstraint(REPEAT_ZERO_OR_MANY), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
                                                               });

    GLOBAL_LITERAL_TABLE["return"] = new LiteralRulesConstruct("return", TokenTreeType::RETURN,
                                                               {LiteralRule({new NameLiteralConstraint("key_return")}, 1),
                                                                LiteralRule({new LiteralConstraint(TokenTreeType::DUMMY)}, REPEAT_ONE_OR_MANY)});

    GLOBAL_LITERAL_TABLE["while"] = new LiteralRulesConstruct("while", TokenTreeType::LOOP,
                                                              {
                                                                  LiteralRule({new NameLiteralConstraint("key_while"), new TupleLiteralConstraint(1), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
                                                              });

    GLOBAL_LITERAL_TABLE["for"] = new LiteralRulesConstruct("for", TokenTreeType::LOOP,
                                                              {
                                                                  LiteralRule({new NameLiteralConstraint("key_for"), 
                                                                  new TupleLiteralConstraint(REPEAT_ONE_OR_MANY), 
                                                                  new NameLiteralConstraint("key_in"), 
                                                                  new LiteralConstraint(TokenTreeType::LIST),
                                                                  new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
                                                              });

    GLOBAL_LITERAL_TABLE["print"] = new LiteralRulesConstruct("print", TokenTreeType::INBUILT_FUNCTION,
                                                              {
                                                                  LiteralRule({new NameLiteralConstraint("key_print"), new TupleLiteralConstraint(REPEAT_ONE_OR_MANY)}, 1),
                                                              });

    GLOBAL_LITERAL_TABLE["__variable__"] = new LiteralRulesConstruct("__variable__", TokenTreeType::VARIABLE,
                                                                     {
                                                                         LiteralRule({new LiteralConstraint(TokenTreeType::VARIABLE)}, 1),
                                                                     });

    GLOBAL_LITERAL_TABLE["__function__"] = new LiteralRulesConstruct("__function__", TokenTreeType::FUNCTIONAL,
                                                                     {
                                                                         LiteralRule({new LiteralConstraint(TokenTreeType::DUMMY), new TupleLiteralConstraint(REPEAT_ZERO_OR_MANY)}, 1),
                                                                     });

    GLOBAL_LITERAL_TABLE["__array__"] = new LiteralRulesConstruct("__array__", TokenTreeType::LIST,
                                                                  {
                                                                      LiteralRule({new LiteralConstraint(TokenTreeType::DUMMY), new LiteralConstraint(TokenTreeType::LIST)}, 1),
                                                                  });

    GLOBAL_LITERAL_TABLE["__dict__"] = new LiteralRulesConstruct("__dict__", TokenTreeType::DICT,
                                                                 {
                                                                     LiteralRule({new LiteralConstraint(TokenTreeType::CURLY_BRACKET_BLOCK)}, 1),
                                                                 });
}
} // namespace turtle