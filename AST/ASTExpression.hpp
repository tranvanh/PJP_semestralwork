//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTEXPRESSION_HPP
#define SIMPLEFRONTEND_ASTEXPRESSION_HPP


#include <iostream>
#include <memory>
#include <vector>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>

class ASTExpression {
public:
    virtual ~ASTExpression() = default;

    virtual llvm::Value *codegen() = 0;
};

#endif //SIMPLEFRONTEND_ASTEXPRESSION_HPP
