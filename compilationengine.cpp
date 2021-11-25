
#include "compilationengine.h"

#include "jacktokenizer.h"

#include <iostream>

using namespace std;

CompilationEngine::CompilationEngine(const char *inFileName)
{
    jt = new JackTokenizer(inFileName);
    nextToken();
    toPrint = true;
}

void CompilationEngine::nextToken()
{
    curToken = peekToken;
    peekToken = jt->nextToken();
}

void CompilationEngine::compile()
{

    //compileClass();
    compileLet();
}

void CompilationEngine::compileClass()
{

    printNonTerminal("class", toPrint);

    expectPeek(TOKEN_CLASS);

    expectPeek(TOKEN_IDENT);

    expectPeek(TOKEN_LBRACE);

    expectPeek(TOKEN_RBRACE);

    printNonTerminal("/class", toPrint);
}

void CompilationEngine::compileLet()
{

    printNonTerminal("letStatement", toPrint);

    expectPeek(TOKEN_LET);

    expectPeek(TOKEN_IDENT);

    expectPeek(TOKEN_EQ);

    compileExpression();

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/letStatement", toPrint);
}

void CompilationEngine::compileExpression()
{
    printNonTerminal("expression", toPrint);

    compileTerm();

    printNonTerminal("/expression", toPrint);
}

void CompilationEngine::compileTerm()
{
    switch (peekToken.type)
    {
    case TOKEN_NUMBER:
    case TOKEN_STRING:
    case TOKEN_TRUE:
    case TOKEN_FALSE:
    case TOKEN_NULL:
    case TOKEN_THIS:
    case TOKEN_IDENT:
        nextToken();
        printTerminal(curToken, toPrint);
        break;
    default:
        peekError(TOKEN_IDENT); // provisorio
    }
}

bool CompilationEngine::peekTokenIs(TokenType t)
{
    return peekToken.type == t;
}

void CompilationEngine::expectPeek(TokenType t)
{
    if (peekTokenIs(t))
    {
        nextToken();

        printTerminal(curToken, toPrint);
    }
    else
    {
        cout << "error" << endl;
        peekError(t);
        exit(0);
    }
}

void CompilationEngine::peekError(TokenType t)
{
    cout << "expected next token to be " << t << ", got " << peekToken.type << " instead" << endl;
    //= fmt.Sprintf(" %v: expected next token to be %s, got %s instead", line, t, p.peekToken.Type)
}
