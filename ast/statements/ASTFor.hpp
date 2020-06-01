//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTFOR_HPP
#define SIMPLEFRONTEND_ASTFOR_HPP


#include "../ASTExpression.hpp"
#include "../ASTBody.hpp"

class ASTFor : public ASTExpression {
public:
    ASTFor(const std::string &ctrl_var,
           std::unique_ptr<ASTExpression> start,
           std::unique_ptr<ASTExpression> end,
           std::unique_ptr<ASTExpression> step,
           std::unique_ptr<ASTBody> body,
           bool downto
    );

    llvm::Value *codegen() override;

private:
    const std::string m_VarName;
    std::unique_ptr<ASTExpression> m_Start, m_End, m_Step;
    std::unique_ptr<ASTBody> body;
    bool m_DownTo;
};


#endif //SIMPLEFRONTEND_ASTFOR_HPP
