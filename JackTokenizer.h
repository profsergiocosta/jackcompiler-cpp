#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

enum TokenType
{
    tKEYWORD,      /**< a keyword */
    tSYMBOL,       /**< a symbol */
    tIDENTIFIER,   /**< an identifier */
    tINT_CONST,    /**< an integer constante */
    tSTRING_CONST, /**< a string constant */
    tUNKNOWN       /**< unknown type */
};

typedef struct
{
    TokenType type;
    const char *start;
    int length;
    int line;
} Token;

class JackTokenizer
{
public:
    JackTokenizer(const char *path);
    char nextToken();

    bool hasMoreTokens();

    ~JackTokenizer();

private:
    bool isDigit();
    bool isAlpha();
    bool isAtEnd();
    char nextChar();
    char peek();
    char peekNext();

    void skipWhitespace();

    char *start;
    const char *current;
    int line;
};

#endif // JACK_TOKENIZER_H