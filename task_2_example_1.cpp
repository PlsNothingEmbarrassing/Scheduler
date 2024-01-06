#include "context.hpp"
#include <cstdlib>
#include <iostream>
#include <deque>

using namespace std;
class fiber
{
private:
    Context context_;
    char *stack_bottom_;
    char *stack_top_;

public:
    fiber(void (*function)())
    {
        // Create stack
        stack_bottom_ = new char[4096];
        stack_top_ = stack_bottom_ + 4096;
        // Setup stack and align
        uintptr_t int_sp = reinterpret_cast<uintptr_t>(stack_top_);
        int_sp &= -16L; // Align boundary
        int_sp -= 128;  // Add red zone
        stack_top_ = reinterpret_cast<char *>(&int_sp);
        // Set context
        context_.rip = reinterpret_cast<void *>(function);
        context_.rsp = reinterpret_cast<void *>(stack_top_);
    }
    ~fiber()
    {
        // Delete stack data
        delete[] stack_bottom_;
    }

    Context get_context() const
    {
        return context_;
    }
};


void foo()
{
    cout << "You called foo"
         << endl;
    exit(EXIT_SUCCESS);
    
}
void goo()
{
    cout << "You called goo" << endl;
    exit(EXIT_SUCCESS);
}


int main()
{
    // Create fiber with foo
    fiber f(foo);

    // Get context
    Context c = f.get_context();

    // Switch context
    set_context(&c);

    return 0;
}