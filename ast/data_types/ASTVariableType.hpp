//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTVARIABLETYPE_HPP
#define SIMPLEFRONTEND_ASTVARIABLETYPE_HPP

#include <llvm/IR/Type.h>

class ASTVariableType {
public:

    virtual llvm::Type *codegen() = 0;
};

#endif //SIMPLEFRONTEND_ASTVARIABLETYPE_HPP
