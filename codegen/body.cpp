//
// Created by Tomas Tran on 04/06/2020.
//

#include "codegen.cpp"
#include "../ast/ASTBody.hpp"

Value *ASTBody::codegen() {
    for (auto &statement : m_Content)
        statement->codegen();

    return Constant::getNullValue(Type::getInt32Ty(TheContext));
}