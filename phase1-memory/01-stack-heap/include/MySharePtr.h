/*
    The shareUniquePtr is use for managing shared ownership of a pointer, it is a simple implementation of a shared_pointer in c++.
    It is a template class that can manage any type of pointer, and it ensueres shared ownership of a pointer.
    The main features of MySharePtr are:
        1. It takes ownership of a pointer and deletes it when the last MySharePtr goes out of a scope.
        2. It support copy semantics, mutiple ptrs can share ownewrship of the same pounter.
        3. It provides operator* and operator-> to accerss the underlying object naturally.
        4. It uses refrence counting to keep track of how many MySharePtr instances are sharing the same pointer, and deletes the pointer when the count goes to zero.
*/

namespace codespace{
    template <typename T>
    class MySharePtr{
        private:
                T* raw_ptr;
                int* ref_count; //to keep track of how many refrences are there to the same pointer

        public:
            //constructor takes ownership of a pointer
            explicit MySharePtr(T* ptr): raw_ptr(ptr), ref_count(new int(1)) {};

            ~MySharePtr(){
                (*ref_count)--;
                if(*ref_count == 0){
                    delete raw_ptr;
                    delete ref_count;
                }
            }

            //copy constructor for shared ownership
            MySharePtr(const MySharePtr& other) : raw_ptr(other.raw_ptr), ref_count(other.ref_count){
                (*ref_count)++;
            }

            T* get() const {
                return raw_ptr;
            }

            T& operator*() const {
                return *raw_ptr;
            }

            T* operator->() const {
                return raw_ptr;
            }

            //copy assignment operator for shared ownership
            MySharePtr& operator=(const MySharePtr& other){
                if(this != &other){
                    //decrease the refcount of current pointer
                    (*ref_count)--;
                    if(*ref_count == 0){
                        delete raw_ptr;
                        delete ref_count;
                    }
                    //copying the new pointer and ref count
                    raw_ptr = other.raw_ptr;
                    ref_count = other.ref_count;
                    (*ref_count)++;
                }
                return *this;
            } 
    };
};

