#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>
#include <formatter.h>
#include <file_handler.h>

int main(int argc, char *argv[]) {
    int verbose = 0;
    const char *input_filename = NULL;

    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s [-v] <input_file>\n", argv[0]);
        return 1;
    }
    
    if (argc == 3) {
        if (strcmp(argv[1], "-v") == 0) {
            verbose = 1;
            input_filename = argv[2];
        } else {
            fprintf(stderr, "Invalid option: %s\n", argv[1]);
            return 1;
        }
    } else if (argc == 2) {
        input_filename = argv[1];
    }

    FileContent sql_file = read_file(input_filename);
    if (sql_file.error != FILE_OK) {
        fprintf(stderr, "Error: %s\n", get_file_error_message(sql_file.error));
        return 1;
    }

    const char* prefix = "formatted_";
    char* output_filename = malloc(strlen(prefix) + strlen(input_filename) + 1);
    if (!output_filename) {
        free_file_content(&sql_file);
        perror("Failed to allocate memory for output filename");
        return 1;
    }
    strcpy(output_filename, prefix);
    strcat(output_filename, input_filename);

    Token *tokens = malloc(sizeof(Token));
    if (!tokens) {
        free_file_content(&sql_file);
        free(output_filename);
        perror("Failed to allocate memory for tokens");
        return 1;
    }

    unsigned int token_count = 0;
    strcat(sql_file.content, " "); 
    find_possible_tokens(sql_file.content, &tokens, &token_count);

    char* result_format = malloc(sizeof(char) * 7200000);
    if (!result_format) {
        free_file_content(&sql_file);
        free(tokens);
        free(output_filename);
        return 1;
    }

    strcpy(result_format, preprocess_format_postprocess(&tokens, token_count));

    FileStatus write_error = write_file(output_filename, result_format);
    if (write_error != FILE_OK) {
        fprintf(stderr, "Error writing output: %s\n", get_file_error_message(write_error));
    }

    free_file_content(&sql_file);
    for (unsigned int i = 0; i < token_count; i++) {
        if (verbose) {
            printf("Token %d: value='%s', raw_value='%s', type=%d, line=%u\n", 
                i, 
                tokens[i].value, 
                tokens[i].raw_value, 
                tokens[i].type,
                tokens[i].line_number);
        }

        free(tokens[i].value);
    }
    free(tokens);
    free(result_format);
    free(output_filename);

    return 0;
}
