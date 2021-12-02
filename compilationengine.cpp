
#include "compilationengine.h"

#include "jacktokenizer.h"

#include "symbol.h"

#include <iostream>
#include <map>

std::map<sym::Kind, Segment>
    kindToSeg = {
        {sym::STATIC, STATIC},
        {sym::FIELD, THIS},
        {sym::VAR, LOCAL},
        {sym::ARG, ARG},
};

using namespace std;

CompilationEngine::CompilationEngine(const char *inFileName, const char *outFileName)
{
    jt = new JackTokenizer(inFileName);
    vm = new VMWriter(outFileName);
    nextToken();
    toPrint = false;
    st = new SymbolTable();
}

void CompilationEngine::nextToken()
{
    curToken = peekToken;
    peekToken = jt->nextToken();
}

void CompilationEngine::compile()
{

    compileClass();
    expectPeek(TOKEN_EOF); // realmente acabou o programa
}

void CompilationEngine::compileClass()
{

    printNonTerminal("class", toPrint);

    expectPeek(TOKEN_CLASS);

    expectPeek(TOKEN_IDENT);
    className = tokenLiteral(curToken);

    // adicionar a verificacao do nome da classe com o nome do arquivo.

    expectPeek(TOKEN_LBRACE);

    while (peekTokenIs(TOKEN_STATIC) || peekTokenIs(TOKEN_FIELD))
    {
        compileClassVarDec();
    }

    while (peekTokenIs(TOKEN_FUNCTION) || peekTokenIs(TOKEN_CONSTRUCTOR) || peekTokenIs(TOKEN_METHOD))
    {
        compileSubroutineDec();
    }

    expectPeek(TOKEN_RBRACE);

    printNonTerminal("/class", toPrint);
}

void CompilationEngine::compileClassVarDec()
{

    printNonTerminal("classVarDec", toPrint);
    sym::Kind kind;

    if (peekTokenIs(TOKEN_FIELD))
    {
        kind = sym::FIELD;
        expectPeek(TOKEN_FIELD);
    }
    else
    {
        kind = sym::STATIC;
        expectPeek(TOKEN_STATIC);
    }

    compileType();
    string vartype = tokenLiteral(curToken);

    expectPeek(TOKEN_IDENT);
    string varname = tokenLiteral(curToken);

    st->define(varname, vartype, kind);

    while (peekTokenIs(TOKEN_COMMA))
    {
        expectPeek(TOKEN_COMMA);
        expectPeek(TOKEN_IDENT);

        varname = tokenLiteral(curToken);
        st->define(varname, vartype, kind);
    }

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/classVarDec", toPrint);
}

void CompilationEngine::compileSubroutineDec()
{

    printNonTerminal("subroutineDec", toPrint);

    st->startSubroutine();

    if (peekTokenIs(TOKEN_CONSTRUCTOR))
    {
        expectPeek(TOKEN_CONSTRUCTOR);
    }
    else if (peekTokenIs(TOKEN_FUNCTION))
    {
        expectPeek(TOKEN_FUNCTION);
    }
    else
    {
        expectPeek(TOKEN_METHOD);
    }

    if (peekTokenIs(TOKEN_VOID))
    {
        expectPeek(TOKEN_VOID);
    }
    else
    {
        compileType();
    }

    expectPeek(TOKEN_IDENT);

    string functionName = className + "." + tokenLiteral(curToken);

    expectPeek(TOKEN_LPAREN);

    if (!peekTokenIs(TOKEN_RPAREN))
    {
        compileParameterList();
    }
    else
    {
        // because of compare xml
        printNonTerminal("parameterList", toPrint);
        printNonTerminal("/parameterList", toPrint);
    }

    expectPeek(TOKEN_RPAREN);

    compileSubroutineBody(functionName);

    printNonTerminal("/subroutineDec", toPrint);
}

void CompilationEngine::compileParameterList()
{
    printNonTerminal("parameterList", toPrint);

    sym::Kind kind = sym::ARG;

    compileType();
    string vartype = tokenLiteral(curToken);

    expectPeek(TOKEN_IDENT);
    string varname = tokenLiteral(curToken);

    st->define(varname, vartype, kind);

    while (peekTokenIs(TOKEN_COMMA))
    {
        expectPeek(TOKEN_COMMA);
        compileType();
        string vartype = tokenLiteral(curToken);

        expectPeek(TOKEN_IDENT);
        string varname = tokenLiteral(curToken);
        st->define(varname, vartype, kind);
    }

    printNonTerminal("/parameterList", toPrint);
}

void CompilationEngine::compileSubroutineBody(string functionName)
{
    printNonTerminal("subroutineBody", toPrint);

    expectPeek(TOKEN_LBRACE);

    while (peekTokenIs(TOKEN_VAR))
    {
        compileVarDec();
    }

    int nLocals = st->varCount(sym::VAR);

    vm->writeFunction(functionName, nLocals);

    compileStatements();

    expectPeek(TOKEN_RBRACE);

    printNonTerminal("/subroutineBody", toPrint);
}

