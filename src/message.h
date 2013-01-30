#ifndef MONTY_MESSAGE_H
#define MONTY_MESSAGE_H

#include <map>
#include <ostream>

namespace Monty {

class Message {
    std::map<std::string, std::string> map;

public:
    Message(const std::string & json);

    std::string get(const std::string & key) const;

};

}

#endif
