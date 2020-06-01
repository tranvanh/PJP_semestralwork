//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTIF_HPP
#define SIMPLEFRONTEND_ASTIF_HPP


#include "../ASTExpression.hpp"
#include "../ASTBody.hpp"

class ASTIf : public ASTExpression {
public:
    ASTIf(std::unique_ptr<ASTExpression> condition,
          std::unique_ptr<ASTBody> then_bod,
          std::unique_ptr<ASTBody> else_body);

    llvm::Value *codegen() override;

private:
    std::unique_ptr<ASTExpression> m_Condition;
    std::unique_ptr<ASTBody> m_ThenBody, m_ElseBody;
};


#endif //SIMPLEFRONTEND_ASTIF_HPP
