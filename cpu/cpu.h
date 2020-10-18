#pragma once
#include <stdio.h>
#include "../stack/stack.h"

struct Cpu{
    Stack_t cpu_stack;
};

void cpu_constructor(Cpu* cp);

void cpu_destructor(Cpu* cp);

void cpu_execute_programm(Cpu* cp, const char* file);

void create_list_file(char* buffer, char* file, size_t count_of_lines);