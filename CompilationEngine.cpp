
#include "CompilationEngine.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "JackTokenizer.h"

using namespace std;

template <typename T>
string to_string(T value)
{
  // create an output string stream
  std::ostringstream os;

  // throw the value into the string stream
  os << value;

  // convert the string stream into a string and return
  return os.str();
}

Segment kind2Segment(ST::Kind kind)
{
  switch (kind)
  {
  case ST::STATIC:
    return STATIC;
  case ST::FIELD:
    return THIS;
  case ST::ARG:
    return ARG;
  case ST::VAR:
    return LOCAL;
  default:
    return NONE;
  }
}

CompilationEngine::CompilationEngine(string inFileName, string outFileName)
{
  jt = new JackTokenizer(inFileName);
  out.open((outFileName + "_").c_str());
  vm = new VMWriter(outFileName.c_str());
  st = new SymbolTable();

  labelNum = 0;
}

void CompilationEngine::nextToken() { jt->advance(); }

void CompilationEngine::eat(string tk)
{
  if (jt->currentToken == tk)
  {
    // cout << "ok" << endl;
    writeTerminal(jt->tagToken());
  }
  else
  {
    cout << tk << " expected " << jt->currentToken << " found" << endl;
    exit(0);
  }
}

void CompilationEngine::compile() { compileClass(); }

bool CompilationEngine::isOp(char ch)
{
  // cout <<"operador: " << ch << endl;
  return (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '&' ||
          ch == '|' || ch == '<' || ch == '>' || ch == '=');
}

void CompilationEngine::compileClass()
{
  tagNonTerminal("class");

  // cout << "class" << endl;

  nextToken();
  eat("class");

  nextToken();
  className_ = jt->currentToken;
  writeTerminal(jt->tagToken());
  nextToken();
  eat("{");
  nextToken();

  while (jt->tokenType() == tKEYWORD &&
         (jt->keyword() == kFIELD || jt->keyword() == kSTATIC))
    compileClassVarDec();

  while (jt->tokenType() == tKEYWORD &&
         (jt->keyword() == kCONSTRUCTOR || jt->keyword() == kFUNCTION ||
          jt->keyword() == kMETHOD))
    compileSubroutine();

  //
  eat("}");
  untagNonTerminal("class");
}

void CompilationEngine::writeIdentifier()
{
  string name = jt->currentToken;
  ST::Kind kind = st->kindOf(name);
  string type;
  if (kind != ST::NONE)
  {
    type = st->typeOf(name);
    write("<identifier type=" + type + " kind=" + st->fromKind(kind) +
          " index=" + to_string(st->indexOf(name)) + "> " + name +
          " </identifier>");
  }
}

void CompilationEngine::compileClassVarDec()
{
  // cout << "compileClassVarDec" << endl;
  string name, type;
  ST::Kind kind;

  tagNonTerminal("classVarDec");

  if (jt->keyword() == kFIELD)
  {
    eat("field");
    kind = ST::FIELD;
  }
  else
  {
    eat("static");
    kind = ST::STATIC;
  }

  nextToken();
  type = getType();

  writeType();
  nextToken();
  // declaracao de variaveis
  name = jt->currentToken;
  st->define(name, type, kind);

  // writeTerminal(jt->tagToken()); // identificador
  writeIdentifier();

  nextToken();

  while (jt->tokenType() == tSYMBOL && jt->symbol() == ',')
  {
    eat(",");
    nextToken();
    // declaracao
    name = jt->currentToken;
    st->define(name, type, kind);

    writeIdentifier();

    // writeTerminal(jt->tagToken()); // identificador
    nextToken();
  }
  eat(";");
  untagNonTerminal("classVarDec");
  nextToken();
}

void CompilationEngine::writeType()
{
  if (jt->tokenType() == tKEYWORD)
  {
    switch (jt->keyword())
    {
    case kINT:
      eat("int");
      break;

    case kCHAR:
      eat("char");
      break;

    case kBOOLEAN:
      eat("boolean");
      break;

    default:
      break;
    }
  }
  else
  {
    writeTerminal(jt->tagToken());
  }
}

string CompilationEngine::getType()
{
  if (jt->tokenType() == tKEYWORD)
  {
    switch (jt->keyword())
    {
    case kINT:
      return "int";
      break;

    case kCHAR:
      return "char";
      break;

    case kBOOLEAN:
      return "boolean";
      break;

    default:
      return "NONE";
      break;
    }
  }
  else
  {
    return jt->currentToken;
  }
}

