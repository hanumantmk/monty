#ifndef MONTY_AST_H
#define MONTY_AST_H

#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>

#include "message.h"
#include "object.h"

#include <assert.h>

namespace Monty {
namespace AST {

class Base: public Object {

protected:
    bool complete;

public:
    bool isComplete()
    {
        return complete;
    }
};

class Statement: public Base {

public:
    virtual ~Statement() {}

    virtual std::string exec(const Message & msg) {return std::string("");}
};

class Arg: public Base {

public:
    virtual ~Arg() {}

    virtual std::string getValue(const Message & msg) {return std::string("");}
};

class Value: public Arg {

public:
    std::string value;
    Value(const std::string & s) : value(s)
    {
        complete = true;
    };

    virtual std::string getValue(const Message & msg)
    {
        return value;
    }

    virtual void print(std::ostream & out) const
    {
        out << "VALUE(" << value << ")";
    }
};

class Lookup: public Arg {
    std::string key;

public:
    Lookup(const std::string & s) : key(s)
    {
        complete = true;
    }

    virtual std::string getValue(const Message & msg)
    {
        return msg.get(key);
    }

    virtual void print(std::ostream & out) const
    {
        out << "LOOKUP(" << key << ")";
    }

};

class Expression: public Base {

public:
    virtual ~Expression() {};

    virtual bool eval(const Message & msg) {return true;}
};

namespace BinaryType {
    extern std::string names[];
}

class Binary: public Expression {
public:
    enum Type {
        EQ,
        NE,
        LT,
        LE,
        GT,
        GE,
        SEQ,
        SNE,
        SLT,
        SLE,
        SGT,
        SGE,
        NUM_ITEMS,
    };

private:
    Binary::Type type;
    Arg * left;
    Arg * right;

public:
    Binary(Binary::Type t, Arg * left, Arg * right) : type(t), left(left), right(right)
    {
        complete = left && right;
    }

    ~Binary() {
        if (left) delete left;
        if (right) delete right;
    }
    
    virtual bool eval(const Message & msg)
    {
        if (! isComplete()) return false;

        std::string lstring = left->getValue(msg);
        std::string rstring = right->getValue(msg);
        const char * lchar = lstring.c_str();
        const char * rchar = rstring.c_str();

        switch (type) {
            case EQ:
                return std::atoi(lchar) == std::atoi(rchar);
            case NE:
                return std::atoi(lchar) != std::atoi(rchar);
            case LT:
                return std::atoi(lchar) < std::atoi(rchar);
            case LE:
                return std::atoi(lchar) <= std::atoi(rchar);
            case GT:
                return std::atoi(lchar) > std::atoi(rchar);
            case GE:
                return std::atoi(lchar) >= std::atoi(rchar);
            case SEQ:
                return lstring.compare(rstring) == 0;
            case SNE:
                return lstring.compare(rstring) != 0;
            case SLT:
                return lstring.compare(rstring) < 0;
            case SLE:
                return lstring.compare(rstring) <= 0;
            case SGT:
                return lstring.compare(rstring) > 0;
            case SGE:
                return lstring.compare(rstring) >= 0;
            default:
                return false;
        }
        assert(0);
        return false;
    }

    virtual void print(std::ostream & out) const
    {
        out << "Binary<" << BinaryType::names[type] << ">(" << left << ", " << right << ")";
    }
};

class Conditional : public Statement {
    Expression * condition;
    Statement * ifTrue;
    Statement * ifFalse;

public:
    Conditional(Expression * e, Statement * ifTrue, Statement * ifFalse) : condition(e), ifTrue(ifTrue), ifFalse(ifFalse)
    {
        complete = condition && ifTrue && ifFalse;
    }

    ~Conditional()
    {
        if (condition) delete(condition);
        if (ifTrue) delete(ifTrue);
        if (ifFalse) delete(ifFalse);
    }

    virtual std::string exec(const Message & msg)
    {
        if (! isComplete()) return std::string("");

        if (condition->eval(msg)) {
            return ifTrue->exec(msg);
        } else {
            return ifFalse->exec(msg);
        }
    }

    virtual void print(std::ostream & out) const
    {
        out << "Conditional(" << condition << ", " << ifTrue << ", " << ifFalse << ")";
    }
};

namespace LogicalType {
    extern std::string names[];
}

class Logical: public Expression {
public:
    enum Type {
        AND,
        OR,
        NUM_ITEMS,
    };

private:
    Logical::Type type;
    std::vector<Expression *> clauses;

public:
    Logical(Logical::Type t, std::vector<Expression *> & c) : type(t), clauses(c)
    {
        complete = true;
    }

    ~Logical()
    {
        for (std::vector<Expression *>::iterator it = clauses.begin(); it != clauses.end(); it++) {
            delete *it;
        }
    }
    
    virtual bool eval(const Message & msg)
    {
        for (std::vector<Expression *>::iterator it = clauses.begin(); it != clauses.end(); it++) {
            bool clauseValue = (**it).eval(msg);

            if (type == Logical::Type::AND) {
                if (! clauseValue) return false;
            } else {
                if (clauseValue) return true;
            }
        }

        return true;
    }

    virtual void print(std::ostream & out) const
    {
        out << "Logical<" << LogicalType::names[type] << ">(";

        for (std::vector<Expression *>::const_iterator it = clauses.begin(); it != clauses.end(); it++) {
            Expression * e = *it;
            out << *e;

            if (it + 1 != clauses.end()) {
                out << ", ";
            }
        }

        out << ")";
    }
};

class Production: public Statement {
    std::string service;
    std::vector<Arg *> path;
    std::vector<std::pair<std::string, Arg *> > params;

public:
    Production(const std::string & service, const std::vector<Arg *> & path, const std::vector<std::pair<std::string, Arg *> > & params) : service(service), path(path), params(params)
    {
        complete = true;
    }

    virtual std::string exec(const Message & msg)
    {
        std::ostringstream out;

        out << service;

        if (path.size()) {
            for (std::vector<Arg *>::iterator it = path.begin(); it != path.end(); it++) {
                out << "/" << (**it).getValue(msg);
            }
        }

        if (params.size()) {
            out << "?";

            for (std::vector<std::pair<std::string, Arg *> >::iterator it = params.begin(); it != params.end(); it++) {
                // TODO: add url encoding
                
                out << it->first << "=" << it->second->getValue(msg);
                if (it + 1 != params.end()) {
                    out << "&";
                }
            }
        }

        return out.str();
    }

    virtual void print(std::ostream & out) const
    {
        out << "Production(" << service << ", PATH(";

        for (std::vector<Arg *>::const_iterator it = path.begin(); it != path.end(); it++) {
            out << **it;

            if (it + 1 != path.end()) {
                out << ", ";
            }
        }

        out << "), PARAMS(";

        for (std::vector<std::pair<std::string, Arg *> >::const_iterator it = params.begin(); it != params.end(); it++) {
            out << it->first << "=" << *(it->second);

            if (it + 1 != params.end()) {
                out << ", ";
            }
        }

        out << ")";
    }
};

}
}

#endif
