#include "interpreter.h"
#include "parser.h"

int isType(TokenType type) {
    return type >= KEYWORD_INT && type <= KEYWORD_VOID;
}

Token* consume(Parser *p, TokenType type, const char *msg) {
    Token *tok = peek(p);
    if (tok->type != type) {
        printf("Parser error: %s\n", msg);
        exit(1);
    }
    return advance(p);
}

int check(Parser *p, TokenType type) {
    return peek(p)->type == type;
}

void parseFunctionSignature(Parser *p) {
    advance(p);
    expect(p, TOKEN_IDENTIFIER);
    expect(p, TOKEN_LPAREN);
    parseParameters(p);
    expect(p, TOKEN_RPAREN);
}

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

        case TOKEN_KEYWORD_START: type_str = "TK_START";break;
        case TOKEN_KEYWORD_END:type_str = "TK_END";		break;

        case KEYWORD_INT:		type_str = "K_INT";		break;
        case KEYWORD_STR:		type_str = "K_STR";		break;
        case KEYWORD_CHAR:		type_str = "K_CHAR";	break;
        case KEYWORD_BOOL:		type_str = "K_BOOL";	break;
        case KEYWORD_HASH:		type_str = "K_HASH";	break;
        case KEYWORD_ARR:		type_str = "K_ARR";		break;
        case KEYWORD_VOID:		type_str = "K_VOID";	break;
        case KEYWORD_FUNC:		type_str = "K_FUNC";	break;
        case KEYWORD_RETURN:	type_str = "K_RETURN";	break;
        case KEYWORD_IF:		type_str = "K_IF";		break;
        case KEYWORD_ELSE:		type_str = "K_ELSE";	break;
        case KEYWORD_USING:     type_str = "K_USING";	break; 
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

static TokenType check_keyword(DynamicArray *value) {
    char buf[64];
    size_t len = value->size;

    if (len >= sizeof(buf))
        return TOKEN_IDENTIFIER;

    for (size_t i = 0; i < len; i++)
        buf[i] = *(char *)get(value, i);

    buf[len] = '\0';

    if (strcmp(buf, "int") == 0)    return KEYWORD_INT;
    if (strcmp(buf, "str") == 0)    return KEYWORD_STR;
    if (strcmp(buf, "char") == 0)   return KEYWORD_CHAR;
    if (strcmp(buf, "bool") == 0)   return KEYWORD_BOOL;
    if (strcmp(buf, "hash") == 0)   return KEYWORD_HASH;
    if (strcmp(buf, "arr") == 0)    return KEYWORD_ARR;
    if (strcmp(buf, "void") == 0)   return KEYWORD_VOID;
    if (strcmp(buf, "func") == 0)   return KEYWORD_FUNC;
    if (strcmp(buf, "return") == 0) return KEYWORD_RETURN;
    if (strcmp(buf, "if") == 0)     return KEYWORD_IF;
    if (strcmp(buf, "else") == 0)   return KEYWORD_ELSE;

    return TOKEN_IDENTIFIER;
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
                    token.type = TOKEN_OPERATOR;
                    break;

                default:
                    if (c >= '0' && c <= '9')
                        token.type = TOKEN_NUMBER;
                    else
                        token.type = TOKEN_IDENTIFIER;
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

            if (is_number) {
                token.type = TOKEN_NUMBER;
            } else {
                token.type = check_keyword(&token.value);
            }
        }

        if (token.value.size == 0) continue;
        if (token.value.size == 1 && *(char*)get(&token.value,0) == '\0') continue;
        push_back(&tokens, &token);
    }

    Token eof = {0};
    eof.type = TOKEN_EOF;
    init_array(&eof.value, sizeof(char));
    push_back(&tokens, &eof);

    return tokens;
}

Token* peek(Parser *p) {
    return get(p->tokens, p->pos);
}

Token* advance(Parser *p) {
    return get(p->tokens, p->pos++);
}

bool match(Parser *p, TokenType type) {
    if (peek(p)->type == type) {
        p->pos++;
        return true;
    }
    return false;
}

void expect(Parser *p, TokenType type) {
    if (!match(p, type)) {
        printf("Parser error: expected token %d\n", type);
        exit(1);
    }
}

void parseFactor(Parser *p) {
    Token *tok = peek(p);

    if (tok->type == TOKEN_NUMBER ||
        tok->type == TOKEN_STRING ||
        tok->type == TOKEN_CHAR) {

        advance(p);
        return;
    }
    if (tok->type == TOKEN_IDENTIFIER) {
        advance(p);

        // function call
        if (match(p, TOKEN_LPAREN)) {
            if (!match(p, TOKEN_RPAREN)) {
                while (1) {
                    parseExpression(p);

                    if (match(p, TOKEN_RPAREN))
                        break;

                    expect(p, TOKEN_COMMA);
                }
            }
        }
        return;
    }
    if (match(p, TOKEN_LPAREN)) {
        parseExpression(p);
        expect(p, TOKEN_RPAREN);
        return;
    }

    printf("Parser error: invalid factor\n");
    exit(1);
}

