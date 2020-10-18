
#include "defines.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <sys/stat.h>
#include "stack.h"
#include "log.h"
#include <time.h>

const char log_file[] = "error_log.txt";



#ifdef CANARY_PROTECTION
    const int CANARY_SIZE = 100; // Last byte is always 0 (or size is 0) 
    
    const Canary* generate_canary(size_t size){
        Canary* can = (Canary*)calloc(1, sizeof(Canary));
        can->size = size;
        can->value = (char*)calloc(size, sizeof(char));
        srand(time(NULL));
        for(int i = 1; i < size; i++){
            can->value[i-1] = rand()%64 + 64;
        }
        return can;
    }

    const Canary* canary = generate_canary(CANARY_SIZE);
    
    int check_left_canary(Stack_t* stack){
        if(canary->size == 0){
            return 1;
        }
        if(!strcmp((char*)(stack->data) - canary->size, canary->value)){
            return 1;
        }else{
            return 0;
        }
    }

#endif

#ifdef DEBUG_ON
    #define IF_DEBUG_ON(code) code
#else
    #define IF_DEBUG_ON(code)
#endif

#ifdef VERIFY_STACK
    const int max_stack_count = 1000;                                                               
    struct pair{                                                                                        
        Stack_t* stack;                                                                                     
        char* file;                                                                                             
    };                                                                                                      
    static pair all_stacks[max_stack_count] = {};
    
    size_t find_stack(Stack_t* stack){
        int curr = 0;
        while(all_stacks[curr].stack != stack && curr < max_stack_count){
            curr++;
        }
        if(curr = max_stack_count - 1 && all_stacks[curr].stack != stack){
            return -1;
        }
        return curr;
    }

    void remove_stack(Stack_t* stack){
        size_t ind = find_stack(stack);                                                                                                                                                     \
        remove(all_stacks[ind].file);                                                                                                                                                       \
        all_stacks[ind].file = NULL;                                                                                                                                                        \
        all_stacks[ind].stack = NULL;  
    }

    void first_save_stack(Stack_t* stack){
        int curr = 0;                                                                                                                                                                       
        while(all_stacks[curr].stack != NULL){                                                                                                                                              
            curr++;                                                                                                                                                                         
        }                                                                                                                                                                                   
        all_stacks[curr].stack = stack;                                                                                                                                                     
        all_stacks[curr].file = (char*)calloc(L_tmpnam+1, sizeof(char));                                                                                                                    

        tmpnam(all_stacks[curr].file);                                                                                                                                                      
        save_current_stack(stack);                                                                                    
    }
#endif



void stack_verification(Stack_t* stack, log_data* data){
    #ifdef ADRESS_VALIDATION                                                                                                                                                                                      
        if(!validate_pointer(stack)){                                                                                                                                         
            stack_dump(stack, log_file, DUMP_STACK_ADRESS, "stack has non-valid adress", data);                                                                                            
            abort();                                                                                                                                                                        
        }                                                                                                                                                                                   
        if(!validate_pointer(stack->data)){                                                                                                                                                      
            stack_dump(stack, log_file, DUMP_STACK_ADRESS + DUMP_DATA_ADRESS, "data has non-valid adress", data);                                                                          
            abort();                                                                                                                                                                        
        }                                                                                                                               
    #endif

    #ifdef CANARY_PROTECTION
        if(check_left_canary(stack) == 0){                                                                                                                                                  
            stack_dump(stack, log_file, DUMP_STACK_ADRESS + DUMP_DATA_ADRESS + DUMP_STACK_LEFT_CANARY, "Left canary is dead inside", data);                                                
            abort();                                                                                                                                                                        
        }
    #endif

    #ifdef VERIFY_STACK
        int check_status = check_stack(stack);                                                                                                                                              
        if(check_status == -1){                                                                                                                                                             
            stack_dump(stack, log_file, DUMP_STACK_ADRESS + DUMP_DATA_ADRESS + DUMP_STACK_SIZE + DUMP_STACK_CAPACITY, "You used direct access to change size or capacity", data);          
            abort();                                                                                                                                                                        
        }                                                                                                                                                                                   
        if(check_status == 0){                                                                                                                                                              
            stack_dump(stack, log_file, DUMP_ALL, "You used direct access", data);                                                                                                         
            abort();                                                                                                                                                                        
        }              
    #endif
}

