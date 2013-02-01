#ifndef MONTY_CONTEXT_H
#define MONTY_CONTEXT_H

#include <llvm/LLVMContext.h>
#include <llvm/DerivedTypes.h>
#include <llvm/Module.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/Support/IRBuilder.h>

#include <map>
#include <string>

namespace Monty {

class Context {
public:
    llvm::LLVMContext ctx;
    llvm::IRBuilder<> builder;
    std::map<std::string, llvm::Value *> values;

    Context(): ctx(), builder(ctx), values() {}
};

}

#endif
