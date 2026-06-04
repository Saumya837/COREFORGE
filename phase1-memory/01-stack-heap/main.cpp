#include <iostream>
#include <memory>

int global_a = 10;

void show_stack(){
    int local_a = 10;
    int local_b = 20;

    std::cout<<"--------Stack---------------\n";
    std::cout << "Stack variable a: " << &local_a << std::endl;
    std::cout << "Stack variable b: " << &local_b << std::endl;
    std::cout << "notice: they are close together\n";
    std::cout << "notice: local_b is LOWER address than local_a\n";
    std::cout << "stack grows DOWNWARD\n\n";
}

void show_heap(){
    int *heap_a = new int(10);
    int *heap_b = new int(20);

    //the memory are architeure dependent so if you are running on mac-os then this will not hold.

    std::cout<<"--------Heap---------------\n";
    std::cout<< "Heap variable a: " << &heap_a << std::endl;
    std::cout<< "Heap variable b: " << &heap_b << std::endl;
    std::cout << "notice: they are NOT close together\n";
    std::cout << "notice: heap_b is HIGHER address than heap_a\n";
    std::cout << "heap grows UPWARD\n\n";

    delete heap_a; // free the memory allocated for heap_a
    delete heap_b; // if not: memeory laek
}

void show_global(){
    std::cout<< "--------Global---------------\n";
    std::cout << "global_var_adress: " << &global_a << std::endl;
}

// BAD: returning pointer to stack memory
int* bad_function() {
    int local = 42;
    return &local;   // returning address of stack variable
}

int* good_function() {
    int* heap_var = new int(42); // allocate on heap
    std::cout << "heap_var address: " << &heap_var << std::endl;
    return heap_var; 
    /*
        return heap memory address is safe to return but two side of a coin
        Note: The caller is responsible for deleting the allocated memory to avoid memory leaks:
    */
}

// BEST in modern C++: let the compiler manage it
std::unique_ptr<int> best_function() {
    return std::make_unique<int>(42);  // heap, but auto-freed
}

void show_sizes() {
    std::cout << "\n=== SIZES (same everywhere) ===\n";
    std::cout << "int:    " << sizeof(int)    << " bytes\n"; //4
    std::cout << "long:   " << sizeof(long)   << " bytes\n";
    std::cout << "char:   " << sizeof(char)   << " bytes\n";
    std::cout << "bool:   " << sizeof(bool)   << " bytes\n";
    std::cout << "int*:   " << sizeof(int*)   << " bytes\n";
    std::cout << "double: " << sizeof(double) << " bytes\n";
}

int main(){
    show_stack();
    show_heap();
    show_global();
    show_sizes();

    int* p = bad_function();
    std::cout << *p << "\n";// undefined behavior: accessing memory that may have been deallocated

    int* q = good_function();
    std::cout << *q << "\n"; // safe to access heap memory
    delete q;

    auto r = best_function();
    std::cout << *r << "\n"; // safe and auto-manasged, no need to delete 
}
// r is a unique_ptr it automatically destroyed here, a destructor is called and memory is freed.