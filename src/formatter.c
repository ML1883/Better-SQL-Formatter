#include <tokenizer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <formatter.h>
#include <ctype.h>

//Om tabjes te inserten van 4 maal spatie
void add_indentation(char* result, size_t* pos, int level) {
    for (int i = 0; i < level * 4; i++) {
        result[(*pos)++] = ' ';
    }
}

char* to_uppercase(const char* str) {
    if (!str) return NULL;  // NULL input geeft null terug
    
    size_t len = strlen(str);
    char* upperStr = malloc(len + 1);  // +1 voor eventueele null terminator
    if (!upperStr) return NULL; //Nog een keer checken of dit goed gaat.
    
    for (size_t i = 0; i < len; i++) {
        upperStr[i] = toupper((unsigned char)str[i]);
    }
    upperStr[len] = '\0';
    
    return upperStr;
}


void format_select_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos, const char* keyword) {
    /*Deze functie is niet alleen voor select onderdelen, maar alles wat erop lijkt. 
    Daarom heeft het ook het keyword nodig.*/
    *pos += sprintf(result + *pos, "%s", keyword); 
    // Ga vervolgens door de items van de select heen
    int item_number = 1;
    int in_case_block = 0;
    char *uppercase_value = malloc(1000 * sizeof(char));
    char *prev_uppercase_value = malloc(1000 * sizeof(char));
    if (!uppercase_value && !prev_uppercase_value) {
        perror("Error: Memory allocation failed\n");
    }

    for (int i = start_idx; i <= end_idx; i++) {
        
        
    
    
        if (tokens[i].type ==  TOKEN_COMMENT) {
            i += format_comments(tokens, i, end_idx, result, pos);
        }
        if (i > end_idx) {
            break; //As we zojuist met code over het einde van de laatste token zijn gegaan, moeten we snel kappen.
        }
        free(uppercase_value);
        uppercase_value = to_uppercase(tokens[i].value);
        // Als we een case hebben moeten we daar speciaal mee omgaan. 
        if (strcmp(uppercase_value, "CASE") == 0) {
            i += format_case_block(tokens, i, end_idx, result, pos, &item_number, 1);
            item_number++;
            continue;
        }
        if (i > end_idx) {
            break; //As we zojuist met code over het einde van de laatste token zijn gegaan, moeten we snel kappen.
        }
        free(uppercase_value);
        uppercase_value = to_uppercase(tokens[i].value);
        if (i > start_idx) { 
            free(prev_uppercase_value);
            prev_uppercase_value = to_uppercase(tokens[i-1].value);
        }
        // printf("Current value of I is: %i \n", i);


        if (strstr(tokens[i].value, "(") != NULL) {
            if (item_number > 1 && tokens[i].raw_value[0] != ',') {
                *pos += sprintf(result + *pos, "\n");
                add_indentation(result, pos, 1);
                *pos += sprintf(result + *pos, ",%s ", tokens[i].raw_value); //raw appendage when in curly brackets.
            } else {
                *pos += sprintf(result + *pos, "\n");
                add_indentation(result, pos, 1);
                *pos += sprintf(result + *pos, " %s", tokens[i].raw_value); //raw appendage when in curly brackets.
            }
            item_number++;
            if (strstr(tokens[i].value, ")") != NULL 
                && strstr(strdup(to_uppercase(tokens[i].raw_value)), "ROW_NUMBER") == NULL) {
                // printf("Open en einde gevonden voor string: %s \n", tokens[i].raw_value);
                continue;
            } else {
                i++;
                while(strstr(tokens[i].value, ")") == NULL) {
                    *pos += sprintf(result + *pos, " %s", tokens[i].raw_value); //raw appendage when in curly brackets.
                    i++;
                    item_number++;
                }        // unsigned int loop_counter = 0;
        // printf("Raw value being checked: %s \n", tokens[i].raw_value);
                continue;
            }
            
        }

        if (tokens[i].type != TOKEN_KEYWORD || 
            strcmp(uppercase_value, "AS") == 0) {

            if (tokens[i].type == TOKEN_IDENTIFIER || tokens[i].type == TOKEN_SELECTITEM) {
                //Hebben we een identifier of een select item? dan handelen we dat hier
                if (prev_uppercase_value && strcmp(prev_uppercase_value, "AS") == 0 ) {
                    *pos += sprintf(result + *pos, " %s", tokens[i].value); //Als die identifier na een as komt, moeten we t op deze manier aanpakken.
                } else if (item_number > 1) {
                    *pos += sprintf(result + *pos, "\n");
                    add_indentation(result, pos, 1);
                    *pos += sprintf(result + *pos, ",%s", tokens[i].value);
                } 
                else {
                    *pos += sprintf(result + *pos, "\n");
                    add_indentation(result, pos, 1);
                    *pos += sprintf(result + *pos, " %s", tokens[i].value);
                }
                
                item_number++;
                
            } else {
                *pos += sprintf(result + *pos, " %s", tokens[i].value);
            }
        } else {
                *pos += sprintf(result + *pos, "%s ", tokens[i].raw_value);
        }
        
    }
    free(uppercase_value);
    free(prev_uppercase_value);
    *pos += sprintf(result + *pos, "\n");
    *pos += sprintf(result + *pos, "\n");
    
}

