
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

std::map<TokenType, Command>
    binOperators = {
        {TOKEN_PLUS, ADD},
        {TOKEN_MINUS, SUB},
        {TOKEN_LT, LT},
        {TOKEN_GT, GT},
        {TOKEN_EQ, EQ},
        {TOKEN_AND, AND},
        {TOKEN_OR, OR},
};

using namespace std;

CompilationEngine::CompilationEngine(const char *inFileName, const char *outFileName)
{
    jt = new JackTokenizer(inFileName);
    vm = new VMWriter(outFileName);
    nextToken();
    toPrint = false;
    st = new SymbolTable();

    ifLabelNum = 0;
    whileLabelNum = 0;
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

    ifLabelNum = 0;
    whileLabelNum = 0;

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
        st->define("this", className, sym::ARG); // first argument of a method is always "this"
        expectPeek(TOKEN_METHOD);
    }

    TokenType tokenType = curToken.type;

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

    compileSubroutineBody(functionName, tokenType);

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

void CompilationEngine::compileSubroutineBody(string functionName, TokenType tokenType)
{
    printNonTerminal("subroutineBody", toPrint);

    expectPeek(TOKEN_LBRACE);

    while (peekTokenIs(TOKEN_VAR))
    {
        compileVarDec();
    }

    int nLocals = st->varCount(sym::VAR);

    vm->writeFunction(functionName, nLocals);

    if (tokenType == TOKEN_CONSTRUCTOR)
    {
        vm->writePush(CONST, st->varCount(sym::FIELD));
        vm->writeCall("Memory.alloc", 1);
        vm->writePop(POINTER, 0);
    }
    else if (tokenType == TOKEN_METHOD)
    {
        vm->writePush(ARG, 0);    // push "this" of calling object
        vm->writePop(POINTER, 0); // set "this" of method to the calling object
    }

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

    bool isArray = false;

    string varName = tokenLiteral(curToken);

    sym::Symbol symbol;

    if (!st->resolve(varName, symbol))
    {
        std::cerr << "Variable " + varName + " not defined" << endl;
        exit(1);
    }

    while (peekTokenIs(TOKEN_LBRACKET)) // array
    {
        expectPeek(TOKEN_LBRACKET);
        compileExpression();

        vm->writePush(kindToSeg[symbol.kind], symbol.index);
        vm->writeArithmetic(ADD);

        expectPeek(TOKEN_RBRACKET);
        isArray = true;
    }

    expectPeek(TOKEN_EQ);

    compileExpression();

    if (isArray)
    {
        vm->writePop(TEMP, 0);    // push result back onto stack
        vm->writePop(POINTER, 1); // pop address pointer into pointer 1
        vm->writePush(TEMP, 0);   // push result back onto stack
        vm->writePop(THAT, 0);    // Store right hand side evaluation in THAT 0.
    }
    else
    {
        vm->writePop(kindToSeg[symbol.kind], symbol.index);
    }

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/letStatement", toPrint);
}

void CompilationEngine::compileDo()
{
    printNonTerminal("doStatement", toPrint);
    expectPeek(TOKEN_DO);

    expectPeek(TOKEN_IDENT);

    compileSubroutineCall();

    vm->writePop(TEMP, 0);

    expectPeek(TOKEN_SEMICOLON);

    printNonTerminal("/doStatement", toPrint);
}

void CompilationEngine::compileSubroutineCall()
{

    string funcname;
    int numArgs = 0;

    string ident = tokenLiteral(curToken); // nome da classe ou objeto

    if (peekTokenIs(TOKEN_LPAREN)) // é um metodo da propria classe
    {
        expectPeek(TOKEN_LPAREN);
        vm->writePush(POINTER, 0); // metodo do proprio objeto
        numArgs = compileExpressionList();
        expectPeek(TOKEN_RPAREN);
        numArgs++;

        funcname = className + "." + ident;
    }
    else
    {
        // pode ser um metodo de um outro objeto ou uma função

        expectPeek(TOKEN_DOT);
        expectPeek(TOKEN_IDENT);

        sym::Symbol symbol;

        funcname = tokenLiteral(curToken);

        if (st->resolve(ident, symbol)) // é metodo de um objeto
        {
            vm->writePush(kindToSeg[symbol.kind], symbol.index);
            funcname = symbol.type + "." + funcname;
            numArgs = 1;
        }
        else
        { // é uma função
            funcname = ident + "." + funcname;
        }

        expectPeek(TOKEN_LPAREN);
        numArgs = compileExpressionList() + numArgs; // adicionar 1 do metodo

        expectPeek(TOKEN_RPAREN);
    }
    vm->writeCall(funcname, numArgs);
}

