#pragma once
#include <memory>
#include<iostream>
#include "MyVector.h"

template <typename T>
class MyStack{
    private:
       MyVector<T> _data;
    
    public:
        void push(T val){
            _data.push_back(val);
        }

        void pop(){
            _data.pop_back();
        }

        T& top(){
            return _data.back();
        }

        bool empty(){
            return _data.empty();
        }

        size_t size(){
            return _data.size();
        }
};
