#ifndef MONTY_PARSEERROR_H
#define MONTY_PARSEERROR_H

#include "object.h"
#include <json/json.h>
#include <string>

namespace Monty {

class ParseError: public Object
{
    std::string str;

public:
    ParseError(std::string s): str(s) {}

    virtual void print(std::ostream & stream) const {
        stream << "parseError(" << str << ")";
    };
};

}

#endif
