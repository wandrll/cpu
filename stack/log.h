#pragma once

#include "stack.h"

enum dump_mode {
    DUMP_STACK_ADRESS = 1,
    DUMP_DATA_ADRESS = 2,
    DUMP_STACK_SIZE = 4,
    DUMP_STACK_CAPACITY = 8,
    DUMP_STACK_LEFT_CANARY = 16,
    DUMP_STACK_RIGHT_CANARY = 32,
    DUMP_STACK_DATA = 64,
    DUMP_ALL = 127
};

typedef struct{
    char* file;
    char* function;
    int line;
}log_data;

void stack_dump(Stack_t* stack, const char* log, int mode, char* message,  log_data* data);
void print_stack_elem(int a, FILE* fp);
void print_stack_elem(double a, FILE* fp);
log_data* create_log_data(char* file, const char* func, int line);

int int_cmp(void* a, FILE* fp);

int random_stack_int_test(Stack_t* stack, size_t count);
