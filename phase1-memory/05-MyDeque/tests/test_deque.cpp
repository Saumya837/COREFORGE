#include "MyDeque.h"
#include <cassert>
#include <iostream>

int main(){
    // test 1: default constructor
    MyDeque<int> d;
    
    // test 1: push_back
    d.push_back(1);
    assert(d.front() == 1);
    std::cout<<"Test1: Push back with single elment from a nullptr passed"<<std::endl;
    
    d.push_back(2);
    assert(d.front() == 1);
    assert(d.tail() == 2);
    std::cout<<"Test2: Push_back test passed"<<std::endl;;
    // d.push_back(3);
    
    // // test 3: push_front
    d.push_front(0);
    d.push_front(-1);
    assert(d.front() == -1);
    std::cout<<"Test3: Push_front test passed"<<std::endl;

    // test 4: pop_back
    d.pop_front();  // removes -1
    assert(d.front() == 0);
    std::cout<<"Test4: Pop_front test passed"<<std::endl;
    
    // test 5: pop_back
    // d.pop_back(); // removes 2
    d.pop_back();
    assert(d.tail() == 1);
    std::cout<<"Test5: Pop_back test passed"<<std::endl;
    
    // Testing iterator

        int expected[] = {0, 1};
        int i = 0;
        for(auto itr = d.begin(); itr != d.end(); ++itr){
            assert(*itr == expected[i++]);
        }
        std::cout << "all tests passed!" << std::endl;
}