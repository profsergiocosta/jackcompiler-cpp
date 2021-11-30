
#include "symbol.h"
#include "symboltable.h"
#include <iostream>
#include <string>

using namespace std;
using namespace sym;

// Constructor
SymbolTable::SymbolTable()
{
  class_scope_.clear();
  subroutine_scope_.clear();
  for (int i = 0; i < 4; i++)
  {
    count[i] = 0;
  }
}

void SymbolTable::startSubroutine(void)
{
  subroutine_scope_.clear();
  count[sym::ARG] = 0;
  count[sym::VAR] = 0;
}

void SymbolTable::define(string name, string type, sym::Kind kind)
{
  Symbol s = {name, type, kind, count[kind]};

  if (kind == sym::STATIC or kind == sym::FIELD)
  {
    class_scope_[name] = s;
  }
  else if (kind == sym::ARG or kind == sym::VAR)
  {
    subroutine_scope_[name] = s;
  }

  count[kind]++;
}

int SymbolTable::varCount(sym::Kind kind) { return count[kind]; }

bool hasKey(map<string, Symbol> &scope, string key)
{
  return !(scope.find(key) == scope.end());
}

Symbol SymbolTable::resolve(string name)
{
  Symbol s;
  if (resolve(name, s))
  {
    return s;
  }
  else
  {
    throw string("Variable not defined!");
  }
}

bool SymbolTable::resolve(string name, Symbol &s)
{
  bool r = true;
  if (hasKey(subroutine_scope_, name))
    s = subroutine_scope_[name];
  else if (hasKey(class_scope_, name))
    s = class_scope_[name];
  else
  {
    r = false;
    throw string("Variable not defined!");
  }

  return r;
}
