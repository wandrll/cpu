#include "cpu/cpu.h"

#include <stdio.h>

int main(int argc, char** argv){
    Cpu cp = {};
    cpu_constructor(&cp);
    cpu_execute_programm(&cp, argv[1]);
    cpu_destructor(&cp);

}