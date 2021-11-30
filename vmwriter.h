#ifndef _VMWRITER_
#define _VMWRITER_

#include <fstream>
#include <string>
using namespace std;

enum Segment
{
  CONST,
  ARG,
  LOCAL,
  STATIC,
  THIS,
  THAT,
  POINTER,
  TEMP
};

enum Command
{
  ADD,
  SUB,
  NEG,
  EQ,
  GT,
  LT,
  AND,
  OR,
  NOT
};

class VMWriter
{
public:
  // Constructor
  VMWriter(string fileName); // Creates a new file and prepares it for writing.

  // Methods
  void writePush(Segment segment, int index);   // Writes a VM push command
  void writePop(Segment segment, int index);    // Writes a VM pop command
  void writeArithmetic(Command command);        // Writes a VM arithmetic command
  void writeLabel(string label);                // Writes a VM label comand
  void writeGoto(string label);                 // Writes a VM goto command
  void writeIf(string label);                   // Writes a VM if-goto command
  void writeCall(string name, int nArgs);       // Writes a VM call command
  void writeFunction(string name, int nLocals); // Writes a VM function command
  void writeReturn(void);                       // Writes a VM return command
  void close(void);                             // Closes the output file

private:
  ofstream out;
};

#endif