void CompilationEngine::compileVarDec()
{
    printNonTerminal("varDec", toPrint);

    expectPeek(TOKEN_VAR);

    sym::Kind kind = sym::VAR;

    compileType();
    string vartype = tokenLiteral(curToken);

    expectPeek(TOKEN_IDENT);
    string varname = tokenLiteral(curToken);

    st->define(varname, vartype, kind);

    while (peekTokenIs(TOKEN_COMMA))
    {
        expectPeek(TOKEN_COMMA);
        expectPeek(TOKEN_IDENT);

        varname = tokenLiteral(curToken);
        st->define(varname, vartype, kind);
    }

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/varDec", toPrint);
}

void CompilationEngine::compileType()
{
    switch (peekToken.type)
    {
    case TOKEN_INT:
        expectPeek(TOKEN_INT);
        break;
    case TOKEN_CHAR:
        expectPeek(TOKEN_CHAR);
        break;
    case TOKEN_BOOLEAN:
        expectPeek(TOKEN_BOOLEAN);
        break;
    case TOKEN_IDENT:
        expectPeek(TOKEN_IDENT);
        break;
    case TOKEN_RETURN:
        return compileReturn();
    default:
        return;
    }
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

    string varName = tokenLiteral(curToken);

    sym::Symbol symbol;

    if (!st->resolve(varName, symbol))
    {
        std::cerr << "Variable " + varName + " not defined" << endl;
        exit(1);
    }

    while (peekTokenIs(TOKEN_LBRACKET))
    {
        expectPeek(TOKEN_LBRACKET);
        compileExpression();
        expectPeek(TOKEN_RBRACKET);
    }

    expectPeek(TOKEN_EQ);

    compileExpression();

    vm->writePop(kindToSeg[symbol.kind], symbol.index);

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/letStatement", toPrint);
}

void CompilationEngine::compileDo()
{
    printNonTerminal("doStatement", toPrint);
    expectPeek(TOKEN_DO);

    expectPeek(TOKEN_IDENT);

    compileSubroutineCall();

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/doStatement", toPrint);
}

void CompilationEngine::compileSubroutineCall()
{

    if (peekTokenIs(TOKEN_LPAREN))
    {
        expectPeek(TOKEN_LPAREN);
        compileExpressionList();
        expectPeek(TOKEN_RPAREN);
    }
    else
    {
        expectPeek(TOKEN_DOT);
        expectPeek(TOKEN_IDENT);

        expectPeek(TOKEN_LPAREN);
        compileExpressionList();
        expectPeek(TOKEN_RPAREN);
    }
}

void CompilationEngine::compileExpressionList()
{

    printNonTerminal("expressionList", toPrint);

    if (!peekTokenIs(TOKEN_RPAREN))
    {
        compileExpression();
    }

    while (peekTokenIs(TOKEN_COMMA))
    {
        expectPeek(TOKEN_COMMA);
        compileExpression();
    }

    printNonTerminal("/expressionList", toPrint);
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

    if (!peekTokenIs(TOKEN_SEMICOLON))
    {
        compileExpression();
    }
    else
    {
        vm->writePush(CONST, 0);
    }

    vm->writeReturn();

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
    printNonTerminal("term", toPrint);

    switch (peekToken.type)
    {
    case TOKEN_NUMBER:
        nextToken();
        printTerminal(curToken, toPrint);
        vm->writePush(CONST, std::stoi(tokenLiteral(curToken)));
        break;
    case TOKEN_FALSE:
    case TOKEN_NULL:
    case TOKEN_TRUE:
        nextToken();
        printTerminal(curToken, toPrint);
        vm->writePush(CONST, 0);
        if (curToken.type == TOKEN_TRUE)
            vm->writeArithmetic(NOT);
        break;
    case TOKEN_THIS:
        nextToken();
        vm->writePush(POINTER, 0);
        printTerminal(curToken, toPrint);
        break;

    case TOKEN_STRING:
        nextToken();
        printTerminal(curToken, toPrint);
        break;

    case TOKEN_IDENT:
        expectPeek(TOKEN_IDENT);
        if (peekTokenIs(TOKEN_LBRACKET))
        {
            // array
            expectPeek(TOKEN_LBRACKET);
            compileExpression();
            expectPeek(TOKEN_RBRACKET);
        }
        else if (peekTokenIs(TOKEN_LPAREN) || peekTokenIs(TOKEN_DOT))
        {
            compileSubroutineCall();
        }
        else
        {
            sym::Symbol symbol;
            string varName = tokenLiteral(curToken);

            if (!st->resolve(varName, symbol))
            {
                std::cerr << "Variable " + varName + " not defined" << endl;
                exit(1);
            }
        }
        break;
    case TOKEN_LPAREN:
        expectPeek(TOKEN_LPAREN);
        compileExpression();
        expectPeek(TOKEN_RPAREN);
        break;
    case TOKEN_MINUS:
    case TOKEN_NOT:
        nextToken();
        printTerminal(curToken, toPrint); // operator
        compileTerm();
        break;

    default:
        cout << "invalid term " << tokenLiteral(peekToken) << endl;
        exit(1);
    }
    printNonTerminal("/term", toPrint);
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
        peekError(t);
    }
}

void CompilationEngine::peekError(TokenType t)
{
    cout << "expected next token to be " << t << ", got " << tokenLiteral(peekToken) << " instead" << endl;
    //= fmt.Sprintf(" %v: expected next token to be %s, got %s instead", line, t, p.peekToken.Type)
    exit(1);
}
