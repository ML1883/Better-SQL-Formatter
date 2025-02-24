// formatter.h
#ifndef FORMATTER_H
#define FORMATTER_H
#include <stdbool.h>

typedef void (*FormatFunction)(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos);

typedef struct {
    const char* keyword;     
    const char* second_word; 
    int start_pos;          
    int end_pos;           
    FormatFunction format_func;  
    const char* format_keyword;
} SQLClause;

char* to_uppercase(const char *str);
void add_indentation(char* result, size_t* pos, int level);
void format_select_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos, const char* keyword); 
void format_from_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos);
void format_where_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos);
void format_unknown_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos);
const char *format_sql(Token** tokens, unsigned int token_count);
char* apply_parenthesis_indentation(const char* input);
const char *preprocess_format_postprocess(Token** tokens, unsigned int token_count);
unsigned int format_comments(Token* tokens, int start_idx, int max_i, char* result, size_t* pos);
unsigned int format_case_block(Token* tokens, int start_idx, int max_i, char* result, size_t* pos, 
                                int *item_number, int indentation);
#endif // FORMATTER_CONFIG_H