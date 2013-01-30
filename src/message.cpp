#include "message.h"
#include <json/json.h>

using namespace Monty;

Message::Message(const std::string & json)
{
    json_object * jobj = json_tokener_parse(json.c_str());

    if (jobj && json_object_is_type(jobj, json_type_object)) {
        json_object_object_foreach(jobj, key, value) {
            std::string v;

            if (value) {
                enum json_type type = json_object_get_type(value);
                long long int i;
                long double d;
                const char * s;

                switch(type) {
                    case json_type_boolean:
                        i = json_object_get_boolean(value);
                        v = std::to_string(i);
                        break;
                    case json_type_double:
                        d = json_object_get_double(value);
                        v = std::to_string(d);
                        break;
                    case json_type_int:
                        i = json_object_get_int(value);
                        v = std::to_string(i);
                        break;
                    case json_type_string:
                        s = json_object_get_string(value);
                        v = s;
                        break;
                    default:
                        v = std::string("");
                        break;
                }
            } else {
                v = std::string("");
            }

            map[key] = v;
        }
    }

    json_object_put(jobj);
}

std::string Message::get(const std::string & key) const
{
    std::map<std::string, std::string>::const_iterator it = map.find(key);

    if (it != map.end()) {
        std::string v(it->second);
        return v;
    } else {
        return std::string("");
    }
}

void Message::print(std::ostream & out) const
{
    out << "Message(";

    for (std::map<std::string, std::string>::const_iterator it = map.begin(); it != map.end(); it++) {
        out << it->first << " => " << it->second;

        it++;

        if (it != map.end()) {
            out << ", ";
        }

        it--;
    }

    out << ")";
}
