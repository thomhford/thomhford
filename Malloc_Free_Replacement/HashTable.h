//
// Created by Thomas Ford on 3/15/23.
//
#pragma once
#ifndef MALLOCREPLACEMENT_HASHTABLE_H
#define MALLOCREPLACEMENT_HASHTABLE_H

#include <sys/mman.h> // Needed for mmap() / munmap()
#include <cerrno> // Needed for printing error messages
#include <iostream> // Needed for printing
#include <algorithm> // Needed for std::swap()

// Node for storing size of memory stored
class MemNode {
public:
    void* address; // Storage of allocated addresses for finding sizes in HashTable
    size_t size; // Needed for deallocation of allocated memory using munmap
    MemNode(){ // Constructor for Node
        this->address = nullptr;
        this->size = 0;
    }
};

//Hash Table for storing size of address space needed
class HashTable {
public:
    MemNode *arr;
    size_t table_capacity;
    size_t table_size;
    void growTable();
    size_t hashing(void* key) const;
public:
    HashTable();
    void* insert(size_t size);
    bool remove(void *ptr);

    size_t getTableCapacity() const;
    size_t getTableSize() const;
};

#endif //MALLOCREPLACEMENT_HASHTABLE_H
