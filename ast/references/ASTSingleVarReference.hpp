//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTSINGLEVARREFERENCE_HPP
#define SIMPLEFRONTEND_ASTSINGLEVARREFERENCE_HPP


#include "ASTReference.hpp"

class ASTSingleVarReference : public ASTReference {
public:
    ASTSingleVarReference(const std::string &name);

    llvm::Value *codegen() override;

    llvm::Value *getAlloc() override;
};


#endif //SIMPLEFRONTEND_ASTSINGLEVARREFERENCE_HPP
