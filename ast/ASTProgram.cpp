//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTProgram.hpp"

ASTProgram::ASTProgram(const std::string &name,
                       std::vector<std::unique_ptr<ASTVariableDef>> global,
                       std::vector<std::unique_ptr<ASTFunction>> functions,
                       std::unique_ptr<ASTBody> main)
        : m_Name(name), m_Global(std::move(global)), m_Functions(std::move(functions)), m_Main(std::move(main)) {}