//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTFUNCTIONPROTOTYPE_HPP
#define SIMPLEFRONTEND_ASTFUNCTIONPROTOTYPE_HPP

#include "../variable/ASTVariable.hpp"

class ASTFunctionPrototype {
public:
    ASTFunctionPrototype(const std::string &name,
                         std::vector<std::unique_ptr<ASTVariable>> params,
                         std::shared_ptr<ASTVariableType> ret
    );

    llvm::Function *codegen();

    const std::string &getName() const;

    std::shared_ptr<ASTVariableType> m_ReturnType;
    std::vector<std::unique_ptr<ASTVariable>> m_Parameters;
private:
    std::string m_Name;
};


#endif //SIMPLEFRONTEND_ASTFUNCTIONPROTOTYPE_HPP
