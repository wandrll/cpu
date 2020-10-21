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

size_t find_pos_by_name(Label* scratches, size_t label_count, char* name);

char* find_name_by_pos(Label* scratches, size_t label_count, char* name);
