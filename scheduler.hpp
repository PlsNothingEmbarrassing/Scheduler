#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include "fiber.hpp"
#include <deque>
// Scheduler class definition
class scheduler {
public:
    scheduler();
    ~scheduler();

    void spawn(fiber *f);
    void do_it();
    void fiber_exit();
    void yield();
    int *get_data() const;

private:
    std::deque<fiber*> fibers_;
    Context *context_;
    fiber *current_fiber_;
};

#endif
