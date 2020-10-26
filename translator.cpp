#include <stdio.h>
#include <stdlib.h>
#include "assembler/assembler.h"


int main(int argc, char** argv){
    char* from = argv[1];
    char* to = argv[2];
    assemble_file(from, to);
    disassemble_file(to);
}