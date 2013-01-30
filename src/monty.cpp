#include "ast.h"
#include "message.h"
#include "rule.h"

#include <iostream>

using namespace Monty;
using namespace std;

int main(int argc, char ** argv)
{
    string sfoo("foo");
    AST::Value foo(sfoo);

    cout << foo << endl;

    Message msg("{\"foo\" : 10, \"bar\" : \"baz\" }");
    Message msg2("{\"foo\" : 11, \"bar\" : \"baz\" }");

    cout << msg << endl;

    string srule(
        "[\"conditional\", {"\
            "\"condition\" : [\"binary\", {"\
                "\"type\" : \"EQ\","\
                "\"left\" : [\"value\", {"\
                    "\"value\" : 10"\
                "}],"\
                "\"right\" : [\"lookup\", {"\
                    "\"key\" : \"foo\""\
                "}]"\
            "}],"\
            "\"ifTrue\" : [\"production\", {"\
                "\"service\" : \"bar\","\
                "\"path\" : ["\
                    "[\"value\", {"\
                        "\"value\" : \"baz\""\
                    "}]"\
                "],"\
                "\"params\" : ["\
                    "["\
                        "\"val\","
                        "[\"lookup\", {"\
                            "\"key\" : \"bar\""\
                        "}]"\
                    "]"\
                "]"\
            "}],"\
            "\"ifFalse\" : [\"production\", {"\
                "\"service\" : \"bar\","\
                "\"path\" : ["\
                    "[\"value\", {"\
                        "\"value\" : \"bop\""\
                    "}]"\
                "],"\
                "\"params\" : []"\
            "}]"\
        "}]"\
    );

    cout << srule << endl;

    Rule rule(srule);

    cout << rule << endl;

    cout << rule.exec(msg) << endl;
    cout << rule.exec(msg2) << endl;

    string sbrokenrule(
        "[\"conditional\", {"\
            "\"condition\" : [\"binary\", {"\
                "\"type\" : \"EQ\","\
                "\"left\" : [\"value\", {"\
                    "\"value\" : 10"\
                "}],"\
                "\"right\" : [\"lookup\", {"\
                    "\"key\" : \"foo\""\
                "}]"\
            "}]"\
        "}]"\
    );

    Rule brokenrule(sbrokenrule);

    cout << brokenrule << endl;
                    
    return 0;
}
