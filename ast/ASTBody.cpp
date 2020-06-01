//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTBody.hpp"

ASTBody::ASTBody(std::vector<std::unique_ptr<ASTExpression>> content)
        : m_Content(std::move(content)) {}