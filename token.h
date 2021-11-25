#ifndef TOKEN_H
#define TOKEN_H

#include <string>

enum TokenType
{
    TOKEN_ILLEGAL,
    TOKEN_EOF,

    TOKEN_IDENT,
    TOKEN_NUMBER,
    TOKEN_STRING,

    // symbols
    // delimitator

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACKET,
    TOKEN_RBRACKET,

    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_DOT,

    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_ASTERISK,
    TOKEN_SLASH,

    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,

    TOKEN_LT,
    TOKEN_GT,
    TOKEN_EQ,

    TOKEN_METHOD,
    TOKEN_STATIC,
    TOKEN_INT,
    TOKEN_BOOLEAN,
    TOKEN_TRUE,
    TOKEN_NULL,
    TOKEN_LET,
    TOKEN_IF,
    TOKEN_WHILE,
    TOKEN_CONSTRUCTOR,
    TOKEN_FIELD,
    TOKEN_VAR,
    TOKEN_CHAR,
    TOKEN_VOID,
    TOKEN_CLASS,
    TOKEN_FALSE,
    TOKEN_DO,
    TOKEN_ELSE,
    TOKEN_RETURN,
    TOKEN_FUNCTION,
    TOKEN_THIS,
};

typedef struct
{
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

bool isDigit(char c);
bool isAlpha(char c);
bool isSymbol(char c);
bool isOperator(char c);

bool isKeyword(std::string key);

TokenType keywordType(std::string key);

void printTerminal(Token tok, bool toPrint);

void printNonTerminal(std::string nonTerminal, bool toPrint);

std::string tokenLiteral(Token tk);
std::string tagToken(Token tk);

#endif