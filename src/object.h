#ifndef MONTY_OBJECT_H
#define MONTY_OBJECT_H

#include <ostream>

namespace Monty {

class Object {

public:
    virtual void print(std::ostream & stream) const {};

//    friend std::ostream & operator<<(std::ostream & out, const Object & obj);
};

};

std::ostream & operator<<(std::ostream & out, const Monty::Object & obj);

#endif
