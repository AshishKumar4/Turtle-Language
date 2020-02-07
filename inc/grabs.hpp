#pragma once

#include <list>
#include <algorithm>
#include <map>
#include <iostream>
#include <string>
#include <vector>

namespace turtle
{

template<typename T>
class Grabs
{
    std::string     name;
    size_t          maxSize;
    std::vector<T*>    heap;
public:
    Grabs(std::string name, size_t init_size = 1000) :
    name(name), maxSize(0)
    {
        std::cout<<"\nInitializing Grabs for "<<name;
        resize(init_size);
    }

    void resize(size_t newSize)
    {
        maxSize += newSize;
        auto tofill = maxSize - heap.size();
        heap.reserve(maxSize);
        for(int i = 0; i < tofill; i++)
        {
            heap.push_back((T*)malloc(sizeof(T)));
        }
    }

    auto grab()
    {
        if(heap.size() <= 1)
        {
            resize(maxSize/2);
        }
        auto val = heap.back();
        heap.pop_back();
        return val;
    }

    void giveBack(T* val)
    {
        heap.push_back(val);
    }
};

}