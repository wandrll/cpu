#include "assembler.h"
#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


assembler_command find_operation(const char* str, size_t* length){
    assert(str != NULL);

    char* buff = (char*)calloc(max_command_name + 1, sizeof(char));
    assert(buff != NULL);

    while(*str == ' '){
        str++;
    }
    sscanf(str, "%s%ln", buff, length);

    for(int i = 0; i < command_count; i++){
        if(!strcmp(buff, command_names[i])){
            free(buff);
            return (assembler_command)i;
        }
    }
    printf("Syntax error: %s", buff);
    free(buff);
    fflush(stdout);
    abort();

    //return ERROR;
}








size_t translate_line(line* str, char* buf){
    assert(str != NULL);
    assert(buf != NULL);

    size_t curr_argc = 0;
    size_t delta = 0;
    double tmp = 0;
    
    char operation = find_operation(str->line, &delta);
    curr_argc = argc[operation];

    memcpy(buf, &operation, sizeof(char));
   // printf("%lu %lu\n", delta, argc);
    const char* args = str->line;

    size_t curr = 0;
    double arg = 0;
    for(size_t i = 0; i < curr_argc; i++){
        sscanf(args+delta, "%lg%ln", &arg, &curr);
        memcpy(buf + sizeof(char) + sizeof(double)*i, &arg, sizeof(double));
        delta += curr;
    }
    return sizeof(char) + sizeof(double) * curr_argc;
}

void add_metadata(FILE* fp, size_t lines_count, size_t count_of_bytes){
    fwrite(&label, sizeof(char), label_size, fp);
    fwrite(&current_version, sizeof(char), 1, fp);
    fwrite(&lines_count, sizeof(size_t), 1, fp);
    fwrite(&count_of_bytes, sizeof(size_t), 1, fp);
}



void assemble_file(char* from_file, char* to_file){
    assert(from_file != NULL);
    assert(to_file != NULL);

    size_t num_of_lines = 0;  
    


    char* ptr_on_buff = read_raw_data(from_file, &num_of_lines);
    line* data = parse_buffer(ptr_on_buff, num_of_lines);
    
    char* tmp = (char*)calloc(num_of_lines * (sizeof(char) + sizeof(double)*max_argc + 1), sizeof(char));    
    size_t curr_offset = 0;

    //fwrite(&num_of_lines, sizeof(size_t), 1, fp);
    size_t d = 0;
    for(size_t i = 0; i < num_of_lines; i++){
        d = translate_line(data + i, tmp + curr_offset);
        curr_offset += d;
    }
    free_data(data, ptr_on_buff);

    FILE* fp = fopen(to_file, "wb");
    add_metadata(fp, num_of_lines, curr_offset);
    fwrite(tmp, sizeof(char), curr_offset, fp);

    free(tmp);
    fclose(fp);
}


void disassemble_file(char* from_file, char* to_file){


}