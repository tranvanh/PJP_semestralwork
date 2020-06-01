//
// Created by Tomas Tran on 01/06/2020.
//

#include "ASTFor.hpp"

ASTFor::ASTFor(const std::string &ctrl_var,
               std::unique_ptr<ASTExpression> start,
               std::unique_ptr<ASTExpression> end,
               std::unique_ptr<ASTExpression> step,
               std::unique_ptr<ASTBody> body,
               bool downto)
        : m_VarName(ctrl_var), m_Start(std::move(start)),
          m_End(std::move(end)), m_Step(std::move(step)),
          m_Body(std::move(body)), m_DownTo(downto) {}