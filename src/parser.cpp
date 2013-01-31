#include "parser.h"
#include "parse_error.h"

#include <cstring>
#include <map>

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

AST::Base * Parser::parseObject(json_object * obj)
{
    if (! obj) throw ParseError(obj, "");

    if (! json_object_is_type(obj, json_type_array)) throw ParseError(obj, "");

    if (json_object_array_length(obj) != 2) throw ParseError(obj, "");

    json_object * jtype = json_object_array_get_idx(obj, 0);

    if (! jtype) throw ParseError(obj, "");

    if (! json_object_is_type(jtype, json_type_string)) throw ParseError(obj, "");

    const char * type = json_object_get_string(jtype);

    json_object * data = json_object_array_get_idx(obj, 1);

    std::map<std::string, lookupFunPtr>::const_iterator it = table.find(type);

    if (it == table.end()) throw ParseError(obj, "");

    return (this->*(it->second))(data);
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

    if (! jval) throw ParseError(ctx, "");

    return new AST::Value(json_object_get_string(jval));
}

AST::Base * Parser::parseLookup(json_object * ctx)
{
    json_object * jkey = json_object_object_get(ctx, "key");

    if (! jkey) throw ParseError(ctx, "");

    return new AST::Lookup(json_object_get_string(jkey));
}

AST::Base * Parser::parseBinary(json_object * ctx)
{
    json_object * jtype = json_object_object_get(ctx, "type");

    if (! jtype ) throw ParseError(ctx, "");

    const char * type = json_object_get_string(jtype);

    if (! type) throw ParseError(ctx, "");

    int ctype = -1;

    for (int i = 0; i < AST::Binary::Type::NUM_ITEMS; i++) {
        if (AST::BinaryType::names[i].compare(type) == 0) {
            ctype = i;
            break;
        }
    }

    if (ctype == -1) throw ParseError(ctx, "");

    std::shared_ptr<AST::Arg> left(parseArg(json_object_object_get(ctx, "left")));
    std::shared_ptr<AST::Arg> right(parseArg(json_object_object_get(ctx, "right")));

    return new AST::Binary((enum Monty::AST::Binary::Type)ctype, left, right);
}

AST::Base * Parser::parseLogical(json_object * ctx)
{
    throw ParseError(ctx, "");
}

AST::Base * Parser::parseConditional(json_object * ctx)
{
    std::shared_ptr<AST::Expression> condition(parseExpression(json_object_object_get(ctx, "condition")));
    std::shared_ptr<AST::Statement> ifTrue(parseStatement(json_object_object_get(ctx, "ifTrue")));
    std::shared_ptr<AST::Statement> ifFalse(parseStatement(json_object_object_get(ctx, "ifFalse")));

    return new AST::Conditional(condition, ifTrue, ifFalse);

}

AST::Base * Parser::parseProduction(json_object * ctx)
{
    json_object * jservice = json_object_object_get(ctx, "service");
    json_object * jparams = json_object_object_get(ctx, "params");
    json_object * jpath = json_object_object_get(ctx, "path");

    if (! jservice) throw ParseError(ctx, "");
    if (! jparams) throw ParseError(ctx, "");
    if (! jpath) throw ParseError(ctx, "");

    if (! json_object_is_type(jparams, json_type_array)) throw ParseError(ctx, "");
    if (! json_object_is_type(jpath, json_type_array)) throw ParseError(ctx, "");

    std::string service(json_object_get_string(jservice));

    std::vector<std::shared_ptr<AST::Arg>> path;

    for (int i = 0; i < json_object_array_length(jpath); i++) {
        std::shared_ptr<AST::Arg> arg(parseArg(json_object_array_get_idx(jpath, i)));
        path.push_back(arg);
    }

    std::vector<std::pair<std::string, std::shared_ptr<AST::Arg>> > params;

    for (int i = 0; i < json_object_array_length(jparams); i++) {
        json_object * jitem = json_object_array_get_idx(jparams, i);

        if (! jitem) continue;
        if (! json_object_is_type(jitem, json_type_array)) continue;

        json_object * jkey = json_object_array_get_idx(jitem, 0);
        json_object * jval = json_object_array_get_idx(jitem, 1);

        std::string key(json_object_get_string(jkey));

        std::shared_ptr<AST::Arg> arg(parseArg(jval));

        params.push_back(std::make_pair(key, arg));
    }

    return new AST::Production(service, path, params);
}

AST::Base * Parser::parse(const std::string & json)
{
    json_object * root = json_tokener_parse(json.c_str());
    std::shared_ptr<json_object> root_holder(root, std::ptr_fun(&json_object_put));

    return parseObject(root);
}

