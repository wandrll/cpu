#pragma once

#include <stdio.h>
#include <stdlib.h>
/** \file */



/** \brief структура хранящая одну строфу 
 *  \param line указатель на строку
 *  \param count количество байтов выделенных на строку
 */
struct line{
    const char* line;
    size_t count;
};


/** \brief выделение памяти для массива строф 
 *  \param count количество 
 */
struct line* data_mem_alloc(size_t count);






/** \brief считывает все строки из файла и сохраняет их в буфер
 *  \param lines_count сюда запишется количество строк
 *  \param file имя файла
 *  \return указатель на буфер содержащий все данные 
 */
char* read_raw_data(const char* file, size_t* lines_count);


/** \brief сохраняет ровно одну строку в файл
 *  \param line указатель на строку
 *  \param fp указатель на файл
 */
void save_line(const struct line* line, FILE* fp);


/** \brief сохраняет все строки из массива указателей в файл
 *  \param data указатель на массив строф
 *  \param count количество
 *  \param file имя файла
 */
void save_data(const struct line* data, size_t count, const char* file);


/** \brief очищает память выделенную для data 
 *  \param data указатель на массив строф
 *  \param buffer_for_strings указатель на начало буфера
 */
void free_data(struct line* data, char* buffer_for_strings);


/** \brief считывает строки из файла .bin 
 *  \param data указатель на массив строф
 *  \param count количество строк
 *  \param bin_file имя бинарника
 *  \return указатель ну буфер
 */
char* read_bin(struct line* data, size_t count, const char* bin_file);


/** \brief сохраняет строфы в  .bin 
 *  \param data указатель на массив строф
 *  \param n количество строк
 *  \param file имя бинарника
 */
void create_bin(const struct line* data, size_t n, const char* file);


/** \brief воссоздает массив строф по буферу
 *  \param data указатель на массив строф
 *  \param ptr_on_buff указатель на буфер
 *  \param num_of_lines количество строк
 *  \return количество считанных строк
 */
void recreate_data(struct line* data, const char* ptr_on_buff, size_t num_of_lines);


/** \brief возвращает размер файла
 *  \param file имя файла
 *  \return количество байт в файле
 */
size_t file_size(const char* file);


/** \brief создает массив структур и заменяет \n на \0
 *  \param buffer буфер с данными
 *  \param count количество строк
 *  \return указатель на массив структур
 */
struct line* parse_buffer(char* buffer, size_t count);

/** \brief находит количество символов c в строке buf
 *  \param buf строка в которой искать
 *  \param c символ который искать
 * \return количество символов
 */

size_t count_of_symbol(const char* buf, char c);