

#include "token.h"

#include <map>
#include <cstring>
//#include <string.h>
#include <string>

#include <iostream>

std::map<std::string, TokenType>
    keywords = {
        {"int", TOKEN_INT},
        {"class", TOKEN_CLASS},
        {"constructor", TOKEN_CONSTRUCTOR},
        {"function", TOKEN_FUNCTION},
        {"method", TOKEN_METHOD},
        {"field", TOKEN_FIELD},
        {"static", TOKEN_STATIC},
        {"var", TOKEN_VAR},
        {"char", TOKEN_CHAR},
        {"boolean", TOKEN_BOOLEAN},
        {"void", TOKEN_VOID},
        {"true", TOKEN_TRUE},
        {"false", TOKEN_FALSE},
        {"null", TOKEN_NULL},
        {"this", TOKEN_THIS},
        {"let", TOKEN_LET},
        {"do", TOKEN_DO},
        {"if", TOKEN_IF},
        {"else", TOKEN_ELSE},
        {"while", TOKEN_WHILE},
        {"return", TOKEN_RETURN},
};

std::string tokenLiteral(Token tk)
{
    std::string s = "";
    const char *current = tk.start;
    int len = tk.length;
    if (tk.type == TOKEN_STRING)
    {
        len = len - 2;
        current++;
    }
    for (int i = 0; i < len; i++)
    {
        s = s + *current++;
    }
    return s;
}

TokenType keywordType(std::string key)
{
    return keywords[key];
}

bool isKeyword(std::string key)
{
    return keywords.count(key);
}

bool isDigit(char c)
{

    return c >= '0' && c <= '9';
}

bool isSymbol(char c)
{

    char symbols[] = "{}()[].,;+-*/&|<>=~";

    return strchr(symbols, c) != NULL;
}

bool isAlpha(char c)
{
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
}

std::string tagToken(Token tk)
{
    std::string literal = tokenLiteral(tk);

    if (tk.type >= TOKEN_METHOD && tk.type <= TOKEN_THIS)
    {
        return "<keyword> " + literal + " </keyword>";
    }
    else if (tk.type >= TOKEN_LPAREN && tk.type <= TOKEN_EQ)
    {
        const char c = *tk.start;
        switch (c)
        {
        case '<':
            return "<symbol> &lt; </symbol>";
        case '>':
            return "<symbol> &gt; </symbol>";
        case '\"':
            return "<symbol> &quot; </symbol>";
        case '&':
            return "<symbol> &amp; </symbol>";
        default:
            return "<symbol> " + literal + " </symbol>";
        }
    }
    else if (tk.type == TOKEN_IDENT)
    {

        return "<identifier> " + literal + " </identifier>";
    }
    else if (tk.type == TOKEN_NUMBER)
    {

        return "<integerConstant> " + literal + " </integerConstant>";
    }
    else if (tk.type == TOKEN_STRING)
    {

        return "<integerConstant> " + literal + " </integerConstant>";
    }
    else if (tk.type == TOKEN_EOF)
    {

        return "<eof> EOF </eof>";
    }
    else
    {
        return "<ilegal> " + literal + " </ilegal>";
    }
}
