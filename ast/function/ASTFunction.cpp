//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTFunction.hpp"

ASTFunction::ASTFunction(std::unique_ptr<ASTFunctionPrototype> prototype,
                         std::vector<std::unique_ptr<ASTVariable>> local,
                         std::unique_ptr<ASTBody> body)
        : m_Prototype(std::move(prototype)), m_LocalVariables(std::move(local)), m_Body(std::move(body)) {}