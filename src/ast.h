#ifndef MONTY_AST_H
#define MONTY_AST_H

#include <string>
#include <sstream>
#include <vector>
#include <cstdlib>
#include <memory>

#include "context.h"
#include "message.h"
#include "object.h"

#include <assert.h>

namespace Monty {
namespace AST {

class Base: public Object {

public:
    virtual ~Base() {}

    virtual llvm::Value * Codegen(Context & ctx) = 0;
};

class Statement: public Base {

public:
    virtual ~Statement() {}

    virtual std::string exec(const Message & msg) = 0;
};

class Expression: public Base {

public:
    virtual ~Expression() {}

    virtual bool eval(const Message & msg) = 0;
};

class Arg: public Base {

public:
    virtual ~Arg() {}

    virtual std::string getValue(const Message & msg) = 0;
};

class Value: public Arg {

public:
    std::string value;
    Value(const std::string & s) : value(s) {}

    virtual std::string getValue(const Message & msg)
    {
        return value;
    }

    virtual void print(std::ostream & out) const
    {
        out << "VALUE(" << value << ")";
    }

    virtual llvm::Value * Codegen(Context & ctx);
};

class Lookup: public Arg {
    std::string key;

public:
    Lookup(const std::string & s) : key(s) { }

    virtual std::string getValue(const Message & msg)
    {
        return msg.get(key);
    }

    virtual void print(std::ostream & out) const
    {
        out << "LOOKUP(" << key << ")";
    }

    virtual llvm::Value * Codegen(Context & ctx);
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
    std::shared_ptr<Arg> left;
    std::shared_ptr<Arg> right;

public:
    Binary(Binary::Type t, std::shared_ptr<Arg> left, std::shared_ptr<Arg> right) : type(t), left(left), right(right) { }
    
    virtual bool eval(const Message & msg)
    {
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
        out << "Binary<" << BinaryType::names[type] << ">(" << *left << ", " << *right << ")";
    }

    virtual llvm::Value * Codegen(Context & ctx);
};

class Conditional : public Statement {
    std::shared_ptr<Expression> condition;
    std::shared_ptr<Statement> ifTrue;
    std::shared_ptr<Statement> ifFalse;

public:
    Conditional(std::shared_ptr<Expression> e, std::shared_ptr<Statement> ifTrue, std::shared_ptr<Statement> ifFalse) : condition(e), ifTrue(ifTrue), ifFalse(ifFalse) { }

    virtual std::string exec(const Message & msg)
    {
        if (condition->eval(msg)) {
            return ifTrue->exec(msg);
        } else {
            return ifFalse->exec(msg);
        }
    }

    virtual void print(std::ostream & out) const
    {
        out << "Conditional(" << condition << ", " << *ifTrue << ", " << *ifFalse << ")";
    }

    virtual llvm::Value * Codegen(Context & ctx);
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
    std::vector<std::shared_ptr<Expression> > clauses;

public:
    Logical(Logical::Type t, std::vector<std::shared_ptr<Expression> > & c) : type(t), clauses(c) { }

    virtual bool eval(const Message & msg)
    {
        for (std::vector<std::shared_ptr<Expression> >::iterator it = clauses.begin(); it != clauses.end(); it++) {
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

        for (std::vector<std::shared_ptr<Expression> >::const_iterator it = clauses.begin(); it != clauses.end(); it++) {
            out << (**it);

            if (it + 1 != clauses.end()) {
                out << ", ";
            }
        }

        out << ")";
    }

    virtual llvm::Value * Codegen(Context & ctx);
};

class Production: public Statement {
    std::string service;
    std::vector<std::shared_ptr<Arg> > path;
    std::vector<std::pair<std::string, std::shared_ptr<Arg> > > params;

public:
    Production(const std::string & service, const std::vector<std::shared_ptr<Arg> > & path, const std::vector<std::pair<std::string, std::shared_ptr<Arg> > > & params) : service(service), path(path), params(params) { }

    virtual std::string exec(const Message & msg)
    {
        std::ostringstream out;

        out << service;

        if (path.size()) {
            for (std::vector<std::shared_ptr<Arg> >::iterator it = path.begin(); it != path.end(); it++) {
                out << "/" << (**it).getValue(msg);
            }
        }

        if (params.size()) {
            out << "?";

            for (std::vector<std::pair<std::string, std::shared_ptr<Arg> > >::iterator it = params.begin(); it != params.end(); it++) {
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

        for (std::vector<std::shared_ptr<Arg> >::const_iterator it = path.begin(); it != path.end(); it++) {
            out << **it;

            if (it + 1 != path.end()) {
                out << ", ";
            }
        }

        out << "), PARAMS(";

        for (std::vector<std::pair<std::string, std::shared_ptr<Arg> > >::const_iterator it = params.begin(); it != params.end(); it++) {
            out << it->first << "=" << *(it->second);

            if (it + 1 != params.end()) {
                out << ", ";
            }
        }

        out << ")";
    }

    virtual llvm::Value * Codegen(Context & ctx);
};

}
}

#endif
