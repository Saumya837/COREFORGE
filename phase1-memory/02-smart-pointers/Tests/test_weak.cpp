#include <iostream>
#include "MySharePtr.h"
#include "MyWeakPtr.h"
#include <cassert>

using namespace coreforge;


int main(){
    MySharePtr<int> p1(new int(43));

    //Test 1: constructor with shared ptr passed
    MyWeakPtr<int> w1(p1);
    assert(p1.use_count() == 1);

    //Test 2: constructor with lock acquiring
    MySharePtr<int> p2 = w1.lock();
    assert(p1.use_count() == 2);

    //Test 3: destruction of weak ptr andits shared cptr acquired after lock
    {
        MyWeakPtr<int> w2(p1);
        MySharePtr<int> p3 = w2.lock();
        assert(p1.use_count() == 3);
    }
    assert(p1.use_count() == 2);


    // Test 4: expired() returns true when shared_ptr dies
    MyWeakPtr<int> w3;
    {
        MySharePtr<int> p4(new int(99));
        w3 = MySharePtr<int>(p4);  // observe p4
        assert(w3.expired() == false);
    }  // p4 dies here
    assert(w3.expired() == true);


    std::cout<<"All Test passed";
}