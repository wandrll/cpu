#include "assembler.h"
#include "data.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

struct Command{
    assembler_command command;
    char** arguments;
};


char** parse_arguments(const char* line, size_t count){
    assert(line != NULL);

    char** res = (char**)calloc(count, sizeof(char*));
    const char* left_ptr = line;
    const char* right_ptr = line;
    char* tmp = NULL;
    for(int i = 0; i < count; i++){
        while(*left_ptr == ' '){
            left_ptr++;
            right_ptr++;
        }
        while(*right_ptr != ' ' && *right_ptr != 0){
            right_ptr++;
        }
        tmp = (char*)calloc(right_ptr-left_ptr+1, sizeof(char));
        memcpy(tmp, left_ptr, right_ptr - left_ptr);
        res[i] = tmp;
        left_ptr = right_ptr;
    }
    return res;

}

assembler_command find_operation(const char* str, size_t* length){
    assert(str != NULL);
    assert(length != NULL);

    const char* left = str;
    const char* right = str;
    while(*left == ' ' || *left == '\t'){
        left++;
        right++;
    }

    while(*right != ' ' && *right != 0){
        right++;
    }

    char* buff = (char*)calloc(right - left + 1, sizeof(char));
    assert(buff != NULL);

    memcpy(buff, left, right-left);
    *length = right - str;
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    #define COMMAND(name, num, argc, code)  \
        if(strcmp(#name, buff) == 0){       \
            free(buff);                     \
            return (assembler_command)num;  \
        }

    #include "../commands.h"
    
    #undef COMMAND
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    free(buff);
    return ERROR;
}

int isJMPoperation(assembler_command op){
    switch(op){
        case V_JMP: return 1;
        case V_JAE: return 1;
        case V_JA: return 1;
        case V_JBE: return 1;
        case V_JB: return 1;
        case V_JE: return 1;
        case V_JN: return 1;
        case V_CALL: return 1;
        default: return 0;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                               //
//                                                                                                                               //
//                                                                                                                               //
//                                  ASSEMBLE BLOCK START                                                                         //
//                                                                                                                               //
//                                                                                                                               //
//                                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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


static size_t save_arguments(Command* com, char* buffer, Label* scratches, size_t label_count){
    assert(buffer != NULL);
    assert(com != NULL);
    assert(scratches != NULL);

    size_t curr_argc = argc[com->command];
    
    if(curr_argc == 0){
        return 0;
    }

    size_t offset = 0;

    if(isJMPoperation(com->command)){
        
        char mode = 1;

        memcpy(buffer, &mode, sizeof(char));
        offset += sizeof(char);

        double res = find_pos_by_name(scratches, label_count, com->arguments[0]);

        memcpy(buffer + offset, &res, sizeof(double));
        offset += sizeof(double);

    }else{
        double arg = 0;
        int read = 0;

        char mode = 0;

        char* curr_arg1 = NULL;
        char* curr_arg2 = NULL;

        char reg = 0;

        for(int i = 0; i < curr_argc; i++){
            mode = 0;

            char* curr_arg1 = com->arguments[i];
            while(*curr_arg1 != 0){
                if(*curr_arg1 == '+'){
                    curr_arg2 = curr_arg1 + 1;
                    *curr_arg1 = 0;
                    mode = 3;
                }
                curr_arg1++;
            }
            char last = *(curr_arg1 - 1);
             
            if(last ==']' || *com->arguments[i] == '['){
                
                mode += 4;
                *(curr_arg1 - 1) = 0;
                *com->arguments[i] = 0;
                curr_arg1 = com->arguments[i] + 1;
            }else{
                curr_arg1 = com->arguments[i];
            }
            
            if(mode & 3){

                memcpy(buffer + offset, &mode, sizeof(char));
                offset += sizeof(char);

                reg = curr_arg1[1] - 'a';

                memcpy(buffer + offset, &reg, sizeof(char));
                offset += sizeof(char);

                arg = atof(curr_arg2);

                memcpy(buffer + offset, &arg, sizeof(double));
                offset += sizeof(double);

            }else{

                read = sscanf(curr_arg1, "%lg", &arg);
                if(read == 1){
                    mode += 1;

                    memcpy(buffer + offset, &mode, sizeof(char));
                    offset += sizeof(char);
                    
                    memcpy(buffer + offset, &arg, sizeof(double));
                    offset += sizeof(double);
                }else{
                    mode += 2;
                    memcpy(buffer + offset, &mode, sizeof(char));
                    offset += sizeof(char);

                
                    reg = curr_arg1[1] - 'a';

                    memcpy(buffer + offset, &reg, sizeof(char));
                    offset += sizeof(char);
                }
            }
        }
    }
    return offset;
}



static size_t translate_line(Command* com, char* buf, size_t offset, Label* scratches, size_t label_count){
    assert(com != NULL);
    assert(buf != NULL);
    assert(scratches != NULL);
        
    char operation = com->command;    
    memcpy(buf + offset, &operation, sizeof(char)); 
    
    return save_arguments(com, buf + offset + sizeof(char), scratches, label_count)+ sizeof(char);
}


void add_metadata(FILE* fp, size_t lines_count, size_t count_of_bytes){
    assert(fp != NULL);

    fwrite(&label, sizeof(char), label_size, fp);
    fwrite(&current_version, sizeof(char), 1, fp);
    fwrite(&lines_count, sizeof(size_t), 1, fp);
    fwrite(&count_of_bytes, sizeof(size_t), 1, fp);
}


void do_assemble_file(Command* commands, size_t count_of_commands, Label* scratches, size_t label_count,  char* to_file){
     
    char* tmp = (char*)calloc(count_of_commands * (sizeof(char) + (sizeof(char) + sizeof(double))*max_argc) + 1, sizeof(char));    

    size_t offset = 0;

    for(size_t i = 0; i < count_of_commands; i++){
        offset += translate_line(commands + i, tmp, offset, scratches, label_count);
    }

    FILE* fp = fopen(to_file, "wb");

    add_metadata(fp, count_of_commands, offset);
    fwrite(tmp, sizeof(char), offset, fp);

    free(tmp);
    fclose(fp);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                               //
//                                   ASSEMBLE BLOCK END                                                                          //
//                                                                                                                               //
//                                                                                                                               //
//                                                                                                                               //
//                                   LABELS BLOCK BEGIN                                                                          //
//                                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static size_t labels_find_offset(assembler_command operation, const char* from, Command* com, int* flag){
    assert(from != NULL);
    assert(com != NULL);

    size_t curr_argc = argc[operation];
    
    if(curr_argc == 0){
        return 0;
    }
    
    size_t offset = 0;

    char** args = parse_arguments(from , curr_argc);
    com->arguments = args;

    offset += sizeof(char);
    
    if(isJMPoperation(com->command)){
        offset += sizeof(double);
    }else{
        double arg = 0;
        int read = 0;

        char* curr_arg1 = NULL;
        char* curr_arg2 = NULL;

        int delta = 0;
        int plus = 0;

        for(int i = 0; i < curr_argc; i++){
            plus = 0;
            delta = 0;
            char* curr_arg1 = com->arguments[i];

            while(*curr_arg1 != 0){
                if(*curr_arg1 == '+'){
                    offset += (sizeof(double) + sizeof(char));
                    plus = 1;
                }
                curr_arg1++;
            }

            char last = *(curr_arg1 - 1);
             
            if(last ==']' || *com->arguments[i] == '['){
                if(last !=']' || *com->arguments[i] != '['){
                    *flag = 2;
                }
                delta = 1;
            }
            
            if(plus == 0){
               read = sscanf(com->arguments[i] + delta, "%lg", &arg);
               if(read == 0){
                   if(com->arguments[i][delta] != 'r' || com->arguments[i][delta + 1] - 'a' >= register_count || com->arguments[i][delta + 2] != 'x'){
                       *flag = 2;
                   }
                   offset += sizeof(char);
               }else{
                   offset += sizeof(double);
               }
            }
        }
    }
    return offset;        

}



static size_t first_iteration_translate_line(line* str, size_t offset, Label* scratches, size_t* currnt_scratch, Command* commands, size_t* count_of_commands, int* flag){
    assert(str != NULL);
    assert(scratches != NULL);
    assert(currnt_scratch != NULL);

    *flag = 0;

    int f = 0;
    int check = -1;
    check = sscanf(str->line, "%d", &f);
    if(str->count == 0 || str->line[0] == '/' || check == -1){
        return 0;
    }

    size_t delta = 0;
    size_t curr_off = 0;
    double tmp = 0;

    const char* res = strchr(str->line, ':');

    if(res != NULL){
        char* buf1 = (char*)calloc(str->count + 1, sizeof(char));
        
        sscanf(str->line, "%s", buf1);
        buf1[strlen(buf1)-1] = 0;

        scratches[*currnt_scratch].name = buf1;
        scratches[*currnt_scratch].position = offset;

        (*currnt_scratch)++;
    
    }else{

        assembler_command operation = find_operation(str->line, &delta);

        commands[*count_of_commands].command = operation;
        if(operation == ERROR){
            *flag = 1;
        }else{
            curr_off = labels_find_offset(operation, str->line + delta, commands + *count_of_commands, flag) + sizeof(char);
            (*count_of_commands)++;
        }
    }

    return curr_off;
}



int first_iteration_of_assembling(line* data, Label* scratches, size_t* count_of_scratches, size_t num_of_lines, Command* commands, size_t* count_of_commands, char* errors){
    assert(data != NULL);
    assert(scratches != NULL);
    assert(commands != NULL);
    assert(errors != NULL);

    int flag = 0;
    int f = 0;
    size_t offset = 0;

    for(size_t i = 0; i < num_of_lines; i++){

        offset += first_iteration_translate_line(data + i, offset, scratches, count_of_scratches ,commands, count_of_commands, &flag);
        errors[i] = flag;
        f+=flag;
    }
    return f;
}




///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                               //
//                                                                                                                               //
//                                                                                                                               //
//                                  LABELS BLOCK END                                                                             //
//                                                                                                                               //
//                                                                                                                               //
//                                                                                                                               //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void call_diagnostic(line* data, size_t num_of_lines, char* errors, char* file_name){
    printf("\e[1;31mWrong syntax:\e[0m\n");
    for(int i = 0; i < num_of_lines; i++){
        if(errors[i] == 1){
            ///printf("%s:%d: \x1b[33;45 %s \x1b[0m\n",file_name, i, data[i].line);
            int delta = 0;
            while(data[i].line[delta] == ' ' || data[i].line[delta] == '\t'){
                delta++;
            }
            printf("Wrong  command:\e[4m\e[92m%s:%d:\e[24m \e[1;31m%s\e[0m\n",file_name, i, data[i].line + delta);

        }
        if(errors[i] == 2){
            int delta = 0;
            while(data[i].line[delta] == ' ' || data[i].line[delta] == '\t'){
                delta++;
            }
            printf("Wrong argument:\e[4m\e[92m%s:%d:\e[24m \e[1;31m%s\e[0m\n",file_name, i, data[i].line + delta);
        }
    }

}




void assemble_file(char* from_file, char* to_file){
    assert(from_file != NULL);
    assert(to_file != NULL);

    size_t num_of_lines = 0;
    char* ptr_on_buff = read_raw_data(from_file, &num_of_lines);
    line* data = parse_buffer(ptr_on_buff, num_of_lines);

    Label* scratches = (Label*)calloc(max_label_count, sizeof(Label));

    Command* commands = (Command*)calloc(num_of_lines, sizeof(Command));

    size_t count_of_commands = 0;
    size_t count_of_scratches = 0;

    char* errors = (char*)calloc(num_of_lines, sizeof(char));

    int flag = first_iteration_of_assembling(data, scratches, &count_of_scratches, num_of_lines, commands, &count_of_commands, errors);

    

    if(flag != 0){
        call_diagnostic(data, num_of_lines, errors, from_file);
        fflush(stdout);
        abort();
    }
    free_data(data, ptr_on_buff);

    do_assemble_file(commands, count_of_commands, scratches, count_of_scratches, to_file);

    
    for(int i = 0; i < count_of_scratches; i++){
        free(scratches[i].name);
    }

    for(int i = 0; i < count_of_commands; i++){
        for(int j = 0; j < argc[commands[i].command]; j++){
            free(commands[i].arguments[j]);
        }
        free(commands[i].arguments);
    }
    free(commands);
    free(errors);
    free(scratches);
}
