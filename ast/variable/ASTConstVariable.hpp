//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTCONSTVARIABLE_HPP
#define SIMPLEFRONTEND_ASTCONSTVARIABLE_HPP


#include "ASTVariableDef.hpp"

// Const Variable
class ASTConstVariable : public ASTVariableDef {
public:
    ASTConstVariable(const std::string &name,
                     int value);

    llvm::Value *codegen() override;

    const std::string m_Name;
    const int m_Value;
};


#endif //SIMPLEFRONTEND_ASTCONSTVARIABLE_HPP
