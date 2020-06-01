//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTARRAY_HPP
#define SIMPLEFRONTEND_ASTARRAY_HPP


#include "ASTVariableType.hpp"
#include "ASTNumber.hpp"

class ASTArray : public ASTVariableType {
public:
    ASTArray(std::unique_ptr<ASTNumber> lower,
             std::unique_ptr<ASTNumber> upper,
             std::shared_ptr<ASTVariableType> type);

    llvm::Type *codegen();

    std::unique_ptr<ASTNumber> lowerIdx, upperIdx;
private:
    std::shared_ptr<ASTVariableType> type;
};


#endif //SIMPLEFRONTEND_ASTARRAY_HPP
