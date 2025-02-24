#include "file_handler.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static const char* VALID_EXTENSIONS[] = {".txt", ".sql"};
static const int NUM_VALID_EXTENSIONS = 2;

FileContent read_file(const char* filename) {
    FileContent fc = {NULL, 0, NULL, FILE_OK};
    const long max_file_size = 10485760;  // 10MB  limiet voor het bestand. Moet eigenlijk 3 zijn...
    
    if (!is_valid_extension(filename)) {
        fc.error = FILE_ERROR_INVALID_EXTENSION;
        return fc;
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        fc.error = FILE_ERROR_OPEN;
        return fc;
    }

    // Haal de bestandsgrote op.
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size > max_file_size) {
        fc.error = FILE_ERROR_MEMORY;
        fclose(file);
        return fc;
    }

    // alloceer genoeg memory voor de file
    fc.content = (char*)malloc(file_size + 1);
    if (!fc.content) {
        fc.error = FILE_ERROR_MEMORY;
        fclose(file);
        return fc;
    }

    // Lees de inhoud van het bestand.
    size_t read_size = fread(fc.content, 1, file_size, file);
    if (read_size != (size_t)file_size) {
        fc.error = FILE_ERROR_READ;
        free(fc.content);
        fc.content = NULL;
        fclose(file);
        return fc;
    }

    fc.content[file_size] = '\0';
    fc.size = file_size;
    
    // Sla de filename op
    fc.filename = strdup(filename);
    if (!fc.filename) {
        fc.error = FILE_ERROR_MEMORY;
        free(fc.content);
        fc.content = NULL;
        fclose(file);
        return fc;
    }

    fclose(file);
    return fc;
}

FileStatus write_file(const char* filename, const char* content) {
    if (!is_valid_extension(filename)) {
        return FILE_ERROR_INVALID_EXTENSION;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        return FILE_ERROR_OPEN;
    }

    size_t content_len = strlen(content);
    size_t written = fwrite(content, 1, content_len, file);
    
    fclose(file);
    
    if (written != content_len) {
        return FILE_ERROR_WRITE;
    }

    return FILE_OK;
}

bool is_valid_extension(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot) {
        return false;
    }

    for (int i = 0; i < NUM_VALID_EXTENSIONS; i++) {
        if (strcasecmp(dot, VALID_EXTENSIONS[i]) == 0) {
            return true;
        }
    }

    return false;
}

void free_file_content(FileContent* fc) {
    if (fc) {
        free(fc->content);
        free(fc->filename);
        fc->content = NULL;
        fc->filename = NULL;
        fc->size = 0;
        fc->error = FILE_OK;
    }
}

const char* get_file_error_message(FileStatus error) {
    switch (error) {
        case FILE_OK:
            return "No error";
        case FILE_ERROR_OPEN:
            return "Failed to open file";
        case FILE_ERROR_READ:
            return "Failed to read file";
        case FILE_ERROR_WRITE:
            return "Failed to write file";
        case FILE_ERROR_MEMORY:
            return "Memory allocation failed";
        case FILE_ERROR_INVALID_EXTENSION:
            return "Invalid file extension";
        default:
            return "Unknown error";
    }
}