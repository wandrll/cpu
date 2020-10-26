#include "cpu/cpu.h"
#include "constants.h"
#include <stdio.h>

int main(int argc, char** argv){
    Cpu cp = {};
    Memory mem = {};
    cpu_constructor(&cp);
    memory_constructor(&mem, memory_size);
    cpu_execute_programm(&cp, &mem, argv[1]);
    memory_destructor(&mem);
    cpu_destructor(&cp);

}