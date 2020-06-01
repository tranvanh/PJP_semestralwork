//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTARRAY_HPP
#define SIMPLEFRONTEND_ASTARRAY_HPP


#include "ASTVariableType.hpp"
#include "../constants/ASTNumber.hpp"

class ASTArray : public ASTVariableType {
public:
    ASTArray(std::unique_ptr<ASTNumber> lower,
             std::unique_ptr<ASTNumber> upper,
             std::shared_ptr<ASTVariableType> type
    );

    llvm::Type *codegen();

    std::unique_ptr<ASTNumber> m_LowerIdx, m_UpperIdx;
private:
    std::shared_ptr<ASTVariableType> m_Type;
};


#endif //SIMPLEFRONTEND_ASTARRAY_HPP
