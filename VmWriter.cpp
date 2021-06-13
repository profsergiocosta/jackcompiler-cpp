#include "VmWriter.h"
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

string segment2String(Segment seg)
{
  switch (seg)
  {
  case CONST:
    return "constant";
  case ARG:
    return "argument";
  case LOCAL:
    return "local";
  case STATIC:
    return "static";
  case THIS:
    return "this";
  case THAT:
    return "that";
  case POINTER:
    return "pointer";
  case TEMP:
    return "temp";
  default:
    return "NONE";
  }
}

string command2String(Command com)
{
  switch (com)
  {
  case ADD:
    return "add";
  case SUB:
    return "sub";
  case NEG:
    return "neg";
  case EQ:
    return "eq";
  case GT:
    return "gt";
  case LT:
    return "lt";
  case AND:
    return "and";
  case OR:
    return "or";
  case NOT:
    return "not";
  default:
    return "NONE";
  }
}

VMWriter::VMWriter(string fileName) { out.open(fileName.c_str()); }

void VMWriter::close(void)
{
  if (out.is_open())
    out.close();
}

void VMWriter::writePush(Segment segment, int index)
{
  out << "push " << segment2String(segment) << " " << index << endl;
}

void VMWriter::writePop(Segment segment, int index)
{
  out << "pop " << segment2String(segment) << " " << index << endl;
}

void VMWriter::writeArithmetic(Command command)
{
  out << command2String(command) << endl;
}

void VMWriter::writeLabel(string label) { out << "label " << label << endl; }
void VMWriter::writeGoto(string label) { out << "goto " << label << endl; }

void VMWriter::writeIf(string label) { out << "if-goto " << label << endl; }

void VMWriter::writeCall(string name, int nArgs)
{
  out << "call " << name << " " << nArgs << endl;
}

void VMWriter::writeFunction(string name, int nLocals)
{
  out << "function " << name << " " << nLocals << endl;
}

void VMWriter::writeReturn(void) { out << "return" << endl; }