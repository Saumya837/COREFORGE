#pragma once
#include <memory>
#include<iostream>
template <typename T>
class MyDeque{
    public:
        struct DequeIterator {
            T*  _cur;
            T*  _chunk_start;
            T*  _chunk_end;
            T** _map_ptr;

            DequeIterator():_cur(nullptr), _chunk_start(nullptr), _chunk_end(nullptr), _map_ptr(nullptr){};

            DequeIterator(T** map_ptr):_cur(*map_ptr), _chunk_start(*map_ptr), _chunk_end(*map_ptr + CHUNK_SIZE), _map_ptr(map_ptr){};

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

    private:
        DequeIterator _begin;
        DequeIterator _end;
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

            //If dequeue is not empty
            if(_data_start != nullptr){
                //copy all the elements in the newArr with recentrin
                std::copy(_data_start, _data_end, (newDeque + recenter_point));

                //deallocating the old space
                _map_alloc.deallocate(_map_start, _map_end - _map_start);
            }

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

        void _check_and_grow_map_back(){
            if(_data_end == _map_end){
                if(_data_start > _map_start)
                    _recenter_forward();
                else
                    _resize_map_with_recentering();
            }
        }

        void _check_and_grow_map_front(){
            if(_data_start == _map_start){
                if(_data_end < _map_end)
                    _recenter_backward();
                else
                    _resize_map_with_recentering();
            }
        }



    public:

        MyDeque():_map_start(nullptr), _map_end(nullptr), _data_start(nullptr), _data_end(nullptr), _begin(nullptr), _end(nullptr){
        }

        MyDeque(int size): MyDeque(int size, T val) {}

        MyDeque(int size, T val): _map_start(nullptr), _map_end(nullptr), _data_start(nullptr), _data_end(nullptr){
            int num_chunks = (size + CHUNK_SIZE - 1)/CHUNK_SIZE;
            int map_capacity = num_chunks << 1;
            _map_start = _map_alloc.allocate(map_capacity);
            _map_end = _map_start + map_capacity;

            //putting the data from _data_start
            _data_start = _map_start + (num_chunks >> 1);
            _data_end = _data_start + num_chunks;

            for(T** p = _data_start; p != _data_end; p++){
                *p = _alloc.allocate(CHUNK_SIZE);
                std::uninitialized_fill(*p, *p + CHUNK_SIZE, val);
            }

            _begin = DequeIterator(_data_start);
            _end = DequeIterator(_data_end - 1);

            // handle partial last chunk
            int remaining = size % CHUNK_SIZE;
            if(remaining == 0)
                _end._cur = _end._chunk_end;
            else
                _end._cur = _end._chunk_start + remaining;
           
        }

        ~MyDeque(){
            if(_map_start == nullptr) return;

            for(T** p = _data_start; p != _data_end; p++){
                for(int i = 0; i< CHUNK_SIZE; i++)
                    std::destroy_at(*p + i);
                _alloc.deallocate(*p, CHUNK_SIZE);
            }
            _map_alloc.deallocate(_map_start, _map_end - _map_start);
        }

        DequeIterator begin() {
            return _begin;  
        }

        DequeIterator end() {
            return _end;  
        }


        void push_back(T val){
            if(_map_start == nullptr){
                _resize_map_with_recentering();
                *(_data_start) = _alloc.allocate(CHUNK_SIZE);
                _data_end = _data_start + 1;

                _begin = DequeIterator(_data_start);
                _end = DequeIterator(_data_start);
                _begin._cur = *(_data_start);
                _end._cur = *(_data_start);
            }

            else if(_end._cur == _end._chunk_end){
                if(_data_end == _map_end)
                    _check_and_grow_map_back();
                *(_data_end) = _alloc.allocate(CHUNK_SIZE);
                _data_end++;
                _end = DequeIterator(_data_end - 1);
            }

            std::construct_at(_end._cur, val);
            _end._cur++;
        }

    
        void push_front(T val){
            if(_map_start == nullptr){
                _resize_map_with_recentering();
                *(_data_start) = _alloc.allocate(CHUNK_SIZE);
                _data_end = _data_start + 1;

                _begin = DequeIterator(_data_start);
                _end = DequeIterator(_data_start);
                _end._cur = *(_data_start);
            }

            else if(_begin._cur == _begin._chunk_start){
                if(_data_start == _map_start)
                    _check_and_grow_map_front();
                _data_start--;
                *(_data_start) = _alloc.allocate(CHUNK_SIZE);
                _begin = DequeIterator(_data_start);
                _begin._cur = _begin._chunk_end;
            }
            _begin._cur--;
            std::construct_at(_begin._cur, val);
        }

        void pop_back(){
            if(_map_start == nullptr) return;

            _end._cur--;
            std::destroy_at(_end._cur);
            //last element in the chunk
            if(_end._cur == _end._chunk_start){
                //deallocate the chunk
                _alloc.deallocate(*(_data_end - 1), CHUNK_SIZE);
                _data_end--;

                _end = DequeIterator(_data_end - 1);
                _end._cur = _end._chunk_end;
            }
        }
        
        void pop_front(){
            if(_map_start == nullptr) return;

            std::destroy_at(_begin._cur);
            _begin._cur++;

            if(_begin._cur == _begin._chunk_end){
                _alloc.deallocate(*(_data_start), CHUNK_SIZE);
                _data_start++;

                _begin = DequeIterator(_data_start);
                _begin._cur = _begin._chunk_start;
            }
        }
};