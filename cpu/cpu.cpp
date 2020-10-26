#define DEBUG_ON

#ifdef DEBUG_ON
    #define IF_DEBUG_ON(code) code
#else
    #define IF_DEBUG_ON(code)
#endif


#include "cpu.h"
#include "../stack/stack.h"
#include "../constants.h"
#include "../stack/log.h"

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

void set_circle(Memory* mem, double r, double x, double y){
    double* vram = mem->data + mem->size/2;
    size_t n = sqrt(mem->size/2);
    double cell_size = win_size / n;
    double currx = 0;
    double curry = 0;
    double currR = 0;
    char R = 0;
    char G = 0;
    char B = 0;
    char* tmp = NULL;
    uint32_t converted = 0;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            curry = i * cell_size + cell_size/2;
            currx = j * cell_size + cell_size/2;
            currR = sqrt(pow(x-currx,2)+ pow(y-curry,2));
            ///printf("%lg %lg %lg | ", currx, curry, currR);
            if(abs(currR - r) < cell_size/2 + 0.001){
                R = rand()%256;
                G = rand()%256;
                B = rand()%256;
                 tmp = (char*)(&converted);
                *(tmp  ) = R;
                *(tmp+1) = G;
                *(tmp+2) = B;
                 vram[i*n + j] = converted;
            }
        }
        //printf("\n");
    }
}

void set_image(Memory* mem,  const char* file){
    double* vram = mem->data + mem->size/2;
    sf::Image im;
    im.loadFromFile(file);
    size_t n = sqrt(mem->size/2);
    sf::Vector2u size = im.getSize();

    //printf("%u %u", size.x, size.y);

    double x_size = double(size.x)/double(n);
    double y_size = double(size.y)/double(n);
    //printf("\n%lg %lg", x_size, y_size);



    sf::Color cl;
    uint32_t col = 0;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            col = 0;
            char* tmp = (char*)(&col);
            cl = im.getPixel(j * x_size + x_size/2, i * y_size + y_size/2);
            *(tmp+0) = cl.r;
            *(tmp+1) = cl.g;
            *(tmp+2) = cl.b;
            vram[i * n + j] = col;
        }
    }

}


void memory_constructor(Memory* mem, size_t count){
    mem->data = (double*)calloc(count, sizeof(double));
    mem->size = count;
}

void memory_destructor(Memory* mem){
    free(mem->data); 
}

void cpu_constructor(Cpu* cp){
    stack_constructor(&(cp->cpu_stack), 10);
    stack_constructor(&(cp->call_stack), 10);

    cp->RIP = 0;
    cp->registers = (double*)calloc(4, sizeof(double));
}

void cpu_destructor(Cpu* cp){
    stack_destructor(&(cp->call_stack));
    stack_destructor(&(cp->cpu_stack));
    free(cp->registers);
}


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

void redraw_from_vram(sf::RenderWindow* win, Memory* mem){
    double* vram = mem->data + mem->size/2;

    size_t n = sqrt(mem->size/2);
    size_t cell_size = win_size / n;
    
    win->clear();
    
    sf::RectangleShape rectangle;
    sf::Color col(0,0,0);
    unsigned char* tmp;
    uint32_t converted = 0;
    rectangle.setOutlineColor(col);
    rectangle.setFillColor(col);
    rectangle.setSize(sf::Vector2f(cell_size, cell_size));
   // printf("--------------------\n");
    for(int i = 0; i < n; i++){
        
        for(int j = 0; j < n; j++){
            converted = vram[i*n + j];
            char* ptr = (char*)&(converted);
            //printf("(%d %d %d) " ,*(ptr) ,*(ptr+1) ,*(ptr+2));      
            col.r = *(ptr  );
            col.g = *(ptr+1);
            col.b = *(ptr+2);
         
            rectangle.setPosition(j * cell_size, i* cell_size);
            rectangle.setOutlineColor(col);
            rectangle.setFillColor(col);
            win->draw(rectangle);
        }
        //printf("\n");

    }

}

