//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTArrayReference.hpp"

ASTArrayReference::ASTArrayReference(const std::string &name,
                                     std::unique_ptr<ASTExpression> index)
        : ASTReference(name), m_Index(std::move(index)) {}