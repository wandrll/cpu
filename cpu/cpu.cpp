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

void memory_constructor(Memory* mem, size_t count){
    mem->data = (double*)calloc(count, sizeof(double));
    mem->size = count;
}

void memory_destructor(Memory* mem){
    free(mem->data); 
}

void cpu_constructor(Cpu* cp){
    stack_constructor(&(cp->cpu_stack), 10);
    stack_constructor(&(cp->call_stack), 10);

    cp->RIP = 0;
    cp->registers = (double*)calloc(4, sizeof(double));
}

void cpu_destructor(Cpu* cp){
    stack_destructor(&(cp->call_stack));
    stack_destructor(&(cp->cpu_stack));
    free(cp->registers);
}


void check_executable_file(FILE* fp){
    assert(fp != NULL);

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

void execute_command(Cpu* cp, Memory* ram, char* buffer){
    assert(cp != NULL);
    assert(buffer != NULL);
    
    Stack_t* stack = &(cp->cpu_stack);
 
    char operation = *((char*)buffer + cp->RIP);
    //printf("%s\n", command_names[operation]);
    cp->RIP += sizeof(char);
/////////////////////////////////////////////////////////////////////////////
    #define COMMAND(name, num, argc, code)\
        case V_ ## name : code break;

    switch(operation){        
        #include "../commands.h"
    }
    
    #undef COMMAND
/////////////////////////////////////////////////////////////////////////////
}

void cpu_execute_programm(Cpu* cp, Memory* mem, const char* file){
    assert(cp != NULL);
    assert(file != NULL);
    FILE* fp = fopen(file, "rb");
    assert(fp != NULL);

    check_executable_file(fp);
    
    size_t count_of_lines = 0;
    size_t count_of_bytes = 0;
    fread(&count_of_lines, sizeof(size_t), 1, fp);
    fread(&count_of_bytes, sizeof(size_t), 1, fp);

    cp->exec_buffer_size = count_of_bytes;

    char* buffer = (char*)calloc(count_of_bytes + 1, sizeof(char));
    fread(buffer, sizeof(char), count_of_bytes, fp);
    //printf("%lu %lu", count_of_lines, count_of_bytes);
    //IF_DEBUG_ON(create_list_file(buffer, "listing_file.txt", count_of_lines, count_of_bytes);)
    
   
    while(cp->RIP < count_of_bytes){
        execute_command(cp, mem, buffer);   
    }

    free(buffer);
    fclose(fp);
}











void create_list_file(char* buffer, const char* file, size_t count_of_lines, size_t  count_of_bytes){
    FILE* fp = fopen(file, "w");
    size_t curr_offset = 0;
    char mode = 0;
    double darg = 0;
    char rarg = 0;
    size_t spaces = 0;
    fprintf(fp,"| Count of lines: %lu\n| Count of bytes %lu\n", count_of_lines, count_of_bytes);
    for(int i = 0; i < count_of_lines; i++){
        fprintf(fp, "| %08lx  |  ", curr_offset);
        char operation = *((char*)buffer + curr_offset);
        curr_offset += sizeof(char);
        fprintf(fp, "%02x  |", operation);
        fprintf(fp,"|");

        spaces = 0;
        for(int j = 0; j < argc[operation]; j++){
            mode = *(buffer + curr_offset);
            curr_offset += sizeof(char);

            if(mode == 1){
                darg = *((double*)(buffer + curr_offset));
                curr_offset += sizeof(double);
                for(int k = 0; k < sizeof(double); k++){
                    unsigned char p = *((char*)(&darg) + k);
                    fprintf(fp," %02x ", p);
                }
                fprintf(fp,"(%8e)  |", darg);
                spaces += 48;
            }else{
                rarg = *(buffer + curr_offset);
                curr_offset += sizeof(char);
                
                fprintf(fp, " %02x %28c", rarg, ' ');

                rarg += 'a';
                fprintf(fp, "(r%cx)%10c |", rarg, ' ');


                spaces += 48;
            }
        }


        for(int z = argc[operation]; z < max_argc ; z++){
                fprintf(fp,"%48c|", ' ');
        }
        fprintf(fp,"|");

        fprintf(fp,"%s", command_names[operation]);
    
        fprintf(fp,"\n");
    }
    fclose(fp);
}