int CompilationEngine::compileExpressionList()
{

    printNonTerminal("expressionList", toPrint);

    int numArgs = 0;

    if (!peekTokenIs(TOKEN_RPAREN))
    {
        compileExpression();
        numArgs++;
    }

    while (peekTokenIs(TOKEN_COMMA))
    {
        expectPeek(TOKEN_COMMA);
        compileExpression();
        numArgs++;
    }

    printNonTerminal("/expressionList", toPrint);
    return numArgs;
}

void CompilationEngine::compileWhile()
{
    printNonTerminal("whileStatement", toPrint);

    string labelTrue, labelFalse;
    labelTrue = "WHILE_EXP" + to_string(whileLabelNum);
    labelFalse = "WHILE_END" + to_string(whileLabelNum);
    whileLabelNum++;

    vm->writeLabel(labelTrue); // while true label to execute statements again

    expectPeek(TOKEN_WHILE);

    expectPeek(TOKEN_LPAREN);

    compileExpression();

    vm->writeArithmetic(NOT);
    vm->writeIf(labelFalse);

    expectPeek(TOKEN_RPAREN);

    expectPeek(TOKEN_LBRACE);

    compileStatements();

    vm->writeGoto(labelTrue);   // Go back to labelTrue and check condition
    vm->writeLabel(labelFalse); // Breaks out of while loop because ~(condition) is true

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

    string labelTrue = "IF_TRUE" + to_string(ifLabelNum);
    string labelFalse = "IF_FALSE" + to_string(ifLabelNum);
    string labelEnd = "IF_END" + to_string(ifLabelNum);

    ifLabelNum++;

    expectPeek(TOKEN_IF);

    expectPeek(TOKEN_LPAREN);

    compileExpression();

    expectPeek(TOKEN_RPAREN);

    vm->writeIf(labelTrue);
    vm->writeGoto(labelFalse);
    vm->writeLabel(labelTrue);

    expectPeek(TOKEN_LBRACE);

    compileStatements();

    expectPeek(TOKEN_RBRACE);

    if (peekTokenIs(TOKEN_ELSE))
    {
        vm->writeGoto(labelEnd);
    }

    vm->writeLabel(labelFalse);

    if (peekTokenIs(TOKEN_ELSE))
    {
        expectPeek(TOKEN_ELSE);

        expectPeek(TOKEN_LBRACE);

        compileStatements();

        expectPeek(TOKEN_RBRACE);
        vm->writeLabel(labelEnd);
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
        TokenType op = curToken.type;
        compileTerm();
        compileOperators(op);
    }

    printNonTerminal("/expression", toPrint);
}

void CompilationEngine::compileOperators(TokenType type)
{

    if (type == TOKEN_ASTERISK)
    {
        vm->writeCall("Math.multiply", 2);
    }
    else if (type == TOKEN_SLASH)
    {
        vm->writeCall("Math.divide", 2);
    }
    else
    {
        vm->writeArithmetic(binOperators[type]);
    }
}

void CompilationEngine::compileTerm()
{
    printNonTerminal("term", toPrint);
    std::string strvalue;
    TokenType tokentype;

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

        strvalue = tokenLiteral(curToken);
        vm->writePush(CONST, strvalue.length());
        vm->writeCall("String.new", 1);
        for (int i = 0; i < strvalue.length(); i++)
        {
            vm->writePush(CONST, (int)strvalue[i]);
            vm->writeCall("String.appendChar", 2);
        }

        break;

    case TOKEN_IDENT:
        expectPeek(TOKEN_IDENT);

        if (peekTokenIs(TOKEN_LPAREN) || peekTokenIs(TOKEN_DOT))
        {
            compileSubroutineCall();
        }
        else
        { // variavel comum ou array

            sym::Symbol symbol;
            string varName = tokenLiteral(curToken);

            if (!st->resolve(varName, symbol))
            {
                std::cerr << "Variable " + varName + " not defined" << endl;
                exit(1);
            }

            if (peekTokenIs(TOKEN_LBRACKET)) // array
            {
                expectPeek(TOKEN_LBRACKET);
                compileExpression();

                vm->writePush(kindToSeg[symbol.kind], symbol.index);
                vm->writeArithmetic(ADD);

                expectPeek(TOKEN_RBRACKET);
                vm->writePop(POINTER, 1); // pop address pointer into pointer 1
                vm->writePush(THAT, 0);   // push the value of the address pointer back onto stack
            }
            else
            {
                vm->writePush(kindToSeg[symbol.kind], symbol.index);
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
        tokentype = curToken.type;
        printTerminal(curToken, toPrint); // operator
        compileTerm();

        if (tokentype == TOKEN_MINUS)
            vm->writeArithmetic(NEG);
        else
            vm->writeArithmetic(NOT);
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
    cout << peekToken.line << ":expected next token to be " << t << ", got " << tokenLiteral(peekToken) << " instead" << endl;
    //= fmt.Sprintf(" %v: expected next token to be %s, got %s instead", line, t, p.peekToken.Type)
    exit(1);
}
