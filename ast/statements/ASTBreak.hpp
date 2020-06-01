//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTBREAK_HPP
#define SIMPLEFRONTEND_ASTBREAK_HPP


#include "../ASTExpression.hpp"

class ASTBreak : public ASTExpression {
public:
    llvm::Value *codegen() override;
};


#endif //SIMPLEFRONTEND_ASTBREAK_HPP
