
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

void CompilationEngine::compile() { compileClass(); }

void CompilationEngine::compileClass()
{

    printNonTerminal("class", toPrint);

    expectPeek(TOKEN_CLASS);

    expectPeek(TOKEN_IDENT);

    expectPeek(TOKEN_LBRACE);

    expectPeek(TOKEN_RBRACE);

    printNonTerminal("/class", toPrint);
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
