#include <iostream>
#include "../include/MyUniquePtr.h"

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
    codeforge::MyUniquePtr<int> p(new int(42));
    
    std::cout << *p << std::endl;  // should print 42

    // Test 2 — modify through dereference
    *p = 100;
    std::cout << *p << std::endl;  // should print 100
    
    codeforge::MyUniquePtr<Person> p1(new Person("Alice", 3));

    // Test 3 — access member through arrow operator
    p1->printInfo();

    p1->age = 21;
    p1->name = "Bob";
    p1->printInfo();  // should print Name: Bob, Age: 21

    return 0;
}