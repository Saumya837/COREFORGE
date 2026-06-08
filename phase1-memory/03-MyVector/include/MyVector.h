#pragma once
#include<memory>
#include<iostream>

template <typename T>
class MyVector{
    private:
        T* _start;
        T* _end;
        T* _capacity_end;
        std::allocator<T> _alloc;

        void _resize_internal(){
            int num_elemets = _end - _start;
            int new_capacity = 2 * (_capacity_end - _start);

            T* newArr = _alloc.allocate(new_capacity);

            //copy all the elements in the newArr
            std::uninitialized_copy(_start, _end, newArr);

            // need to destroy old objects first before freeing memory
            for (T* p = _start; p != _end; ++p)
                std::destroy_at(p);

            //deallocating the old space
            _alloc.deallocate(_start, _capacity_end - _start);

            //updating the parameters _start, _end, _capacity_end
            _start = newArr;
            _end = _start + num_elemets;
            _capacity_end = _start + new_capacity;
        }

    public:

        MyVector() : _start(nullptr), _end(nullptr), _capacity_end(nullptr){}; 
        MyVector(int n):  _start(n > 0 ? _alloc.allocate(n) : nullptr),
                  _end(_start + n),
                  _capacity_end(_start + n){
            if (n > 0)
                std::uninitialized_fill(_start, _end, T{});
        }

        MyVector(int n, T val):  _start(n > 0 ? _alloc.allocate(n) : nullptr),
                  _end(_start + n),
                  _capacity_end(_start + n){
            if (n > 0)
                std::uninitialized_fill(_start, _end, val);
        }

        // destructor
        ~MyVector();

        

        // resize(int n){
        //     //Todo: first look if we sufficiant space 
        //     //Todo: if not call resize_internals to allocate 2*n size
        //     //Todo: change _end to nth element
        //     //Todo: later increase size assigning _size = _end - _start
        // }

        // push_back(int n){

        // }

    // element access
    T& operator[](int i);
    T& at(int i);
    T& front();
    T& back();

    // modifiers
    void push_back(T val);
    void emplace_back();
    void pop_back();
    void clear();

    // size & capacity
    size_t size(){
        return _end - _start;
    }

    size_t capacity(){
        return _capacity_end - _start;
    }

    bool empty(){
        if(_end == _start)
            return true;
        return false;
    }

    void reserve(int n);

    // iterators
    T* begin();
    T* end();
};