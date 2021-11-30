#ifndef _SYMBOLTABLE_
#define _SYMBOLTABLE_

#include "symbol.h"

#include <map>
#include <string>
using namespace std;

class SymbolTable
{
public:
  SymbolTable();

  void startSubroutine(void);
  void define(string name, string type, sym::Kind kind);
  int varCount(sym::Kind kind);
  bool resolve(string name, sym::Symbol &s);
  sym::Symbol resolve(string name);

private:
  map<string, sym::Symbol> class_scope_;
  map<string, sym::Symbol> subroutine_scope_;
  int count[4];
};

#endif
