#ifndef MONTY_RULE_H
#define MONTY_RULE_H

#include "ast.h"

namespace Monty {

class Rule {
    Statement * statement;

Public:
    Rule(const std::string & json);
    ostream & operator<<(ostream & out, const Rule & x);
}

}

#endif
