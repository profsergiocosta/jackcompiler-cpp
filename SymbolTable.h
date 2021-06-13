#ifndef _SYMBOLTABLE_
#define _SYMBOLTABLE_

#include <map>
#include <string>
using namespace std;

namespace ST
{

  enum Kind
  {
    STATIC,
    FIELD,
    ARG,
    VAR,
    NONE
  };
};

struct Symbol
{
  string type;
  ST::Kind kind;
  int index;
};

class SymbolTable
{
public:
  // Constructor
  SymbolTable();

  // Methods
  void startSubroutine(void);
  void define(string name, string type, ST::Kind kind);
  int varCount(ST::Kind kind);
  ST::Kind kindOf(string name);
  string typeOf(string name);
  int indexOf(string name);
  string fromKind(ST::Kind k);

  bool lookup(string name, Symbol &s);

private:
  map<string, Symbol> class_scope_;
  map<string, Symbol> subroutine_scope_;
  int count[4];
};

#endif
