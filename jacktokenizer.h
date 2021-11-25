#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include "token.h"

#include <map>

using namespace std;

class JackTokenizer
{
public:
    JackTokenizer(const char *path);
    Token nextToken();
    bool hasMoreTokens();

    ~JackTokenizer();

private:
    bool isAtEnd();

    Token errorToken(const char *message);

    char nextChar();
    char peek();
    char peekNext();
    Token newToken(TokenType type);

    Token number();
    Token identifier();
    Token symbol();
    Token tkstring();

    void skipWhitespace();

    char *start;
    char *current;
    int line;
};

#endif // JACK_TOKENIZER_H