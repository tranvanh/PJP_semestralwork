//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTFunctionCall.hpp"

ASTFunctionCall::ASTFunctionCall(const std::string &name,
                                 std::vector<std::unique_ptr<ASTExpression>> args)
        : m_Name(name), m_Arguments(std::move(args)) {}