void format_from_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos) {
    //Hieronder de code om de from section te handelen
    *pos += sprintf(result + *pos, "FROM\n");
    add_indentation(result, pos, 1);
    
    bool in_join = false;
    bool first_table_done = false;
    bool join_type_found = false;
    int join_level = 1;
    char *uppercase_value = malloc(1000 * sizeof(char));
    if (!uppercase_value) {
        perror("Error: Memory allocation failed\n");
    }

    for (int i = start_idx; i <= end_idx; i++) {
        if (tokens[i].type ==  TOKEN_COMMENT) {
            i += format_comments(tokens, i, end_idx, result, pos);
        }
        if (i > end_idx) {
            break; //As we zojuist met code over het einde van de laatste token zijn gegaan, moeten we snel kappen.
        }
        free(uppercase_value);
        uppercase_value = to_uppercase(tokens[i].value);
        
        if (strcmp(uppercase_value, "CASE") == 0) {
                int item_number = 0;
                i += format_case_block(tokens, i, end_idx, result, pos, &item_number, 2);
        }
        if (i > end_idx) {
            break; //As we zojuist met code over het einde van de laatste token zijn gegaan, moeten we snel kappen.
        }

        free(uppercase_value);
        uppercase_value = to_uppercase(tokens[i].value);
        if (tokens[i].type == TOKEN_KEYWORD) {
            // Handelen van het keywoord as voor de eerste tabel dus de from.
            if (strcmp(uppercase_value, "AS") == 0 && first_table_done) {
                *pos += sprintf(result + *pos, "%s ", uppercase_value);
                continue;
            }
            // Handelen van left right inner and outer
            if (strcmp(uppercase_value, "LEFT") == 0 || 
                strcmp(uppercase_value, "RIGHT") == 0 || 
                strcmp(uppercase_value, "INNER") == 0 || 
                strcmp(uppercase_value, "OUTER") == 0 ||
                strcmp(uppercase_value, "FULL") == 0) {
                if(join_type_found) { //Als we een FULL OUTER doen of dergelijke, willen we niet dat dit zorgt voor meer enters.
                    add_indentation(result, pos, join_level);
                    *pos += sprintf(result + *pos, "%s ", uppercase_value);
                    join_type_found = true;
                } else {
                    *pos += sprintf(result + *pos, "\n\n");
                    add_indentation(result, pos, join_level);
                    *pos += sprintf(result + *pos, "%s ", uppercase_value);
                    join_type_found = true;
                }
                
            }
            // Handelen van join keyword
            else if (strcmp(uppercase_value, "JOIN") == 0) {
                if (!join_type_found) {
                    *pos += sprintf(result + *pos, "\n");
                    add_indentation(result, pos, join_level);
                }
                *pos += sprintf(result + *pos, "%s ", uppercase_value);
                in_join = true;
                join_type_found = false;
            }
            // On keyword te handelen
            else if (strcmp(uppercase_value, "ON") == 0) {
                *pos += sprintf(result + *pos, "\n");
                add_indentation(result, pos, join_level + 1);
                *pos += sprintf(result + *pos, "%s ", uppercase_value);
            } else {
                *pos += sprintf(result + *pos, "%s ", tokens[i].value);
            }
        } else if (tokens[i].type == TOKEN_IDENTIFIER) {
            *pos += sprintf(result + *pos, "%s ", tokens[i].value);
            
            // Voeg een nieuwe line in na de eerste table definitie (inclusief de alias
            if (!first_table_done && !in_join) {
                if (i + 1 >= end_idx || 
                    (tokens[i + 1].type != TOKEN_IDENTIFIER && 
                     (tokens[i + 1].type != TOKEN_KEYWORD || 
                      strcmp(to_uppercase(tokens[i + 1].value), "AS") == 0))) {
                    //*pos += sprintf(result + *pos, "\n");
                    first_table_done = true;
                }
            }

        } else if (tokens[i].type == TOKEN_OPERATOR) {
            if(strcmp(uppercase_value, "AND") == 0 || strcmp(uppercase_value, "OR") == 0) {
                *pos += sprintf(result + *pos, "\n");
                add_indentation(result, pos, join_level + 1);
                *pos += sprintf(result + *pos, "%s ", uppercase_value);
            } else {
                *pos += sprintf(result + *pos, "%s ", uppercase_value);
            }
        } else {
            *pos += sprintf(result + *pos, "%s ", tokens[i].raw_value);
        }
    }
    *pos += sprintf(result + *pos, "\n\n");
    free(uppercase_value);
}


