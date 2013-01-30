#include "object.h"

std::ostream & operator<<(std::ostream & out, const Monty::Object & obj) {
    obj.print(out);

    return out;
}

