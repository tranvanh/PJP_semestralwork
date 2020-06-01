//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTWHILE_HPP
#define SIMPLEFRONTEND_ASTWHILE_HPP


#include "../ASTExpression.hpp"
#include "../ASTBody.hpp"

class ASTWhile : public ASTExpression {
public:
    ASTWhile(std::unique_ptr<ASTExpression> condition,
             std::unique_ptr<ASTBody> body
    );

    llvm::Value *codegen() override;

private:
    std::unique_ptr<ASTExpression> m_Condition;
    std::unique_ptr<ASTBody> body;
};


#endif //SIMPLEFRONTEND_ASTWHILE_HPP
