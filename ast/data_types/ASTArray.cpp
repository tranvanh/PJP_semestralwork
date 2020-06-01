//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTArray.hpp"

ASTArray::ASTArray(std::unique_ptr<ASTNumber> lower,
                   std::unique_ptr<ASTNumber> upper,
                   std::shared_ptr<ASTVariableType> type)
        : m_LowerIdx(std::move(lower)), m_UpperIdx(std::move(upper)), m_Type(std::move(type)) {}