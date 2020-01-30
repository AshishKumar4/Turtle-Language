#include "tokens.hpp"
#include "DataTypes.hpp"
#include "tokenRules.hpp"

#include "library/trie.hpp"
#include "library/common.hpp"

#include "parser/literal.hpp"
#include "parser/bracket.hpp"
#include "parser/quotestr.hpp"

#include <exception>
#include <map>

#include "parser/literal.hpp"

namespace turtle
{

FunctionTreeNode *genNewFunction(std::string name, TupleTreeNode *params, CodeBlock *block)
{
    // Verify that the TupleTreeNode dosen't contain any constants!
    if (!params->isDynamic())
    {
        errorHandler(SyntacticError("You cannot provide static values in initialization parameters of functions"));
        return nullptr;
    }
    FunctionTreeNode *func = new FunctionTreeNode(name, *params, *block);
    return func;
}

TokenTree *genTokenTreeNodeFromList(TokenTreeType type, std::vector<TokenTree *> &list, std::vector<LiteralRule> &rules, variableContext_t &context)
{
    TokenTree *node = nullptr;
    switch (type)
    {
    case TokenTreeType::DEFINE:
        std::cout << "\nDEFINE:";
        // We construct a new function!

        node = genNewFunction(list[1]->getName(), (TupleTreeNode *)list[2], ((CodeBlock *)list[3])); // Name, param, codeblock
        // Wrap our new node into a PlaceHolder Variable and put it into the context
        (*context.back())[list[1]->getName()] = new VariableTreeNode(node, list[1]->getName());
        break;
    case TokenTreeType::FUNCTIONAL:
        std::cout << "FUNCTIONAL:";
        node = list[0];
        switch (node->getType())
        {
        case TokenTreeType::VARIABLE:
        {
            // Generally Functions are put in variables
            if (((VariableTreeNode *)node)->getStoreType() == TokenTreeType::UNKNOWN)
            {
                errorHandler(NotImplementedError("Future Calling"));
                break;
            }
            node = solveVariablePlaceHolder(node);
            if (node == nullptr)
            {
                errorHandler(SyntacticError("Variable has NULL value"));
                break;
            }
        }
        case TokenTreeType::FUNCTIONAL:
        case TokenTreeType::INBUILT_FUNCTION:
        {
            ((FunctionTreeNode *)node)->setParams((TupleTreeNode *)list[1]);
            break;
        }
        case TokenTreeType::CONSTANT:
        {
            errorHandler(NotImplementedError("Constant Function calling"));
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
        std::cout << "CONDITIONAL:";
        break;
    case TokenTreeType::LOOP:
        std::cout << "LOOP:";
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
    return new TokenTree(TokenTreeType::UNKNOWN, "__unknown__");
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

TokenDigesterReturn_t LiteralRulesConstruct::tokenTreeBuilder(TokenTree **list, int index, int size, variableContext_t &context)
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
    return TokenDigesterReturn_t(genTokenTreeNodeFromList(this->getType(), tmpList, satisfiedRules, context), finger - index);
}

TokenTree *packageDict(std::vector<TokenTree *> list, BracketInfo btype, variableContext_t &context)
{
    return new DictTreeNode(list);
}

TokenTree *packageSet(std::vector<TokenTree *> list, BracketInfo btype, variableContext_t &context)
{
    return nullptr;
}

auto genUnknownVariable(std::string name, variableContext_t &context)
{
    auto newVar = new VariableTreeNode(nullptr, name);
    // Put this into the last context!
    (*(context.back()))[name] = newVar;
    std::cout << "Context size: "<< context.size() <<name;
    return newVar;
}

VariableTreeNode *contextSolver(Token *tok, variableContext_t &context)
{
    // Starting from the end, search for context to the begining
    for (int i = context.size() - 1; i >= 0; i--)
    {
        try
        {
            auto currContext = (*context[i]);

            auto var = currContext[tok->data];
            if (var != nullptr)
            {
                return var;
            }
        }
        catch (...)
        {
            ;
        }
    }
    return genUnknownVariable(tok->data, context);
}

TokenDigesterReturn_t tokenDigester_literal(Token **list, int index, int size, variableContext_t &context)
{
    // Find out the type of literal
    auto tok = list[index];
    try
    {
        // printf("\nsearching in global literal table..");
        auto obj = GLOBAL_LITERAL_TABLE[tok->data];
        if (obj == nullptr)
        {
            TokenTree *newTok = contextSolver(tok, context);

            // if (((PlaceHolderTreeNode *)newTok)->getStoreType() != TokenTreeType::UNKNOWN)
            // {
            //     // If its a defined placeholder (a variable), solve it and return its contents
            //     newTok = ((VariableTreeNode *)newTok)->getValue();
            // }

            // If a literal is followed by a bracket, it means something!
            if (index + 1 < size && list[index + 1]->type == TokenType::BRACKET)
            {
                auto brack = list[index + 1]->data;
                if (brack == "(")
                {
                    // Its a function call!
                    return TokenDigesterReturn_t(new TempLiteralWrapperNode(newTok, GLOBAL_LITERAL_TABLE["__function__"], TokenTreeType::TEMP_LITERAL_WRAPPER), 1);
                }
                else if (brack == "[")
                {
                    // Its Array Indexing
                    return TokenDigesterReturn_t(new TempLiteralWrapperNode(newTok, GLOBAL_LITERAL_TABLE["__array__"], TokenTreeType::TEMP_LITERAL_WRAPPER), 1);
                }
                else if (brack == "{")
                {
                    if (tok->data == "dict" || tok->data == "d")
                    {
                        auto [node, popped] = bracketSolver(list, index + 1, size, TOKEN_BRACKET_TYPE_TABLE["{"], context, packageDict);
                        return TokenDigesterReturn_t(node, popped + 1);
                    }
                    else if (tok->data == "set" || tok->data == "s")
                    {
                        auto [node, popped] = bracketSolver(list, index + 1, size, TOKEN_BRACKET_TYPE_TABLE["{"], context, packageSet);
                        return TokenDigesterReturn_t(node, popped + 1);
                    }
                    else
                        errorHandler(ParserError("A Literal cannot be followed by Curly Braces!"));
                }
            }
            return TokenDigesterReturn_t(newTok, 1);
        }
        // Must be some keyword
        return TokenDigesterReturn_t(new TempLiteralTreeNode(tok->data, obj), 1);
    }
    catch (...)
    {
        return TokenDigesterReturn_t(genUnknownVariable(tok->data, context), 1);
    }
}

TokenDigesterReturn_t tokenDigester_number(Token **list, int index, int size, variableContext_t &context)
{
    auto tok = list[index];
    // Find out the type of number
    auto num = tok->data;

    int base = 10;
    try
    {
        switch (num[1])
        {
        case 'x':
            base = 16;
            break;
        case 'o':
            base = 8;
            break;
        case 'b':
            base = 2;
            break;
        default:
            base = 10;
        }
        auto number = std::stoull(num, nullptr, base);
        // std::cout<<"num["<<number<<"]";
        Integer *intnum = new Integer((uint64_t)number, DataPassMode::COPY);
        ConstantTreeNode *node = new ConstantTreeNode(intnum, num);
        return TokenDigesterReturn_t(node, 1);
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return TokenDigesterReturn_t((TokenTree *)NULL, 1);
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
                                                               LiteralRule({new NameLiteralConstraint("key_if"), new TupleLiteralConstraint(1), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
                                                               LiteralRule({new NameLiteralConstraint("key_elif"), new TupleLiteralConstraint(1), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, REPEAT_ZERO_OR_MANY),
                                                               LiteralRule({new NameLiteralConstraint("key_else"), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, REPEAT_ZERO_OR_ONE),
                                                           });

    GLOBAL_LITERAL_TABLE["elif"] = new LiteralRulesConstruct("elif", TokenTreeType::DUMMY, {});

    GLOBAL_LITERAL_TABLE["else"] = new LiteralRulesConstruct("else", TokenTreeType::DUMMY, {});

    GLOBAL_LITERAL_TABLE["define"] = new LiteralRulesConstruct("define", TokenTreeType::DEFINE,
                                                               {
                                                                   LiteralRule({new NameLiteralConstraint("key_define"), new LiteralConstraint(TokenTreeType::DUMMY), new TupleLiteralConstraint(REPEAT_ZERO_OR_MANY), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
                                                               });

    GLOBAL_LITERAL_TABLE["while"] = new LiteralRulesConstruct("while", TokenTreeType::LOOP,
                                                              {
                                                                  LiteralRule({new NameLiteralConstraint("key_while"), new TupleLiteralConstraint(1), new LiteralConstraint(TokenTreeType::CODEBLOCK)}, 1),
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