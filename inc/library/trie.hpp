#pragma once 

#include "iostream"
#include "vector"
#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

#define MAX_ELEMENTS    64

template<typename T>
class TrieNode
{
    TrieNode<T>**  nodes; 
    T       data;
public:
    TrieNode(T data, int max_nodes) : data(data)
    {
        nodes = new TrieNode<T>[max_nodes]();
    }

    TrieNode(int max_nodes) : data(NULL)
    {
        nodes = new TrieNode<T>[max_nodes]();
    }

    TrieNode<T>* getNode(int index)
    {
        return nodes[index];
    }

    TrieNode<T>* getNode(char index)
    {
        return getNode((int)index);
    }

    int setNode(int index, T data)
    {
        if(nodes[index] == NULL)
        {
            nodes[index] = new TrieNode<T>(data, MAX_ELEMENTS);
            return MAX_ELEMENTS;
        }
        return 0;
    }

    void setData(T data)
    {
        this->data = data;
    }

    T getData()
    {
        return this->data;
    }
};

template <typename T>
class Trie
{
    TrieNode<T>     *nodes;
public:
    Trie()
    {

    }

    // T get(std::string inp)
    // {
    //     TrieNode<T>* trie;
    //     // for(auto i : inp)
    //     // {

    //     // }
    //     return 
    // }
};