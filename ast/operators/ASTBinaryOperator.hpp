//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTBINARYOPERATOR_HPP
#define SIMPLEFRONTEND_ASTBINARYOPERATOR_HPP


#include "../ASTExpression.hpp"
#include "../../Lexer.hpp"

class ASTBinaryOperator : public ASTExpression {
public:
    ASTBinaryOperator(Token op,
                      std::unique_ptr<ASTExpression> LHS,
                      std::unique_ptr<ASTExpression> RHS
    );

    llvm::Value* codegen() override;
private:
    Token op;
    std::unique_ptr<ASTExpression> m_LHS, m_RHS;
};


#endif //SIMPLEFRONTEND_ASTBINARYOPERATOR_HPP
