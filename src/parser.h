#ifndef MONTY_PARSER_H
#define MONTY_PARSER_H

#include <vector>
#include <json/json.h>
#include "ast.h"
#include <iostream>

namespace Monty {

class ParserNode: public Object {
public:
    enum Type {
        CLASS,
        ATTRIBUTE,
        ARRAY,
    };

private:
    enum Type type;
    const char * str;
    int index;

public:
    ParserNode(ParserNode::Type t, const char * s) : type(t), str(s) {}
    ParserNode(ParserNode::Type t, int i) : type(t), index(i) {}

    virtual void print(std::ostream & out) const
    {
        switch (type) {
            case Type::CLASS:
                out << "<" << str << ">.";
                break;
            case Type::ATTRIBUTE:
                out << str;
                break;
            case Type::ARRAY:
                out << "[" << index << "]";
                break;
        }
    }
};

class Parser {
    std::vector<ParserNode> path;

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

private:
    void throwError();
    void throwError(const char * str);
};

}

#endif
