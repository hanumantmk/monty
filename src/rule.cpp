#include "rule.h"
#include "parser.h"

using namespace Monty;

Rule::Rule(const std::string & json)
{
    Parser p;
    AST::Base * obj = p.parse(json);

    statement = static_cast<AST::Statement *>(obj);
}

std::string Rule::exec(const Message & msg)
{
    return statement->exec(msg);
}

void Rule::print(std::ostream & out) const
{
    out << "Rule(" << *statement << ")";
}
