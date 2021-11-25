
#include <iostream>

#include "JackTokenizer.h"

using namespace std;

int main()
{

    JackTokenizer *tkz = new JackTokenizer("teste.jack");
    char c;
    while ((c = tkz->nextToken()) != '\0')
    {
        cout << c << endl;
    }

    return 0;
}