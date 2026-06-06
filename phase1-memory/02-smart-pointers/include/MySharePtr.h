/*
    MySharedPtr - A custom implementation of std::shared_ptr in C++

    Core Concept:
        Manages shared ownership of a heap pointer using reference counting.
        Multiple MySharedPtr instances can own the same pointer.
        Data is deleted only when the last owner goes out of scope.

    Ownership Model:
        - ref_count  → tracks how many MySharedPtr instances own the data
        - weak_count → tracks how many MyWeakPtr instances observe the data
        - Both counts live in a shared ControlBlock on the heap

    Memory Management:
        - data deleted when ref_count hits 0
        - ControlBlock deleted when both ref_count and weak_count hit 0
        - This separation allows MyWeakPtr to safely check if data is alive

    Features:
        1. constructor      → takes ownership, initializes ref_count to 1
        2. destructor       → decrements ref_count, deletes when last owner dies
        3. copy constructor → shares ownership, increments ref_count
        4. copy assignment  → releases current, takes new ownership
        5. get()            → returns raw pointer without transferring ownership
        6. operator*        → dereference like a real pointer
        7. operator->       → member access like a real pointer
        8. use_count()      → how many shared owners exist

    Friend:
        MyWeakPtr has access to private constructor
        used by MyWeakPtr::lock() to borrow temporary ownership
*/

#pragma once
#include "MyWeakPtr.h"
#include "ControlBlock.h"

namespace coreforge{

    template <typename T>
    class MySharePtr{
        template<typename U> friend class MyWeakPtr;
        private:
                T* raw_ptr;
                ControlBlock *c;

                // only for MyWeakPtr::lock()
                MySharePtr(T* ptr, ControlBlock* block): raw_ptr(ptr), c(block) {
                    c->ref_count++;
                };

                
        public:
            //constructor takes ownership of a pointer
            explicit MySharePtr(T* ptr): raw_ptr(ptr), c(new ControlBlock()) {};

            // empty shared_ptr — owns nothing
            MySharePtr() : raw_ptr(nullptr), c(nullptr) {};

            ~MySharePtr(){
                if(c == nullptr) return;
                (c->ref_count)--;

                if(c->ref_count == 0){
                    delete raw_ptr;
                    raw_ptr = nullptr;
                    if(c->weak_count == 0){
                        delete c;
                    }
                }
            }

            //copy constructor for shared ownership
            MySharePtr(const MySharePtr& other) : raw_ptr(other.raw_ptr), c(other.c){
                (c->ref_count)++;
            }

            //lock is granted for weakPtr


            T* get() const {
                return raw_ptr;
            }

            T& operator*() const {
                return *raw_ptr;
            }

            T* operator->() const {
                return raw_ptr;
            }

            int use_count() const {
                return c->ref_count;
            }

            //copy assignment operator for shared ownership
            MySharePtr& operator=(const MySharePtr& other){
                if(this != &other){
                    //decrease the refcount of current pointer
                    (c->ref_count)--;

                    if(c->ref_count == 0){
                        delete raw_ptr;
                        raw_ptr = nullptr;
                        if(c->weak_count == 0){
                            delete c;
                        }
                    }
                    //copying the new pointer and ref count
                    raw_ptr = other.raw_ptr;
                    c = other.c;
                    (c->ref_count)++;
                }
                return *this;
            } 
    };
};

