//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTVARIABLE_HPP
#define SIMPLEFRONTEND_ASTVARIABLE_HPP


#include "ASTVariableDef.hpp"
#include "ASTVariableType.hpp"

class ASTVariable : public ASTVariableDef {
public:
    ASTVariable(const std::string &name, std::shared_ptr<ASTVariableType> type);

    llvm::Value *codegen() override;

    const std::string name;
    std::shared_ptr<ASTVariableType> type;
};

#endif //SIMPLEFRONTEND_ASTVARIABLE_HPP