void format_where_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos) {
    *pos += sprintf(result + *pos, "WHERE\n");
    add_indentation(result, pos, 1);
    
    bool new_condition = false;
    char *uppercase_value = malloc(1000 * sizeof(char));
    if (!uppercase_value) {
        perror("Error: Memory allocation failed\n");
    }
    
    for (int i = start_idx; i <= end_idx; i++) {
        if (tokens[i].type ==  TOKEN_COMMENT) {
            i += format_comments(tokens, i, end_idx, result, pos);
        }
        free(uppercase_value);
        uppercase_value = to_uppercase(tokens[i].value);
        if (i > end_idx) {
            break; //As we zojuist met code over het einde van de laatste token zijn gegaan, moeten we snel kappen.
        }
        if (strcmp(uppercase_value, "CASE") == 0) {
            int item_number = 0;
            i += format_case_block(tokens, i, end_idx, result, pos, &item_number, 1);
        }
        if (i > end_idx) {
            break; //As we zojuist met code over het einde van de laatste token zijn gegaan, moeten we snel kappen.
        }

        free(uppercase_value);
        uppercase_value = to_uppercase(tokens[i].value); //Als we een paar blokken zijn opgeschoten, moeten we dit aanpassen.
        if (tokens[i].type == TOKEN_OPERATOR) {
            if (strcmp(uppercase_value, "AND") == 0 || strcmp(uppercase_value, "OR") == 0) {
                *pos += sprintf(result + *pos, "\n");
                add_indentation(result, pos, 1);
                *pos += sprintf(result + *pos, "%s ", uppercase_value);
                new_condition = true;
            } else {
                *pos += sprintf(result + *pos, "%s ", uppercase_value);
            }
        } else if (tokens[i].type != TOKEN_KEYWORD) {
            *pos += sprintf(result + *pos, "%s ", tokens[i].raw_value);
        }
    }
    *pos += sprintf(result + *pos, "\n\n");

    free(uppercase_value);
}


void format_unknown_section(Token* tokens, int start_idx, int end_idx, char* result, size_t* pos) {
    /* Simpele catch all functie die alles wat we nog niet hebben behandeld doet.
    Die behandelen door het maar gewoon weg te schrijven.
    Wordt momenteel niet gebruikt.*/
    for (int i = start_idx; i <= end_idx; i++) {
        *pos += sprintf(result + *pos, "%s ", tokens[i].raw_value);
    }
    *pos += sprintf(result + *pos, "\n\n");
}

