#ifndef MONTY_MESSAGE_H
#define MONTY_MESSAGE_H

#include <map>
#include <ostream>

#include "object.h"

namespace Monty {

class Message: public Object {
    std::map<std::string, std::string> map;

public:
    Message(const std::string & json);

    std::string get(const std::string & key) const;
    virtual void print(std::ostream & out) const;
};

}

#endif
