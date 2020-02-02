#include "iostream"
#include "string"
#include "stdlib.h"
#include "stdint.h"
#include "stdio.h"
#include "tokens.hpp"
#include "DataTypes.hpp"

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

std::string interpreter(std::string input)
{
    auto toks = turtle::genTokenStack(input);
    std::cout << std::endl;
    for (auto i : toks)
    {
        std::cout << "\t" << i->getName();
        fflush(stdout);
    }

    auto ast = turtle::simpleASTmaker(toks, turtle::GLOBAL_CONTEXT, true);
    // Solve the ast -->
    // auto node = ast;//[0];
    auto node = symbolicASTexecutor({ast}, turtle::GLOBAL_CONTEXT);
    // std::cout << std::endl << ast->stringRepresentation();
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
        catch (...)
        {
            std::cout << "Error Occured!"; //<< e.what();
        }
        setColor(BRIGHT_GREEN);
        std ::cout << std::endl << val <<std::endl;
    }
}

int main()
{
    printf("\nInitializing Turtle...\n");
    turtle::init_parser();
    shellLoop();
    // Trie<std::string>   trie;
    // trie.save("help", "WOW THIS WORKS");
    // trie.save("fuck", "THIS");
    // trie.save("helpme", "FUCKING");
    // trie.save("helpm", "WORKS");

    // std::cout<<trie.get("help") << std::endl<<trie.get("fuck") << std::endl<<trie.get("helpme") << std::endl<<trie.get("helpm") << std::endl;

    return 0;
}