#include "rule.h"
#include <cstring>
#include "ast.h"
#include <json/json.h>

using namespace Monty;

AST::Statement * parseJson(const std::string & json);
AST::Statement * parseStatement(json_object * ctx);
AST::Expression * parseExpression(json_object * ctx);
AST::Arg * parseArg(json_object * ctx);
AST::Arg * parseValue(json_object * ctx);
AST::Arg * parseLookup(json_object * ctx);
AST::Expression * parseBinary(json_object * ctx);
AST::Expression * parseLogical(json_object * ctx);
AST::Statement * parseConditional(json_object * ctx);
AST::Statement * parseProduction(json_object * ctx);

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

bool parseObject(json_object * obj, json_object ** ele, const char ** type)
{
    if (! obj) return false;

    if (! json_object_is_type(obj, json_type_array)) return false;

    if (json_object_array_length(obj) != 2) return false;

    json_object * jtype = json_object_array_get_idx(obj, 0);

    if (! jtype) return false;

    if (! json_object_is_type(jtype, json_type_string)) return false;

    *type = json_object_get_string(jtype);

    *ele = json_object_array_get_idx(obj, 1);

    return true;
}

AST::Statement * parseStatement(json_object * ctx)
{
    const char * type;
    json_object * obj;

    if (! parseObject(ctx, &obj, &type)) return NULL;

    if (std::strcmp(type, "conditional") == 0) {
        return parseConditional(obj);
    } else if (std::strcmp(type, "production") == 0) {
        return parseProduction(obj);
    } else {
        return NULL;
    }
}

AST::Expression * parseExpression(json_object * ctx)
{
    const char * type;
    json_object * obj;

    if (! parseObject(ctx, &obj, &type)) return NULL;

    if (std::strcmp(type, "binary") == 0) {
        return parseBinary(obj);
    } else if (std::strcmp(type, "logical") == 0) {
        return parseLogical(obj);
    } else {
        return NULL;
    }
}

AST::Arg * parseArg(json_object * ctx)
{
    const char * type;
    json_object * obj;

    if (! parseObject(ctx, &obj, &type)) return NULL;

    if (std::strcmp(type, "value") == 0) {
        return parseValue(obj);
    } else if (std::strcmp(type, "lookup") == 0) {
        return parseLookup(obj);
    } else {
        return NULL;
    }
}

AST::Arg * parseValue(json_object * ctx)
{
    json_object * jval = json_object_object_get(ctx, "value");

    if (! jval) return NULL;

    return new AST::Value(json_object_get_string(jval));
}

AST::Arg * parseLookup(json_object * ctx)
{
    json_object * jkey = json_object_object_get(ctx, "key");

    if (! jkey) return NULL;

    return new AST::Lookup(json_object_get_string(jkey));
}

AST::Expression * parseBinary(json_object * ctx)
{
    json_object * jtype = json_object_object_get(ctx, "type");

    if (! jtype ) return NULL;

    const char * type = json_object_get_string(jtype);

    if (! type) return NULL;

    int ctype = -1;

    for (int i = 0; i < AST::Binary::Type::NUM_ITEMS; i++) {
        if (AST::BinaryType::names[i].compare(type) == 0) {
            ctype = i;
            break;
        }
    }

    if (ctype == -1) return NULL;

    return new AST::Binary((enum Monty::AST::Binary::Type)ctype, parseArg(json_object_object_get(ctx, "left")), parseArg(json_object_object_get(ctx, "right")));
}

AST::Expression * parseLogical(json_object * ctx)
{
    return NULL;
}

AST::Statement * parseConditional(json_object * ctx)
{
    return new AST::Conditional(parseExpression(json_object_object_get(ctx, "condition")), parseStatement(json_object_object_get(ctx, "ifTrue")), parseStatement(json_object_object_get(ctx, "ifFalse")));

}

AST::Statement * parseProduction(json_object * ctx)
{
    json_object * jservice = json_object_object_get(ctx, "service");
    json_object * jparams = json_object_object_get(ctx, "params");
    json_object * jpath = json_object_object_get(ctx, "path");

    if (! jservice) return NULL;
    if (! jparams) return NULL;
    if (! jpath) return NULL;

    if (! json_object_is_type(jparams, json_type_array)) return NULL;
    if (! json_object_is_type(jpath, json_type_array)) return NULL;

    std::string service(json_object_get_string(jservice));

    std::vector<AST::Arg *> path;

    for (int i = 0; i < json_object_array_length(jpath); i++) {
        path.push_back(parseArg(json_object_array_get_idx(jpath, i)));
    }

    std::vector<std::pair<std::string, AST::Arg *> > params;

    for (int i = 0; i < json_object_array_length(jparams); i++) {
        json_object * jitem = json_object_array_get_idx(jparams, i);

        if (! jitem) continue;
        if (! json_object_is_type(jitem, json_type_array)) continue;

        json_object * jkey = json_object_array_get_idx(jitem, 0);
        json_object * jval = json_object_array_get_idx(jitem, 1);

        std::string key(json_object_get_string(jkey));

        params.push_back(std::make_pair(key, parseArg(jval)));
    }

    return new AST::Production(service, path, params);
}

void Rule::print(std::ostream & out) const
{
    out << "Rule(" << *statement << ")";
}
