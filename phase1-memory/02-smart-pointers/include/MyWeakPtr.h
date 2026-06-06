
#pragma once
#include<memory>
#include "ControlBlock.h"


namespace codeforge{
    template<typename T> class MySharePtr;

    template <typename T>
    class MyWeakPtr{
        private:
            T* raw_ptr;
            ControlBlock* c;

        public:
            //default Constructor
            MyWeakPtr(): raw_ptr(nullptr), c(nullptr) {};

            //With SharedPtr passed
            MyWeakPtr(const MySharePtr<T>& shr): raw_ptr(shr.raw_ptr), c(shr.c) {
                c->weak_count++;
            };

            MySharePtr<T> lock(){
                if(c == nullptr){
                    throw std::runtime_error("lock called on empty WeakPtr");
                }
                else if(c->ref_count > 0){
                    return MySharePtr<T>(raw_ptr, c);
                }
                return MySharePtr<T>(nullptr); // expired
            }

            bool expired(){
                if(c == nullptr){
                    // only if expiry checking on sharedNullptr
                    return true;
                }
                else if(c->ref_count == 0){
                    return true;
                }
                else{
                    return false;
                }
            }

            int usage_count(){
                return c->ref_count;
            }

            void reset(){
                if(c == nullptr) return;
                c->weak_count--;
                raw_ptr = nullptr;
                c = nullptr;
            }
    };
};