void memory_dump(Memory* mem){
    printf("---------------------\n");
    for(int i = 0; i < mem->size; i++){
        printf("%lg ", mem->data[i]);
    }
    printf("\n");
}

void execute_command(Cpu* cp, Memory* ram, char* buffer){
    assert(cp != NULL);
    assert(buffer != NULL);
    
    Stack_t* stack = &(cp->cpu_stack);
 
    char operation = *((char*)buffer + cp->RIP);
    //printf("%s\n", command_names[operation]);
    cp->RIP += sizeof(char);
/////////////////////////////////////////////////////////////////////////////
    #define COMMAND(name, num, argc, code)\
        case V_ ## name : code break;

    switch(operation){        
        #include "../commands.h"
    }
    
    #undef COMMAND
/////////////////////////////////////////////////////////////////////////////
}

void cpu_execute_programm(Cpu* cp, Memory* mem, const char* file){
    assert(cp != NULL);
    assert(file != NULL);
    FILE* fp = fopen(file, "rb");
    assert(fp != NULL);

    check_executable_file(fp);
    
    size_t count_of_lines = 0;
    size_t count_of_bytes = 0;
    fread(&count_of_lines, sizeof(size_t), 1, fp);
    fread(&count_of_bytes, sizeof(size_t), 1, fp);

    cp->exec_buffer_size = count_of_bytes;

    char* buffer = (char*)calloc(count_of_bytes + 1, sizeof(char));
    fread(buffer, sizeof(char), count_of_bytes, fp);
    //printf("%lu %lu", count_of_lines, count_of_bytes);
    IF_DEBUG_ON(create_list_file(buffer, "listing_file.txt", count_of_lines, count_of_bytes);)
    
    sf::RenderWindow win(sf::VideoMode(win_size,win_size), "CPU");

    while(cp->RIP < count_of_bytes || win.isOpen()){
        // /memory_dump(mem);
        if(cp->RIP < count_of_bytes){
            execute_command(cp, mem, buffer);
   
        }
        sf::Event event;
        while(win.pollEvent(event)){
            if(event.type == sf::Event::Closed){
                win.close();
            }
        }
        redraw_from_vram(&win, mem);
        win.display();
    }
    win.~RenderWindow();
    free(buffer);
    fclose(fp);
}











void create_list_file(char* buffer, const char* file, size_t count_of_lines, size_t  count_of_bytes){
    FILE* fp = fopen(file, "w");
    size_t curr_offset = 0;
    char mode = 0;
    double darg = 0;
    char rarg = 0;
    size_t spaces = 0;
    fprintf(fp,"| Count of lines: %lu\n| Count of bytes %lu\n", count_of_lines, count_of_bytes);
    for(int i = 0; i < count_of_lines; i++){
        fprintf(fp, "| %08lx  |  ", curr_offset);
        char operation = *((char*)buffer + curr_offset);
        curr_offset += sizeof(char);
        fprintf(fp, "%02x  |", operation);
        fprintf(fp,"|");

        spaces = 0;
        for(int j = 0; j < argc[operation]; j++){
            mode = *(buffer + curr_offset);
            curr_offset += sizeof(char);

            if(mode & 2){
                rarg = *(buffer + curr_offset);
                curr_offset += sizeof(char);
                
                fprintf(fp, " %02x %28c", rarg, ' ');

                rarg += 'a';
                fprintf(fp, "(r%cx)%10c |", rarg, ' ');


                spaces += 48;
                
            }
            if(mode & 1){
                darg = *((double*)(buffer + curr_offset));
                curr_offset += sizeof(double);
                for(int k = 0; k < sizeof(double); k++){
                    unsigned char p = *((char*)(&darg) + k);
                    fprintf(fp," %02x ", p);
                }
                fprintf(fp,"(%8e)  |", darg);
                spaces += 48;
            }
        }


        for(int z = argc[operation]; z < max_argc ; z++){
                fprintf(fp,"%48c|", ' ');
        }
        fprintf(fp,"|");

        fprintf(fp,"%s", command_names[operation]);
    
        fprintf(fp,"\n");
    }
    fclose(fp);
}