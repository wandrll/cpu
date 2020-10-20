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

    sscanf(str, "%s%ln", buff, length);
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    #define COMMAND(name, num, argc, code)\
        if(strcmp(#name, buff) == 0) return (assembler_command)num;

    #include "../commands.h"
    
    #undef COMMAND
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    printf("Syntax error: %s", buff);
    free(buff);
    fflush(stdout);
    abort();

}




size_t save_arguments(assembler_command operation, char* buffer, const char* from){
    size_t curr_argc = argc[operation];
    size_t offset = 0;
    char* read_s = (char*)calloc(10, sizeof(char));
    double d = 0;
    int read = 0;
    int delta = 0;
    int len = 0;
    char mode = 0;

    for(int i = 0; i < curr_argc; i++){
        read = sscanf(from + delta, "%lg%n", &d, &len);
        delta += len;
        if(read == 1){
            mode = 1;
            memcpy(buffer + offset, &mode, sizeof(char));
            offset += sizeof(char);
            memcpy(buffer + offset, &d, sizeof(double));
            offset += sizeof(double);
        }else{
            sscanf(from + delta, "%s%n", read_s, &len);
            if(read_s[0] != 'r' || read_s[1] - 'a' > register_count || read_s[2] != 'x'){
                printf("Wrong register\n");
                fflush(stdout);
                abort;
            }
            mode = 0;
            memcpy(buffer + offset, &mode, sizeof(char));
            offset += sizeof(char);

            char c = read_s[1] - 'a';
            memcpy(buffer + offset, &c, sizeof(char));
            offset += sizeof(char);
        }
    }
    return offset;
}



size_t translate_line(line* str, char* buf, size_t offset){
    assert(str != NULL);
    assert(buf != NULL);


    size_t delta = 0;
    double tmp = 0;
    size_t curr_off = 0;

    char operation = find_operation(str->line, &delta);
    
    memcpy(buf + offset, &operation, sizeof(char));
    
    curr_off = save_arguments((assembler_command)operation, buf + offset + sizeof(char), str->line + delta) + sizeof(char);
    
    return curr_off;
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
    size_t offset = 0;

    size_t d = 0;
    for(size_t i = 0; i < num_of_lines; i++){
        d = translate_line(data + i, tmp, offset);
        offset += d;
    }

    free_data(data, ptr_on_buff);

    FILE* fp = fopen(to_file, "wb");
    add_metadata(fp, num_of_lines, offset);
    fwrite(tmp, sizeof(char), offset, fp);

    free(tmp);
    fclose(fp);
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

size_t disassemble_line(char* buffer, FILE* fp){
    size_t curr_offset = 0;
    char operation = *((char*)buffer);
    char mode = 0;
    double darg = 0;
    char rarg = 0;
    curr_offset += sizeof(char);
    fprintf(fp, "%s ", command_names[operation]);

    size_t argc_curr = argc[operation];
    for(int i = 0; i < argc_curr; i++){
        mode = *(buffer + curr_offset);
        curr_offset += sizeof(char);
        if(mode == 1){
            darg = *((double*)(buffer + curr_offset));
            curr_offset += sizeof(double);
            fprintf(fp, "%lg ", darg);
        }else{
            rarg = *((char*)(buffer + curr_offset)) + 'a';
            curr_offset += sizeof(char);
            fprintf(fp, "r%cx ", rarg);    
        }
    }
    fprintf(fp, "\n");
    return curr_offset;
}



void disassemble_file(char* from_file){
    assert(from_file != NULL);
    FILE* fp = fopen(from_file, "rb");
    assert(fp != NULL);

    check_executable_file(fp);
    
    size_t count_of_lines = 0;
    size_t count_of_bytes = 0;
    fread(&count_of_lines, sizeof(size_t), 1, fp);
    fread(&count_of_bytes, sizeof(size_t), 1, fp);


    char* buffer = (char*)calloc(count_of_bytes + 1, sizeof(char));
    fread(buffer, sizeof(char), count_of_bytes, fp);

    size_t curr = 0;

    FILE* fq = fopen("disassemble.txt", "w");

    for(int i = 0; i < count_of_lines; i++){
        curr += disassemble_line(buffer+curr, fq);
    }

    free(buffer);
    fclose(fp);
    fclose(fq);
}