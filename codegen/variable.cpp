//
// Created by Tomas Tran on 04/06/2020.
//
#include "codegen.hpp"

Value *ASTVariable::codegen() {
    auto type_value = m_Type->codegen();
    if (!type_value)
        return nullptr;

    TheModule->getOrInsertGlobal(m_Name, type_value);
    GlobalVariable *global_var = TheModule->getNamedGlobal(m_Name);
    global_var->setLinkage(GlobalValue::InternalLinkage);

    if (type_value == Type::getInt32Ty(TheContext))
        global_var->setInitializer(ConstantInt::get(TheContext, APInt(32, 0, true)));
    else
        global_var->setInitializer(ConstantAggregateZero::get(type_value));


    global_vars[m_Name] = std::make_pair(global_var, m_Type);

    return global_vars[m_Name].first;
}

// Const declaration
Value *ASTConstVariable::codegen() {
    const_vars[m_Name] = ConstantInt::get(TheContext, APInt(32, m_Value, true));

    return const_vars[m_Name];
}