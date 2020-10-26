#pragma once
const int win_size = 512;
const int memory_size = 8192;
const unsigned int max_command_name = 6;
const unsigned int max_argc = 3;
const unsigned char current_version = 2;
const unsigned int max_label_count = 20;
const int register_count = 4;
const char label[] = "GG";
const unsigned int label_size = 2;
//const int command_count = 22;
///////////////////////////////////////////////////////////////////////////////////////

#define COMMAND(name, num, argc, code)\
 V_ ## name = num,

enum assembler_command{
    #include"commands.h" 
    ERROR
};

#undef COMMAND
///////////////////////////////////////////////////////////////////////////////////////

#define COMMAND(name, num, argc, code)\
    #name,

const char command_names[][max_command_name+1] = {    
    #include"commands.h"
};

#undef COMMAND
///////////////////////////////////////////////////////////////////////////////////////
#define COMMAND(name, num, argc, code)\
    argc,

const int argc[] = {
    #include "commands.h"
};

#undef COMMAND
///////////////////////////////////////////////////////////////////////////////////////

