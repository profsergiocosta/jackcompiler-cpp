#include "vmwriter.h"
#include <fstream>
#include <iostream>
#include <string>
#include <map>
using namespace std;

std::map<Segment, std::string>
    segments = {
        {CONST, "constant"},
        {ARG, "argument"},
        {LOCAL, "local"},
        {THIS, "this"},
        {THAT, "that"},
        {POINTER, "pointer"},
        {STATIC, "static"},
        {TEMP, "temp"},

};

std::map<Command, std::string>
    commands = {
        {ADD, "add"},
        {SUB, "sub"},
        {NEG, "neg"},
        {EQ, "eq"},
        {GT, "gt"},
        {LT, "lt"},
        {AND, "and"},
        {OR, "or"},
        {NOT, "not"},

};

VMWriter::VMWriter(string fileName) { out.open(fileName.c_str()); }

void VMWriter::close(void)
{
  if (out.is_open())
    out.close();
}

void VMWriter::writePush(Segment segment, int index)
{
  out << "push " << segments[segment] << " " << index << endl;
}

void VMWriter::writePop(Segment segment, int index)
{
  out << "pop " << segments[segment] << " " << index << endl;
}

void VMWriter::writeArithmetic(Command command)
{
  out << commands[command] << endl;
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