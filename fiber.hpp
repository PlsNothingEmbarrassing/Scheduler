#ifndef FIBER_HPP
#define FIBER_HPP

#include "context.hpp"
#include <cstdint>
// Fiber class definition
class fiber{
    public:    
    // Constructor with function pointer
    fiber(void (*function)(), int *data_ptr = nullptr);    
    // Deconstructor
    ~fiber();

    Context* get_context();
    int* get_data() const;
    void switch_to_scheduler(Context* scheduler_context);

    private:    
    Context context_;
    char *stack_bottom_;
    char* stack_top_;
    int *data_ptr_;
    
    void initialise_stack();    
    
};


#endif
