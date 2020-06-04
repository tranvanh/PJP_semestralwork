//
// Created by Tomas Tran on 04/06/2020.
//

#include "codegen.cpp"
#include "../ast/data_types/ASTInteger.hpp"
#include "../ast/data_types/ASTArray.hpp"

Type *ASTInteger::codegen() {
    return Type::getInt32Ty(TheContext);
}

Type *ASTArray::codegen() {
    Type *elem_type = m_Type->codegen();
    int size = (m_UpperIdx->m_Value) - (m_LowerIdx->m_Value) + 1;
    return ArrayType::get(elem_type, size);
}