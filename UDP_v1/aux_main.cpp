#include <iostream>

struct B
{
    float arr[2];
};

struct A
{
    int x;
    int c[2];
    B b;
};

void print_A(A obj)
{
    std::cout << obj.x << "\n" << obj.c[0] << ", " << obj.c[1] << "\n" << obj.b.arr[0] << ", " << obj.b.arr[1] << "\n";
}

int main() {
    //std::vector<char> bytes = {101, 53, 2};
    //std::cout << find_hash(bytes) << "\n";
    
    
    A obj1;
    obj1.x = 1;
    obj1.c[0] = 22;
    obj1.c[1] = 84;
    obj1.b.arr[0] = 3.1415;
    obj1.b.arr[1] = 2.71828;

    print_A(obj1);

    A obj2;
    obj2 = obj1;
    obj2.x = 48;

    std::cout << "Espacio\n";

    print_A(obj2);
    
    return 0;
}