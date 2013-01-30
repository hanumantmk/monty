#ifndef MONTY_RULE_H
#define MONTY_RULE_H

#include <string>

#include "ast.h"
#include "object.h"

namespace Monty {

class Rule: public Object {
    AST::Statement * statement;

public:
    Rule(const std::string & json);
    virtual void print(std::ostream & stream) const;
    std::string exec(const Message & msg);
};

}

#endif
