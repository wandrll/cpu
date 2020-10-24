#pragma once
#include <stdio.h>
#include "../stack/stack.h"
/** Класс процессор который может выполнять одну отдельную программу.
 * \param cpu_stack Процессорный стэк, на нем выполняются основные операции 
 * \param RIP Текущее смещение в байтах от начала программы
 * \param registers Массив регистров
 */
struct Cpu{
    Stack_t cpu_stack;
    Stack_t call_stack;
    size_t RIP;
    size_t exec_buffer_size;
    double* registers;
};

struct Memory{
    size_t size;
    double* data;
};

void memory_constructor(Memory* mem, size_t count);

void memory_destructor(Memory* mem);

/**Конструктор процессора
 * \param cp  указатель на процессор
 */
void cpu_constructor(Cpu* cp);

/**Деструктор процессора
 * \param cp  указатель на процессор
 */
void cpu_destructor(Cpu* cp);

/**Проверка, что идет исполнение файла с верным форматом и версией
 * \param fp файл который нужно проверить
 */
void check_executable_file(FILE* fp);

/**Исполнение программы на указанном процессоре
 * \param cp  указатель на процессор
 * \param file имя файла который нужно выполнить
 */
void cpu_execute_programm(Cpu* cp, Memory* ram, const char* file);

/**Создание listing файла
 * \param buffer указатель на буфер с командами
 * \param file имя файла куда записать
 * \param count_of_lines количество команд
 * \param count_of_bytes количество байт в buffer
 */
void create_list_file(char* buffer, const char* file, size_t count_of_lines, size_t  count_of_bytes);