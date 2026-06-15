#include<iostream>
#include<cassert>
#include "MemoryPool.h"

int main(){
    memoryPool<int> pool;

    // Test 1 — LIFO check
    int* a = pool.allocate();
    *a = 42;
    
    pool.deallocate(a);
    int* b = pool.allocate();
    assert(b == a);
    std::cout << "Test 1 passed: slot reuse confirmed" << std::endl;
}