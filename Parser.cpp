#include "Parser.hpp"

Parser::Parser()
        : MilaContext(), MilaBuilder(MilaContext), MilaModule("mila", MilaContext) {}

Parser::Parser(const std::string &file_name)
        : MilaContext(), MilaBuilder(MilaContext), MilaModule("mila", MilaContext), Lexer(file_name) {

    m_precedence_table[Token::tok_less] = 10;
    m_precedence_table[Token::tok_lessEqual] = 10;
    m_precedence_table[Token::tok_greater] = 10;
    m_precedence_table[Token::tok_greaterEqual] = 10;
    m_precedence_table[Token::tok_equal] = 10;
    m_precedence_table[Token::tok_notEqual] = 10;
    // 3rd degree priority
    m_precedence_table[Token::tok_plus] = 20;
    m_precedence_table[Token::tok_minus] = 20;
    m_precedence_table[Token::tok_or] = 20;

    // 2nd degree prioty
    m_precedence_table[Token::tok_multiply] = 40;
    m_precedence_table[Token::tok_div] = 40;
    m_precedence_table[Token::tok_mod] = 40;
    m_precedence_table[Token::tok_and] = 40;
}

bool Parser::validateToken(Token correct) {
    if(m_CurrTok == correct)
        return true;

    throw "Expect: '" + tokenToStr(correct) + "'. Received: '" + tokenToStr(m_CurrTok) + "'.";
}

bool Parser::Parse() {
    getNextToken();
    return true;
}

const Module &Parser::Generate() {

    // create writeln function
    {
        std::vector<Type *> Ints(1, Type::getInt32Ty(MilaContext));
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(MilaContext), Ints, false);
        Function *F = Function::Create(FT, Function::ExternalLinkage, "writeln", MilaModule);
        for (auto &Arg : F->args())
            Arg.setName("x");
    }

    // create main function
    {
        FunctionType *FT = FunctionType::get(Type::getInt32Ty(MilaContext), false);
        Function *MainFunction = Function::Create(FT, Function::ExternalLinkage, "main", MilaModule);

        // block
        BasicBlock *BB = BasicBlock::Create(MilaContext, "entry", MainFunction);
        MilaBuilder.SetInsertPoint(BB);

        // call writeln with value from lexel
        MilaBuilder.CreateCall(MilaModule.getFunction("writeln"), {
                ConstantInt::get(MilaContext, APInt(32, m_Lexer.numVal()))
        });

        // return 0
        MilaBuilder.CreateRet(ConstantInt::get(Type::getInt32Ty(MilaContext), 0));
    }

    return this->MilaModule;
}

/*
 * Simple token buffer.
 * CurTok is the current token the parser is looking at
 * getNextToken reads another token from the lexer and updates curTok with ts result
 * Every function in the parser will assume that CurTok is the cureent token that needs to be parsed
 */

int Parser::getNextToken() {
    return m_CurrTok = m_Lexer.getToken();
}

std::unique_ptr<ASTExpression> Parser::parseParenthesisExpr() {
    return std::unique_ptr<ASTExpression>();
}
