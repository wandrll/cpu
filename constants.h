#pragma once
const unsigned int max_command_name = 4;
const unsigned int max_argc = 2;
const unsigned char current_version = 1;
const int register_count = 4;
const char label[] = "GG";
const unsigned int label_size = 2;
const int command_count = 15;
///////////////////////////////////////////////////////////////////////////////////////

#define COMMAND(name, num, argc, code)\
 V_ ## name = num,

enum assembler_command{
    #include"commands.h"
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

