#pragma once 
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
#include "exception"

#include "library/common.hpp"

namespace turtle
{

struct turtleString 
{
    std::string str;

    turtleString(std::string str) :
    str(str)
    {
    }

    turtleString(int num) : 
    str(std::to_string(num))
    {

    }

    operator std::string() 
    {
        return str;
    }

    operator bool() 
    {
        std::cout<<"\n]]]>>>>>"<<str<<std::endl;
        if("" == str)
            return false;
        return true;
    }

    turtleString operator + (turtleString &obj)
    {
        return (std::string)str + (std::string)obj;
    }

    turtleString operator - (turtleString &obj)
    {
        errorHandler(NotImplementedError("Subtraction of String types"));
        return obj;
    }

    turtleString operator * (turtleString &obj)
    {
        errorHandler(NotImplementedError("Multiplication of String types"));
        return obj;
    }

    turtleString operator / (turtleString &obj)
    {
        errorHandler(NotImplementedError("Division of String types"));
        return obj;
    }

    turtleString operator < (turtleString &obj)
    {
        // errorHandler(NotImplementedError("Division of String types"));
        return (std::string)str < (std::string)obj;
    }

    turtleString operator > (turtleString &obj)
    {
        // errorHandler(NotImplementedError("Division of String types"));
        return (std::string)str > (std::string)obj;
    }

    turtleString operator == (turtleString &obj)
    {
        // errorHandler(NotImplementedError("Division of String types"));
        return (std::string)str == (std::string)obj;
    }

    turtleString operator <= (turtleString &obj)
    {
        // errorHandler(NotImplementedError("Division of String types"));
        return (std::string)str <= (std::string)obj;
    }

    turtleString operator >= (turtleString &obj)
    {
        // errorHandler(NotImplementedError("Division of String types"));
        return (std::string)str >= (std::string)obj;
    } 
    
    turtleString operator % (turtleString &obj)
    {
        errorHandler(NotImplementedError("Modulo of String types"));
        // return (std::string)str % (std::string)obj;
        return obj;
    }
};

struct turtleFloat
{
    float num;

    turtleFloat(float num) :
    num(num)
    {
    }

    operator std::string() 
    {
        return std::to_string((float)num);
    }

    operator float()
    {
        return num;
    }
};

struct turtleInt
{
    int64_t num;

    turtleInt(int64_t num) :
    num(num)
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

}