unsigned int format_comments(Token* tokens, int start_idx, int max_i, char* result, size_t* pos) {
    /* Speciale functie die comments schrijft. Return het aantal geskipte tokens*/
    int i = start_idx;
    unsigned int skipped_tokens = 0;
    while (tokens[i].type == TOKEN_COMMENT && i <= max_i) {
        if (tokens[i].line_number > tokens[i-1].line_number) {
            *pos += sprintf(result + *pos, "\n" );
        }
        *pos += sprintf(result + *pos, " %s", tokens[i].raw_value);
        i++;
        skipped_tokens++;
    }

    // *pos += sprintf(result + *pos, "\n");
    return skipped_tokens;
}

unsigned int format_case_block(Token* tokens, int start_idx, int max_i, char* result, size_t* pos, 
                                int *item_number, int indentation) {
    /* Speciale functie die case-when blocks schrijft. Return het aantal geskipte tokens */
    int i = start_idx;
    unsigned int skipped_tokens = 0;
    int in_case_block = 1;
    char* uppercase_value = to_uppercase(tokens[i].value);
    
    if (strcmp(uppercase_value, "CASE") == 0) {
        if (*item_number > 1) {
            *pos += sprintf(result + *pos, "\n");
            add_indentation(result, pos, indentation);
            *pos += sprintf(result + *pos, ",%s", tokens[i].value);
        } else {
            *pos += sprintf(result + *pos, "\n");
            add_indentation(result, pos, indentation);
            *pos += sprintf(result + *pos, " %s", tokens[i].value);
        }
        i++;
        skipped_tokens++;
    }
    
    while (in_case_block && i < max_i) {
        free(uppercase_value);
        uppercase_value = to_uppercase(tokens[i].value);
        
        if (tokens[i].type == TOKEN_COMMENT) {
            skipped_tokens += format_comments(tokens, i, max_i, result, pos);
            i += skipped_tokens;
            continue;
        }
        
        if (strcmp(uppercase_value, "WHEN") == 0 || strcasecmp(tokens[i].value, "ELSE") == 0) {
            // Voor when en else, een nieuwe lijn met tabjes
            *pos += sprintf(result + *pos, "\n");
            add_indentation(result, pos, indentation + 1);
            *pos += sprintf(result + *pos, "%s ", tokens[i].value);
        } else if (strcmp(uppercase_value, "THEN") == 0) {
            // THEN statements allemaal op 1 lijn
            *pos += sprintf(result + *pos, " %s ", tokens[i].value);
        } else if (strcmp(uppercase_value, "END") == 0) {
            // Klaar met case block, terug naar normale indentatie.
            in_case_block = 0;
            *pos += sprintf(result + *pos, "\n");
            add_indentation(result, pos, indentation);
            *pos += sprintf(result + *pos, "%s", tokens[i].value);
        } else {
            // De rest op 1 lijn.
            *pos += sprintf(result + *pos, "%s ", tokens[i].value);
        }
        
        i++;
        skipped_tokens++;
    }
    
    free(uppercase_value);
    return skipped_tokens;
}



