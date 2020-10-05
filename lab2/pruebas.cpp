#include <iostream>
#include <string>

using namespace std;

int main()
{
    string s1("hola");
    cout << s1.length() << "\n";

    char a[] = {'h', 'o', 'l', 'a'};
    string s2(a); 
    cout << s2.length() << "\n";

    return 0;
}