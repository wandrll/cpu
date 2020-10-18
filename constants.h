#pragma once
const unsigned int max_command_name = 4;
const unsigned int max_argc = 1;
const unsigned char current_version = 1;
const char label[] = "GG";
const unsigned int label_size = 2;
const int command_count = 11;
enum assembler_command{
    HET ,
    PUSH ,
    ADD ,
    SUB ,
    MUL ,
    OUT ,
    DUMP ,
    DIV ,
    SQRT ,
    SIN ,
    COS
};
const char command_names[][max_command_name+1] = {"het",
                                                  "push",
                                                  "add",
                                                  "sub",
                                                  "mul",
                                                  "out",
                                                  "dump",
                                                  "div",
                                                  "sqrt",
                                                  "sin",
                                                  "cos"};

                               const int argc[] = {0,
                                                   1,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   0,
                                                   0,};
