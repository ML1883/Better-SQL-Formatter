#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    FILE_OK = 0,
    FILE_ERROR_OPEN,
    FILE_ERROR_READ,
    FILE_ERROR_WRITE,
    FILE_ERROR_MEMORY,
    FILE_ERROR_INVALID_EXTENSION
} FileStatus;

typedef struct {
    char* content;
    size_t size;
    char* filename;
    FileStatus error;
} FileContent;


FileContent read_file(const char* filename);
FileStatus write_file(const char* filename, const char* content);
bool is_valid_extension(const char* filename);
void free_file_content(FileContent* fc);
const char* get_file_error_message(FileStatus error);

#endif // FILE_HANDLER_H