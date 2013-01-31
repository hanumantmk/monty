#include "rule.h"
#include <cstring>
#include <map>
#include "ast.h"
#include "parse_error.h"
#include <json/json.h>

using namespace Monty;

AST::Statement * parseJson(const std::string & json);
AST::Statement * parseStatement(json_object * ctx);
AST::Expression * parseExpression(json_object * ctx);
AST::Arg * parseArg(json_object * ctx);
AST::Base * parseValue(json_object * ctx);
AST::Base * parseLookup(json_object * ctx);
AST::Base * parseBinary(json_object * ctx);
AST::Base * parseLogical(json_object * ctx);
AST::Base * parseConditional(json_object * ctx);
AST::Base * parseProduction(json_object * ctx);

Rule::Rule(const std::string & json)
{
    statement = parseJson(json);
}

std::string Rule::exec(const Message & msg)
{
    return statement->exec(msg);
}

AST::Statement * parseJson(const std::string & json)
{
    json_object * jobj = json_tokener_parse(json.c_str());

    AST::Statement * statement = parseStatement(jobj);

    json_object_put(jobj);

    return statement;
}

typedef AST::Base * (* lookupFunPtr)(json_object *);

const std::map<std::string, lookupFunPtr> table = {
    {"conditional", &parseConditional},
    {"production", &parseProduction},
    {"binary", &parseBinary},
    {"logical", &parseLogical},
    {"value", &parseValue},
    {"lookup", &parseLookup},
};

AST::Base * parseObject(json_object * obj)
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

    return it->second(data);
}

AST::Expression * parseExpression(json_object * obj)
{
    return static_cast<AST::Expression *>(parseObject(obj));
}

AST::Arg * parseArg(json_object * obj)
{
    return static_cast<AST::Arg *>(parseObject(obj));
}

AST::Statement * parseStatement(json_object * obj)
{
    return static_cast<AST::Statement *>(parseObject(obj));
}

AST::Base * parseValue(json_object * ctx)
{
    json_object * jval = json_object_object_get(ctx, "value");

    if (! jval) throw ParseError(ctx, "");

    return new AST::Value(json_object_get_string(jval));
}

AST::Base * parseLookup(json_object * ctx)
{
    json_object * jkey = json_object_object_get(ctx, "key");

    if (! jkey) throw ParseError(ctx, "");

    return new AST::Lookup(json_object_get_string(jkey));
}

AST::Base * parseBinary(json_object * ctx)
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

AST::Base * parseLogical(json_object * ctx)
{
    throw ParseError(ctx, "");
}

AST::Base * parseConditional(json_object * ctx)
{
    std::shared_ptr<AST::Expression> condition(parseExpression(json_object_object_get(ctx, "condition")));
    std::shared_ptr<AST::Statement> ifTrue(parseStatement(json_object_object_get(ctx, "ifTrue")));
    std::shared_ptr<AST::Statement> ifFalse(parseStatement(json_object_object_get(ctx, "ifFalse")));

    return new AST::Conditional(condition, ifTrue, ifFalse);

}

AST::Base * parseProduction(json_object * ctx)
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

void Rule::print(std::ostream & out) const
{
    out << "Rule(" << *statement << ")";
}
