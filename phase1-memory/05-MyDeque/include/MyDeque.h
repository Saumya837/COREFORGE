#pragma once
#include <memory>
#include<iostream>
template <typename T>
class MyDeque{
    private:
        T** _map_start; // map start address which hold the pointer start address
        T** _map_end; //map start address which hold the pointer start address
        T** _data_start;
        T** _data_end;

        const static int CHUNK_SIZE = 8; 
        std::allocator<T> _alloc;
        std::allocator<T*> _map_alloc;

        void _resize_map_with_recentering(){
            int num_elements = _data_end - _data_start;
            int new_capacity = (_map_start == _map_end) ? 2 : 2 * (_map_end - _map_start);

            T** newDeque = _map_alloc.allocate(new_capacity);

            int recenter_point = num_elements >> 1;

            //copy all the elements in the newArr with recentrin
            std::copy(_data_start, _data_end, (newDeque + recenter_point));

            //deallocating the old space
            _map_alloc.deallocate(_map_start, _map_end - _map_start);

            _map_start = newDeque;
            _data_start = _map_start + recenter_point;
            _data_end = _data_start + num_elements;
            _map_end = _map_start + new_capacity;

        };

        void _recenter_backward(){
            if (_map_start == _map_end){
                _resize_map_with_recentering();
                return;
            }

            if(_map_start == _data_start && _map_end == _data_end){
                _resize_map_with_recentering();
                return;
            }
            
            int num_free_blocks = (_map_end - _data_end);
            int shift = (num_free_blocks + 1) >> 1; // (3 + 1) = 2 shift backward will give me more space in front

            std::copy_backward(_data_start, _data_end, _data_end + shift);

            _data_start += shift;
            _data_end += shift;

        }

        void _recenter_forward(){
            if (_map_start == _map_end){
                _resize_map_with_recentering();
                return;
            }

            if(_map_start == _data_start && _map_end == _data_end){
                _resize_map_with_recentering();
                return;
            }
            
            int num_free_blocks = (_data_start - _map_start);
            int shift = (num_free_blocks + 1) >> 1; // (3 + 1) = 2 shift forward will give me more space in front

            std::copy(_data_start, _data_end, _data_start - shift);

            _data_start -= shift;
            _data_end -= shift;
        }


    public:
        struct DequeIterator {
            T*  _cur;
            T*  _chunk_start;
            T*  _chunk_end;
            T** _map_ptr;

            void next(){
                _cur++;
                if(_cur == _chunk_end){
                    _map_ptr++;
                    _chunk_start = *(_map_ptr);
                    _chunk_end = _chunk_start + CHUNK_SIZE;
                    _cur = _chunk_start;
                }
            }

            void prev(){
                if(_cur == _chunk_start){
                    _map_ptr--;
                    _chunk_start = *(_map_ptr);
                    _chunk_end = _chunk_start + CHUNK_SIZE;
                    _cur = _chunk_end;
                }
                _cur--;
            }
        };

        MyDeque():_map_start(nullptr), _map_end(nullptr), _data_start(nullptr), _data_end(nullptr){
        }

        MyDeque(int size){};

        ~MyDeque(){
            if(_map_start == nullptr) return;

            for(T** p = _data_start; p != _data_end; p++){
                for(int i = 0; i< CHUNK_SIZE; i++)
                    std::destroy_at(*p + i);
                _alloc.deallocate(*p, CHUNK_SIZE);
            }

            _map_alloc.deallocate(_map_start, _map_end - _map_start);
        }

        void push_back();
        void push_front();
        void pop_back();
        void pop_front();
};