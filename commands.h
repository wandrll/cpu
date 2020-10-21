#define PUSH(num)                                                   \
    stack_push(stack, arg ## num);

#define GET_ARG(num)                                                \
    double arg ## num = *((double*)(buffer + *offset));             \
    *offset += sizeof(double);

#define GET_REG(num)                                                \
    char reg ## num = *(buffer + *offset);                          \
    *offset += sizeof(char);

#define GET_ARG_FROM_REG(num_to, num_from)                          \
    double arg ## num_to = cp->registers[reg ## num_from];

#define POP(num)                                                    \
    double arg ## num = 0;                                          \
    stack_pop(stack, &arg ## num);

#define GET_MODE                                                    \
    char mode = *(buffer + *offset);                                \
    *offset += sizeof(char);

#define REGISTER(num)                                               \
    cp->registers[reg ## num]

COMMAND (HET,0,0,{

})

COMMAND (PUSH,1,1, {
                GET_MODE
                if(mode == 1){
                    GET_ARG(1)
                    PUSH(1)
                }else{
                    GET_REG(1)
                    GET_ARG_FROM_REG(1,1)
                    PUSH(1)
                }   
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
            *offset += sizeof(char);
            GET_REG(1)

            *offset += sizeof(char);
            GET_REG(2)
            
            REGISTER(1) = REGISTER (2);
})

COMMAND(POP,13,1, {
            *offset += sizeof(char);
            GET_REG(1)
            POP(1)
            REGISTER(1) = arg1;
})

COMMAND(COPY,14,0, {
            double arg1 = 1;
            stack_back(stack, &arg1);
            PUSH(1)
})

COMMAND(JMP,15,1, {
            *offset += sizeof(char);
            GET_ARG(1)
            *offset = (size_t)arg1;
            //printf(" %lu ", *offset);
})

COMMAND(JAE,16,1, {
            
            POP(1)
            POP(2)
            
            *offset += sizeof(char);
            GET_ARG(3)
            
            if(arg2 >= arg1){
                *offset = (size_t)arg3;
            }else{
                PUSH(2)
            }

}) 

#undef PUSH 
#undef GET_ARG 
#undef GET_REG
#undef GET_ARG_FROM_REG 
#undef POP
#undef GET_MODE
#undef REGISTER
