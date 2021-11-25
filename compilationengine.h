#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <string>
#include "jacktokenizer.h"

#include "token.h"

using namespace std;

class CompilationEngine
{
public:
    CompilationEngine(const char *inFileName);
    void nextToken();
    void compile();
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();
    void compileSubroutineCall();
    void compileSubroutineCall(string subName);

private:
    bool peekTokenIs(TokenType t);
    void expectPeek(TokenType t);
    void peekError(TokenType t);

    JackTokenizer *jt;

    Token curToken;
    Token peekToken;
};

#endif
