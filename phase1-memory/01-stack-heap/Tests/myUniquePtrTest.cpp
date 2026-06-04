#include <iostream>
#include "../include/myUniquePtr.h"
#include <cassert>

class Person {
    public:
        std::string name;
        int age;

        Person(std::string n, int a) : name(n), age(a) {}

        void printInfo(){
            std::cout << "Name: " << name << ", Age:" << age << std::endl;
        }
};

int main() {
    // Test 1 — basic int pointer
    codeforge::MyUniquePtr<int> p1(new int(42));
    
    std::cout << *p1 << std::endl;  // should print 42

    // Test 2 — modify through dereference
    *p1 = 100;
    std::cout << *p1 << std::endl;  // should print 100
    
    codeforge::MyUniquePtr<Person> p2(new Person("Alice", 3));

    // Test 3 — access member through arrow operator
    p2->printInfo();

    p2->age = 21;// 
    p2->name = "Bob";
    p2->printInfo();  // should print Name: Bob, Age: 21

    codeforge::MyUniquePtr<int> p3(std::move(p1));
    std::cout << *p3 << std::endl;  // should print 100
    assert(p1.get() == nullptr); 

    codeforge::MyUniquePtr<int> p4(new int(200));
    p3 = std::move(p4);
    std::cout << *p3 << std::endl;  

    return 0;
}