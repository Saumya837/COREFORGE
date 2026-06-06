#include<iostream>
#include "../include/MySharePtr.h"
#include <cassert>

using namespace coreforge;

class Person {
    public:
        std::string name;
        int age;

        Person(std::string n, int a) : name(n), age(a) {}

        void printInfo(){
            std::cout << "Name: " << name << ", Age:" << age << std::endl;
        }
};

int main(){

    // Test 1: Copy Constructor Test
    MySharePtr<int> p1(new int(43));
    MySharePtr<int> p2(p1);

    assert(p1.get() == p2.get());
    assert(p1.use_count() == 2);
    assert(p2.use_count() == 2);


    // Test 2: Copy Assignment operator Test
    MySharePtr<Person> p3(new Person("Sanketh", 24));
    MySharePtr<Person> p4(new Person("Somya", 29));
    p4 = p3;
    assert(p3.get() == p4.get());
    assert(p3.use_count() == 2);
    assert(p4.use_count() == 2);


    //Test 3: dereference shared Pointer
    assert(*p1 == 43);
    assert(*p2 == 43);

    //Test 4: operator -> 
    assert(p3->name == "Sanketh");
    assert(p3->age == 24);

    assert(p4->name == "Sanketh");
    assert(p3->age == 24);

    MySharePtr<int> p5(new int(99));
    // Test 5: ref count decrements when owner dies
    {
        MySharePtr<int> p6(p5);
        assert(p5.use_count() == 2);
    } // inner owners destroyed, p5 use_count should be 1

    assert(p5.use_count() == 1);

    std::cout<<"All test Passed"<<std::endl;
}

