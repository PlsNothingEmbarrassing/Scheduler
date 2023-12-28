#include "context.hpp"
#include <cstdlib>
#include <iostream>
using namespace std;
int main()
{
    volatile int x = 0;

    Context c;
    int reVal = get_context(&c);
    cout << "a message" << endl;
    if (x == 0)
    {
        x += 1;
        set_context(&c);
    }
    return 0;
}