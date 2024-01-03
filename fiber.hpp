#ifndef FIBER_HPP
#define FIBER_HPP

#include "context.hpp"
#include <cstdint>

class fiber {
public:
    fiber(void (*function)(), int *data_ptr = nullptr);
    ~fiber();

    Context* get_context();
    int *get_data() const;
    void switch_to_scheduler(Context* scheduler_context);

private:
    Context context_;
    char *stack_bottom_;
    char *stack_top_;
    int *data_ptr_;
};

#endif

