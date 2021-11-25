
#include <iostream>

#include "jacktokenizer.h"
#include "compilationengine.h"

#include "token.h"

using namespace std;

void printXML()
{
    JackTokenizer *tkz = new JackTokenizer("Main.jack");

    cout << "<tokens>" << endl;
    while (tkz->hasMoreTokens())
    {
        Token tk = tkz->nextToken();
        if (tk.type != TOKEN_EOF)
            cout << tagToken(tk) << endl;
    }
    cout << "</tokens>" << endl;
}

int main()
{
    CompilationEngine *compiler = new CompilationEngine("Main.jack");
    compiler->compile();

    return 0;
}