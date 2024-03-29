//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTFunctionPrototype.hpp"

ASTFunctionPrototype::ASTFunctionPrototype(const std::string &name,
                                           std::vector<std::unique_ptr<ASTVariable>> params,
                                           std::shared_ptr<ASTVariableType> ret)
        : m_Parameters(std::move(params)), m_ReturnType(ret), m_Name(name) {}

const std::string &ASTFunctionPrototype::getName() const { return m_Name; }