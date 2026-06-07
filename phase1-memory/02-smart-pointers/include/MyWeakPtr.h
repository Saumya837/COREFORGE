/*
    MyWeakPtr - A custom implementation of std::weak_ptr in C++

    Core Concept:
        Observes a shared pointer without owning it.
        Does not keep the data alive — data lives and dies with MySharedPtr.
        Temporary ownership can be borrowed via lock(), which returns a MySharedPtr.

    Ownership Model:
        - ref_count  → tracks how many MySharedPtr instances own the data
        - weak_count → tracks how many MyWeakPtr instances observe the data
        - Both counts live in a shared ControlBlock on the heap

    Memory Management:
        - weak_ptr never increments or decrements ref_count
        - weak_ptr only increments/decrements weak_count
        - data deleted when ref_count hits 0
        - ControlBlock deleted when both ref_count and weak_count hit 0

    Features:
        1. constructor from MySharedPtr  → observe without owning
        2. lock()                        → borrow temporary ownership, returns MySharedPtr
        3. expired()                     → check if data is still alive
        4. reset()                       → stop observing

    Design Decisions:
        - lock() throws on nullptr weak_ptr  → developer mistake
        - lock() returns empty MySharedPtr   → valid expired case
        - use_count() removed               → use lock().use_count() instead
*/

#pragma once
#include<memory>
#include "ControlBlock.h"


namespace coreforge{
    template<typename T> class MySharePtr;

    template <typename T> 
    class MyWeakPtr{
        private:
            T* raw_ptr;
            ControlBlock* c;

        public:
            //default Constructor
            MyWeakPtr(): raw_ptr(nullptr), c(nullptr) {};

            //With SharedPtr passed
            MyWeakPtr(const MySharePtr<T>& shr): raw_ptr(shr.raw_ptr), c(shr.c) {
                c->weak_count++;
            };

            MySharePtr<T> lock(){
                if(c == nullptr){
                    throw std::runtime_error("lock called on empty WeakPtr");
                }
                else if(c->ref_count > 0){
                    return MySharePtr<T>(raw_ptr, c);
                }
                return MySharePtr<T>(nullptr); // expired
            }

            bool expired(){
                if(c == nullptr){
                    // only if expiry checking on sharedNullptr
                    return true;
                }
                else if(c->ref_count == 0){
                    return true;
                }
                else{
                    return false;
                }
            }

            int usage_count(){
                return c->ref_count;
            }

            void reset(){
                if(c == nullptr) return;
                c->weak_count--;
                raw_ptr = nullptr;
                c = nullptr;
            }
    };
};