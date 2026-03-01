#include "interpreter.h"

typedef enum {
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_OPERATOR,
    TOKEN_CHAR,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    DynamicArray value;
} Token;

void printToken(Token tok) {
    const char *type_str = "";

    switch (tok.type) {
        case TOKEN_IDENTIFIER: type_str = "IDENTIFIER"; break;
        case TOKEN_NUMBER:     type_str = "NUMBER";     break;
        case TOKEN_STRING:     type_str = "STRING";     break;
        case TOKEN_OPERATOR:   type_str = "OPERATOR";   break;
        case TOKEN_CHAR:       type_str = "CHAR";       break;
        case TOKEN_SEMICOLON:  type_str = "SEMICOLON";  break;
        case TOKEN_COMMA:      type_str = "COMMA";      break;
        case TOKEN_LPAREN:     type_str = "LPAREN";     break;
        case TOKEN_RPAREN:     type_str = "RPAREN";     break;
        case TOKEN_LBRACE:     type_str = "LBRACE";     break;
        case TOKEN_RBRACE:     type_str = "RBRACE";     break;
        case TOKEN_EOF:        type_str = "EOF";        break;
    }

    printf("%-12s : ", type_str);

    for (size_t i = 0; i < tok.value.size; i++) {
        char c = *(char *)get(&tok.value, i);
        putchar(c);
    }
    printf("\n");
}

DynamicArray removeComments(DynamicArray code) {
    DynamicArray result;
    init_array(&result, sizeof(char));

    char prev_char = '\0';

    bool in_string = false;
    for (size_t i = 0; i < code.size; i++) {
        char c = *(char *)get(&code, i);

        if ((c == '"' || c == '\'') && prev_char != '\\') {
            in_string = !in_string;
        }
        
        if (!in_string) {
            if (c == '#') {
                while (i < code.size && *(char *)get(&code, i) != '\n') {
                    i++;
                }
                c = '\n';
            }
        }
        push_back(&result, &c);
        prev_char = c;
    }

    char zero = '\0';
    push_back(&result, &zero);
    return result;
}

DynamicArray splitTokens(DynamicArray code) {
    DynamicArray parts;
    init_array(&parts, sizeof(DynamicArray));

    DynamicArray current;
    init_array(&current, sizeof(char));

    const char* single_tokens = ";,(){}";

    for (size_t i = 0; i < code.size; i++) {
        char c = *(char *)get(&code, i);

        if (c == ' ' || c == '\t' || c == '\n') {
            if (current.size > 0) {
                push_back(&parts, &current);
                init_array(&current, sizeof(char));
            }
            continue;
        }

        bool is_single = false;
        for (size_t j = 0; single_tokens[j]; j++) {
            if (c == single_tokens[j]) {
                if (current.size > 0) {
                    push_back(&parts, &current);
                    init_array(&current, sizeof(char));
                }
                DynamicArray single;
                init_array(&single, sizeof(char));
                push_back(&single, &c);
                push_back(&parts, &single);
                is_single = true;
                break;
            }
        }
        if (!is_single) {
            push_back(&current, &c);
        }
    }

    if (current.size > 0) {
        push_back(&parts, &current);
    }

    return parts;
}

DynamicArray tokenize(DynamicArray parts) {
    DynamicArray tokens;
    init_array(&tokens, sizeof(Token));

    for (size_t i = 0; i < parts.size; i++) {
        DynamicArray part = *(DynamicArray *)get(&parts, i);
        if (part.size == 0) continue;

        Token token = {0};
        init_array(&token.value, sizeof(char));

        for (size_t j = 0; j < part.size; j++) {
            char ch = *(char *)get(&part, j);
            push_back(&token.value, &ch);
        }

        if (part.size == 1) {
            char c = *(char *)get(&part, 0);
            switch (c) {
                case ';': token.type = TOKEN_SEMICOLON; break;
                case ',': token.type = TOKEN_COMMA; break;
                case '(': token.type = TOKEN_LPAREN; break;
                case ')': token.type = TOKEN_RPAREN; break;
                case '{': token.type = TOKEN_LBRACE; break;
                case '}': token.type = TOKEN_RBRACE; break;
                case '+': case '-': case '*': case '/': case '=': 
                    token.type = TOKEN_OPERATOR; break;
                default: 
                    token.type = (c >= '0' && c <= '9') ? TOKEN_NUMBER : TOKEN_IDENTIFIER;
                    break;
            }
        } else {
            bool is_number = true;
            for (size_t j = 0; j < part.size; j++) {
                char ch = *(char *)get(&part, j);
                if (ch < '0' || ch > '9') {
                    is_number = false;
                    break;
                }
            }
            token.type = is_number ? TOKEN_NUMBER : TOKEN_IDENTIFIER;
        }
        push_back(&tokens, &token);
    }
    return tokens;
}

int entry(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <source_file>\n", argv[0]);
        return 1;
    }

    DynamicArray tok = tokenize(splitTokens(removeComments(read(argv[1]))));

    for (size_t i = 0; i < tok.size; i++) {
        Token token = *(Token *)get(&tok, i);
        printToken(token);
    }
    return 0;
}