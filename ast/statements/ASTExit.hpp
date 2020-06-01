//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTEXIT_HPP
#define SIMPLEFRONTEND_ASTEXIT_HPP


#include "../ASTExpression.hpp"

class ASTExit : public ASTExpression {
public:
    llvm::Value *codegen() override;
};

#endif //SIMPLEFRONTEND_ASTEXIT_HPP
