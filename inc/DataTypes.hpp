#pragma once
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
#include "exception"
#include "functional"

#include "library/common.hpp"

namespace turtle
{

enum class turtleObjectType
{
    STRING,
    INT,
    FLOAT,
    LIST,
    num,
};

struct turtleObject
{
public:
    turtleObjectType type;
    turtleObject(turtleObjectType type) : type(type)
    {

    }
    virtual ~turtleObject() = default;
};

struct turtleString : public turtleObject
{
public:
    std::string str;

    turtleString(std::string str) : str(str), turtleObject(turtleObjectType::STRING)
    {
    }

    operator std::string()
    {
        return str;
    }

    operator bool()
    {
        std::cout << "\n]]]>>>>>" << str << std::endl;
        if ("" == str)
            return false;
        return true;
    }

    turtleString operator+(turtleString &obj)
    {
        return (std::string)str + (std::string)obj;
    }
};

struct turtleFloat : public turtleObject
{
public:
    double num;

    turtleFloat(double num) : num(num), turtleObject(turtleObjectType::FLOAT)
    {
    }

    operator std::string()
    {
        return std::to_string((double)num);
    }

    operator double()
    {
        return num;
    }

    turtleFloat operator%(turtleFloat obj)
    {
        errorHandler(NotImplementedError("Modulo of Float types"));
        // return (std::string)str % (std::string)obj;
        return obj;
    }
};

struct turtleInt : public turtleObject
{
public:
    int64_t num;

    turtleInt(int64_t num) : num(num), turtleObject(turtleObjectType::INT)
    {
    }

    operator std::string()
    {
        return std::to_string((int64_t)num);
    }

    operator int64_t()
    {
        return num;
    }
};
} // namespace turtle