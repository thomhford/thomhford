//
// Created by Thomas Ford on 3/16/23.
//

#include <chrono> // Needed for capturing current time
#include "allocator.h"
#include "catch.h"

TEST_CASE("MallocReplacement"){
    SECTION("grow_table"){
        Allocator a = Allocator();
        bool* boolArr[5];
        // Initial capacity of table is 10 and size 0
        CHECK(a.get_capacity() == 10);
        CHECK(a.get_size() == 0);
        for (auto &Bool: boolArr) {
            Bool = (bool *) a.allocate(sizeof(bool));
        }
        // Hash Table has a load factor of 0.5 so should not yet have grown. Size = 5
        CHECK(a.get_capacity() == 10);
        CHECK(a.get_size() == 5);
        bool add1 =(bool*)a.allocate(sizeof(bool));
        // Hash Table now should have grown as 6/10 > .5. Capacity should have doubled with growth. Size = 6
        CHECK(a.get_capacity() == 20);
        CHECK(a.get_size() == 6);

    }
    SECTION("check values"){
        Allocator a = Allocator();
        int var_length = 100;
        // Array of ints
        int *ptr;
        // Allocate space for array
        ptr = (int*) a.allocate(sizeof(int)*var_length);
        CHECK(a.get_capacity() == 10);
        CHECK(a.get_size() == 1);
        // Set values of array
        for (int i = 0; i < var_length; i++) {
            *(ptr+i) = i;
        }
        // Check values in array are what was added
        for(int i = 0; i < var_length; i++)
        {
            CHECK(*(ptr+i) == i);
        }
    }
    SECTION("allocate/deallocate bool"){
        Allocator a = Allocator();
        int size = 100;
        // Array of bool
        bool *ptr[100];
        CHECK(a.get_capacity() == 10);
        CHECK(a.get_size() == 0);
        // Allocate memory and assign values
        for (int i = 0; i < size; i++) {
            ptr[i] = (bool*) a.allocate(sizeof(bool) * size);
            *ptr[+i] = false;
        }
        // Check grow and size
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 100);
        // Check values are assigned in array of bool
        for(int i = 0; i < size; i++)
        {
            CHECK(*ptr[i] == false);
        }
        // Remove half of allocated values
        for (int i = size/2; i < size; ++i) {
            a.deallocate(ptr[i]);
        }
        // Check that half of values were removed from table
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 50);
        // Check that remaining values are correct
        for(int i = 0; i < size/2; i++)
        {
            CHECK(*ptr[i] == false);
        }
        // Remove the rest of the values
        for (int i = 0; i < size/2; ++i) {
            a.deallocate(ptr[i]);
        }
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 0);
    }

    SECTION("allocate/deallocate ints") {
        int size = 100;
        Allocator a = Allocator();
        // Array of ints
        int *ptr[size];
        CHECK(a.get_capacity() == 10);
        CHECK(a.get_size() == 0);
        // Allocate space and assign values
        for (int i = 0; i < size; ++i) {
            ptr[i] = (int *) a.allocate(sizeof(int));
            *ptr[i] = i;
        }
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 100);
        // Check assigned values are correct
        for(int i = 0; i < size; i++)
        {
            CHECK(*ptr[i] == i);
        }
        // Remove half of values
        for (int i = size/2; i < size; ++i) {
            a.deallocate(ptr[i]);
        }
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 50);
        // Check the values are still correct
        for(int i = 0; i < size/2; i++)
        {
            CHECK(*ptr[i] == i);
        }
        // Remove rest of values
        for (int i = 0; i < size/2; ++i) {
            a.deallocate(ptr[i]);
        }
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 0);
    }
    SECTION("allocate/deallocate char") {
        int size = 100;
        Allocator a = Allocator();
        // Array of chars
        char *ptr[size];
        CHECK(a.get_capacity() == 10);
        CHECK(a.get_size() == 0);
        // Allocate and set
        for (int i = 0; i < size; ++i) {
            ptr[i] = (char *) a.allocate(sizeof(char));
            *ptr[i] = (char)i;
        }
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 100);
        // Check values set
        for(int i = 0; i < size; i++)
        {
            CHECK(*ptr[i] == i);
        }
        for (int i = size/2; i < size; ++i) {
            a.deallocate(ptr[i]);
        }
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 50);
        // Check remaining values are correct
        for(int i = 0; i < size/2; i++)
        {
            CHECK(*ptr[i] == i);
        }
        for (int i = 0; i < size/2; ++i) {
            a.deallocate(ptr[i]);
        }
        // Check that all was deallocated
        CHECK(a.get_capacity() == 320);
        CHECK(a.get_size() == 0);
    }
    SECTION("allocate/deallocate large"){
        Allocator a = Allocator();
        int size = 100000;
        // Array of bool
        bool *ptr[size];
        CHECK(a.get_capacity() == 10);
        CHECK(a.get_size() == 0);
        // Allocate memory and assign values
        for (int i = 0; i < size; i++) {
            ptr[i] = (bool*) a.allocate(sizeof(bool) * size);
            *ptr[+i] = false;
        }
        // Check grow and size
        CHECK(a.get_capacity() == 327680);
        CHECK(a.get_size() == 100000);

        // Remove allocated values
        for (int i = 0; i < size; ++i) {
            a.deallocate(ptr[i]);
        }
        // Check that values were removed from table
        CHECK(a.get_capacity() == 327680);
        CHECK(a.get_size() == 0);
    }
    // Check attempt to deallocate nullptr results in exception
    SECTION("remove nullptr"){
        Allocator a = Allocator();
        REQUIRE_THROWS(a.deallocate(nullptr));
    }
    // Check attempt to deallocate address not in HashTable
