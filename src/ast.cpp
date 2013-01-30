#include "ast.h"

using namespace Monty::AST;

std::string BinaryType::names[] = {
    "EQ",
    "NE",
    "LT",
    "LE",
    "GT",
    "GE",
    "SEQ",
    "SNE",
    "SLT",
    "SLE",
    "SGT",
    "SGE",
};

std::string LogicalType::names[] = {
    "AND",
    "OR",
};
