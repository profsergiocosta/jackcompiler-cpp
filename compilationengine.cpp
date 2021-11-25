
#include "compilationengine.h"

#include "jacktokenizer.h"

#include <iostream>

using namespace std;

CompilationEngine::CompilationEngine(const char *inFileName)
{
    jt = new JackTokenizer(inFileName);
    nextToken();
}

void CompilationEngine::nextToken()
{
    curToken = peekToken;
    peekToken = jt->nextToken();
}

void CompilationEngine::compile() { compileClass(); }

void CompilationEngine::compileClass()
{
    expectPeek(TOKEN_CLASS);
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
        cout << tagToken(curToken) << endl;
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