void CompilationEngine::compileSubroutine()
{
  st->startSubroutine();

  tagNonTerminal("subroutineDec");

  subroutineType = jt->keyword();

  if (jt->keyword() == kCONSTRUCTOR)
  {
    eat("constructor");
  }
  else if (jt->keyword() == kFUNCTION)
    eat("function");
  else
  {
    st->define("this", className_,
               ST::ARG); // first argument of a method is always "this"
    eat("method");
  }

  nextToken();

  if (jt->tokenType() == tKEYWORD && jt->keyword() == kVOID)
    eat("void");
  else
    writeType();

  nextToken();
  writeTerminal(jt->tagToken()); // identificador

  currentFunctionName = className_ + "." + jt->currentToken;

  nextToken();
  eat("(");
  nextToken();
  compileParameterList();
  eat(")");
  nextToken();
  compileSubroutineBody();

  untagNonTerminal("subroutineDec");
}

void CompilationEngine::compileParameterList()
{
  string name, type;
  ST::Kind kind = ST::ARG;

  tagNonTerminal("parameterList");

  if (jt->tokenType() != tSYMBOL)
  {
    type = getType();

    writeType();
    nextToken();

    name = jt->currentToken;
    st->define(name, type, kind);

    writeIdentifier();
    nextToken();

    while (jt->tokenType() == tSYMBOL && jt->symbol() == ',')
    {
      eat(",");
      nextToken();

      type = getType();

      writeType();
      nextToken();

      name = jt->currentToken;
      st->define(name, type, kind);

      writeIdentifier();
      nextToken();
    }
  }

  untagNonTerminal("parameterList");
}
void CompilationEngine::compileSubroutineBody()
{
  tagNonTerminal("subroutineBody");

  eat("{");
  nextToken();

  while (jt->tokenType() == tKEYWORD && jt->keyword() == kVAR)
    compileVarDec();

  int nLocals = st->varCount(ST::VAR);
  vm->writeFunction(currentFunctionName, nLocals);

  if (subroutineType == kCONSTRUCTOR)
  {
    vm->writePush(CONST, st->varCount(ST::FIELD));
    vm->writeCall("Memory.alloc", 1);
    vm->writePop(POINTER, 0);
  }
  else if (subroutineType == kMETHOD)
  {
    vm->writePush(ARG, 0);    // push "this" of calling object
    vm->writePop(POINTER, 0); // set "this" of method to the calling object
  }
  compileStatements();
  eat("}");
  untagNonTerminal("subroutineBody");
  nextToken();
}
void CompilationEngine::compileVarDec()
{
  string name, type;
  ST::Kind kind = ST::VAR;

  tagNonTerminal("varDec");
  eat("var");
  nextToken();

  type = getType();

  writeType();
  nextToken();

  // declaracao
  name = jt->currentToken;
  st->define(name, type, kind);

  writeIdentifier();
  nextToken();

  while (jt->tokenType() == tSYMBOL && jt->symbol() == ',')
  {
    eat(",");
    nextToken();
    name = jt->currentToken;
    st->define(name, type, kind);

    writeIdentifier();
    nextToken();
  }

  eat(";");
  untagNonTerminal("varDec");
  nextToken();
}

bool CompilationEngine::isStatement(string token)
{
  return (token == "let" || token == "if" || token == "while" ||
          token == "do" || token == "return");
}

void CompilationEngine::compileStatements()
{
  tagNonTerminal("statements");

  while (jt->tokenType() == tKEYWORD && isStatement(jt->currentToken))
  {
    switch (jt->keyword())
    {
    case kLET:
      compileLet();
      break;

    case kDO:
      compileDo();
      break;

    case kIF:
      compileIf();
      break;

    case kWHILE:
      compileWhile();
      break;

    case kRETURN:
      compileReturn();
      break;
    default:
      break;
    }
  }

  untagNonTerminal("statements");
}

void CompilationEngine::compileDo()
{
  tagNonTerminal("doStatement");
  eat("do");
  nextToken();
  compileSubroutineCall();
  eat(";");
  vm->writePop(TEMP, 0);
  untagNonTerminal("doStatement");
  nextToken();
}

void CompilationEngine::compileLet()
{
  tagNonTerminal("letStatement");
  eat("let");
  nextToken();

  string lhsVar = jt->currentToken;
  bool isArray = false;
  writeIdentifier();
  nextToken();

  if (jt->tokenType() == tSYMBOL && jt->symbol() == '[')
  {
    eat("[");
    nextToken();
    vm->writePush(kind2Segment(st->kindOf(lhsVar)), st->indexOf(lhsVar));
    compileExpression();
    vm->writeArithmetic(ADD);
    eat("]");
    nextToken();
    isArray = true;
  }

  eat("=");
  nextToken();
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
    vm->writePop(kind2Segment(st->kindOf(lhsVar)), st->indexOf(lhsVar));
  }

  eat(";");
  untagNonTerminal("letStatement");
  nextToken();
}

