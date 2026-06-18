#include "SpinLock.h"

namespace CoreThread{
    void SpinLock::lock(){
        while(flag.test_and_set()){
            __builtin_ia32_pause(); 
        }
    }

    void SpinLock::unlock(){
        flag.clear();
    }
};