const char* format_sql(Token** tokens, unsigned int token_count) {
    /*Functie die blokken select statements formateert*/
    if (!tokens || !*tokens || token_count == 0) {
        return "Error: Invalid tokens array\n";
    }

    char* result = malloc(7200000 * sizeof(char));
    if (!result) {
        return "Error: Memory allocation failed\n";
    }
    
    size_t pos = 0;
    Token* token_array = *tokens;
    
    // Maak onze clause array met specifieke functie voor elke clause. 
    SQLClause clauses[] = {
        {"SELECT", NULL, -1, -1, (FormatFunction)format_select_section, "SELECT"},
        {"INTO", NULL, -1, -1, (FormatFunction)format_select_section, "INTO"},
        {"FROM", NULL, -1, -1, format_from_section, NULL},
        {"WHERE", NULL, -1, -1, format_where_section, NULL},
        {"GROUP", "BY", -1, -1, (FormatFunction)format_select_section, "GROUP BY"},
        {"ORDER", "BY", -1, -1, (FormatFunction)format_select_section, "ORDER BY"},
        {"HAVING", NULL, -1, -1, (FormatFunction)format_select_section, "HAVING"}
    };

    const int num_clauses = sizeof(clauses) / sizeof(clauses[0]);

    // Nu gaan we kijken waar de clauses zijn.
    // We knippen dit in twee stapjes op omdat we anders het probleem krijgen dat de specifieke volgorde
    // in onze array met clauses niet gevolgd wordt. 
    // In de eerste pass kijken we louter waar elk keyword begint
    int in_curly_braces = 0;
    for (int i = 0; i < token_count; i++) {
        // printf("Token: %s\n", token_array[i].value);

        if (strstr(token_array[i].value, "(") != NULL) {
            in_curly_braces = 1;
        }
        
        //Probleem dat we geen innerqueries meer kunnen doen, op hoger niveau oplossen
        //We willen niet dat een order by in een over functie problemen oplevert.
        if (token_array[i].type == TOKEN_KEYWORD && !in_curly_braces ) { 
            const char* uppercase_value = to_uppercase(token_array[i].value);
            
            for (int j = 0; j < num_clauses; j++) {
                if (strcmp(uppercase_value, clauses[j].keyword) == 0) {
                    // printf("Found keyword %s at position %d\n", clauses[j].keyword, i);
                    
                    if (clauses[j].second_word != NULL) {
                        if (i + 1 < token_count && 
                            strcmp(to_uppercase(token_array[i + 1].value), clauses[j].second_word) == 0) {
                            clauses[j].start_pos = i + 2;
                            // printf("Setting start of %s %s to %d\n", clauses[j].keyword, clauses[j].second_word, i + 2);
                            i++;
                        }
                    } else {
                        clauses[j].start_pos = i + 1;
                        // printf("Setting start of %s to %d\n", clauses[j].keyword, i + 1);
                    }
                    break;
                }
            }
        }
        if (strstr(token_array[i].value, ")") != NULL ) {
            in_curly_braces = 0;
        }
    }

    // Tweede pas kijken waar alle clauses eindigen
    for (int i = 0; i < num_clauses; i++) {
        if (clauses[i].start_pos != -1) {
            // Hier vinden we de volgende clause die daadwerkelijke bestaat
            int end_pos = token_count - 1;  // De default waarde is het einde van ozne volledige statement.
            for (int j = i + 1; j < num_clauses; j++) {
                if (clauses[j].start_pos != -1) {
                    end_pos = clauses[j].start_pos - 2;
                    break;
                }
            }
            clauses[i].end_pos = end_pos;
            // printf("Clause %s: start=%d, end=%d\n", clauses[i].keyword, clauses[i].start_pos, clauses[i].end_pos);
        }
    }

    // Vervolgens formatteren we elke clause die we gevonden hebben.
    for (int i = 0; i < num_clauses; i++) {
        if (clauses[i].start_pos != -1 && clauses[i].end_pos != -1) {
            if (clauses[i].format_keyword) {
                format_select_section(token_array, clauses[i].start_pos, 
                                    clauses[i].end_pos, result, &pos, 
                                    clauses[i].format_keyword);
                // printf("Current result adding select is: %s", result);
            } else {
                clauses[i].format_func(token_array, clauses[i].start_pos, 
                                     clauses[i].end_pos, result, &pos);
                // printf("Current result adding others is: %s", result);
            }
        }
    }

    // printf("Output is: %s\n", result);
    return result;
}



