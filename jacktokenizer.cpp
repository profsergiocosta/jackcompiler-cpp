

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "jacktokenizer.h"
#include "token.h"

using namespace std;

static char *readFile(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "Could not open file \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(fileSize + 1);

    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

JackTokenizer::JackTokenizer(const char *path)
{
    start = readFile(path);
    current = start;
    line = 1;
}

Token JackTokenizer::errorToken(const char *message)
{
    Token token;
    token.type = TOKEN_ILLEGAL;
    token.start = message;
    token.length = (int)strlen(message);
    token.line = line;
    return token;
}

bool JackTokenizer::hasMoreTokens()
{
    return !isAtEnd();
}

bool JackTokenizer::isAtEnd()
{
    return *current == '\0';
}

char JackTokenizer::nextChar()
{
    current++;
    return current[-1];
}

char JackTokenizer::peek()
{
    return *current;
}

char JackTokenizer::peekNext()
{
    if (isAtEnd())
        return '\0';
    return current[1];
}

void JackTokenizer::skipWhitespace()
{
    for (;;)
    {
        char c = peek();
        switch (c)
        {
        case ' ':
        case '\r':
        case '\t':
            nextChar();
            break;
        case '\n':
            line++;
            nextChar();
            break;
        case '/':
            if (peekNext() == '/')
            {
                // A comment goes until the end of the line.
                while (peek() != '\n' && !isAtEnd())
                    nextChar();
            }
            else if (peekNext() == '*')
            {
                nextChar();
                while (!isAtEnd())
                {
                    if (peek() == '*' && peekNext() == '/')
                    {
                        nextChar();
                        nextChar();
                        break;
                    }

                    nextChar();
                }
            }

            else
            {
                return;
            }
            break;

        default:
            return;
        }
    }
}

Token JackTokenizer::nextToken()
{

    skipWhitespace();
    start = current;

    if (isAtEnd())
        return newToken(TOKEN_EOF);

    char c = nextChar();

    if (c == '"')
        return tkstring();

    if (isDigit(c))
        return number();

    if (isSymbol(c))
        return symbol();

    if (isAlpha(c))
        return identifier();

    return newToken(TOKEN_EOF);
}

Token JackTokenizer::number()
{

    while (isDigit(peek()))
        nextChar();

    // Look for a fractional part.
    if (peek() == '.' && isDigit(peekNext()))
    {
        // Consume the ".".
        nextChar();

        while (isDigit(peek()))
            nextChar();
    }

    return newToken(TOKEN_NUMBER);
}

Token JackTokenizer::identifier()
{
    while (isAlpha(peek()) || isDigit(peek()))
        nextChar();

    Token tk = newToken(TOKEN_IDENT);

    string literal = tokenLiteral(tk);
    if (isKeyword(literal))
    {
        tk.type = keywordType(literal);
    }

    return tk;
}

Token JackTokenizer::tkstring()
{
    while (peek() != '"' && !isAtEnd())
    {
        if (peek() == '\n')
            line++;
        nextChar();
    }

    if (isAtEnd())
        return errorToken("Unterminated string.");

    // The closing quote.
    nextChar();
    return newToken(TOKEN_STRING);
}

Token JackTokenizer::symbol()
{
    char c = *start;
    switch (c)
    {
    case '(':
        return newToken(TOKEN_LPAREN);
    case ')':
        return newToken(TOKEN_RPAREN);
    case '{':
        return newToken(TOKEN_LBRACE);
    case '}':
        return newToken(TOKEN_RBRACE);
    case '[':
        return newToken(TOKEN_LBRACKET);
    case ']':
        return newToken(TOKEN_RBRACKET);
    case ';':
        return newToken(TOKEN_SEMICOLON);
    case ',':
        return newToken(TOKEN_COMMA);
    case '.':
        return newToken(TOKEN_DOT);
    case '-':
        return newToken(TOKEN_MINUS);
    case '+':
        return newToken(TOKEN_PLUS);
    case '/':
        return newToken(TOKEN_SLASH);
    case '*':
        return newToken(TOKEN_ASTERISK);
    case '&':
        return newToken(TOKEN_AND);
    case '|':
        return newToken(TOKEN_OR);
    case '~':
        return newToken(TOKEN_NOT);
    case '>':
        return newToken(TOKEN_GT);
    case '<':
        return newToken(TOKEN_LT);
    case '=':
        return newToken(TOKEN_EQ);

    default:
        return newToken(TOKEN_ILLEGAL);
    }
}

Token JackTokenizer::newToken(TokenType type)
{
    Token token;
    token.type = type;
    token.start = start;
    token.length = (int)(current - start);
    token.line = line;
    return token;
}

JackTokenizer::~JackTokenizer()
{
    free(start);
}