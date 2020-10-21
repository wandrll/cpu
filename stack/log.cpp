#include "defines.h"
#include <string.h>
#include "stack.h"
#include "log.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <assert.h>
const double precision = 1e-6;

#ifdef CANARY_PROTECTION
    extern Canary* canary;
#endif

void print_stack_elem(int a, FILE* fp){
    fprintf(fp, "%d", a);
}

void print_stack_elem(double a, FILE* fp){
    fprintf(fp, "%.32lg", a);
}


int read_and_cmp_stack_elems(int a, FILE* fp){
    int b  = 0;
    fscanf(fp, "%d", &b);

    return a - b; 
}

int read_and_cmp_stack_elems(double a, FILE* fp){
    double b  = 0;
    fscanf(fp, "%lg", &b);
    if(abs(a - b) < precision){
        return 0;
    }else{
        return a - b; 
    }
}

log_data* create_log_data(const char* file, const char* func, const int line){
    log_data* data = (log_data*)calloc(1, sizeof(log_data));
    data->file = (char*)calloc(strlen(file)+1, sizeof(char));
    strcpy(data->file, file);

    data->function = (char*)calloc(strlen(func)+1, sizeof(char));
    strcpy(data->function, func);

    data->line = line;
    return data;
}

void delete_log_data(log_data* data){
    free(data->file);
    free(data->function);
    free(data);
}


void stack_dump(Stack_t* stack, const char* log_name, const int mode, const char* message, log_data* data){
    assert(stack != NULL);
    FILE* fp = fopen(log_name, "a");
    fprintf(fp, "\n\n\n\n###############################################################\n");
    fprintf(fp, "------------------------------\n");
    fprintf(fp, "| message: %s \n", message);

    fprintf(fp, "| FILE: %s \n", data->file);
    fprintf(fp, "| FUNCTION: %s \n", data->function);
    fprintf(fp, "| LINE: %d \n", data->line);
    
    if(mode & DUMP_STACK_ADRESS){
        fprintf(fp, "| stack adress: %p \n",stack);
    }

    if(mode & DUMP_DATA_ADRESS){
        fprintf(fp, "| data adress: %p \n",stack->data);
    }

    if(mode & DUMP_STACK_SIZE){
        fprintf(fp,"| stack size: %lu  \n",stack->size);
    }

    if(mode & DUMP_STACK_CAPACITY){
        fprintf(fp,"| stack capacity: %lu  \n",stack->capacity);
    }

    fprintf(fp,"------------------------------\n");
    #ifdef CANARY_PROTECTION
            if(mode & DUMP_STACK_LEFT_CANARY){
                if(canary->size != 0){
                    fprintf(fp, "good canary: %s\n", canary->value);
                    fprintf(fp, "left canary: %s\n", (char*)stack->data-canary->size);
                }
            }   
    #endif
    if(mode & DUMP_STACK_DATA){
        size_t count = (stack->size + stack->capacity)/2 + (stack->size + stack->capacity)%2;
        for(int i = 0; i < count; i++){
            fprintf(fp, "data[%d] = ", i);
            print_stack_elem((stack->data)[i], fp);
            fprintf(fp, "\n");
        }
        fprintf(fp, "------------------------------\n");
    }
    #ifdef CANARY_PROTECTION
            if(mode & DUMP_STACK_RIGHT_CANARY){
                if(canary->size != 0){
                    fprintf(fp, "right canary: %s\n", (char*)stack->data + (stack->capacity * sizeof(stack_elem_t)));
                }
            }  
    #endif
    fprintf(fp, "###############################################################\n");
    fflush(fp);
    fclose(fp);
    delete_log_data(data);
}



//////////////TESTS///////////////


int random_stack_int_test(Stack_t* stack, size_t count){
    //remove("log.txt");
    assert(stack != NULL);
    srand(time(NULL));
    int curr = 100;
    stack_elem_t tmp = 0;
    for(int i = 0; i < count; i++){
        if(i % 1 == 0){
            stack_dump(stack, "log.txt", DUMP_ALL, "just regular log", create_log_data(__FILE__, __FUNCTION__, __LINE__));
        }
        curr = rand()%100;
        if(curr > 25){
            tmp = ((int)rand()/(10000));
            tmp  = tmp + 0.2;
            stack_push(stack, tmp);
        }else{
            stack_pop(stack, &tmp); 
            
        }
    }
    return 1;
} 



