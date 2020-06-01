//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTBODY_HPP
#define SIMPLEFRONTEND_ASTBODY_HPP

#include "ASTExpression.hpp"

//Function/Loop body
class ASTBody {
public:
    ASTBody(std::vector<std::unique_ptr<ASTExpression>> content);

    llvm::Value *codegen();

    std::vector<std::unique_ptr<ASTExpression>> m_Content;
};

#endif //SIMPLEFRONTEND_ASTBODY_HPP
