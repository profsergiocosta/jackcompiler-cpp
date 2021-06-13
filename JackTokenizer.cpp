#include "JackTokenizer.h"
#include <stdlib.h> /* atoi */
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

using namespace std;

template <typename T>

string to_string(T value)
{
  ostringstream os;
  os << value;
  return os.str();
}

JackTokenizer::JackTokenizer(string fname)
{
  arq.unsetf(ios_base::skipws);
  arq.open(fname.c_str());
  currentChar = 0;

  string a[] = {string("int"), string("class"), string("constructor"),
                string("function"), string("method"), string("field"),
                string("static"), string("var"), string("char"),
                string("boolean"), string("void"), string("true"),
                string("false"), string("null"), string("this"),
                string("let"), string("do"), string("if"),
                string("else"), string("while"), string("return")};

  std::set<string> s1(a, a + 21);
  keywords = s1;
}

void JackTokenizer::advance()
{
  currentToken = nextToken();
  while (int(*currentToken.rbegin()) == 0 && hasMoreTokens())
    currentToken = nextToken();
}
bool JackTokenizer::hasMoreTokens() { return !arq.eof(); }

bool JackTokenizer::isKeyword(string t)
{
  bool x = keywords.find(t) != keywords.end();
  return x;
}

bool JackTokenizer::isStringConst(string t)
{
  return (t[0] == '\"') && (t[t.length() - 1] == '\"');
}

bool JackTokenizer::isIntConst(string t) { return isdigit(*t.rbegin()); }

bool JackTokenizer::isIdentifier(string t)
{
  if (isdigit(*t.rbegin()))
    return false;

  string::const_iterator it = t.begin();

  while (it != t.end() && (isalpha(*it) || isdigit(*it) || *it == '_'))
    ++it;

  return it == t.end();
}

TokenType JackTokenizer::tokenType()
{
  if (isKeyword(currentToken))
    return tKEYWORD;
  else if (isSymbol(currentToken))
    return tSYMBOL;
  else if (isStringConst(currentToken))
    return tSTRING_CONST;
  else if (isIntConst(currentToken))
    return tINT_CONST;
  else if (isIdentifier(currentToken))
    return tIDENTIFIER;
  else
    return tUNKNOWN;
}

bool JackTokenizer::isSymbol(char t)
{
  string symbols = "{}()[].,;+-*/&|<>=~";
  return symbols.find(t) != -1;
}

bool JackTokenizer::isSymbol(string t)
{
  return isSymbol(*currentToken.rbegin());
}

Keyword JackTokenizer::keyword()
{
  if (currentToken == "class")
    return kCLASS;
  if (currentToken == "method")
    return kMETHOD;
  if (currentToken == "function")
    return kFUNCTION;
  if (currentToken == "constructor")
    return kCONSTRUCTOR;
  if (currentToken == "int")
    return kINT;
  if (currentToken == "boolean")
    return kBOOLEAN;
  if (currentToken == "char")
    return kCHAR;
  if (currentToken == "void")
    return kVOID;
  if (currentToken == "var")
    return kVAR;
  if (currentToken == "static")
    return kSTATIC;
  if (currentToken == "field")
    return kFIELD;
  if (currentToken == "let")
    return kLET;
  if (currentToken == "do")
    return kDO;
  if (currentToken == "if")
    return kIF;
  if (currentToken == "else")
    return kELSE;
  if (currentToken == "while")
    return kWHILE;
  if (currentToken == "return")
    return kRETURN;
  if (currentToken == "true")
    return kTRUE;
  if (currentToken == "false")
    return kFALSE;
  if (currentToken == "null")
    return kNULL;

  return kTHIS;
}

char JackTokenizer::symbol() { return currentToken[0]; }

string JackTokenizer::identifier() { return currentToken; }

int JackTokenizer::intVal() { return atoi(currentToken.c_str()); }

string JackTokenizer::stringVal()
{
  string s = currentToken;
  s.erase(s.length() - 1, 1);
  s.erase(0, 1);
  return s;
}

char JackTokenizer::nextChar()
{
  if (!arq.eof())
  {
    arq >> currentChar;
  }
  else
    currentChar = 0;
  return currentChar;
}

string JackTokenizer::nextToken()
{
  string s = "";
  while (hasMoreTokens())
  {
    s = "";
    switch (currentChar)
    {
    case '/':
      s = s + currentChar;
      nextChar();
      if (currentChar == '/')
      {
        ignoreComments("//");
      }
      else if (currentChar == '*')
      {
        ignoreComments("/*");
      }
      else
      {
        return s;
      }
      break;
    case ' ':
    case '\t':
    case '\n':
    case '\r':
      nextChar();
      break;

    default:
      s = concatToken(currentChar);
      return s;
      break;
    }
  }

  return "error token";
}

string JackTokenizer::concatToken(char ch)
{
  string s = "";
  s = s + ch;
  if (isSymbol(ch))
  {
    nextChar();
    return s;
  }

  if (ch == '"')
  {
    while (!arq.eof() && (ch = nextChar()) && ch != '\"' && ch != '\n' &&
           ch != '\r' && !isSymbol(ch))
      s = s + ch;

    s = s + '\"';
    nextChar();
  }
  else
  {
    while (!arq.eof() && (ch = nextChar()) != ' ' && ch != '\n' && ch != '\r' &&
           !isSymbol(ch))
      s = s + ch;
  }
  return s;
}

void JackTokenizer::ignoreComments(const char *t)
{
  int endComment = 0;
  char look = 0;
  if (strcmp(t, "//") == 0)
    while (look != '\n')
      look = nextChar();
  else
  {
    while (!endComment)
    {
      look = nextChar();
      if (look == '*')
      {
        while (look == '*')
          look = nextChar();
        if (look == '/')
        {
          endComment = 1;
          look = nextChar();
        }
      }
    }
  }
}

string JackTokenizer::tagToken()
{
  switch (tokenType())
  {
  case tKEYWORD:
    return "<keyword> " + currentToken + " </keyword>";
    break;

  case tSYMBOL:
    if (symbol() == '<')
      return "<symbol> &lt; </symbol>";
    else if (symbol() == '>')
      return "<symbol> &gt; </symbol>";
    else if (symbol() == '\"')
      return "<symbol> &quot; </symbol>";
    else if (symbol() == '&')
      return "<symbol> &amp; </symbol>";
    else
    {
      string sym(1, symbol());
      return "<symbol> " + sym + " </symbol>";
    }
    break;

  case tIDENTIFIER:
    return "<identifier> " + identifier() + " </identifier>";
    break;

  case tINT_CONST:
    return "<integerConstant> " + to_string(intVal()) + " </integerConstant>";
    break;

  case tSTRING_CONST:
    return "<stringConstant> " + stringVal() + " </stringConstant>";
    break;

  default:
    return "<ERROR> " + currentToken + " </ERROR>";
    break;
  }
}
