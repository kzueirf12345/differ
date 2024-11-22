#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>

#include "utils.h"
#include "logger/liblogger.h"

char* to_lower(char* const str) 
{
    lassert(!is_invalid_ptr(str), "");

    for (size_t ind = 0; str[ind] != '\0'; ++ind) 
        str[ind] = (char)tolower(str[ind]);

    return str;
}

char* to_upper(char* const str) 
{
    lassert(!is_invalid_ptr(str), "");

    for (size_t ind = 0; str[ind] != '\0'; ++ind) 
        str[ind] = (char)toupper(str[ind]);

    return str;
}

enum PtrState is_invalid_ptr(const void* ptr)
{
    if (ptr == NULL)
    {
        return PTR_STATES_NULL;
    }

    char filename[] = "/tmp/chupapi_munyanya.XXXXXX";
    const int fd = mkstemp(filename);

    if (fd == -1) 
    {
        perror("Can't mkstemp file");
        return PTR_STATES_ERROR;
    }
    
    errno = 0;
    const ssize_t write_result = write(fd, ptr, 1);

    if (remove(filename))
    {
        perror("Can't remove temp file");
        return PTR_STATES_ERROR;
    }

    if (close(fd))
    {
        perror("Can't close temp file");
        return PTR_STATES_ERROR;
    }

    if (write_result == 1)
        return PTR_STATES_VALID;

    if (errno == EFAULT) 
    {
        errno = 0;
        return PTR_STATES_INVALID;
    }
    
    perror("Unpredictable errno state, after write into temp file");
    return PTR_STATES_ERROR;
}

int is_empty_file (FILE* file)
{
    if (is_invalid_ptr(file))
    {
        fprintf(stderr, "Is empty file nvalid\n");
        return -1;
    }

    int seek_temp = SEEK_CUR;

    if (fseek(file, 0, SEEK_END))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    const int res = ftell(file) > 2;

    if (fseek(file, 0, seek_temp))
    {
        fprintf(stderr, "Can't fseek file\n");
        return -1;
    }

    return res;
}

#ifndef INOUT_ELEM_CODE
#define INOUT_ELEM_CODE "%lX"
#endif /*INOUT_ELEM_CODE*/
#ifndef INOUT_ELEM_T
#define INOUT_ELEM_T uint64_t
#endif /*INOUT_ELEM_T*/

#define INOUT_ELEM_SIZE sizeof(INOUT_ELEM_T)

int data_to_str(const void* const data, const size_t size, char* str, const size_t str_size)
{
    if (is_invalid_ptr(data)) return -1;
    if (is_invalid_ptr(str))  return -1;
    if (!size)                return -1;

    char temp_str[INOUT_ELEM_SIZE  * 4] = {};
    for (size_t offset = 0; offset < size; 
         offset += (size - offset >= INOUT_ELEM_SIZE  ? INOUT_ELEM_SIZE  : sizeof(uint8_t)))
    {
        if (size - offset >= INOUT_ELEM_SIZE )
        {
            if (snprintf(temp_str, INOUT_ELEM_SIZE * 4, INOUT_ELEM_CODE,
                         *(const INOUT_ELEM_T*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return -1;
            }
        }
        else
        {
            if (snprintf(temp_str, sizeof(uint8_t) * 4, INOUT_ELEM_CODE, 
                         *(const uint8_t*)((const char*)data + offset)) <= 0)
            {
                perror("Can't snprintf byte on temp_str");
                return -1;
            }
        }

        if (!strncat(str, temp_str, str_size))
        {
            perror("Can't stract str and temp_str");
            return -1;
        }
    }

    return 0;
}
#undef INOUT_ELEM_SIZE