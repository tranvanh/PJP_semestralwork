//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTVARIABLEDEF_HPP
#define SIMPLEFRONTEND_ASTVARIABLEDEF_HPP


#include "../ASTExpression.hpp"

class ASTVariableDef : public ASTExpression {
public:
    virtual llvm::Value *codegen() = 0;
};


#endif //SIMPLEFRONTEND_ASTVARIABLEDEF_HPP
