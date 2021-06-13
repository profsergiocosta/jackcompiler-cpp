#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include <fstream>
#include <set>
#include <string>

using namespace std;

enum TokenType
{
  tKEYWORD,      /**< a keyword */
  tSYMBOL,       /**< a symbol */
  tIDENTIFIER,   /**< an identifier */
  tINT_CONST,    /**< an integer constante */
  tSTRING_CONST, /**< a string constant */
  tUNKNOWN       /**< unknown type */
};

enum Keyword
{
  kCLASS,       /**< class */
  kMETHOD,      /**< method */
  kFUNCTION,    /**< function */
  kCONSTRUCTOR, /**< constructor */
  kINT,         /**< int */
  kBOOLEAN,     /**< boolean */
  kCHAR,        /**< char */
  kVOID,        /**< void */
  kVAR,         /**< var */
  kSTATIC,      /**< static */
  kFIELD,       /**< field */
  kLET,         /**< let */
  kDO,          /**< do */
  kIF,          /**< if */
  kELSE,        /**< else */
  kWHILE,       /**< while */
  kRETURN,      /**< return */
  kTRUE,        /**< true */
  kFALSE,       /**< false */
  kNULL,        /**< null */
  kTHIS,        /**< this */
  kUNKNOWN      /**< unknow */
};

class JackTokenizer
{
public:
  JackTokenizer(string fname);
  void advance();
  bool hasMoreTokens();

  TokenType tokenType();

  Keyword keyword();

  char symbol();
  string identifier();

  int intVal();

  string stringVal();

  bool isSymbol(char t);
  bool isSymbol(string t);
  bool isKeyword(string t);
  bool isStringConst(string t);
  bool isIntConst(string t);
  bool isIdentifier(string t);

  string tagToken();

  ifstream arq;
  string::iterator it;
  string currentToken;
  char currentChar;

  char nextChar();
  void ignoreComments(const char *t);
  string nextToken();
  string concatToken(char ch);

  std::set<string> keywords;
};

#endif // JACK_TOKENIZER_H