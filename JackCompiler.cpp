
#include <iostream>
#include "CompilationEngine.h"
#include "JackTokenizer.h"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <vector>

const char *tokenTypeStr[] = {"tKEYWORD", "tSYMBOL", "tIDENTIFIER",
                              "tINT_CONST", "tSTRING_CONST", "tUNKNOWN"};

bool isFile(string in)
{
  struct stat buf;
  stat(in.c_str(), &buf);
  return S_ISREG(buf.st_mode);
}

bool isDir(string in)
{
  struct stat buf;
  stat(in.c_str(), &buf);
  return S_ISDIR(buf.st_mode);
}

bool isJackFile(string in)
{
  return in.rfind(".jack") != string::npos ? true : false;
}

int main(int argc, char **argv)
{
  // le o diretorio
  vector<string> files; /**< Vector with VM file names. */
  string path;

  string input = argv[1];
  string output;

  if (isDir(input))
  {
    cout << "dir " << endl;
    if (*input.rbegin() == '/')
      path = input;
    else
      path = input + "/";

    DIR *dp;
    struct dirent *dirp;
    if ((dp = opendir(input.c_str())) == NULL)
    {
      cerr << "Error opening " << input << endl;
    }

    while ((dirp = readdir(dp)) != NULL)
    {
      string str(dirp->d_name);
      if (str == "." || str == "..")
        continue;
      if (isJackFile(str))
        files.push_back(string(path + str));
    }
    closedir(dp);
  }
  else
  {
    cout << "file" << endl;
    files.push_back(input);
  }

  for (int i = 0; i < files.size(); i++)
  {
    input = files[i];
    output = input.substr(0, input.find_last_of(".")) + ".vm";
    cout << files[i].c_str() << "--" << output << endl;

    CompilationEngine *engine = new CompilationEngine(input, output);
    engine->compile();
  }
}
