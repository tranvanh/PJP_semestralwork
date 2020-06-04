//
// Created by Tomas Tran on 04/06/2020.
//

#include "codegen.cpp"
#include "../ast/constants/ASTNumber.hpp"
#include "../ast/constants/ASTString.hpp"

Value *ASTNumber::codegen() {
    return ConstantInt::get(TheContext, APInt(32, m_Value, true));
}

Value *ASTString::codegen() {
    return Builder.CreateGlobalString(StringRef(m_Str));
}