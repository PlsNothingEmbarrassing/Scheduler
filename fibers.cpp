#include "fiber.hpp"
#include "scheduler.hpp"
#include "context.hpp"
#include <cstdlib>
#include <iostream>
// Fiber constructor
fiber::fiber(void (*function)(), int *data_ptr) : data_ptr_(data_ptr) {
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
    };
// Fiber deconstructor
fiber::~fiber(){
    // Delete stack data
        delete[] stack_bottom_;
}
// Retrieves the fibers saved context
Context* fiber:: get_context(){
        return &context_;
    }
// Returns pointer to (shared) data
int* fiber::get_data() const
    {
        return data_ptr_;
    }
// Called when fiber yields and returns back to the scheduler
void fiber::switch_to_scheduler(Context* scheduler_context){
        swap_context(&context_, scheduler_context);
    }
// Scheduler constructor
scheduler::scheduler() : context_(nullptr), current_fiber_(nullptr) {}
// Scheduler deconstructor
scheduler::~scheduler()
    { // Delete fibers
        for (fiber *f : fibers_)
        {
            delete f;
        }
        // clear deque
        fibers_.clear();
    }
// Spawn fiber by adding it to scheduler queue
void scheduler:: spawn(fiber *f)
    {
        fibers_.push_back(f);
    }
// Fibers are run until none left in queue in round-robin schedule
void scheduler::do_it(){
     // hold context to return to here
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
// Exiting currently running fiber by returning to scheduler context
void scheduler::fiber_exit(){
    // Calling set context with saved context
        set_context(context_);
}
// Currently executing fiber has context saved, execution is switched to next fiber in queue and fiber placed back onto queue
void scheduler:: yield(){
    if(current_fiber_){
            // Save current fiber context, add back to queue and return to scheduler
            fibers_.push_back(current_fiber_);
            current_fiber_->switch_to_scheduler(context_);
            
        }
}
// Retrieves the data ptr for the currently executing fiber
int* scheduler::get_data() const {
    if (current_fiber_)
        {
            return current_fiber_->get_data();
        }
        return nullptr;

}