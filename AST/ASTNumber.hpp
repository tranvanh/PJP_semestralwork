//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTNUMBER_HPP
#define SIMPLEFRONTEND_ASTNUMBER_HPP


#include "ASTExpression.hpp"

class ASTNumber : public ASTExpression {
public:
    ASTNumber(int val);

    llvm::Value *codegen() override;

    int value;
};


#endif //SIMPLEFRONTEND_ASTNUMBER_HPP
