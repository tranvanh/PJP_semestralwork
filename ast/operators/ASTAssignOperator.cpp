//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTAssignOperator.hpp"

ASTAssignOperator::ASTAssignOperator(std::unique_ptr<ASTReference> variable,
                                     std::unique_ptr<ASTExpression> value)
        : m_Variable(std::move(variable)), m_Value(std::move(value)) {}