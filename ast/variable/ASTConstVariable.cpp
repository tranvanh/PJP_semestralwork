//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTConstVariable.hpp"

ASTConstVariable::ASTConstVariable(const std::string &name,
                                   int value)
        : m_Name(name), m_Value(value) {}