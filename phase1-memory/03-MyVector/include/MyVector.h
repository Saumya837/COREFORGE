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
            int new_capacity = (_capacity_end == _start) ? 1 : 2 * (_capacity_end - _start);

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
        ~MyVector(){
            if(_start == nullptr) return;  

            // need to destroy old objects first before freeing memory
            for (T* p = _start; p != _end; ++p)
                std::destroy_at(p);

            _alloc.deallocate(_start, _capacity_end - _start);
        }


        void resize(int n){
            if(n > size()){
                reserve(n);
                std::uninitialized_fill(_end, _start + n, T{});
            }
            else{
                //Shrinking to n size
                for (T* p = _start + n; p != _end; ++p)
                    std::destroy_at(p);
            }
            _end = _start + n;
        }

   

    // element access
    T& operator[](int i){
        int num_ele = _end - _start;
        if(i < 0 || i >= num_ele){
            throw std::runtime_error("Index out of Bounds");
        }
        return *(_start + i);
    }

    T& front(){
        if(empty()) 
            throw std::runtime_error("Vector is Empty");
        return *(_start);
    }

    T& back(){
        if(empty()) 
            throw std::runtime_error("Vector is Empty");
        return *(_end - 1);
    }

    // modifiers
    void push_back(T val){
        if(_end == _capacity_end){
                _resize_internal();
        }
        //memory is uninitialized, construct properly
        new (_end) T(val); 
        _end++;
    }

    //need to learn perfect forwarding planned for weekends
    void emplace_back() = delete;

    void pop_back(){
        if(empty()) 
            throw std::runtime_error("Vector is Empty");
        std::destroy_at(_end - 1);
        _end--;
    }

    void clear(){
        if (empty()) return;
        for(T* p = _start; p <_end; p++){
            std::destroy_at(p);
        }
        _end = _start;
    }

    // size & capacity
    size_t size(){
        return _end - _start;
    }

    size_t capacity(){
        return _capacity_end - _start;
    }

    bool empty(){
        return _start == _end;
    }

    void reserve(int n){
        if(capacity() >= n) return;
        else{
           int old_size = _end - _start;
           T* newVec =  _alloc.allocate(n);

           //copy all the elements in the newArr
           std::uninitialized_copy(_start, _end, newVec);

           for (T* p = _start; p < _end; p++){
                std::destroy_at(p);
           }

           _alloc.deallocate(_start, _capacity_end - _start);

           _start = newVec;
           _end = _start + old_size;
           _capacity_end = _start + n;
        }
    }

    // iterators
    T* begin(){
        return _start;
    }
    T* end(){
        return _end;
    }
};