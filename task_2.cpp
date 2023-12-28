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

class scheduler
{
private:
    deque<fiber *> fibers_;
    Context *context_;

public:
    scheduler() = default;

    ~scheduler()
    { // Delete fibers
        for (fiber *f : fibers_)
        {
            delete f;
        }
        // clear deque
        fibers_.clear();
    }
    void spawn(fiber *f)
    {
        fibers_.push_back(f);
    }
    void do_it()
    {
        Context s_context;
        context_ = &s_context;
        if (get_context(context_) == 0)
        {
            // while not empty
            while (!fibers_.empty())
            {
                // FIFO remove fiber from front of queue
                fiber *f = fibers_.front();
                fibers_.pop_front();

                Context c = f->get_context();
                set_context(&c);
            }
        }
    }
    void fiber_exit()
    {
        set_context(context_);
    }
};

void foo()
{
    cout << "You called foo"
         << endl;
    exit(EXIT_SUCCESS);
    // set_context(&gooContext);
}
void goo()
{
    cout << "You called goo" << endl;
    exit(EXIT_SUCCESS);
}

// void func1()
// {
//     cout << "Fiber 1" << endl;
//     s.fiber_exit();
// }
// void func2()
// {
//     cout << "Fiber 2" << endl;
//     s.fiber_exit();
// }
// Define scheduler as global
// scheduler s;

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