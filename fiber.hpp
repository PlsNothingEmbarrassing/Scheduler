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
    // ptr to shared data
    int *data_ptr_;

public:
    fiber(void (*function)(), int *data_ptr = nullptr) : data_ptr_(data_ptr)
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

    Context* get_context()
    {
        return &context_;
    }
    int *get_data() const
    {
        return data_ptr_;
    }
    void switch_to_scheduler(Context* scheduler_context){
        swap_context(&context_, scheduler_context);
    }
};

class scheduler
{
private:
    deque<fiber *> fibers_;
    Context *context_;
    // ptr to current fiber
    fiber *current_fiber_;

public:
    scheduler() : context_(nullptr), current_fiber_(nullptr) {}

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
                current_fiber_ = fibers_.front();
                fibers_.pop_front();               
                // Switch context
                set_context(current_fiber_->get_context());
                // reset ptr after fiber has finished
                current_fiber_ = nullptr;
            }
        }
    }
    void fiber_exit()
    { // Calling set context with saved context
        set_context(context_);
    }
    int *get_data() const
    {
        if (current_fiber_)
        {
            return current_fiber_->get_data();
        }
        return nullptr;
    }
    void yield(){
        if(current_fiber_){
            fibers_.push_back(current_fiber_);
            current_fiber_->switch_to_scheduler(context_);
            
        }

    }
};