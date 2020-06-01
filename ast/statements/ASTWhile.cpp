//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTWhile.hpp"

ASTWhile::ASTWhile(std::unique_ptr<ASTExpression> condition,
                   std::unique_ptr<ASTBody> body)
        : m_Condition(std::move(condition)), m_Body(std::move(body)) {}