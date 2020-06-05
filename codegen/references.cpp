//
// Created by Tomas Tran on 04/06/2020.
//

#include "codegen.hpp"

// Get variable value from stack
Value * ASTSingleVarReference::codegen ()
{
    if ( const_vars.find(m_Name) != const_vars.cend() )
        return const_vars[m_Name];

    // Search for var
    TVarInfo info;

    if ( named_values.find(m_Name) != named_values.end() )
        info = named_values[m_Name];
    else if ( global_vars.find(m_Name) != global_vars.end() )
        info = global_vars[m_Name];
    else
        throw "Using an undeclared variable";

    return Builder.CreateLoad(info.first, m_Name);
}
// Find variable address
Value * ASTSingleVarReference::getAlloc()
{
    TVarInfo info;
    if ( named_values.find(m_Name) != named_values.cend() )
        info = named_values[m_Name];
    else if ( global_vars.find(m_Name) != global_vars.cend() )
        info = global_vars[m_Name];

    return info.first;
}

// Get array elem value from stack
Value * ASTArrayReference::codegen ()
{
    return Builder.CreateLoad(getAlloc());
}
// Find array elem address
Value * ASTArrayReference::getAlloc()
{
    // Search for var
    TVarInfo info;
    if ( named_values.find(m_Name) != named_values.cend() )
        info = named_values[m_Name];
    else if ( global_vars.find(m_Name) != global_vars.cend() )
        info = global_vars[m_Name];
    else
        throw "Undeclared array variable";

    // Calculating elem address
    std::vector<Value *> idx_list;
    auto start_idx = std::dynamic_pointer_cast<ASTArray>(info.second) -> m_LowerIdx -> codegen();
    auto idx = Builder.CreateSub(m_Index -> codegen(), start_idx);

    idx_list.push_back(ConstantInt::get(Type::getInt32Ty(TheContext), 0));
    idx_list.push_back(idx);

    return Builder.CreateGEP(info.first, idx_list);
}