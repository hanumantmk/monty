#include "object.h"

std::ostream & operator<<(std::ostream & out, const Monty::Object & obj) {
    obj.print(out);

    return out;
}

std::ostream & operator<<(std::ostream & out, const Monty::Object * obj) {
    if (obj) {
        obj->print(out);
    } else {
        out << "NULL";
    }

    return out;
}
