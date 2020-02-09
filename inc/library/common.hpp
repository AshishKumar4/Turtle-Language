#pragma once 
#include "string"
// #include "tokens.hpp"

namespace turtle
{

class TurtleError : public std::exception
{
protected:
    const std::string error;
public:
    TurtleError(std::string error, std::string type): std::exception(), error(type + ": " + error) {}

    const char *what() const throw()
    {
        return error.c_str();
    }
};

class ParserError : public TurtleError
{   
public:
    ParserError(std::string error): TurtleError(error, "GrammerError") {}
};

class SyntacticError : public TurtleError
{   
public:
    SyntacticError(std::string error): TurtleError(error, "SyntacticError") {}
};

class SymanticError : public TurtleError
{   
public:
    SymanticError(std::string error): TurtleError(error, "SymanticError") {}
};

class NotImplementedError : public TurtleError
{   
public:
    NotImplementedError(std::string error): TurtleError(error, "NotImplemented") {}
};
}

void errorHandler(std::string errorMsg);
void errorHandler(std::exception exp);
void errorHandler(turtle::TurtleError exp);