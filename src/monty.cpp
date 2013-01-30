#include "ast.h"

#include <iostream>

using namespace Monty::AST;
using namespace std;

int main(int argc, char ** argv)
{
    string sfoo("foo");
    Value foo(sfoo);

    cout << foo << endl;
    return 0;
}
