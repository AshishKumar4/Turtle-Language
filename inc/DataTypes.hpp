#pragma once 
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
#include "exception"

namespace turtle
{

enum class MemoryType
{
    INTEGER,
    FLOAT,
    STRING,
    OTHER
};

enum class DataPassMode
{
    COPY,
    REFERENCE
};

// STRINGS ARE BY DEFAULT IMMUTABLE

struct MemoryObj
{
    MemoryType  type;
    uintptr_t   address;
    size_t    size;
};

#define INT_MAX_SIZE    8   // Bytes

class MemoryWrapper;

typedef std::tuple<MemoryWrapper*, std::string > MemoryWrapperOperationResult_t;

class MemoryWrapper
{
protected: 
    MemoryObj obj;
public:
    MemoryWrapper(size_t size, MemoryType type)
    {
        obj.size = size;
        obj.address = (uintptr_t)malloc(size);
        memset((void*)obj.address, 0, size);
    }

    MemoryWrapper(uintptr_t address, size_t size, DataPassMode mode, MemoryType type)
    {
        obj.size = size;
        if (mode == DataPassMode::COPY)
        {
            obj.address = (uintptr_t)malloc(size);
            memcpy((void*)obj.address, (void*)address, obj.size);
        }
        else if(mode == DataPassMode::REFERENCE)
        {
            obj.address = address;
        }
        else 
        {
            throw "DataPassMode undefined!";
        }
    }

    MemoryType getType()
    {
        return obj.type;
    }

    void setValue(uintptr_t buff)
    {
        memcpy((void*)obj.address, (void*)buff, obj.size);
    }

    virtual std::string getResultString() = 0;

    // All Child classes must follow simple Operator logics
    // Like +, -, /, *,

    virtual MemoryWrapper* operator + (MemoryWrapper &obj);
    virtual MemoryWrapper* operator - (MemoryWrapper &obj);
    virtual MemoryWrapper* operator * (MemoryWrapper &obj);
    virtual MemoryWrapper* operator / (MemoryWrapper &obj);

    virtual  bool getBooleanValue() = 0;
};


class Integer : public MemoryWrapper
{  
    // Integers are by default only INT_MAX_SIZE bytes
    int64_t    val;
protected:
public:
    Integer() : MemoryWrapper(INT_MAX_SIZE, MemoryType::INTEGER)
    {
    }

    Integer(int64_t value, DataPassMode mode=DataPassMode::REFERENCE) : MemoryWrapper((uintptr_t)&value, INT_MAX_SIZE, mode, MemoryType::INTEGER), val(value)
    {
    }

    Integer(int64_t *value, DataPassMode mode=DataPassMode::REFERENCE) : MemoryWrapper((uintptr_t)value, INT_MAX_SIZE, mode, MemoryType::INTEGER), val(*value)
    {
    }

    std::string getResultString()
    {
        return std::to_string(val);
    }

    auto getValue()
    {
        return val;
    }

    bool getBooleanValue()
    {
        if(val != 0)
        {
            return true;
        }
        std::cout<< " << " << val;
        return false;
    }

    MemoryWrapper* operator +(MemoryWrapper  &obj)
    {
        return new Integer(this->val + ((Integer*)&obj)->getValue());
    }

    MemoryWrapper* operator -(MemoryWrapper  &obj)
    {
        return new Integer(this->val - ((Integer*)&obj)->getValue());
    }

    MemoryWrapper* operator /(MemoryWrapper  &obj)
    {
        return new Integer(this->val / ((Integer*)&obj)->getValue());
    }

