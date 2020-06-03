#ifndef PJPPROJECT_PARSER_HPP
#define PJPPROJECT_PARSER_HPP

#include "Lexer.hpp"

#include "llvm/ADT/APFloat.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "ast/ASTExpression.hpp"
#include "ast/constants/ASTNumber.hpp"
#include "ast/constants/ASTString.hpp"
#include "ast/data_types/ASTVariableType.hpp"
#include "ast/variable/ASTVariable.hpp"
#include "ast/variable/ASTConstVariable.hpp"
#include "ast/ASTBody.hpp"
#include "ast/function/ASTFunctionPrototype.hpp"
#include "ast/function/ASTFunction.hpp"
#include "ast/statements/ASTIf.hpp"
#include "ast/statements/ASTFor.hpp"
#include "ast/statements/ASTWhile.hpp"
#include "ast/statements/ASTBreak.hpp"
#include "ast/statements/ASTExit.hpp"
#include "ast/references/ASTArrayReference.hpp"
#include "ast/operators/ASTAssignOperator.hpp"
#include "ast/function/ASTFunctionCall.hpp"

using namespace llvm;

class Parser {
public:
    Parser();

    Parser(const std::string &file_name);

    ~Parser() = default;

    bool Parse();             // parse
    const Module &Generate(); // generate


    std::unique_ptr<ASTExpression> parseExpression();

    std::unique_ptr<ASTExpression> parseStatement();

    //constants
    std::unique_ptr<ASTNumber> parseNumberExpr();

    std::unique_ptr<ASTString> parseStringExpr();

    // expressions
    std::unique_ptr<ASTExpression> parseParenthesisExpr();

    std::unique_ptr<ASTExpression> parseIdentifierExpr();

    std::unique_ptr<ASTExpression> parsePrimaryExpr();

    std::unique_ptr<ASTExpression> parseBinaryOperatorRHS(int precedence, std::unique_ptr<ASTExpression> LHS);

    //variables

    std::shared_ptr<ASTVariableType> parseVarType();

    std::vector<std::unique_ptr<ASTVariable>> parseVarDeclaration();

    std::vector<std::unique_ptr<ASTConstVariable>> parseConstVarDeclaration();

    // Functions/statements
    std::unique_ptr<ASTExpression> parseFunctionCall(const std::string &name);

    std::unique_ptr<ASTBody> parseBody();

    std::unique_ptr<ASTFunctionPrototype> parseFunctionPrototype();

    std::unique_ptr<ASTFunction> parseFunction();

    std::unique_ptr<ASTIf> parseIfStmt();

    std::unique_ptr<ASTFor> parseForStmt();

    std::unique_ptr<ASTWhile> parseWhileStmt();

    std::unique_ptr<ASTBreak> parseBreak();

    std::unique_ptr<ASTExit> parseExit();

    //references

    std::unique_ptr<ASTArrayReference> parseArrayReference(const std::string &name);
    std::unique_ptr<ASTAssignOperator> parseAssign(const std::string & var_name);
    std::unique_ptr<ASTAssignOperator> parseAssign(std::unique_ptr<ASTReference> var_ref);


private:
    Token getNextToken();
    int getPrecedence();
    bool validateToken(Token tok);

    Lexer m_Lexer;            // lexer is used to read tokens
    Token m_CurrTok;               // to keep the current token

    LLVMContext MilaContext;   // llvm context
    IRBuilder<> MilaBuilder;   // llvm builder
    Module MilaModule;         // llvm module

    std::map<Token, int> m_precedence_table;
};

#endif //PJPPROJECT_PARSER_HPP
