//
// Created by Thomas Ford on 3/13/23.
//

#ifndef MALLOCREPLACEMENT_ALLOCATOR_H
#define MALLOCREPLACEMENT_ALLOCATOR_H

#include "HashTable.h"

// Methods for allocating and deallocating memory
class Allocator {
public:
    HashTable hashTable;
    Allocator();
    ~Allocator();
    void* allocate(size_t size);
    void deallocate(void *ptr) ;
    size_t get_capacity() const;
    size_t get_size() const;
};

#endif //MALLOCREPLACEMENT_ALLOCATOR_H



// valgrind
