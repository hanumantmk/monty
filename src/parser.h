#ifndef MONTY_PARSER_H
#define MONTY_PARSER_H

#include <stack>
#include <json/json.h>
#include "ast.h"

namespace Monty {

class Parser {
    std::stack<std::string> path;

public:
    Parser();
    ~Parser();

    AST::Base * parse(const std::string & json);
    AST::Base * parseValue(json_object * ctx);
    AST::Base * parseLookup(json_object * ctx);
    AST::Base * parseBinary(json_object * ctx);
    AST::Base * parseLogical(json_object * ctx);
    AST::Base * parseConditional(json_object * ctx);
    AST::Base * parseProduction(json_object * ctx);
    AST::Base * parseObject(json_object * obj);
    AST::Expression * parseExpression(json_object * obj);
    AST::Arg * parseArg(json_object * obj);
    AST::Statement * parseStatement(json_object * obj);
};

}

#endif
