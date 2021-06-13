#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include <fstream>
#include <string>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VmWriter.h"

using namespace std;

class CompilationEngine
{
public:
  CompilationEngine(string inFileName, string outFileName);
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
  void eat(string tk);
  void writeTerminal(string terminal);
  void writeIdentifier();
  void tagNonTerminal(string nonTerminal);
  void untagNonTerminal(string nonTerminal);
  void write(string s);
  void writeType();
  bool isOp(char ch);
  bool isStatement(string token);

  // novos

  string getType();

  // privados
  JackTokenizer *jt;
  ofstream out;
  SymbolTable *st;
  VMWriter *vm;

  string className_;
  int numArgs;

  string currentFunctionName;
  Keyword subroutineType;
  int labelNum;
};

#endif
