#include <iostream>
#include <unistd.h>
#include <termios.h>
#include <sstream>
#include <string>
#include <queue>

using namespace std;

void push_string_to_queue(string s, queue<char> &q)
{
    cout << s.size() << "\n";
  for (int i = 0; i < s.size(); ++i)
  {
    cout << s[i] << "\n";
    q.push(s[i]);
  }
}

int main()
{
    std::stringstream s;
    s << 4;
    s << "hola";
    std::string str = s.str();
    cout << str << "\n";

    queue<char> q;
    push_string_to_queue(str, q);
    
    cout << "chau";

    while (!q.empty())
    {
        cout << q.front();
        q.pop();
    }

    return 0;
}