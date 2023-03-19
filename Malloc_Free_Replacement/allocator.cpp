//
// Created by Thomas Ford on 3/13/23.
//

#include "allocator.h"

#define DELETED ((void*) -1)


Allocator::Allocator(){
    hashTable = HashTable();
}

Allocator::~Allocator(){
    for (int i = 0; i < hashTable.table_capacity; ++i) {
        if (hashTable.arr[i].address == nullptr || hashTable.arr[i].address == DELETED){
            continue;
        }
        hashTable.remove(hashTable.arr[i].address);
    }
    if (munmap(hashTable.arr, hashTable.table_capacity * (sizeof(MemNode))) == -1) {
        perror("munmap() status_code");
        std::cout << errno;
        exit(EXIT_FAILURE);
    }
    hashTable.arr = nullptr;
    hashTable.table_capacity = 0;
    hashTable.table_size = 0;
}

void* Allocator::allocate(size_t size) {
    // size_t check against negatives well
    long check_size = (long)size;
    // If size to allocate is less than 0, BAD
    if (check_size <= 0) {
        throw std::runtime_error("Invalid Size Allocation!");
    }
    void* ptr = hashTable.insert(size);
    if (ptr == nullptr){
        std::cout << "Failed to allocate!" << std::endl;
    }
    return ptr;
}

void Allocator::deallocate(void *ptr) {
    if (ptr == nullptr){
        throw std::runtime_error("Cannot Remove Nullptr!");
    }
    if(!hashTable.remove(ptr)){
        std::cout << "failed to remove: " << ptr << std::endl;
    }
}
size_t Allocator::get_size() const{
   return hashTable.getTableSize();
}
size_t Allocator::get_capacity() const {
    return hashTable.getTableCapacity();
}





