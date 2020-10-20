#pragma once

#include <stdio.h>
#include "defines.h"

//typedef int stack_elem_t;
enum stack_end_signal{
    STACK_OK, 
    STACK_EMPTY, 
    STACK_INCREASE_ERROR,  
    STACK_DECREASE_ERROR, 
    STACK_CONSTRUCTION_ERROR, 
    STACK_NULL
};




//// STACK FUNCIONS BLOCK ////
struct Stack_t{
    stack_elem_t* data;
    size_t size;
    size_t capacity;
};

stack_end_signal stack_constructor(Stack_t* stack, size_t capacity);
stack_end_signal stack_push(Stack_t* stack, stack_elem_t elem_to_push);
stack_end_signal stack_pop(Stack_t* stack, stack_elem_t* result);
stack_end_signal stack_destructor(Stack_t* stack);
stack_end_signal stack_back(Stack_t* stack, stack_elem_t* result);
////                      ////


//// STACK PROTECTION BLOCK ////
int validate_pointer(void* a);
void save_current_stack(Stack_t* stack);
int check_stack(Stack_t* stack);
struct Canary{
    size_t size;
    char* value;
};
//const Canary* generate_canary(size_t size);
////                        ////

