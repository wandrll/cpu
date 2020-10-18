#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <sys/stat.h>
#include "data.h"
/** \file */

size_t count_of_symbol(const char* buf, char c){
    size_t count = 0;
    while(*buf != 0){
        buf = strchr(buf, c);
        count++;
        buf++;
    }
    return count;
}


size_t file_size(const char* file){
    assert(file != NULL);
    struct stat st = {};
    stat(file, &st);
    return st.st_size;
}


struct line* data_mem_alloc(size_t count){
    struct line* data = (struct line*)calloc(count, sizeof(struct line));

    return data;
}


char* read_raw_data(const char* file, size_t* lines_count){
    assert(file != NULL);
    FILE* fb = fopen(file, "rb");
    assert(fb != NULL);
   
    size_t fsize = file_size(file);
    char* buffer_for_strings = (char*)calloc(fsize + 2, sizeof(char));
    fread(buffer_for_strings + 1, sizeof(char), fsize, fb);

    *lines_count = count_of_symbol(buffer_for_strings + 1, '\n');
    
    fclose(fb);
    
    return buffer_for_strings;
}


struct line* parse_buffer(char* buffer, size_t count){
    assert(buffer != NULL);

    struct line* data = data_mem_alloc(count);
    assert(data != NULL);

    char* first_ptr = buffer + 1;
    char* second_ptr = first_ptr;
    int curr_line = 0;

    for(int i = 0; i < count; i++){
        first_ptr = strchr(first_ptr, '\n');
        *first_ptr = '\0';            
        data[i].count = first_ptr - second_ptr ;
        data[i].line = second_ptr;
        first_ptr++;
        second_ptr = first_ptr;       
    }

    return data;
}




void create_bin(const struct line* data, size_t n, const char* file){
    assert(data != NULL);

    FILE* fp = fopen(file, "wb");
    putc('\0', fp);
    for(size_t i = 0; i < n; i++){
        fwrite(data[i].line, sizeof(char), data[i].count, fp);
        putc('\0', fp);
    }
    
    fclose(fp);
}



char* read_bin(struct line* data, size_t count, const char* bin_file){
    assert(data != NULL);
    FILE* fb = fopen(bin_file, "rb");
    assert(fb != NULL);
    int fsize = file_size(bin_file);
    char* buffer_for_strings = (char*)calloc(fsize + 2, sizeof(char));
    
    fread(buffer_for_strings, sizeof(char), fsize, fb);

    char* to = buffer_for_strings + 1;
    int curr_count = 0;
    int curr_line = 0;

    for(size_t i = 1; i < fsize; i++){
        if(buffer_for_strings[i] == '\0'){
            data[curr_line].count = curr_count;
            data[curr_line].line = to;
            curr_count = 0;
            curr_line++;
            to = buffer_for_strings + i + 1;
        }else{
            curr_count++;
        }
    }

    fclose(fb);
    return buffer_for_strings;
}




void save_line(const struct line* line, FILE* fp){
    assert(line != NULL);
    assert(fp != NULL);
    
    fwrite(line->line, sizeof(char), line->count, fp);
    putc('\n', fp);

}


void save_data(const struct line* data, size_t count, const char* file){
    assert(data != NULL);
    assert(file != NULL);
    FILE* fp = fopen(file, "a");
    assert(fp != NULL);

    for(int i = 0; i < count; i++){
        save_line(&data[i], fp);
    }
    const char end[] = "################################################################################\n"; // 81 символов
    fwrite(end, sizeof(char),sizeof(end)-1 , fp);
    fclose(fp);
}

void free_data(struct line* data, char* buffer_for_strings){
    assert(data != NULL);
    assert(buffer_for_strings != NULL);
    free(buffer_for_strings);
    free(data);
}

void recreate_data(struct line* data, const char* ptr_on_buff, size_t num_of_lines){
    assert(data != NULL);
    assert(ptr_on_buff != NULL);

    int currl = 0;
    const char* curr = ptr_on_buff + 1;
    const char* to = ptr_on_buff + 1;

    while(currl < num_of_lines){
        if(*curr == '\0'){
            data[currl].line = to;
            data[currl].count = curr - to;
            to = curr + 1;
            currl++;
        }
        curr++;
    }
}

