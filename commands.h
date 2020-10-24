#define NUMERIC_OFF sizeof(double)
#define MODE_OFF sizeof(char)
#define REGISTER_OFF sizeof(char)
#define RIP cp->RIP
#define PUSH(num)                                                   \
    stack_push(stack, arg ## num);

#define GET_ARG(num)                                                \
    double arg ## num = *((double*)(buffer + RIP));                 \
    RIP += sizeof(double);

#define GET_REG(num)                                                \
    char reg ## num = *(buffer + RIP);                              \
    RIP += sizeof(char);

#define GET_COMMON_ARG(num)                                         \
    char mode = *(buffer + RIP);                                    \
    RIP += sizeof(char);                                            \
    double tmp = 0;                                                 \
    if(mode & 2){                                                   \
        tmp += cp->registers[*(buffer + RIP)];                      \
        RIP += sizeof(char);                                        \
    }                                                               \
    if(mode & 1){                                                   \
        tmp += *((double*)(buffer + RIP));                          \
        RIP += sizeof(double);                                      \
    }                                                               \
    double arg ## num = -228;                                       \
    if(mode & 4){                                                   \
        arg ## num = ram->data[(size_t)tmp];                        \
    }else{                                                          \
        arg ## num = tmp;                                           \
    }                                                               
    
    
    

#define POP(num)                                                    \
    double arg ## num = -29;                                        \
    if(stack_pop(stack, &arg ## num) != STACK_OK){                  \
        printf("%lu ", RIP);                                        \
    }

#define GET_MODE                                                    \
    char mode = *(buffer + RIP);                                    \
    RIP += sizeof(char);

#define REGISTER(num)                                               \
    cp->registers[reg ## num]


COMMAND (RET,0,0,{
    if((cp->call_stack).size == 0){
        RIP = cp->exec_buffer_size;
    }else{
        double arg1 = 0;
        stack_pop(&(cp->call_stack), &arg1);
        RIP = (size_t)arg1;
    }
})

COMMAND (PUSH,1,1, {
            GET_COMMON_ARG(1)
            PUSH(1)   
})

COMMAND (ADD,2,0, {
            POP(1)
            POP(2)
            arg1 = arg1 + arg2;
            PUSH(1)
}) 

COMMAND (SUB,3,0, {
            POP(1)
            POP(2)
            arg1 = arg2 - arg1;
            PUSH(1);
})

COMMAND (MUL,4,0, {
            POP(1)
            POP(2)
            arg1 = arg2 * arg1;
            PUSH(1);
})

COMMAND (OUT,5,0, {
            POP(1)
            printf("%lg \n", arg1);
            fflush(stdout);
})

COMMAND (DUMP,6,0, {
            stack_dump(stack, "log.txt", DUMP_ALL, "dump in execution", create_log_data(__FILE__, __FUNCTION__, __LINE__));
})

COMMAND (DIV,7,0, {
            POP(1)
            POP(2)
            arg1 = arg2 / arg1;
            PUSH(1);
})

COMMAND(SQRT,8,0, {
            POP(1)
            arg1 = sqrt(arg1);
            PUSH(1);
            })

COMMAND(SIN,9,0, {
            POP(1)
            arg1 = sqrt(arg1);
            PUSH(1);
})

COMMAND(COS,10,0, {
            POP(1)
            arg1 = cos(arg1);
            PUSH(1);
}) 

COMMAND(IN,11,0, {
            double arg1 = 0;
            printf("Enter the number\n");
            scanf("%lg", &arg1);
            PUSH(1);            
})

COMMAND(MOV,12,2, {
            RIP += MODE_OFF;
            GET_REG(1)

            RIP += MODE_OFF;
            GET_REG(2)
            
            REGISTER(1) = REGISTER (2);
})

COMMAND(POP,13,1, {
            GET_MODE
            double tmp = 0;
            if(mode & 4){
                if(mode & 2){                                                   
                    tmp += cp->registers[*(buffer + RIP)];                      
                    RIP += sizeof(char);                                        
                }                                                               
                if(mode & 1){                                                   
                    tmp += *((double*)(buffer + RIP));                          
                    RIP += sizeof(double);                                      
                }
                POP(1)
                ram->data[(size_t)tmp] = arg1;            
            }else{
                GET_REG(1)
                POP(1)
                REGISTER(1) = arg1;
            }
            
})

COMMAND(COPY,14,0, {
            double arg1 = 1;
            stack_back(stack, &arg1);
            PUSH(1)
})

COMMAND(JMP,15,1, {
            RIP += MODE_OFF;
            GET_ARG(1)
            RIP = (size_t)arg1;
})

COMMAND(JAE,16,1, {
            
            POP(1)
            POP(2)
            
            RIP += MODE_OFF;
            GET_ARG(3)
            
            if(arg2 >= arg1){
                RIP = (size_t)arg3;
            }else{
                PUSH(2)
            }

})
COMMAND(JA,17,1, {
            
            POP(1)
            POP(2)
            
            RIP += MODE_OFF;
            GET_ARG(3)
            
            if(arg2 > arg1){
                RIP = (size_t)arg3;
            }else{
                PUSH(2)
            }

})  
COMMAND(JBE,18,1, {
            
            POP(1)
            POP(2)
            
            RIP += MODE_OFF;
            GET_ARG(3)
            
            if(arg2 <= arg1){
                RIP = (size_t)arg3;
            }else{
                PUSH(2)
            }

}) 
COMMAND(JB,19,1, {
            
            POP(1)
            POP(2)
            
            RIP += MODE_OFF;
            GET_ARG(3)
            
            if(arg2 < arg1){
                RIP = (size_t)arg3;
            }else{
                PUSH(2)
            }

})
COMMAND(JE,20,1, {
            
            POP(1)
            POP(2)
            
            RIP += MODE_OFF;
            GET_ARG(3)
            
            if(arg2 == arg1){
                RIP = (size_t)arg3;
            }else{
                PUSH(2)
            }

}) 
COMMAND(JN,21,1, {
            
            POP(1)
            POP(2)
            
            RIP += MODE_OFF;
            GET_ARG(3)
            
            if(arg2 != arg1){
                RIP = (size_t)arg3;
            }else{
                PUSH(2)
            }

})

COMMAND(POW,22,1, {
            POP(1)
            RIP += MODE_OFF;
            GET_ARG(2)
            arg1 = pow(arg1, arg2);
            PUSH(1);
})

COMMAND(CALL, 23, 1, {
           RIP += MODE_OFF;
           GET_ARG(1)
           stack_push(&(cp->call_stack), RIP);
           RIP = (size_t)arg1;
})

#undef PUSH 
#undef GET_ARG 
#undef GET_REG
#undef GET_ARG_FROM_REG 
#undef POP
#undef GET_MODE
#undef REGISTER
#undef RIP
#undef NUMERIC_OFF
#undef MODE_OFF
#undef REGISTER_OFF