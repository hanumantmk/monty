#include "ast.h"
#include "message.h"

#include <iostream>

using namespace Monty;
using namespace std;

int main(int argc, char ** argv)
{
    string sfoo("foo");
    AST::Value foo(sfoo);

    cout << foo << endl;

    Message msg("{\"foo\" : \"bar\", \"baz\" : 12 }");

    cout << msg << endl;
    return 0;
}
