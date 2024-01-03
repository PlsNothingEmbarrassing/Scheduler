#include "simpletest.h"
#include "fiber.hpp"
#include "scheduler.hpp"
#include <iostream>


using namespace std;

char const *groups[]={
    "fibers",
};
void func1(){
    cout << "Fiber executed" << endl;
}
DEFINE_TEST_G(BasicFiberExecutionTest, fibers){
    scheduler s;  
    int x = 0;
    int *dataPtr = &x;    
    fiber f1(func1, dataPtr);

}