#include "assembler.h"
#include "../cpu/cpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                 LABEL PREPARATION BEGIN                                                                            ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int no_exist_label_on_pos(Label* scratches, size_t labels_count, size_t pos){
    assert(scratches != NULL);

    for(int i = 0; i < labels_count; i++){
        if(scratches[i].position == pos){
            return 0;
        }
    }
    return 1;
}

char* find_name_by_pos(Label* scratches, size_t label_count, size_t pos){
    assert(scratches != NULL);

    for(int i = 0; i < label_count; i++){
        if(scratches[i].position == pos){
            return scratches[i].name;
        }
    }

    printf("Wrong position: %lu\n", pos);
    fflush(stdout);
    abort();
}


void find_label_if_jmp(char* buffer, size_t* offset, Label* scratches, size_t* labels_count){
    assert(buffer != NULL);
    assert(scratches != NULL);

    char operation = *((char*)buffer + *offset);
    char mode = 0;
    
    *offset += sizeof(char);

    fflush(stdout);
    if(isJMPoperation((assembler_command)operation)){
        *offset += sizeof(char);
        
        size_t pos = *((double*)(buffer + *offset));
        
        *offset += sizeof(double);

        if(no_exist_label_on_pos(scratches, *labels_count, pos)){
            
            scratches[*labels_count].position = pos;

            char* tmp1 =  (char*)calloc(9, sizeof(char)); 
            
            strcat(tmp1, "label");
            sprintf(tmp1 + 5, "%lu", *labels_count);

            scratches[*labels_count].name = tmp1;
            
            (*labels_count)++;
            return;
        }
    }

    size_t argc_curr = argc[operation];

    for(int i = 0; i < argc_curr; i++){
        mode = *(buffer + *offset);
        *offset += sizeof(char);
        if(mode & 1){
            *offset += sizeof(double);
        }
        if(mode & 2){
            *offset += sizeof(char);
        }
    }
}

size_t create_labels(char* buffer, size_t count_of_lines, Label* scratches){

    FILE* fq = fopen("disassemble.txt", "w");
    size_t curr = 0;
    size_t labels_count = 0;
    for(int i = 0; i < count_of_lines; i++){
        find_label_if_jmp(buffer, &curr, scratches, &labels_count);
    }

    fclose(fq);
    return labels_count;
}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                             LABEL PREPARATION END                                                                  ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                             DISASSEMBLE BEGIN                                                                      ///
///                                                                                                                                    ///
///                                                                                                                                    ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////






size_t disassemble_line(char* buffer, FILE* fp, Label* scratches, size_t labels_count){
    assert(buffer != NULL);
    assert(fp != NULL);
    assert(scratches != NULL);

    size_t curr_offset = 0;
    char operation = *((char*)buffer);
    curr_offset += sizeof(char);

    fprintf(fp, "%s ", command_names[operation]);

    if(isJMPoperation((assembler_command)operation)){
        curr_offset += sizeof(char);

        size_t pos = (size_t)(*((double*)(buffer + curr_offset)));
        curr_offset += sizeof(double);
            
        char* s = find_name_by_pos(scratches, labels_count, pos);
        fprintf(fp, "%s\n", s);
    }else{

        char mode = 0;
        double darg = 0;
        char rarg = 0;

        size_t argc_curr = argc[operation];

        for(int i = 0; i < argc_curr; i++){
            mode = *(buffer + curr_offset);
            curr_offset += sizeof(char);
            if(mode & 4){
                fprintf(fp, "[");
            }
            if(mode & 2){
                rarg = *((char*)(buffer + curr_offset)) + 'a';
                curr_offset += sizeof(char);
                fprintf(fp, "r%cx", rarg);    
            }

            if(mode & 1 && mode & 2){
                fprintf(fp, "+");
            }

            if(mode & 1){
                darg = *((double*)(buffer + curr_offset));
                curr_offset += sizeof(double);
                fprintf(fp, "%lg", darg);
            }
            

            if(mode & 4){
                fprintf(fp, "]");
            }
            fprintf(fp, " ");

        }
        fprintf(fp, "\n");
    }
    return curr_offset;

}

void do_disassemble_file(char* buffer, size_t count_of_lines, Label* scratches, size_t count_of_labels){
    assert(buffer != NULL);
    assert(scratches != NULL);

    FILE* fq = fopen("disassemble.txt", "w");
    
    size_t offset = 0, 
           off = 0, 
           curr_label = 0;
    
    if(curr_label < count_of_labels && scratches[curr_label].position == offset){
        fprintf(fq,"%s:\n", scratches[curr_label].name);
        curr_label++;
    }
    
    for(int i = 0; i < count_of_lines; i++){

        off = offset;
        offset += disassemble_line(buffer + off, fq, scratches, count_of_labels);

        if(curr_label < count_of_labels && scratches[curr_label].position == offset){
            fprintf(fq,"%s:\n", scratches[curr_label].name);
            curr_label++;
        }
    
    }
    

    fclose(fq);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                             DISASSEMBLE END                                                                        ///
///                                                                                                                                    ///
///                                                                                                                                    ///
///                                                                                                                                    ///
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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




int labels_cmp(const void* a, const void* b){
    return ((Label*)a)->position - ((Label*)b)->position; 
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
    
    Label* scratches = (Label*)calloc(max_label_count, sizeof(Label));

    size_t scratches_count = create_labels(buffer, count_of_lines, scratches);

    qsort(scratches, scratches_count, sizeof(Label), labels_cmp);

    do_disassemble_file(buffer, count_of_lines,scratches,scratches_count);
    for(int i = 0; i < scratches_count; i++){
        free(scratches[i].name);
    }
    free(scratches);
    free(buffer);
    fclose(fp);
}