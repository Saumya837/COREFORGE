#include "MyVector.h"
#include <iostream>
#include <cassert>

int main(){
    // test 1: default constructor
    MyVector<int> d;

    std::cout<<"Test1: empty test passed"<<std::endl;
    assert(d.empty() == true);
    
    // test 1: push_back
    d.push_back(1);
    assert(d.front() == 1);
    std::cout<<"Test2: Push_back test passed"<<std::endl;
    
    d.push_back(2);
    assert(d.front() == 1);
    assert(d.back() == 2);
    std::cout<<"Test3: Push_back test passed"<<std::endl;;
    // d.push_back(3);
    
    // // test 3: push_front
    d.push_back(0);
    assert(d.back() == 0);
    std::cout<<"Test4: Push_front test passed"<<std::endl;

    
    // Testing iterator
    int expected[] = {1, 2, 0};
    int i = 0;
    for(auto itr = d.begin(); itr != d.end(); ++itr){
        assert(*itr == expected[i++]);
    }

    d.pop_back();
    assert(d.back() == 2);
    std::cout<<"Test5: Pop_back test passed"<<std::endl;

    d.pop_back();
    assert(d.size() == 1);
    std::cout<<"Test6: size test passed"<<std::endl;

    d.pop_back();
    assert(d.size() == 0);
    std::cout<<"Test7: size test passed"<<std::endl;


    try {
        d.pop_back();
        assert(false && "should have thrown on empty pop");
    } catch(const std::runtime_error& e) {
        std::cout << "Test8: empty pop_back throws correctly" << std::endl;
    }

    std::cout << "all tests passed!" << std::endl;
}