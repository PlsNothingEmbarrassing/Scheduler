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
        stack_top_ = reinterpret_cast<char *>(int_sp);
        // Set context
        context_.rip = reinterpret_cast<void *>(function);
        context_.rsp = reinterpret_cast<void *>(stack_top_);
    }
    ~fiber()
    {
        // Delete stack data
        delete[] stack_bottom_;
    }

    Context get_context()
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
    scheduler() : context_(nullptr) {}

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
    { // hold context to return to here
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
                // Switch context
                set_context(&c);
            }
        }
    }
    void fiber_exit()
    { // Calling set context with saved context
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
// Define scheduler as global
scheduler s;
void func1()
{
    cout << "Fiber 1" << endl;
    s.fiber_exit();
}
void func2()
{
    cout << "Fiber 2" << endl;
    s.fiber_exit();
}

int main()
{
    // Create fibers with foo
    fiber f2(func2);
    fiber f1(func1);
    // spawn fibers (push on queue)
    s.spawn(&f1);
    s.spawn(&f2);

    s.do_it();

    return 0;
}