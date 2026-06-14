#include <iostream>
#include <cassert>
#include "MyStack.h"

int main(){
    MyStack<int> st;

    st.push(1);
    assert(st.top() == 1);
    std::cout<<"Test1: Push in stack Passed"<<std::endl;

    st.push(2);
    st.push(3);
    st.pop();
    assert(st.top() == 2);
    std::cout<<"Test2: Pop in stack Passed"<<std::endl;

    assert(st.size() == 2);
    std::cout<<"Test3: size in stack Passed"<<std::endl;

    assert(st.size() == 2);
    std::cout<<"Test4: size in stack Passed"<<std::endl;

    st.pop();
    st.pop();
    assert(st.size() == 0);
    assert(st.empty() == true);
    std::cout<<"Test5: empty in stack Passed"<<std::endl;

    std::cout<<"All test Passed"<<std::endl;
}