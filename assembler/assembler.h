#pragma once

#include "../constants.h"
#include <stdio.h>


struct Label{
    size_t position;
    char* name;
};

void assemble_file(char* from_file, char* to_file);

void disassemble_file(char* from_file);

int isJMPoperation(assembler_command op);


