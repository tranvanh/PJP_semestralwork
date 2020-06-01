//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTFUNCTION_HPP
#define SIMPLEFRONTEND_ASTFUNCTION_HPP


#include "ASTFunctionPrototype.hpp"
#include "../ASTBody.hpp"

class ASTFunction {
public:
    ASTFunction(std::unique_ptr<ASTFunctionPrototype> prototype,
                std::vector<std::unique_ptr<ASTVariable>> local,
                std::unique_ptr<ASTBody> body
    );

    llvm::Function *codegen();

private:
    std::unique_ptr<ASTFunctionPrototype> m_Prototype;
    std::vector<std::unique_ptr<ASTVariable>> m_LocalVariables;
    std::unique_ptr<ASTBody> m_Body;
};


#endif //SIMPLEFRONTEND_ASTFUNCTION_HPP
