#include "context.hpp"
#include <cstdlib>
#include <iostream>

using namespace std;
// Global contexts for switching
Context gooContext;
Context fooContext;

void foo()
{
    cout << "You called foo"
         << endl;
    set_context(&gooContext);
}
void goo()
{
    cout << "You called goo" << endl;
    exit(EXIT_SUCCESS);
}

int main()
{
    // Allocate space for stack
    char fooStack[4096];
    char gooStack[4096];

    // Stack grows downwards so set ptr to end of data
    char *fooSp = fooStack + 4096;
    // cast sp to uintptr_t for bitwise operations
    uintptr_t int_sp = reinterpret_cast<uintptr_t>(fooSp);
    int_sp &= -16L; // Align boundary
    int_sp -= 128;  // Add red zone
    fooSp = reinterpret_cast<char *>(&int_sp);
    // Set rip and rsp
    // Pass pointer to function body
    fooContext.rip = reinterpret_cast<void *>(foo);
    fooContext.rsp = &fooSp;

    // Setup goo stack
    char *gooSp = gooStack + 4096;
    uintptr_t goo_sp = reinterpret_cast<uintptr_t>(gooSp);
    goo_sp &= 16L;
    goo_sp -= 128;
    gooSp = reinterpret_cast<char *>(&goo_sp);
    gooContext.rip = reinterpret_cast<void *>(goo);
    gooContext.rsp = &fooSp;

    // Switch context
    set_context(&fooContext);

    return 0;
}