////////////////////////////////////STACK FUNCTIONS///////////////////////////////////////

stack_end_signal stack_constructor(Stack_t* stack, size_t capacity){
    assert(stack != NULL);

    size_t delta = 0;

    #ifdef CANARY_PROTECTION
        delta = canary->size;
    #endif

    char* tmp = (char*)calloc(1, sizeof(stack_elem_t)*capacity + 2 * delta);
    if(tmp == NULL){
        return STACK_CONSTRUCTION_ERROR;
    }
    stack->capacity = capacity;
    stack->size = 0;

    #ifdef CANARY_PROTECTION
        memcpy(tmp, canary->value, canary->size);
        memcpy(tmp + canary->size + sizeof(stack_elem_t)*capacity, canary->value, canary->size);
    #endif
    
    stack->data = (stack_elem_t*)(tmp + delta);

    IF_DEBUG_ON(first_save_stack(stack);)
    return STACK_OK;
}



static stack_end_signal stack_increase_capacity(Stack_t* stack){
    assert(stack != NULL);
    
    size_t delta = 0;

    #ifdef CANARY_PROTECTION
        delta = canary->size;
    #endif

    char* tmp = (char*)calloc(1, sizeof(stack_elem_t)*(stack->capacity * 2 + 1) + 2 * delta);
    if(tmp != NULL){
            
        #ifdef CANARY_PROTECTION
            memcpy(tmp, canary->value, canary->size);
            memcpy(tmp + canary->size + sizeof(stack_elem_t)*(stack->capacity * 2 + 1), canary->value, canary->size);
        #endif

        stack_elem_t* new_data = (stack_elem_t*)(tmp + delta);
        for(int i = 0; i < stack->size; i++){
            new_data[i] = stack->data[i];
        }

        free(((char*)stack->data) - delta);
        stack->data = new_data;
        stack->capacity = stack->capacity * 2 + 1;
        return STACK_OK;
    }else{
        stack_dump(stack, log_file, DUMP_ALL - DUMP_STACK_DATA, "Error caused by attempt to increase stack capacity", create_log_data(__FILE__, __FUNCTION__, __LINE__));
        return STACK_DECREASE_ERROR;
    }
}



stack_end_signal stack_push(Stack_t* stack, stack_elem_t elem_to_push){
    
    IF_DEBUG_ON(stack_verification(stack, create_log_data(__FILE__, __FUNCTION__, __LINE__));)

    if(stack->size == stack->capacity){
        if(stack_increase_capacity(stack) == STACK_INCREASE_ERROR){
            return STACK_INCREASE_ERROR;
        }
    }

    stack->data[stack->size] = elem_to_push;
    stack->size++;
    
    IF_DEBUG_ON(save_current_stack(stack);)
    return STACK_OK;
}



static stack_end_signal stack_decrease_capacity(Stack_t* stack){
    assert(stack != NULL);
    size_t delta = 0;

    #ifdef CANARY_PROTECTION
        delta = canary->size;
    #endif

    char* tmp = (char*)calloc(1, sizeof(stack_elem_t)*(stack->capacity / 2) + 2 * delta);
    if(tmp != NULL){
            
        #ifdef CANARY_PROTECTION
            memcpy(tmp, canary->value, canary->size);
            memcpy(tmp + canary->size + sizeof(stack_elem_t)*(stack->capacity / 2), canary->value, canary->size);
        #endif

        stack_elem_t* new_data = (stack_elem_t*)(tmp + delta);
        for(int i = 0; i < stack->size; i++){
            new_data[i] = stack->data[i];
        }

        free(((char*)stack->data) - delta);
        stack->data = new_data;
        stack->capacity = stack->capacity / 2;
        return STACK_OK;
    }else{
        stack_dump(stack, log_file, DUMP_ALL - DUMP_STACK_DATA, "Error caused by attempt to decrease stack capacity", create_log_data(__FILE__, __FUNCTION__, __LINE__));
        return STACK_DECREASE_ERROR;
    }
}


