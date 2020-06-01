//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTIf.hpp"

ASTIf::ASTIf(std::unique_ptr<ASTExpression> condition,
             std::unique_ptr<ASTBody> then_body,
             std::unique_ptr<ASTBody> else_body)
        : m_Condition(std::move(condition)), m_ThenBody(std::move(then_body)), m_ElseBody(std::move(else_body)) {}