//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTREFERENCE_HPP
#define SIMPLEFRONTEND_ASTREFERENCE_HPP


#include "../ASTExpression.hpp"

class ASTReference : public ASTExpression {
public:
    ASTReference(const std::string &name);

    virtual llvm::Value *codegen() = 0;

    virtual llvm::Value *getAlloc() = 0;

    const std::string m_Name;
};


#endif //SIMPLEFRONTEND_ASTREFERENCE_HPP
