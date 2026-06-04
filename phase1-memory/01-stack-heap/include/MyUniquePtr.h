#include<iostream>

namespace codeforge{

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
            };


            //get the raw pointer
            T* get(){
                return raw_ptr;
            }

           // derefrence like a real pointer
            T& operator*(){
                return *raw_ptr;
            }

            // arrow operator like a real pointer
            T* operator->(){
                return raw_ptr;
            }

            //DELETE copy constructor and copy assignment
            // this is what makes it UNIQUE - only one owner
            MyUniquePtr(const MyUniquePtr&) = delete;
            MyUniquePtr& operator = (const MyUniquePtr&) = delete;
    };

};