    MemoryWrapper* operator *(MemoryWrapper  &obj)
    {
        return new Integer(this->val * ((Integer*)&obj)->getValue());
    }
};

// class Float : public MemoryWrapper
// {  
//     // Floats are by default only INT_MAX_SIZE bytes
//     float    val;
// protected:
// public:
//     Float() : MemoryWrapper(INT_MAX_SIZE, MemoryType::FLOAT)
//     {
//     }

//     Float(int64_t value, DataPassMode mode=DataPassMode::REFERENCE) : MemoryWrapper((uintptr_t)&value, INT_MAX_SIZE, mode, MemoryType::Float), val(value)
//     {
//     }

//     Float(int64_t *value, DataPassMode mode=DataPassMode::REFERENCE) : MemoryWrapper((uintptr_t)value, INT_MAX_SIZE, mode, MemoryType::Float), val(*value)
//     {
//     }

//     std::string getResultString()
//     {
//         return std::to_string(val);
//     }

//     auto getValue()
//     {
//         return val;
//     }

//     MemoryWrapper* operator +(MemoryWrapper  &obj)
//     {
//         return new Float(this->val + ((Float*)&obj)->getValue());
//     }

//     MemoryWrapper* operator -(MemoryWrapper  &obj)
//     {
//         return new Float(this->val - ((Float*)&obj)->getValue());
//     }

//     MemoryWrapper* operator /(MemoryWrapper  &obj)
//     {
//         return new Float(this->val / ((Float*)&obj)->getValue());
//     }

//     MemoryWrapper* operator *(MemoryWrapper  &obj)
//     {
//         return new Float(this->val * ((Float*)&obj)->getValue());
//     }
// };

class Strings : public MemoryWrapper
{
    std::string    val;
public:
    // Strings(size_t size) : MemoryWrapper(size, MemoryType::STRING)
    // {

    // }

    Strings(char* str, size_t size, DataPassMode mode) : MemoryWrapper((uintptr_t)str, size, mode, MemoryType::STRING), val(str)
    {

    }

    Strings(uintptr_t addr, size_t size, DataPassMode mode) : MemoryWrapper(addr, size, mode, MemoryType::STRING), val((char*)(addr))
    {

    }

    // Strings(std::string &str, size_t size, DataPassMode mode) : MemoryWrapper((uintptr_t)str.c_str, size, mode, STRING)
    // {

    // }

    Strings(std::string str, DataPassMode mode = DataPassMode::REFERENCE) : MemoryWrapper((uintptr_t)str.c_str(), str.length(), mode, MemoryType::STRING), val(str)
    {

    }

    // Strings(std::string &str, DataPassMode mode = DataPassMode::REFERENCE) : MemoryWrapper((uintptr_t)str.c_str(), str.length(), mode, MemoryType::STRING), val(str)
    // {

    // }

    bool getBooleanValue()
    {
        if(val != "")
        {
            return true;
        }
        return false;
    }

    std::string getResultString()
    {
        return val;
    }

    MemoryWrapper* operator +(MemoryWrapper  &obj)
    {
        this->val += ((Strings*)&obj)->getResultString();
        return this;
    }

};

class IntConst : public Integer
{
public:
    IntConst(int64_t value) : Integer(value, DataPassMode::REFERENCE)
    {
    }

    IntConst(int64_t &value) : Integer(value, DataPassMode::REFERENCE)
    {
    }

};

class IntVar : public Integer
{
public:
    IntVar(int64_t value) : Integer(value, DataPassMode::COPY)
    {
    }

    IntVar(int64_t &value) : Integer(value, DataPassMode::COPY)
    {
    }
};

class StrConst : public Strings
{
public: 
    StrConst(char* str, size_t size) : Strings(str, size, DataPassMode::REFERENCE)
    {

    }

    // StrConst(std::string &str, size_t size) : Strings(str, size, REFERENCE)
    // {

    // }


    StrConst(std::string &str) : Strings(str, DataPassMode::REFERENCE)
    {

    }

    StrConst(uintptr_t addr, size_t size) : Strings(addr, size, DataPassMode::REFERENCE)
    {

    }
};

class StrVar : public Strings
{
public: 
    StrVar(char* str, size_t size) : Strings(str, size, DataPassMode::COPY)
    {

    }

    // StrVar(std::string &str, size_t size) : Strings(str, size, REFERENCE)
    // {

    // }


    StrVar(std::string &str) : Strings(str, DataPassMode::COPY)
    {

    }

    StrVar(uintptr_t addr, size_t size) : Strings(addr, size, DataPassMode::COPY)
    {

    }
};

}