//    SECTION("remove previously removed address"){
//        Allocator a = Allocator();
//        int* ptr;
//        ptr = (int*) a.allocate(sizeof(int));
//        a.deallocate(ptr);
//        a.deallocate(ptr);
//    }
    // Check attempt to allocate negative size results in exception
    SECTION("add invalid"){
        Allocator a = Allocator();
        REQUIRE_THROWS(a.allocate(-1));
    }
}



TEST_CASE("MallocReplacement Timing"){
    std::cout << "\nMalloc Replacement Timing:" << std::endl;
    uint64_t ITER_COUNT = 100;
    auto startTime = std::chrono::high_resolution_clock::now();

    while ( (std::chrono::high_resolution_clock::now() - startTime).count() < 1000000000);

    uint64_t totalTime = 0;
    int size = 100;

    for (int i = 0; i < 10; i++) {
        for (int iter = 0; iter < ITER_COUNT; iter++) {
            Allocator b = Allocator();

            // TIME IT!
            auto start = std::chrono::high_resolution_clock::now();

            int *intArr[size];
            for (auto &Int: intArr) {
                Int = (int *) b.allocate(sizeof(int));
            }
            for (auto &Int: intArr) {
                b.deallocate(Int);
            }

            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            totalTime += duration.count();
        }
        uint64_t averageTime = totalTime / ITER_COUNT;
        std::cout << size << " " << averageTime << " <allocations, time(microseconds)>" << std::endl;
        size += 100;
    }
    std::cout << std::endl;
}

TEST_CASE("Malloc Timing"){
    std::cout << "Malloc Timing:" << std::endl;
    uint64_t ITER_COUNT = 100;
    auto startTime = std::chrono::high_resolution_clock::now();

    while ( (std::chrono::high_resolution_clock::now() - startTime).count() < 1000000000);

    uint64_t totalTime = 0;
    int size = 100;

    for (int i = 0; i < 10; i++) {
        for (int iter = 0; iter < ITER_COUNT; iter++) {
            // TIME IT!
            auto start = std::chrono::high_resolution_clock::now();

            int *intArr[size];
            for (auto &Int: intArr) {
                Int = (int *) malloc(sizeof(int));
            }
            for (auto &Int: intArr) {
                free(Int);
            }

            auto stop = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
            totalTime += duration.count();
        }
        uint64_t averageTime = totalTime / ITER_COUNT;
        std::cout << size << " " << averageTime << " <allocations, time(microseconds)>" << std::endl;
        size += 100;
    }
    std::cout << std::endl;
}