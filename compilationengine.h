#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <string>
#include "jacktokenizer.h"

#include "token.h"
#include "symboltable.h"

using namespace std;

class CompilationEngine
{
public:
    CompilationEngine(const char *inFileName);
    void nextToken();
    void compile();
    void compileClass();
    void compileClassVarDec();
    void compileSubroutineDec();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileType();
    void compileStatement();
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

private:
    bool peekTokenIs(TokenType t);
    void expectPeek(TokenType t);
    void peekError(TokenType t);

    JackTokenizer *jt;
    SymbolTable *st;
    bool toPrint;

    Token curToken;
    Token peekToken;
};

#endif
