#define DEBUG_ON

#ifdef DEBUG_ON
    #define IF_DEBUG_ON(code) code
#else
    #define IF_DEBUG_ON(code)
#endif


#include "cpu.h"
#include "../stack/stack.h"
#include "../constants.h"
#include "../stack/log.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

void cpu_constructor(Cpu* cp){
    stack_constructor(&(cp->cpu_stack), 10);
}

void cpu_destructor(Cpu* cp){
    stack_destructor(&(cp->cpu_stack));
}


void check_executable_file(FILE* fp){
    char* tmplabel = (char*)calloc(label_size + 1, sizeof(char));
 
    unsigned char version = 0;


    fread(tmplabel, sizeof(char), label_size, fp);

    if(strcmp(tmplabel, label)){
        printf("Wrong executable file. Programm will be aborted");
        fflush(stdout);
        abort();
    }

    free(tmplabel);

    fread(&version, sizeof(char), 1, fp);

    if(version != current_version){
        printf("Wrong version of executable file. Programm will be aborted");
        fflush(stdout);
        abort();
    }
}

size_t execute_command(Stack_t* stack, char* buffer){
    size_t curr_offset = 0;
    char operation = *((char*)buffer);
    curr_offset += sizeof(char);

    switch(operation){
        case PUSH:{
            double arg = *((double*)(buffer + curr_offset));
            curr_offset += sizeof(double);
            stack_push(stack, arg);
            break;
        }
        case ADD:{
            double arg1 = 0, 
                   arg2 = 0;
            stack_pop(stack, &arg1);
            stack_pop(stack, &arg2);
            stack_push(stack, arg1+arg2);
            break;
        }
        case SUB:{
            double arg1 = 0, 
                   arg2 = 0;
            stack_pop(stack, &arg1);
            stack_pop(stack, &arg2);
            stack_push(stack, arg1-arg2);
            break;
        }
        case MUL:{
            double arg1 = 0, 
                   arg2 = 0;
            stack_pop(stack, &arg1);
            stack_pop(stack, &arg2);
            stack_push(stack, arg1*arg2);
            break;
        }
        case OUT:{
            double arg1 = 1;
            stack_pop(stack, &arg1);
            printf("%lg", arg1);
            break;
        }
        case DUMP:{
            stack_dump(stack, "log.txt", DUMP_ALL, "dump in execution", create_log_data(__FILE__, __FUNCTION__, __LINE__));
            break;
        }
        case DIV:{
            double arg1 = 0, 
                   arg2 = 0;
            stack_pop(stack, &arg1);
            stack_pop(stack, &arg2);
            stack_push(stack, arg1/arg2);
            break;
        }
        case SQRT:{
            double arg1 = 0;
            stack_pop(stack, &arg1);
            stack_push(stack, sqrt(arg1));
            break;
        }
        case SIN:{
            double arg1 = 0;
            stack_pop(stack, &arg1);
            stack_push(stack, sin(arg1));
            break;
        }
        case COS:{
            double arg1 = 0;
            stack_pop(stack, &arg1);
            stack_push(stack, cos(arg1));
            break;
        }
        case HET:{

            break;
        }
 



        
    }
    return curr_offset;
}

void cpu_execute_programm(Cpu* cp, const char* file){
    assert(file != NULL);
    FILE* fp = fopen(file, "rb");
    assert(fp != NULL);

    check_executable_file(fp);
    
    size_t count_of_lines = 0;
    size_t count_of_bytes = 0;
    fread(&count_of_lines, sizeof(size_t), 1, fp);
    fread(&count_of_bytes, sizeof(size_t), 1, fp);


    char* buffer = (char*)calloc(count_of_bytes + 1, sizeof(char));
    fread(buffer, sizeof(char), count_of_bytes, fp);

    IF_DEBUG_ON(create_list_file(buffer, "listing_file.txt", count_of_lines);)
    
    size_t curr = 0;
    size_t offset = 0;

    for(int i = 0; i < count_of_lines; i++){
        curr = execute_command(&(cp->cpu_stack), buffer+offset);
        offset += curr;
    }
    free(buffer);
    fclose(fp);
}

void create_list_file(char* buffer, char* file, size_t count_of_lines){
    FILE* fp = fopen(file, "w");
    size_t curr_offset = 0;
    for(int i = 0; i < count_of_lines; i++){
        
        fprintf(fp, "%08lx  |  ", curr_offset);
        char operation = *((char*)buffer + curr_offset);
        curr_offset += sizeof(char);

        fprintf(fp, "%02x  |  ", operation);

        for(int j = 0; j < argc[operation]; j++){
            double d = *((double*)(buffer + curr_offset));
            curr_offset += sizeof(double);
            for(int k = 0; k < sizeof(double); k++){
                unsigned char p = *((char*)(&d) + k);
                fprintf(fp,"%02x ", p);
            }
            fprintf(fp,"(%8e)  ", d);
        }
        size_t sd = (sizeof(double)*3 + 16)*argc[operation];

        for(int y = 0; y < (sizeof(double)*3 + 16)*max_argc-sd; y++){
            fprintf(fp, " ");
        }
        fprintf(fp, "|");
        fprintf(fp,"%s", command_names[operation]);
    
        fprintf(fp,"\n");
    }
    fclose(fp);
}