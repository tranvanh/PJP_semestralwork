//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTFUNCTIONCALL_HPP
#define SIMPLEFRONTEND_ASTFUNCTIONCALL_HPP


#include "../ASTExpression.hpp"

class ASTFunctionCall : public ASTExpression {
public:
    ASTFunctionCall(const std::string &name,
                    std::vector<std::unique_ptr<ASTExpression>> args
    );

    llvm::Value *codegen() override;

private:
    std::string m_Name;
    std::vector<std::unique_ptr<ASTExpression>> m_Arguments;
};


#endif //SIMPLEFRONTEND_ASTFUNCTIONCALL_HPP
