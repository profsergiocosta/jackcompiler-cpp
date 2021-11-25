
#include "JackTokenizer.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>

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

bool JackTokenizer::isDigit()
{
    char c = peek();
    return *current >= '0' && c <= '9';
}

bool JackTokenizer::isAlpha()
{
    char c = peek();
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           c == '_';
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

char JackTokenizer::nextToken()
{

    skipWhitespace();
    return nextChar();
}

JackTokenizer::~JackTokenizer()
{
    free(start);
}