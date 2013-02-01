#include "ast.h"
#include "context.h"

using namespace Monty::AST;
using namespace Monty;

llvm::Value * Value::Codegen(Context & ctx)
{
    return ctx.builder.CreateGlobalStringPtr(value.c_str());
}

llvm::Value * Lookup::Codegen(Context & ctx)
{
    return NULL;
}

llvm::Value * Conditional::Codegen(Context & ctx)
{
    return NULL;
}

llvm::Value * Production::Codegen(Context & ctx)
{
    return NULL;
}

llvm::Value * Binary::Codegen(Context & ctx)
{
    return NULL;
}

llvm::Value * Logical::Codegen(Context & ctx)
{
    return NULL;
}
