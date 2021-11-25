
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
    //compileLet();
    compileStatements();

    expectPeek(TOKEN_EOF); // realmente acabou o programa
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

void CompilationEngine::compileStatements()
{
    printNonTerminal("statements", toPrint);
    while (isStatement(peekToken.type))
    {
        compileStatement();
    }
    printNonTerminal("/statements", toPrint);
}

void CompilationEngine::compileStatement()
{
    switch (peekToken.type)
    {
    case TOKEN_LET:
        return compileLet();
    case TOKEN_WHILE:
        return compileWhile();
    case TOKEN_IF:
        return compileIf();
    case TOKEN_DO:
        return compileDo();
    case TOKEN_RETURN:
        return compileReturn();
    default:
        return;
    }
}

void CompilationEngine::compileLet()
{

    printNonTerminal("letStatement", toPrint);

    expectPeek(TOKEN_LET);

    expectPeek(TOKEN_IDENT);

    while (peekTokenIs(TOKEN_LBRACKET))
    {
        expectPeek(TOKEN_LBRACKET);
        compileExpression();
        expectPeek(TOKEN_RBRACKET);
    }

    expectPeek(TOKEN_EQ);

    compileExpression();

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/letStatement", toPrint);
}

void CompilationEngine::compileDo()
{
    printNonTerminal("doStatement", toPrint);
    expectPeek(TOKEN_DO);

    expectPeek(TOKEN_IDENT);

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/doStatement", toPrint);
}

void CompilationEngine::compileWhile()
{
    printNonTerminal("whileStatement", toPrint);
    expectPeek(TOKEN_WHILE);

    expectPeek(TOKEN_LPAREN);

    compileExpression();

    expectPeek(TOKEN_RPAREN);

    expectPeek(TOKEN_LBRACE);

    compileStatements();

    expectPeek(TOKEN_RBRACE);

    printNonTerminal("/whileStatement", toPrint);
}
void CompilationEngine::compileReturn()
{
    printNonTerminal("returnStatement", toPrint);

    expectPeek(TOKEN_RETURN);

    compileExpression();

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/returnStatement", toPrint);
}
void CompilationEngine::compileIf()
{
    printNonTerminal("ifStatement", toPrint);

    expectPeek(TOKEN_IF);

    expectPeek(TOKEN_LPAREN);

    compileExpression();

    expectPeek(TOKEN_RPAREN);

    expectPeek(TOKEN_LBRACE);

    compileStatements();

    expectPeek(TOKEN_RBRACE);

    if (peekTokenIs(TOKEN_ELSE))
    {
        expectPeek(TOKEN_ELSE);

        expectPeek(TOKEN_LBRACE);

        compileStatements();

        expectPeek(TOKEN_RBRACE);
    }

    printNonTerminal("/ifStatement", toPrint);
}

void CompilationEngine::compileSubroutineCall()
{
}

void CompilationEngine::compileExpression()
{
    printNonTerminal("expression", toPrint);

    compileTerm();

    while (isOperator(*peekToken.start))
    {
        nextToken();
        printTerminal(curToken, toPrint); // operator
        compileTerm();
    }

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