char* apply_parenthesis_indentation(const char* input) {
    //Probleem met deze code: haakjes in de comments zorgen ook voor enter wat bij een SL comment een probleem is.
    size_t length = strlen(input);
    char* output = malloc(length * 8);  // Alloceer genoeg ruimte zodat onze spaces geen buffer overflow veroorzaken (worst-kaas scenario)
    if (!output) {
        return "Error: Memory allocation failed\n";
    }

    int indent_level = 0;
    const int INDENT_SIZE = 4;
    int line_indent = 0;
    int new_line = 1;
    size_t in_pos = 0, out_pos = 0;
    size_t last_pos = strlen(input) - 1;

    while (input[in_pos] != '\0') { 
         
        //Detecteer een nieuwe regel en meet de indent die we hebben
        if (new_line) {
            line_indent = 0;
            while (input[in_pos] == ' ') {  // Tel hoeveel bestaande spaties we hebben
                // printf("Current letter space: %c\n", input[in_pos]);
                output[out_pos++] = input[in_pos++];
                line_indent++;
            }
            
            new_line = 0;
        }
        

        char c = input[in_pos];

        // Als we een haakje openen zien dan verhogen we de indent met 1
        if (c == '(' && in_pos < last_pos && input[in_pos + 1] != ')') {
            output[out_pos++] = c;
            output[out_pos++] = '\n';
            indent_level++;  // Increase indent level after opening parenthesis
            for (int i = 0; i < line_indent + (indent_level * INDENT_SIZE); i++) {
                output[out_pos++] = ' ';
            }
            
            // Begin met tabben van alle regels tussen haakjes, en vergeet geen nested haakjes mee te nemen
            int nested_parens = 1; 
            while (nested_parens > 0 && input[in_pos] != '\0') {
                c = input[++in_pos]; 

                if (c == '(') {
                    nested_parens++;
                } else if (c == ')') {
                    nested_parens--; 
                    if (nested_parens == 0) {
                        break; 
                    }
                }

                // Handelen van nieuwe lines en de indentatie die er al is binnen de haakjes. 
                if (c == '\n') {
                    output[out_pos++] = c;  // Hou de nieuwlijn
                    // En voeg de identation dei we willen toe 
                    for (int i = 0; i < line_indent + (indent_level * INDENT_SIZE); i++) {
                        output[out_pos++] = ' ';
                    }
                } else {
                    output[out_pos++] = c;  // Of voeg gewoon het huidige karakter toe
                }
            }
            continue;  // Na het sluiten van de paranthesis kunnen we de rest skippen en de volgende karakter uitlezen
        }
        // ... en een als we een hakje sluiten zien dan verlagen we de indent met 1
        else if (c == ')' && in_pos > 1 && input[in_pos - 1] != '(') {
            indent_level--;
            output[out_pos++] = '\n';
            for (int i = 0; i < line_indent + (indent_level * INDENT_SIZE); i++) {
                output[out_pos++] = ' ';
            }
            output[out_pos++] = c;
        }
        // En hoe ga je om met de rest van de karakters?
        // Je print ze gewoon
        else {
            // printf("Current letter else: %c\n", c);
            output[out_pos++] = c;
            // printf("Current output: %s\n", output);
        }

        // Detecteer dat we een nieuwe regel hebben
        if (c == '\n') {
            new_line = 1;
        }

        in_pos++;
    }

    output[out_pos] = '\0';
    return output;
}


