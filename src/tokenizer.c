#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tokenizer.h>
#include <ctype.h>


char* clean_commas(const char* str) {
    // Deze functie gebruiken we om de commas eruit te halen, zodat die geen rommel veroorzaken.
    if (!str) return NULL;
    
    char* cleaned = strdup(str);
    char* src = cleaned;
    char* dst = cleaned;
    
    while (*src) {
        if (*src != ',' && *src != '\n' && *src != '\r') {
            *dst = *src;
            dst++;
        }
        src++;
    }
    *dst = '\0';
    
    return cleaned;
}

void find_possible_tokens(const char *input, Token **tokens, int *token_count) {
    // printf("tokenizing tokens\n");

    //Het kan lang worden
    size_t length_input = strlen(input); //Iets om hier een out of bounds te vangen?
    unsigned int char_counter = 0;
    
    char *temp_string = malloc(1000 * sizeof(char));
    if (!temp_string) {
        return;
    }

    unsigned int line_number_temp = 0;
    int is_comment = 0;
    int is_in_ml_comment = 0;
    int regel_nummer_line_comment = -1; 
    //Simpele tokenizing voor nu; elke spatie en nieuweline is een nieuwe token
    for (size_t i = 0; i < length_input; i++) {
        if (input[i] != ' ' && input[i] != '\n' && input[i] != '\r' && input[i] != '\t') {
            // printf("Normale letter gevonden: %c  \n", input[i]);
            temp_string[char_counter] = input[i];
            char_counter++;
        } else if (char_counter > 0) {
            temp_string[char_counter] = '\0'; 
            // printf("Spatie gevonden: %c \n", input[i]);
            // printf("Token is: %s \n", temp_string);
            // printf("ml comment is: %i \n", is_ml_comment);
            // printf("tempstring[0] is: %c \n", temp_string[char_counter - 1]);
            // printf("tempstring[1] is: %c \n", temp_string[char_counter]);

            *tokens = realloc(*tokens, ((*token_count) + 1) * sizeof(Token)); //Moet beter, maarja.
            if (!*tokens) {
                free(temp_string);
                return; 
            }
            if (char_counter > 1 && temp_string[0] == '/' && temp_string[1] == '*')  { //Dit dekt niet 1 woord ML comments.
                is_comment = 1;
                is_in_ml_comment = 1;
                
            } 
            if (char_counter > 1 && temp_string[0] == '-' && temp_string[1] == '-')  { 
                regel_nummer_line_comment = line_number_temp;
                is_comment = 1;
            }
            (*tokens)[*token_count].value = clean_commas(strdup(temp_string));
            (*tokens)[*token_count].raw_value = strdup(temp_string);
            (*tokens)[*token_count].type = tokenize(temp_string, is_comment);
            (*tokens)[*token_count].line_number = line_number_temp;
            (*tokens)[*token_count].length = strlen(clean_commas(temp_string));
            (*tokens)[*token_count].raw_length = strlen(temp_string);

            if (char_counter > 1 && temp_string[char_counter - 2] == '*' && temp_string[char_counter - 1] == '/')  {
                is_comment = 0;
                is_in_ml_comment = 0;
            }
            if(input[i] == '\n' || input[i] == '\r' ) {
                //Line number plussen aan het einde omdat we anders het laatste woord van de zin niet correct categoriseren.
                line_number_temp++;  
            }
            if (regel_nummer_line_comment != -1 
                && line_number_temp > regel_nummer_line_comment //+1 omdat we anders de line number niet correct benaderen 
                && is_in_ml_comment == 0)  { 
                is_comment = 0;
            }

            *token_count = (*token_count) + 1;
            char_counter = 0;
            memset(temp_string, 0, 1000);
           
        }

    }
    
    free(temp_string);

}


