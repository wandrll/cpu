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
    free(buff);
    printf("Syntax error: %s", buff);
    fflush(stdout);
    abort();
}

int isJMPoperation(assembler_command op){
    switch(op){
        case V_JMP: return 1;
        case V_JAE: return 1;
        default: return 0;
    }
}

size_t find_pos_by_name(Label* scratches, size_t label_count, char* name){
    assert(scratches != NULL);
    assert(name != NULL);

    for(int i = 0; i < label_count; i++){
        if(strcmp(name, scratches[i].name) == 0){
            return scratches[i].position;
        }
    }

    printf("Wrong label: %s", name);
    fflush(stdout);
    abort();
}


size_t save_arguments(assembler_command operation, char* buffer, const char* from, Label* scratches, size_t label_count){
    assert(buffer != NULL);
    assert(from != NULL);
    assert(scratches != NULL);

    size_t curr_argc = argc[operation];
    
    if(curr_argc == 0){
        return 0;
    }

    size_t offset = 0;

    char* read_s = (char*)calloc(256, sizeof(char));


    if(isJMPoperation(operation)){
        
        sscanf(from, "%s", read_s);
        
        char mode = 1;
        memcpy(buffer, &mode, sizeof(char));
        offset += sizeof(char);

        double res = find_pos_by_name(scratches, label_count, read_s);

        memcpy(buffer + offset, &res, sizeof(double));
        offset += sizeof(double);

    }else{
        double arg = 0;
        int read = 0;
        int delta = 0;
        int len = 0;
        char mode = 0;

        for(int i = 0; i < curr_argc; i++){
            read = sscanf(from + delta, "%lg%n", &arg, &len);
            delta += len;

            if(read == 1){
                mode = 1;
                memcpy(buffer + offset, &mode, sizeof(char));
                offset += sizeof(char);
                
                memcpy(buffer + offset, &arg, sizeof(double));
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
    }
    free(read_s);
    return offset;
}



size_t translate_line(line* str, char* buf, size_t offset, Label* scratches, size_t label_count, int* false_lines){
    assert(str != NULL);
    assert(buf != NULL);


    size_t delta = 0;
    double tmp = 0;
    size_t curr_off = 0;
    
    if(strchr(str->line, ':') != NULL){
        (*false_lines)++;
        return 0;
    }
    char operation = find_operation(str->line, &delta);    
    memcpy(buf + offset, &operation, sizeof(char));
    
    curr_off = save_arguments((assembler_command)operation, buf + offset + sizeof(char), str->line + delta, scratches, label_count) + sizeof(char);
    
    return curr_off;
}


void add_metadata(FILE* fp, size_t lines_count, size_t count_of_bytes){
    fwrite(&label, sizeof(char), label_size, fp);
    fwrite(&current_version, sizeof(char), 1, fp);
    fwrite(&lines_count, sizeof(size_t), 1, fp);
    fwrite(&count_of_bytes, sizeof(size_t), 1, fp);
}


void do_assemble_file(line* data, Label* scratches, size_t label_count, size_t num_of_lines, char* to_file){
     
    char* tmp = (char*)calloc(num_of_lines * (sizeof(char) + sizeof(double)*max_argc + 1), sizeof(char));    
    size_t offset = 0;

    int false_lines = 0;

    for(size_t i = 0; i < num_of_lines; i++){
        offset += translate_line(data + i, tmp, offset, scratches, label_count, &false_lines);
    }

    FILE* fp = fopen(to_file, "wb");
    
    num_of_lines-=false_lines;
    add_metadata(fp, num_of_lines, offset);
    fwrite(tmp, sizeof(char), offset, fp);

    free(tmp);
    fclose(fp);
}



size_t scratches_find_offset(assembler_command operation, const char* from){
    size_t curr_argc = argc[operation];
    if(curr_argc == 0){
        return 0;
    }
    size_t offset = 0;

    char* read_s = (char*)calloc(10, sizeof(char));
    if(isJMPoperation(operation)){
        offset += sizeof(char);
        offset += sizeof(double);
    }else{
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
                offset += sizeof(char);
                offset += sizeof(double);
            }else{
                sscanf(from + delta, "%s%n", read_s, &len);
                if(read_s[0] != 'r' || read_s[1] - 'a' > register_count || read_s[2] != 'x'){
                    printf("Wrong register\n");
                    fflush(stdout);
                    abort;
                }
                mode = 0;
                offset += sizeof(char);
                char c = read_s[1] - 'a';
                offset += sizeof(char);
            }
        }
    }
    free(read_s);
    return offset;
}



size_t translate_line_scratches(line* str, size_t offset, Label* scratches, size_t* currnt_scratch){
    assert(str != NULL);

    size_t delta = 0;
    double tmp = 0;
    size_t curr_off = 0;
    const char* res = strchr(str->line, ':');
    if(res != NULL){
        char* buf1 = (char*)calloc(256, sizeof(char));
        
        sscanf(str->line, "%s", buf1);
        int d = strlen(buf1);
        buf1[d-1] = 0;

        char* buf = (char*)calloc(d , sizeof(char));
        memcpy(buf, buf1, d-1);

        free(buf1);

        scratches[*currnt_scratch].name = buf;
        scratches[*currnt_scratch].position = offset;

        (*currnt_scratch)++;
        return 0;
    }

    char operation = find_operation(str->line, &delta);
    
    curr_off = scratches_find_offset((assembler_command)operation, str->line + delta) + sizeof(char);
    
    return curr_off;
}



size_t save_scratches(line* data, Label* scratches, size_t num_of_lines){
      
    size_t offset = 0;
    size_t current_scratch = 0;

    for(size_t i = 0; i < num_of_lines; i++){
        offset += translate_line_scratches(data + i, offset, scratches, &current_scratch);
    }
    return current_scratch;
}








void assemble_file(char* from_file, char* to_file){
    size_t num_of_lines = 0;
    char* ptr_on_buff = read_raw_data(from_file, &num_of_lines);
    line* data = parse_buffer(ptr_on_buff, num_of_lines);

    Label* scratches = (Label*)calloc(max_label_count, sizeof(Label));
    size_t count_of_scratches = save_scratches(data, scratches, num_of_lines);
    //printf("%lu ", count_of_scratches);
    for(int i = 0; i < count_of_scratches; i++){
     //   printf("%s, %ld\n",scratches[i].name, scratches[i].position);
    }
    do_assemble_file(data, scratches,count_of_scratches,  num_of_lines, to_file);



}
