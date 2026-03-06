#ifndef PARSER_H
#define PARSER_H

#include "../headers/dynamic_array.h"

typedef enum TokenType TokenType;
typedef struct Token Token;
typedef struct Parser Parser;

enum TokenType {
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
    TOKEN_EOF,

    TOKEN_KEYWORD_START,

    KEYWORD_INT,
    KEYWORD_STR,
    KEYWORD_CHAR,
    KEYWORD_BOOL,
    KEYWORD_HASH,
    KEYWORD_ARR,
    KEYWORD_VOID,
    KEYWORD_FUNC,
    KEYWORD_RETURN,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_USING,

    TOKEN_KEYWORD_END
};

struct Token {
    TokenType type;
    DynamicArray value;
};

struct Parser {
    DynamicArray *tokens;
    size_t pos;
};

// parser helpers
Token* peek(Parser *p);
Token* advance(Parser *p);
Token* consume(Parser *p, TokenType type, const char *msg);
int check(Parser *p, TokenType type);
int isType(TokenType type);
void expect(Parser *p, TokenType type);
void parseParameters(Parser *p);
void parseFunctionSignature(Parser *p);
void parseFactor(Parser *p);
void parseTerm(Parser *p);
void parseExpression(Parser *p);
void parseStatement(Parser *p);
void parseVariable(Parser *p);
void parseReturn(Parser *p);
void parseIf(Parser *p);
void parseInstructionOrCall(Parser *p);
void parseImport(Parser *p);
void parseFunction(Parser *p);
Parser* parser(DynamicArray *tokens);

#endif