//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTBinaryOperator.hpp"

ASTBinaryOperator::ASTBinaryOperator(Token op,
                                     std::unique_ptr<ASTExpression> LHS,
                                     std::unique_ptr<ASTExpression> RHS)
        : m_Op(op), m_LHS(std::move(LHS)), m_RHS(std::move(RHS)) {}