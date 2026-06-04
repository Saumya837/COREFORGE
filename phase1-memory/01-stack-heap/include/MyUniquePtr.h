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
            }

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

            /* && operator is used when it is not needed by any one
             else and we want to tansfer the ownership
            */
           
            //DELETE copy constructor and copy assignment
            // this is what makes it UNIQUE - only one owner
            MyUniquePtr(const MyUniquePtr&) = delete;
            MyUniquePtr& operator = (const MyUniquePtr&) = delete;
    };

};