stack_end_signal stack_pop(Stack_t* stack, stack_elem_t* result){
    
    IF_DEBUG_ON(stack_verification(stack, create_log_data(__FILE__, __FUNCTION__, __LINE__));)

   
    if(stack->size == 0){
        stack_dump(stack, log_file, DUMP_ALL, "warning, attempt to pop from empty stack",create_log_data(__FILE__, __FUNCTION__, __LINE__));
        return STACK_EMPTY;
    }else{
        if(stack->size < stack->capacity / 4){
            if(stack_decrease_capacity(stack) == STACK_DECREASE_ERROR){
                return STACK_DECREASE_ERROR;
            }
        }
        *result = stack->data[stack->size - 1];
        stack->data[stack->size - 1] = 0;
        stack->size--;
        
        IF_DEBUG_ON(save_current_stack(stack);)

        return STACK_OK;
    }
}


stack_end_signal stack_back(Stack_t* stack, stack_elem_t* result){
    
    IF_DEBUG_ON(stack_verification(stack, create_log_data(__FILE__, __FUNCTION__, __LINE__));)

    if(stack->size == 0){
        //printf("WARNING. BACK FROM EMPTY STACK. EXECUTION WON'T BE STOPPED\n");
        stack_dump(stack, log_file, DUMP_ALL, "warning, attempt to pop from empty stack",create_log_data(__FILE__, __FUNCTION__, __LINE__));
        return STACK_EMPTY;
    }else{
        *result = stack->data[stack->size - 1];
        
        return STACK_OK;
    }
}

stack_end_signal stack_clear(Stack_t* stack){
    IF_DEBUG_ON(stack_verification(stack, create_log_data(__FILE__, __FUNCTION__, __LINE__));)

    for(int i = 0; i < stack->size; i++){
        stack->data[i] = 0;
    }

    stack->size = 0;

    IF_DEBUG_ON(save_current_stack(stack);)
    return STACK_OK;
}



stack_end_signal stack_destructor(Stack_t* stack){

    IF_DEBUG_ON(remove_stack(stack);)

    stack->size = 0;
    stack->capacity = 0;
    size_t delta = 0;

    #ifdef CANARY_PROTECTION
        delta = canary->size;
    #endif
        
    free(((char*)stack->data) - delta);

    stack->data = NULL;

    return STACK_OK;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////




////////////////////////PROTECTION BLOCK///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef VERIFY_STACK

void save_current_stack(Stack_t* stack){
    assert(stack != NULL);
    char* file = all_stacks[find_stack(stack)].file;
    assert(file != NULL);
    
    chmod(file, S_IRUSR+S_IWUSR + S_IRGRP + S_IWGRP + S_IROTH);
    FILE* fp = fopen(file, "wb");
    
    fprintf(fp, "%lu %lu\n",stack->size, stack->capacity);
    
    for(int i = 0; i < stack->capacity; i++){
        print_stack_elem((stack->data)[i], fp);
        fprintf(fp,"\n");
    }
    chmod(file, S_IRUSR);
    fclose(fp);
}

static void read_stack_elem(int* a, FILE* fp){
    fscanf(fp, "%d", a);
}

static void read_stack_elem(double* a, FILE* fp){
    fscanf(fp, "%lg", a);
}

const double precision = 1e-6;
static int cmp_elem_stack(int a, int b){
    return a - b;
}

static int cmp_elem_stack(double a, double b){
    if(abs(a - b) < precision){
        return 0;
    }else{
        return a - b;
    }
}

int check_stack(Stack_t* stack){
    assert(stack != NULL);
    char* file = all_stacks[find_stack(stack)].file;
    chmod(file, S_IRUSR);

    size_t size_r, capacity_r;
    FILE* fp = fopen(file, "rb");
    assert(fp != NULL);
    fscanf(fp, "%lu %lu", &size_r, &capacity_r);
    if(size_r != stack->size || capacity_r != stack->capacity){
        if(size_r != stack->size){
            return -1;
        }
        if(capacity_r != stack->capacity){
            return -1;
        }
    }else{
        for(size_t i = 0; i < capacity_r; i++){
            stack_elem_t tmp;
            read_stack_elem(&tmp, fp);
            if(cmp_elem_stack(stack->data[i], tmp) != 0){
                return 0;
            }

        }
    }
    fclose(fp);
    chmod(file, S_IRUSR);

    return 1;
}

#endif

int validate_pointer(void* a){
    if(a < (void*)4096){
        return 0;
    }else{
        return 1;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////