const char* preprocess_format_postprocess(Token** tokens, unsigned int token_count) {
    if (!tokens || !*tokens || token_count == 0) {
        return "Error: Invalid tokens array\n";
    }

    char* final_result = malloc(7200000 * sizeof(char));
    if (!final_result) {
        return "Error: Memory allocation failed\n";
    }

    char *uppercase_value = malloc(1000 * sizeof(char));
    if (!uppercase_value) {
        perror("Error: Memory allocation failed\n");
    }
    char *uppercase_value_2 = malloc(1000 * sizeof(char));
    if (!uppercase_value_2) {
        perror("Error: Memory allocation failed\n");
    }


    Token* token_array = *tokens;
    size_t final_pos = 0;
    size_t current_pos = 0;
    int in_curly_brackets = 0;

    // Keywords die ons moeten stoppen om een select statement in te lezen. 
    const char* stop_words[] = {"DROP", "CREATE", "ALTER", "SELECT", "UPDATE", 
                                "INSERT", "WITH", "USE", "DELETE", "UNION", "LIMIT",
                                "OFFSET", NULL};

    while (current_pos < token_count) {
        free(uppercase_value);
        uppercase_value = to_uppercase(token_array[current_pos].value);
        if (token_array[current_pos].type == TOKEN_COMMENT) {
            int line_difference = token_array[current_pos].line_number - token_array[current_pos - 1].line_number;
            for (int i = 0; i < line_difference; i++) {
                strcat(final_result + final_pos, "\n");
                final_pos++;
            }
            Token* token_array = *tokens;
            current_pos += format_comments(token_array, current_pos, token_count, final_result, &final_pos);
        }
        // Kijk of de huidige token een select is.
        // printf("Huidige pos: %d \n", current_pos);
        if (strcmp(uppercase_value, "SELECT") == 0 && token_array[current_pos].type == TOKEN_KEYWORD) {
            unsigned int select_start = current_pos;
            
            // Vind het einde van de huidige select statement.
            while (current_pos < token_count) {
                free(uppercase_value);
                uppercase_value = to_uppercase(token_array[current_pos].value);
                
                // Ook een check of we toevallig een puntkomma vinden. Lang leve afwijkinge van de SQL standard. 
                if (strcmp(token_array[current_pos].value, ";") == 0) {
                    current_pos++;
                    break;
                }
                
                // Kijk of we een stop woord hebben gevonden
                int found_stop = 0;
                if (current_pos > select_start) {  // Check alleen nadat je de eerste select passeert
                    for (const char** stop = stop_words; *stop != NULL; stop++) {
                        if (strcmp(uppercase_value, *stop) == 0 && token_array[current_pos].type == TOKEN_KEYWORD) {
                            found_stop = 1;
                            break;
                        }
                    }

                    if (strcmp(uppercase_value, ")") == 0 && in_curly_brackets == 1) {
                        in_curly_brackets -= 1;
                        // current_pos++;
                        break; //Breek uit als we eerder een curly bracket hadden en we dat weer tegencomen

                    }

                    if (found_stop) {
                        break;  // Niet de huidige positie incrementeren; we willen deze namelijk weer kunnen verwerken
                    }
                }
                
                current_pos++;
            }
            


            // Formateer ons gevonden select block
            unsigned int block_size = current_pos - select_start;
            Token* block_tokens = &token_array[select_start];
            const char* formatted_sql = format_sql(&block_tokens, block_size);
            
            // Zet onze geformatte block achterin onze resultaten.
            size_t formatted_length = strlen(formatted_sql);
            if (final_pos + formatted_length < 7200000) {
                strcpy(final_result + final_pos, formatted_sql);
                final_pos += formatted_length;
            }
        } else {
            // Als we een non select keyword hebben schrijven we tot het volgende keyword. 
            while (current_pos < token_count) {
                const char* current_value = token_array[current_pos].value;
                const char* uppercase_value_2 = to_uppercase(token_array[current_pos].value);

                if (strcmp(current_value, "(") == 0) {
                    in_curly_brackets += 1;
                }

                // Kijken of we 1 van onze stop words hebben gevonden.
                if (token_array[current_pos].type == TOKEN_KEYWORD) {
                    int found_stop = 0;
                    for (const char** stop = stop_words; *stop != NULL; stop++) {
                        if (strcmp(uppercase_value_2, *stop) == 0) {
                            found_stop = 1;
                            break;
                        }
                    }
                    if (found_stop) { //Als we een stopwoord hebben gevonden, dan beeindigen, verder verwerken
                        if(strcmp(uppercase_value_2, "SELECT") == 0) {
                            strcat(final_result + final_pos, "\n\n");
                            final_pos += 2;
                            break;
                        }
                        //Als we geen select hebben gevonden als stopwoord, dan enter, inserten, spatie en verder. 
                        strcat(final_result + final_pos, "\n");
                        final_pos++;
                        strcat(final_result + final_pos, current_value);
                        strcat(final_result + final_pos, " ");
                        final_pos += strlen(current_value) + 1;
                        current_pos++;
                        break;  
                    }
                }
                
                // Voeg teken op het einde toe en plaats een spatie erachter
                if (final_pos + strlen(current_value) + 1 < 7200000) {
                    strcat(final_result + final_pos, current_value);
                    strcat(final_result + final_pos, " ");
                    final_pos += strlen(current_value) + 1;
                    
                }
                
                current_pos++;
            }
        }
    }
    // Stop een paar mooie tabjes om de code wat te verfrissen. 
    final_result = apply_parenthesis_indentation(final_result);
    return final_result;
}


