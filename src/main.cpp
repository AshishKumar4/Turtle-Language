#include "iostream"
#include "string"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "tokens.hpp"
#include "DataTypes.hpp"

#include "sstream"
#include "fstream"

#include "ast/ast.hpp"
#include "library/shell_colors.hpp"

#define SHELL_PROMPT_TEXT "Turtle-beta"

inline void displayPrompt()
{
    int sys_tmp;
    setColor(RED);
    std::cout << SHELL_PROMPT_TEXT;
    setColor(GREEN);
    std::cout << " > ";
    setColor(WHITE);
}

auto interpreter(std::string input)
{
    auto nodes = turtle::genTokenStack(input);
    std::cout << std::endl;
    for (auto i : nodes)
    {
        std::cout << "\t" << i->getName();
        fflush(stdout);
    }

    // std::vector<std::string>
    turtle::TokenTree* node;
    std::vector<turtle::TokenTree*> asts;

    int tmpIndex = 0, i = 0;
    while (tmpIndex < nodes.size())
    {
        if (i >= nodes.size() || (nodes[i]->getType() == turtle::TokenTreeType::OPERATOR && nodes[i]->getName() == ";"))
        {
            auto ast = turtle::simpleASTmaker(std::vector<turtle::TokenTree *>(&nodes[tmpIndex], &nodes[i]), turtle::GLOBAL_CONTEXT, true);
            std::cout<<std::endl;
            // asts.push_back(ast);
            // node = ast->execute(turtle::GLOBAL_CONTEXT);
            node = symbolicASTexecutor({ast}, turtle::GLOBAL_CONTEXT);
            tmpIndex = i + 1;
        }
        ++i;
    }
    
    return node->stringRepresentation();
}

std::vector<std::string> SHELL_HISTORY;

void shellLoop()
{
    while (true)
    {
        displayPrompt();

        std::string userInput;
        std::getline(std::cin, userInput);

        setColor(YELLOW);
        std::string val;
        try
        {
            SHELL_HISTORY.push_back(userInput);
            val = interpreter(userInput);
        }
        catch (std::exception &e)
        {
            std::cout << "Error Occured! " << e.what();
        }
        setColor(BRIGHT_GREEN);
        std ::cout << std::endl
                   << val << std::endl;
    }
}

void executeCode(std::string filename)
{
    auto in = std::fstream(filename);
    std::string code(static_cast<std::stringstream const &>(std::stringstream() << in.rdbuf()).str());
    std::cout << "\nExecuting Code -->\n\n";
    setColor(YELLOW);
    auto val = interpreter(code);
    setColor(BRIGHT_GREEN);
    std ::cout << std::endl
               << val << std::endl;
}

int main(int argc, char **argv)
{
    printf("\nInitializing Turtle...\n");
    turtle::init_parser();
    if (argc > 1)
    {
        executeCode(std::string(argv[1]));
    }
    else
    {
        shellLoop();
    }
    // Trie<std::string>   trie;
    // trie.save("help", "WOW THIS WORKS");
    // trie.save("fuck", "THIS");
    // trie.save("helpme", "FUCKING");
    // trie.save("helpm", "WORKS");

    // std::cout<<trie.get("help") << std::endl<<trie.get("fuck") << std::endl<<trie.get("helpme") << std::endl<<trie.get("helpm") << std::endl;

    return 0;
}