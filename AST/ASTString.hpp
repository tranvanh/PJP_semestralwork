//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTSTRING_HPP
#define SIMPLEFRONTEND_ASTSTRING_HPP

#include "ASTExpression.hpp"

class ASTString : public ASTExpression {
public:
    ASTString(const std::string &str);

    llvm::Value *codegen() override;

    std::string str;
};

#endif //SIMPLEFRONTEND_ASTSTRING_HPP
