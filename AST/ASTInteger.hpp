//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTINTEGER_HPP
#define SIMPLEFRONTEND_ASTINTEGER_HPP


#include "ASTVariableType.hpp"

class ASTInteger : public ASTVariableType {
public:
    llvm::Type *codegen();

};


#endif //SIMPLEFRONTEND_ASTINTEGER_HPP
