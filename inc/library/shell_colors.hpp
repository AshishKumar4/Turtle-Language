#pragma once 

#include "iostream"

/*
         foreground background
black        30         40
red          31         41
green        32         42
yellow       33         43
blue         34         44
magenta      35         45
cyan         36         46
white        37         47

reset             0  (everything back to normal)
bold/bright       1  (often a brighter shade of the same colour)
underline         4
inverse           7  (swap foreground and background colours)
bold/bright off  21
underline off    24
inverse off      27

"\033[1;31mbold red text\033[0m\n"
*/

enum ConsoleColor
{
    BLACK = 30,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    WHITE,
    BRIGHT_BLACK = 90,
    BRIGHT_RED,
    BRIGHT_GREEN,
    BRIGHT_BLUE,
    BRIGHT_MAGENTA,
    BRIGHT_CYAN,
    BRIGHT_WHITE
};

void setColor(ConsoleColor foreground, ConsoleColor background=BLACK)
{
    std::cout << "\033[" << foreground << ";" << background + 10 << "m";
}
