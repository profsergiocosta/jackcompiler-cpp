
#include <iostream>

#include "JackTokenizer.h"
#include "token.h"

using namespace std;

int main()
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

    return 0;
}