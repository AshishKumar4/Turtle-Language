#include "DataTypes.hpp"
#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "iostream"
#include "exception"
#include "library/common.hpp"

namespace turtle
{
MemoryWrapper* MemoryWrapper::operator+(MemoryWrapper &obj)
{
    errorHandler(NotImplementedError("Operator Addition"));
    return nullptr;
}

MemoryWrapper* MemoryWrapper::operator-(MemoryWrapper &obj)
{
    errorHandler(NotImplementedError("Operator Subtraction"));
    return nullptr;
}

MemoryWrapper* MemoryWrapper::operator*(MemoryWrapper &obj)
{
    errorHandler(NotImplementedError("Operator Multiplication"));
    return nullptr;
}

MemoryWrapper* MemoryWrapper::operator/(MemoryWrapper &obj)
{
    errorHandler(NotImplementedError("Operator Division"));
    return nullptr;
}
} // namespace turtle