void CompilationEngine::compileWhile()
{
  string labelTrue, labelFalse;
  labelTrue = "WHILETRUE" + to_string(labelNum);
  labelFalse = "WHILEFALSE" + to_string(labelNum);
  labelNum++;

  vm->writeLabel(labelTrue); // while true label to execute statements again
  tagNonTerminal("whileStatement");
  eat("while");
  nextToken();
  eat("(");
  nextToken();
  compileExpression();
  vm->writeArithmetic(NOT);
  vm->writeIf(labelFalse);
  eat(")");
  nextToken();
  eat("{");
  nextToken();
  compileStatements();
  vm->writeGoto(labelTrue); // Go back to labelTrue and check condition
  vm->writeLabel(
      labelFalse); // Breaks out of while loop because ~(condition) is true

  eat("}");

  untagNonTerminal("whileStatement");
  nextToken();
}

void CompilationEngine::compileReturn()
{
  tagNonTerminal("returnStatement");
  eat("return");
  nextToken();

  if (jt->tokenType() != tSYMBOL ||
      (jt->tokenType() == tSYMBOL && jt->symbol() != ';'))
  {
    compileExpression();
    vm->writeReturn();
  }
  else
  {
    vm->writePush(CONST, 0);
    vm->writeReturn();
  }

  eat(";");
  untagNonTerminal("returnStatement");
  nextToken();
}
void CompilationEngine::compileIf()
{
  string labelElse, labelContinue;
  labelElse = "IFFALSE" + to_string(labelNum);
  labelContinue = "CONTINUE" + to_string(labelNum);
  labelNum++;

  tagNonTerminal("ifStatement");
  eat("if");
  nextToken();
  eat("(");
  nextToken();
  compileExpression();
  vm->writeArithmetic(NOT);
  vm->writeIf(labelElse);

  eat(")");
  nextToken();
  // cout << "aqui" << endl;
  eat("{");
  nextToken();
  compileStatements();
  vm->writeGoto(labelContinue);
  eat("}");
  nextToken();
  vm->writeLabel(labelElse);
  if (jt->tokenType() == tKEYWORD && jt->keyword() == kELSE)
  {
    eat("else");
    nextToken();
    eat("{");
    nextToken();
    compileStatements();
    eat("}");
    nextToken();
  }
  vm->writeLabel(labelContinue);

  untagNonTerminal("ifStatement");
}

void CompilationEngine::compileExpression()
{
  tagNonTerminal("expression");
  compileTerm();

  while (jt->tokenType() == tSYMBOL && isOp(jt->symbol()))
  {
    char op = jt->symbol();
    writeTerminal(jt->tagToken());
    nextToken();
    compileTerm();
    switch (op)
    {
    case '+':
      vm->writeArithmetic(ADD);
      break;
    case '-':
      vm->writeArithmetic(SUB);
      break;
    case '*':
      vm->writeCall("Math.multiply", 2);
      break;
    case '/':
      vm->writeCall("Math.divide", 2);
      break;
    case '&':
      vm->writeArithmetic(AND);
      break;
    case '|':
      vm->writeArithmetic(OR);
      break;
    case '<':
      vm->writeArithmetic(LT);
      break;
    case '>':
      vm->writeArithmetic(GT);
      break;
    case '=':
      vm->writeArithmetic(EQ);
      break;
    case '~':
      vm->writeArithmetic(NOT);
      break;
    }
  }

  untagNonTerminal("expression");
}

