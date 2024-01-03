#include "context.hpp"
#include <cstdlib>
#include <iostream>
#include <deque>
#include "fiber.hpp"
#include "scheduler.hpp"

using namespace std;

// Define scheduler as global
scheduler s;
void func1()
{
    cout << "Fiber 1 before" << endl;
    s.yield();
    int *dp = s.get_data();
    cout << "Fiber 1 after: " << *dp << endl;
    *dp += 1;
    s.fiber_exit();
}
void func2()
{
    int *dp = s.get_data();    
    cout
        << "Fiber 2 before: " << *dp << endl;
    s.yield();    
    cout << "Fiber 2 after: " << *dp <<endl;
    s.fiber_exit();
}
void func3(){
    int *dp = s.get_data();
    cout << "Fiber 3: " << *dp << endl;
    *dp +=1;
    s.fiber_exit();

}

int main()
{
    // Create fibers with foo
    int x = 0;
    int *dataPtr = &x;
    fiber f2(func2, dataPtr);
    fiber f1(func1, dataPtr);
    fiber f3(func3, dataPtr);
    // spawn fibers (push on queue)
    s.spawn(&f1);
    s.spawn(&f2);
    s.spawn(&f3);

    s.do_it();

    return 0;
}