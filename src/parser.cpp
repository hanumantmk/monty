#include "parser.h"
#include "parse_error.h"

#include <cstring>
#include <sstream>
#include <map>

#include <iostream>

using namespace Monty;

Parser::Parser()
{
}

Parser::~Parser()
{
}

typedef AST::Base * (Parser::*lookupFunPtr)(json_object *);

const std::map<std::string, lookupFunPtr> table = {
    {"binary"      , &Parser::parseBinary}      ,
    {"conditional" , &Parser::parseConditional} ,
    {"logical"     , &Parser::parseLogical}     ,
    {"lookup"      , &Parser::parseLookup}      ,
    {"production"  , &Parser::parseProduction}  ,
    {"value"       , &Parser::parseValue}       ,
};

void Parser::throwError()
{
    throwError("");
}

void Parser::throwError(const char * str)
{
    std::ostringstream out;

    out << str << " @";

    for (std::vector<ParserNode>::iterator it = path.begin(); it != path.end(); it++) {
        out << *it;
    }

    throw ParseError(out.str());
}

AST::Base * Parser::parseObject(json_object * obj)
{
    if (! obj) throwError("object is null");

    if (! json_object_is_type(obj, json_type_array)) throwError("object isn't an array");

    if (json_object_array_length(obj) != 2) throwError("object doesn't have len 2");

    json_object * jtype = json_object_array_get_idx(obj, 0);

    if (! jtype) throwError("No object at index 0 for type");

    if (! json_object_is_type(jtype, json_type_string)) throwError("type isn't a string");

    const char * type = json_object_get_string(jtype);

    json_object * data = json_object_array_get_idx(obj, 1);

    std::map<std::string, lookupFunPtr>::const_iterator it = table.find(type);

    if (it == table.end()) throwError("unknown type");

    path.push_back(ParserNode(ParserNode::Type::CLASS, type));

    AST::Base * out = (this->*(it->second))(data);

    path.pop_back();

    return out;
}

AST::Expression * Parser::parseExpression(json_object * obj)
{
    return static_cast<AST::Expression *>(parseObject(obj));
}

AST::Arg * Parser::parseArg(json_object * obj)
{
    return static_cast<AST::Arg *>(parseObject(obj));
}

AST::Statement * Parser::parseStatement(json_object * obj)
{
    return static_cast<AST::Statement *>(parseObject(obj));
}

AST::Base * Parser::parseValue(json_object * ctx)
{
    json_object * jval = json_object_object_get(ctx, "value");

    if (! jval) throwError("value");

    return new AST::Value(json_object_get_string(jval));
}

AST::Base * Parser::parseLookup(json_object * ctx)
{
    json_object * jkey = json_object_object_get(ctx, "key");

    if (! jkey) throwError("key");

    return new AST::Lookup(json_object_get_string(jkey));
}

AST::Base * Parser::parseBinary(json_object * ctx)
{
    json_object * jtype = json_object_object_get(ctx, "type");

    if (! jtype ) throwError("type");

    const char * type = json_object_get_string(jtype);

    if (! type) throwError("type");

    int ctype = -1;

    for (int i = 0; i < AST::Binary::Type::NUM_ITEMS; i++) {
        if (AST::BinaryType::names[i].compare(type) == 0) {
            ctype = i;
            break;
        }
    }

    if (ctype == -1) throwError("type");

    path.push_back(ParserNode(ParserNode::Type::ATTRIBUTE, "left"));
    std::shared_ptr<AST::Arg> left(parseArg(json_object_object_get(ctx, "left")));
    if (! left) throwError("left");
    path.pop_back();

    path.push_back(ParserNode(ParserNode::Type::ATTRIBUTE, "right"));
    std::shared_ptr<AST::Arg> right(parseArg(json_object_object_get(ctx, "right")));
    if (! right) throwError("right");
    path.pop_back();

    return new AST::Binary((enum Monty::AST::Binary::Type)ctype, left, right);
}

AST::Base * Parser::parseLogical(json_object * ctx)
{
    throwError();

    return NULL;
}

AST::Base * Parser::parseConditional(json_object * ctx)
{
    path.push_back(ParserNode(ParserNode::Type::ATTRIBUTE, "condition"));
    std::shared_ptr<AST::Expression> condition(parseExpression(json_object_object_get(ctx, "condition")));
    if (! condition) throwError("condition");
    path.pop_back();

    path.push_back(ParserNode(ParserNode::Type::ATTRIBUTE, "ifTrue"));
    std::shared_ptr<AST::Statement> ifTrue(parseStatement(json_object_object_get(ctx, "ifTrue")));
    if (! ifTrue) throwError("ifTrue");
    path.pop_back();

    path.push_back(ParserNode(ParserNode::Type::ATTRIBUTE, "ifFalse"));
    std::shared_ptr<AST::Statement> ifFalse(parseStatement(json_object_object_get(ctx, "ifFalse")));
    if (! ifFalse) throwError("ifFalse");
    path.pop_back();

    return new AST::Conditional(condition, ifTrue, ifFalse);

}

AST::Base * Parser::parseProduction(json_object * ctx)
{
    json_object * jservice = json_object_object_get(ctx, "service");
    json_object * jparams = json_object_object_get(ctx, "params");
    json_object * jpath = json_object_object_get(ctx, "path");

    if (! jservice) throwError("no service");
    if (! jparams) throwError("no params");
    if (! jpath) throwError("no path");

    if (! json_object_is_type(jservice, json_type_string)) throwError("service isn't a string");
    if (! json_object_is_type(jparams, json_type_array)) throwError("params isn't an array");
    if (! json_object_is_type(jpath, json_type_array)) throwError("path isn't an array");

    std::string service(json_object_get_string(jservice));

    std::vector<std::shared_ptr<AST::Arg>> apath;

    path.push_back(ParserNode(ParserNode::Type::ATTRIBUTE, "path"));
    for (int i = 0; i < json_object_array_length(jpath); i++) {
        path.push_back(ParserNode(ParserNode::Type::ARRAY, i));
        std::shared_ptr<AST::Arg> arg(parseArg(json_object_array_get_idx(jpath, i)));
        path.pop_back();
        apath.push_back(arg);
    }
    path.pop_back();

    std::vector<std::pair<std::string, std::shared_ptr<AST::Arg>> > params;

    path.push_back(ParserNode(ParserNode::Type::ATTRIBUTE, "params"));
    for (int i = 0; i < json_object_array_length(jparams); i++) {
        path.push_back(ParserNode(ParserNode::Type::ARRAY, i));
        json_object * jitem = json_object_array_get_idx(jparams, i);

        if (! jitem) throwError("no item at index");
        if (! json_object_is_type(jitem, json_type_array)) throwError("isn't array");
        if (json_object_array_length(jitem) != 2) throwError("isn't array len 2");

        json_object * jkey = json_object_array_get_idx(jitem, 0);
        json_object * jval = json_object_array_get_idx(jitem, 1);

        if (! json_object_is_type(jkey, json_type_string)) throwError("param keys must be strings");

        std::string key(json_object_get_string(jkey));

        std::shared_ptr<AST::Arg> arg(parseArg(jval));

        params.push_back(std::make_pair(key, arg));
        path.pop_back();
    }
    path.pop_back();

    return new AST::Production(service, apath, params);
}

AST::Base * Parser::parse(const std::string & json)
{
    json_object * root = json_tokener_parse(json.c_str());
    std::shared_ptr<json_object> root_holder(root, std::ptr_fun(&json_object_put));

    return parseObject(root);
}

