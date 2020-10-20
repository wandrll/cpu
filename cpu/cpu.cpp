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
    cp->registers = (double*)calloc(4, sizeof(double));
}

void cpu_destructor(Cpu* cp){
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

void execute_command(Cpu* cp, char* buffer, int* offset){
    assert(cp != NULL);
    assert(buffer != NULL);
    
    Stack_t* stack = &(cp->cpu_stack);
 
    char operation = *((char*)buffer + *offset);

    *offset += sizeof(char);

    #define COMMAND(name, num, argc, code)\
        case V_ ## name : code break;

    switch(operation){        
        #include "../commands.h"
    }
    
    #undef COMMAND
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
    
    int offset = 0;
   
    for(int i = 0; i < count_of_lines; i++){
        execute_command(cp, buffer, &offset);
     //   
    }
    free(buffer);
    fclose(fp);
}

void create_list_file(char* buffer, char* file, size_t count_of_lines){
    FILE* fp = fopen(file, "w");
    size_t curr_offset = 0;
    char mode = 0;
    double darg = 0;
    char rarg = 0;
    size_t spaces = 0;
    for(int i = 0; i < count_of_lines; i++){
        fprintf(fp, "%08lx  |  ", curr_offset);
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
                fprintf(fp,"(%8e) |", darg);
                spaces += 48;
            }else{
                rarg = *(buffer + curr_offset);
                curr_offset += sizeof(char);
                
                fprintf(fp, " %02x ", rarg);
                for(int z = 0; z < 28; z++){
                    fprintf(fp," ");
                }
                rarg += 'a';
                fprintf(fp, "(r%cx)", rarg);
                for(int z = 0; z < 10; z++){
                    fprintf(fp," ");
                }
                fprintf(fp,"|");

                spaces += 48;
            }
        }


        size_t sd = 48 * max_argc;
        for(int z = spaces + 1; z <= sd; z++){
            if(z % 48 == 0){
                fprintf(fp,"|");

            }else{
                fprintf(fp," ");
            }
        }
        fprintf(fp,"|");

        fprintf(fp,"%s", command_names[operation]);
    
        fprintf(fp,"\n");
    }
    fclose(fp);
}