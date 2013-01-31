#ifndef MONTY_PARSEERROR_H
#define MONTY_PARSEERROR_H

#include "object.h"
#include <json/json.h>

namespace Monty {

class ParseError: public Object
{
    json_object * obj;
    const char * str;

public:

    ParseError(json_object * o, const char * s): obj(o), str(s) {}

    virtual void print(std::ostream & stream) const {
        stream << "parseError(" << str << ", " << json_object_to_json_string(obj) << ")";
    };
};

}

#endif
