//
// Created by Thomas Ford on 3/15/23.
//

#include "HashTable.h"

#define INITIAL_TABLE_CAPACITY 10
#define LOAD_FACTOR 0.5
#define ERROR (-1)
#define DELETED ((void*) -1)
#define PAGE_SIZE 4096

// HashTable Constructor
HashTable::HashTable(){
    table_capacity = INITIAL_TABLE_CAPACITY;
    table_size = 0;
    arr = (MemNode *)mmap(nullptr, table_capacity * (sizeof(MemNode)), PROT_READ | PROT_WRITE,
                                 MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (this->arr == MAP_FAILED){
        perror("mmap() status_code");
        std::cout << errno;
    }
    for (int i = 0; i < table_capacity; i++) {
        arr[i] = MemNode();
    }
}
/**
 * If size of table / table capacity is greater than LOAD_FACTOR expand the array
 */
void HashTable::growTable(){
    size_t new_table_capacity = 2 * table_capacity; // Double capacity
    auto new_arr = (MemNode*)(mmap(nullptr, new_table_capacity * sizeof(MemNode), PROT_READ | PROT_WRITE,
                                       MAP_PRIVATE | MAP_ANONYMOUS, 0, 0));
    if (new_arr == MAP_FAILED) {
        perror("mmap() status_code");
        std::cout << errno;
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < new_table_capacity; i++) {
        new_arr[i] = MemNode();
    }
    size_t old_table_capacity = table_capacity;
    table_capacity = new_table_capacity;
    for (size_t i = 0; i < old_table_capacity; i++) { // Loop through old array and set values of new array
        if (arr[i].address != nullptr && arr[i].address != DELETED) {
            size_t index = hashing(arr[i].address);
            while (new_arr[index].address != nullptr) {
                index = (index + 1) % new_table_capacity;
            }
            new_arr[index] = this->arr[i];
        }
    }
    std::swap(arr, new_arr); // Swap value of variables
    if (munmap(new_arr, old_table_capacity) == ERROR) {
        perror("munmap() status_code");
        std::cout << errno;
        exit(EXIT_FAILURE);
    }
}
/**
 * Allocate memory and add address and size memory allocated to hash array
 * @param size Space needed to allocate
 * @return the address created through allocation
 */
void* HashTable::insert(size_t size){
    if ((double) (table_size + 1) / (double) table_capacity > LOAD_FACTOR) {
        growTable();
    }
    // round up the allocation size to the next multiple of the page size
    size_t blockSize = ((size + (PAGE_SIZE - 1)) / PAGE_SIZE) * PAGE_SIZE;
    void *ptr = mmap(nullptr, blockSize, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap() status_code");
        std::cout << errno;
        return nullptr;
    }
    size_t index = hashing(ptr);
    // Loop though array and find openings to insert values
    while (arr[index].address != nullptr && arr[index].address != DELETED) {
        index = (index + 1) % table_capacity;
    }
    arr[index].address = ptr;
    arr[index].size = blockSize;
    table_size++;
    return ptr;
}
bool HashTable::remove(void *ptr){
    size_t index = hashing(ptr);
    while (arr[index].address != nullptr || arr[index].address == DELETED) {
        if (arr[index].address == ptr) {
            arr[index].address = DELETED;
            if (munmap(ptr, arr[index].size) == ERROR) {
                perror("munmap() status_code");
                std::cout << errno;
                exit(EXIT_FAILURE);
            }
            table_size--;
            return true;
        }
        index = (index + 1) % table_capacity;
    }
    return false;
}

size_t HashTable::hashing(void* key) const {
    return ((size_t)key * 31) % table_capacity;
}

size_t HashTable::getTableCapacity() const {
    return table_capacity;
}

size_t HashTable::getTableSize() const {
    return table_size;
}