void parseTerm(Parser *p) {
    parseFactor(p);

    while (1) {
        Token *tok = peek(p);

        if (tok->type == TOKEN_OPERATOR) {
            char op = *(char*)get(&tok->value,0);

            if (op == '*' || op == '/') {
                advance(p);
                parseFactor(p);
                continue;
            }
        }
        break;
    }
}

void parseExpression(Parser *p) {
    parseTerm(p);

    while (1) {
        Token *tok = peek(p);

        if (tok->type == TOKEN_OPERATOR) {
            char op = *(char*)get(&tok->value,0);

            if (op == '+' || op == '-') {
                advance(p);
                parseTerm(p);
                continue;
            }
        }

        break;
    }
}

void parseParameters(Parser *p) {
    if (peek(p)->type == TOKEN_RPAREN)
        return;

    while (1) {
        advance(p); // type
        expect(p, TOKEN_IDENTIFIER);

        if (!match(p, TOKEN_COMMA)) break;
    }
}

void parseStatement(Parser *p) {
    if (match(p, KEYWORD_IF)) {
        parseIf(p);
        return;
    }
    if (match(p, KEYWORD_RETURN)) {
        parseReturn(p);
        return;
    }
    if (isType(peek(p)->type)) {
        parseVariable(p);
        return;
    }
    if (peek(p)->type == TOKEN_IDENTIFIER) {
        parseInstructionOrCall(p);
        return;
    }

    printf("Parser error: invalid statement\n");
    exit(1);
}

void parseVariable(Parser *p) {
    TokenType type = advance(p)->type;
    Token *name = consume(p, TOKEN_IDENTIFIER, "Expected variable name");

    if (match(p, TOKEN_OPERATOR)) {   // '='
        parseExpression(p);
    }
    consume(p, TOKEN_SEMICOLON, "Expected ';' after variable declaration");
}

void parseReturn(Parser *p) {
    if (!check(p, TOKEN_SEMICOLON)) {
        parseExpression(p);
    }
    consume(p, TOKEN_SEMICOLON, "Expected ';' after return");
}

void parseIf(Parser *p) {
    consume(p, TOKEN_LPAREN, "Expected '(' after if");
    parseExpression(p);
    consume(p, TOKEN_RPAREN, "Expected ')'");

    consume(p, TOKEN_LBRACE, "Expected '{'");

    while (!check(p, TOKEN_RBRACE)) {
        parseStatement(p);
    }
    consume(p, TOKEN_RBRACE, "Expected '}'");
    if (match(p, KEYWORD_ELSE)) {
        consume(p, TOKEN_LBRACE, "Expected '{' after else");

        while (!check(p, TOKEN_RBRACE)) {
            parseStatement(p);
        }
        consume(p, TOKEN_RBRACE, "Expected '}'");
    }
}

void parseInstructionOrCall(Parser *p) {
    Token *name = consume(p, TOKEN_IDENTIFIER, "Expected identifier");

    if (match(p, TOKEN_LPAREN)) {
        if (!check(p, TOKEN_RPAREN)) {
            do {
                parseExpression(p);
            } while (match(p, TOKEN_COMMA));
        }
        consume(p, TOKEN_RPAREN, "Expected ')'");
    }
    else {
        parseExpression(p);
    }

    consume(p, TOKEN_SEMICOLON, "Expected ';'");
}

void parseImport(Parser *p) {
    Token *file = consume(p, TOKEN_STRING, "Expected script name");

    consume(p, TOKEN_SEMICOLON, "Expected ';' after import");
}

void parseFunction(Parser *p) {
    parseFunctionSignature(p);

    consume(p, TOKEN_LBRACE, "Expected '{' after function declaration");

    while (!check(p, TOKEN_RBRACE)) {
        parseStatement(p);
    }

    consume(p, TOKEN_RBRACE, "Expected '}' after function body");
}

Parser* parser(DynamicArray *tokens) {
    Parser *p = alloc(sizeof(Parser));
    p->tokens = tokens; // point to a valid array
    p->pos = 0;

    while (peek(p)->type != TOKEN_EOF) {
        if (match(p, KEYWORD_FUNC)) parseFunction(p);
        else if (match(p, KEYWORD_USING)) parseImport(p);
        else {
            Token *t = peek(p);
            if (t->type == TOKEN_IDENTIFIER && t->value.size == 0) {
                advance(p);
                continue;
            }
            printf("Parser error: unexpected token\n");
            exit(1);
        }
    }

    return p;
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

    Parser* p = parser(&tok);

    free_array(p->tokens);
    free_array(&tok);
    
    return 0;
}
