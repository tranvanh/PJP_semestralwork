//
// Created by Tomas Tran on 01/06/2020.
//

#ifndef SIMPLEFRONTEND_ASTPROGRAM_HPP
#define SIMPLEFRONTEND_ASTPROGRAM_HPP


#include "ASTExpression.hpp"
#include "variable/ASTVariableDef.hpp"
#include "function/ASTFunction.hpp"

class ASTProgram : public ASTExpression {
public:
    ASTProgram(const std::string &name,
               std::vector<std::unique_ptr<ASTVariableDef>> global,
               std::vector<std::unique_ptr<ASTFunction>> functions,
               std::unique_ptr<ASTBody> main
    );

    llvm::Value *codegen() override;

    std::unique_ptr<llvm::Module> runCodegen(const std::string &output_file);

    const std::string m_Name;
    std::vector<std::unique_ptr<ASTVariableDef>> m_Global;
    std::vector<std::unique_ptr<ASTFunction>> m_Functions;
    std::unique_ptr<ASTBody> m_Main;
};


#endif //SIMPLEFRONTEND_ASTPROGRAM_HPP
