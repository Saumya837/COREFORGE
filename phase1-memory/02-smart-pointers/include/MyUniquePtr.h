/*
    MyUniquePtr is a simple implementation of a unique pointer in C++.
    It is a template class that can manage any type of heap pointer,
    and ensures single ownership at any time.

    Features:
        1. Takes ownership of a heap pointer and deletes it
           when MyUniquePtr goes out of scope.

        2. Supports move semantics — ownership can be transferred
           but never copied.

        3. Provides operator* and operator-> to access
           the underlying object naturally.

        4. Copy constructor and copy assignment are deleted
           to enforce unique ownership.
*/

#pragma once

namespace coreforge{

    template<typename T>
    /*
        Template T is the genric type of the pointer we manage, 
        without this you have diffrent MyUniquePtr for each Type, e.g for int
        you will have MyUniquePtr<int> and for double you will have MyUniquePtr<double>
    */ 

    class MyUniquePtr{
        private: 
            T* raw_ptr; 

        public:
            //Constructor takes the ownership of a heap pointer
            explicit MyUniquePtr(T* ptr) : raw_ptr(ptr) {};

            ~MyUniquePtr(){
                delete raw_ptr;
            }

            /* && operator is used when it is not needed by any one
             else and we want to tansfer the ownership
            */
            MyUniquePtr(MyUniquePtr&& other) noexcept : raw_ptr(other.raw_ptr){
                other.raw_ptr = nullptr;
            }


            //get the raw pointer
            T* get() const{
                return raw_ptr;
            }

           // derefrence like a real pointer
            T& operator*() const {
                return *raw_ptr;
            }

            // arrow operator like a real pointer
            T* operator->() const {
                return raw_ptr;
            }

            MyUniquePtr& operator=(MyUniquePtr&& other) noexcept{
                if(this != &other){
                    delete raw_ptr;
                    raw_ptr = other.raw_ptr;
                    other.raw_ptr = nullptr;
                }
                return *this;
            }
            
           
            //DELETE copy constructor and copy assignment
            // this is what makes it UNIQUE - only one owner
            MyUniquePtr(const MyUniquePtr&) = delete;
            MyUniquePtr& operator = (const MyUniquePtr&) = delete;
    };

};


