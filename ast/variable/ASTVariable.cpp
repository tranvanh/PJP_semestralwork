//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTVariable.hpp"

ASTVariable::ASTVariable(const std::string &name,
                         std::shared_ptr<ASTVariableType> type)
        : m_Name(name), m_Type(type) {}