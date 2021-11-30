#ifndef _SYMBOL_
#define _SYMBOL_

#include <string>

using namespace std;

namespace sym
{

    enum Kind
    {
        STATIC,
        FIELD,
        ARG,
        VAR
    };

    struct Symbol
    {
        string name;
        string type;
        Kind kind;
        int index;
    };

};

#endif