TokenType tokenize(const char *input, int is_comment) {
    if (input == NULL || *input == '\0') {
        return TOKEN_UNKNOWN;
    }

    if (is_comment == 1) {
        return TOKEN_COMMENT;
    }

    if (input[0] == '-' && input[1] == '-') {
        return TOKEN_COMMENT;
    }     

    // Check for keywords
    const char *keywords[] = {
        "SELECT", "FROM", "WHERE", "INSERT", "UPDATE", "DELETE", 
        "CREATE", "DROP", "ALTER", "JOIN", "ON", "GROUP", "ORDER",
        "LEFT", "RIGHT", "INNER", "BY", "AS", "INTO", "WITH", "UNION",
        "LIMIT", "OFFSET", "HAVING", NULL
    };
    

    for (const char **keyword = keywords; *keyword != NULL; keyword++) {
        if (strcasecmp(input, *keyword) == 0) {
            return TOKEN_KEYWORD;
        }
    }

    // Kijk of het een nummer is.
    if (isdigit(input[0]) || (input[0] == '-' && isdigit(input[1]))) {
        size_t length_str = strlen(input);
        size_t i; //checken of er een minus sign is
        if (input[0] == '-') {
            i = 1;
        } else {
            i = 0;
        }
        for (; i < length_str; i++) { //loop over de string na eventueel minus sign
            if (!isdigit(input[i]) && input[i] != '.') { //als we dan geen number tegenkomen dat ook geen punt is (decimaal seperator), dan breaken we.
                break;
            }
        }
        if (i == length_str) { //Als we de volledige string hebben gezien en we geen characters zijn tegengekomen die non-numeriek zijn, hebben we een cijfer te pakken
            return TOKEN_NUMBER;
        }
    }

    // Kijken of er we een string hebben gevonden (gemarkeerd door single quotes aan het begin en einde.  
    // Hier zit een erg grote aanname in dat de string een enkel woord is
    size_t length_str = strlen(input);
    if (length_str > 1 && input[0] == '\'' && input[length_str - 1] == '\'') {
        return TOKEN_STRING;
    }

    // Check voor operators
    const char *operators[] = {
        "+", "-", "*", "/", "=", "<", ">", "<=", ">=", "<>", "!=", "AND", "OR", "NOT", NULL
    };
    for (const char **operator = operators; *operator != NULL; operator++) {
        if (strcasecmp(input, *operator) == 0) { //Als het gelijk is aan elkaar dan hebben we een operator gevonden.
            return TOKEN_OPERATOR;
        }
    }

    // Check voor symbolen
    // TODO: Opschonen van de deze vergelijkingen, misschien een switch statement?
    if (strchr(",", input[0]) != NULL && length_str == 1) {
        return TOKEN_COMMA;
    }

    if (strchr("()", input[0]) != NULL && length_str == 1) {
        return TOKEN_PARENTHESIS;
    }
    
    if (strchr(";", input[0]) != NULL && length_str == 1) {
        return TOKEN_SEMICOLON;
    }

    // Check of we een select item hebben
    // Dit is een beetje raar omdat we al een clean_commas hebben
    if (isalnum(input[0]) || input[0] == '_' || input[0] == ',' || input[0] == '[' || input[0] == ']' ) { //Begint het met een letter of lager streepje?
        int comma_present = 0;
        for (size_t i = 0; i < length_str; i++)  { //Dan lopen we door de string heen
            if (input[i] == ',') { //Hebben we een comma?
                comma_present = 1;
            } else if (!isalnum(input[i]) && input[i] != '_' && input[i] != '[' && input[i] != ']'
                && input[i] != ',' && input[i] != '.'
                && input[i] != '*' && input[i] != '(' && input[i] != ')') { //Als we een non-alfanumeriek character tegekomen die geen laag streepje is 
                return TOKEN_UNKNOWN; //Geef dan terug dat we niet weten wat voor token we hebben.    
            }
        }
        if (comma_present == 1) {
            return TOKEN_SELECTITEM;
        } else {
            return TOKEN_IDENTIFIER; //Als alles alfanumeriek is of een laag streepje, weten we dat we inderdaad met een identifier te maken hebben
        }
        
    }

    return TOKEN_UNKNOWN; //Als we op 1 of andere manier al deze stappen ontstappen, dan eindigen we hier. Dan is het onbekend.
}
