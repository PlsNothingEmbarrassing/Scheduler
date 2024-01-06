#include "simpletest.h"
#include "fiber.hpp"
#include "scheduler.hpp"
#include <iostream>
#include <sstream>
#include <string>
// Compile file from main repo with: clang++ -std=c++17 -o task_3_tests -I external/simpletest task_3_tests.cpp fibers.cpp context.o external/simpletest/simpletest.cpp

using namespace std;

char const *groups[]={
    "fibers",
};

scheduler s;

void func1(){
    // Increments val by 1
    int* val = s.get_data();
    *val+=1;
    s.fiber_exit();
}
void testYield1()
    {
    // Prints data value, yields, adds 2 to data value when resumes execution and prints again to demonstrate yield
    int *dp = s.get_data();
    cout << *dp;       
    s.yield();
    *dp+=2;             
    cout << *dp;
    s.fiber_exit();
}
void testYield2(){
    // Increments data value then prints it
    int *dp = s.get_data();
    *dp +=1;
    cout << *dp;    
    s.fiber_exit();

}
void foo()
{
    
    s.fiber_exit();
    
}
void goo()
{
    
    s.fiber_exit();
}
void spawnFiber(){
    int *dp = s.get_data();
    fiber f1(func1, dp);
    s.spawn(&f1);
    s.fiber_exit();
}
DEFINE_TEST_G(BasicFiberExecutionTest, fibers){
    
    int val =1;      
    fiber f1(func1, &val);
    s.spawn(&f1);
    
    s.do_it();
    TEST_MESSAGE(val == 2, "Failed to modify data");
    

}

DEFINE_TEST_G(MutipleFiberExecutionTest, fibers){        
    fiber f1(foo);
    fiber f2(goo);
    s.spawn(&f1);
    s.spawn(&f2);
    
    s.do_it();
    // Check that if no fiber currently executing returns a nullptr
    int* data = s.get_data();
    // If reached this point then fibers successfully ran
    TEST_MESSAGE(data == nullptr, "Running multiple fibers failed");
    

}
DEFINE_TEST_G(YieldTest, fibers){
    // Testing that data is being modified in correct order
    int val =1;      
    fiber f1(testYield1, &val);
    fiber f2(testYield2, &val);
    s.spawn(&f1);
    s.spawn(&f2);
    // Save original cout buffer
    streambuf* original_buf = cout.rdbuf();
    // Create new sstream to capture cout
    ostringstream captured_cout;
    cout.rdbuf(captured_cout.rdbuf());
    // Run fibers    
    s.do_it();
    // Restore original buffer after fibers have run
    cout.rdbuf(original_buf);
    cout << captured_cout.str();
    // Test that data value is being modified in correct order from captured sstream
    TEST_MESSAGE(captured_cout.str() == "124", "Failed to modify data");
    

}


DEFINE_TEST_G(SpawnFromFiber, fibers){
    // Fiber will spawn another fiber which will modify passed data
    int val = 0;
    fiber f1(spawnFiber, &val);
    s.spawn(&f1);
    s.do_it();

    TEST_MESSAGE(val == 1, "Fiber failed to spawn next fiber");

}
int main(){
    bool pass = true;
    for (auto group : groups)
		pass &= TestFixture::ExecuteTestGroup(group, TestFixture::Verbose);

	return pass ? 0 : 1;
}