void CompilationEngine::compileTerm()
{
  tagNonTerminal("term");

  string id;
  string oldTok;
  string str_const;

  switch (jt->tokenType())
  {
  case tINT_CONST:
    vm->writePush(CONST, jt->intVal());
    writeTerminal(jt->tagToken());
    break;

  case tSTRING_CONST:
    str_const = jt->stringVal();
    vm->writePush(CONST, str_const.length());
    vm->writeCall("String.new", 1);
    for (int i = 0; i < str_const.length(); i++)
    {
      vm->writePush(CONST, (int)str_const[i]);
      vm->writeCall("String.appendChar", 2);
    }
    writeTerminal(jt->tagToken());
    break;

  case tKEYWORD:
    writeTerminal(jt->tagToken());

    switch (jt->keyword())
    {
    case kTRUE:
      vm->writePush(CONST, 0);
      vm->writeArithmetic(NOT);
      break;
    case kFALSE:
      vm->writePush(CONST, 0);
      break;
    case kNULL:
      vm->writePush(CONST, 0);
      break;
    case kTHIS:
      vm->writePush(POINTER, 0); // this refers to pointer 0
      break;
    }
    break;

  case tIDENTIFIER:

    id = jt->currentToken; // save current token
    nextToken();

    if (jt->tokenType() == tSYMBOL && jt->symbol() == '[')
    {
      oldTok = jt->currentToken;
      jt->currentToken = id;
      writeIdentifier();
      jt->currentToken = oldTok;

      eat("[");
      nextToken();
      compileExpression();
      vm->writePush(kind2Segment(st->kindOf(id)), st->indexOf(id));
      vm->writeArithmetic(ADD);

      eat("]");
      vm->writePop(POINTER, 1); // pop address pointer into pointer 1
      vm->writePush(
          THAT, 0); // push the value of the address pointer back onto stack
      nextToken();
    }
    else if (jt->tokenType() == tSYMBOL &&
             (jt->symbol() == '.' || jt->symbol() == '('))
    {
      cout << "subrotina " << id << endl;

      compileSubroutineCall(id);
    }
    else
    {
      oldTok = jt->currentToken;
      jt->currentToken = id;
      vm->writePush(kind2Segment(st->kindOf(id)), st->indexOf(id));
      writeIdentifier();
      jt->currentToken = oldTok;
    }

    untagNonTerminal("term");
    return; // has already advanced
    break;

  case tSYMBOL:
    if (jt->symbol() == '(')
    { // '(' expression ')'

      eat("(");
      nextToken();
      compileExpression();
      eat(")");
    }
    else
    { // unaryOp term

      char uOp = jt->symbol();
      cout << "unario" << endl;

      writeTerminal(jt->tagToken());
      nextToken();
      compileTerm();

      switch (uOp)
      {
      case '-':
        vm->writeArithmetic(NEG);
        break;
      case '~':
        vm->writeArithmetic(NOT);
        break;
      default:
        cerr << "unario nao reconhecid" << endl;
        break;
      }

      untagNonTerminal("term");
      return; // has already advanced
    }

    break;

  default:
    break;
  }

  nextToken();
  untagNonTerminal("term");
}

void CompilationEngine::compileExpressionList()
{
  tagNonTerminal("expressionList");
  numArgs = 0;

  if (jt->tokenType() != tSYMBOL || (jt->symbol() == '('))
    compileExpression();
  numArgs++;

  while (jt->tokenType() == tSYMBOL && jt->symbol() == ',')
  {
    eat(",");
    nextToken();
    numArgs++;
    compileExpression();
  }

  untagNonTerminal("expressionList");
}

void CompilationEngine::compileSubroutineCall(string subName)
{
  string oldTok = jt->currentToken;
  jt->currentToken = subName;
  writeTerminal(jt->tagToken()); // identificador

  string fname = jt->currentToken;
  jt->currentToken = oldTok;

  if (jt->tokenType() == tSYMBOL && jt->symbol() == '(')
  {
    eat("(");
    nextToken();
    compileExpressionList();
    vm->writeCall(fname, numArgs);
    eat(")");
  }
  else
  {
    eat(".");
    nextToken();

    fname = fname + "." + jt->currentToken;

    writeTerminal(jt->tagToken()); // identificador
    nextToken();
    eat("(");
    nextToken();
    compileExpressionList();

    eat(")");
  }
  vm->writeCall(fname, numArgs);
  nextToken();
}
void CompilationEngine::compileSubroutineCall()
{
  writeTerminal(jt->tagToken()); // identificador
  string fname = jt->currentToken;
  nextToken();

  if (jt->tokenType() == tSYMBOL && jt->symbol() == '(')
  {
    eat("(");
    nextToken();
    compileExpressionList();
    eat(")");
  }
  else
  {
    eat(".");
    nextToken();
    writeTerminal(jt->tagToken()); // identificador
    fname = fname + "." + jt->currentToken;
    nextToken();
    eat("(");
    nextToken();
    compileExpressionList();
    eat(")");
  }
  vm->writeCall(fname, numArgs);
  nextToken();
}

void CompilationEngine::writeTerminal(string terminal) { write(terminal); }

void CompilationEngine::tagNonTerminal(string nonTerminal)
{
  write("<" + nonTerminal + ">");
}

void CompilationEngine::untagNonTerminal(string nonTerminal)
{
  write("</" + nonTerminal + ">");
}

void CompilationEngine::write(string s) { out << s << endl; }
