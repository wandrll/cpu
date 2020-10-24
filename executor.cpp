#include "cpu/cpu.h"

#include <stdio.h>

int main(int argc, char** argv){
    Cpu cp = {};
    Memory mem = {};
    cpu_constructor(&cp);
    memory_constructor(&mem, 2048);
    cpu_execute_programm(&cp, &mem, argv[1]);
    memory_destructor(&mem);
    cpu_destructor(&cp);

}