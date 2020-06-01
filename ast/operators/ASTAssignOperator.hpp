//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTASSIGNOPERATOR_HPP
#define SIMPLEFRONTEND_ASTASSIGNOPERATOR_HPP


#include "../ASTExpression.hpp"
#include "../references/ASTReference.hpp"

class ASTAssignOperator : public ASTExpression {
    ASTAssignOperator(std::unique_ptr<ASTReference> variable,
                      std::unique_ptr<ASTExpression> value
    );

    llvm::Value* codegen() override;
    const std::unique_ptr<ASTReference> m_Variable;
    const std::unique_ptr<ASTExpression> m_Value;

};


#endif //SIMPLEFRONTEND_ASTASSIGNOPERATOR_HPP
