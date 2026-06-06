
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
                if(c->ref_count > 0){
                    return MySharePtr(raw_ptr, c);
                }
            }

            bool lock_acuired(){
                //Todo: 
            }
    };
};