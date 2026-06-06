/*
    The shareUniquePtr is use for managing shared ownership of a pointer, it is a simple implementation of a shared_pointer in c++.
    It is a template class that can manage any type of pointer, and it ensueres shared ownership of a pointer.
    The main features of MySharePtr are:
        1. It takes ownership of a pointer and deletes it when the last MySharePtr goes out of a scope.
        2. It support copy semantics, mutiple ptrs can share ownewrship of the same pounter.
        3. It provides operator* and operator-> to accerss the underlying object naturally.
        4. It uses refrence counting to keep track of how many MySharePtr instances are sharing the same pointer, and deletes the pointer when the count goes to zero.
*/

#include "./MyWeakPtr.h"

namespace coreforge{

    struct ControlBlock{
        int ref_count; //to keep track of how many refrences are there to the same pointer
        int weak_count;

        ControlBlock(): ref_count(1), weak_count(0) {};
    };

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

            ~MySharePtr(){
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

