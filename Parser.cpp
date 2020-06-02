#include "Parser.hpp"
#include "ast/references/ASTSingleVarReference.hpp"

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

static std::string tokenToStr(Token tok) {
    switch (tok) {
        case tok_identifier :
            return "identifier";
        case tok_number :
            return "number";
        case tok_eof :
            return "EOF";
        case tok_error :
            return "ERROR";
        case tok_string :
            return "string";


        case tok_plus :
            return "+";
        case tok_minus :
            return "-";
        case tok_multiply :
            return "*";
        case tok_equal :
            return "=";
        case tok_notEqual :
            return "<>";
        case tok_less :
            return "<";
        case tok_lessEqual :
            return "<=";
        case tok_greater :
            return ">";
        case tok_greaterEqual :
            return ">=";
        case tok_leftParenthesis :
            return "(";
        case tok_rightParenthesis :
            return ")";
        case tok_leftBracket :
            return "[";
        case tok_rightBracket :
            return "]";
        case tok_assign :
            return ":=";
        case tok_comma :
            return ",";
        case tok_semicolon :
            return ";";
        case tok_dot :
            return ".";
        case tok_colon :
            return ":";

        case tok_program :
            return "program";
        case tok_const :
            return "const";
        case tok_var :
            return "var";
        case tok_begin :
            return "begin";
        case tok_end :
            return "end";
        case tok_div :
            return "DIV";
        case tok_mod :
            return "MOD";
        case tok_integer :
            return "integer";
        case tok_array :
            return "array";
        case tok_of :
            return "of";
        case tok_if :
            return "if";
        case tok_then :
            return "then";
        case tok_else :
            return "else";
        case tok_while :
            return "while";
        case tok_do :
            return "dow";
        case tok_for :
            return "for";
        case tok_to :
            return "to";
        case tok_downto :
            return "downto";
        case tok_or :
            return "or";
        case tok_and :
            return "and";

        case tok_procedure :
            return "procedure";
        case tok_function :
            return "function";
        case tok_forward :
            return "forward";
        case tok_break :
            return "break";
        case tok_exit :
            return "exit";

        case tok_write :
            return "writeln";
        case tok_read :
            return "readln";

        default:
            return "UNDEFINED TOKEN";
    }
}

bool Parser::validateToken(Token tok) {
    if (m_CurrTok == tok)
        return true;

    throw "Expect: '" + tokenToStr(tok) + "'. Received: '" + tokenToStr(m_CurrTok) + "'.";
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

Token Parser::getNextToken() {
    return m_CurrTok = m_Lexer.getToken();
}


std::unique_ptr<ASTExpression> Parser::parseFunctionCall(const std::string &name) {

    std::vector<std::unique_ptr<ASTExpression>> args;
    getNextToken();

    while (m_CurrTok != tok_rightParenthesis) {
        auto argument = parseExpression();
        if (argument)
            args.push_back(std::move(argument));
        else
            return nullptr;

        if (m_CurrTok == tok_rightParenthesis)
            break;

        validateToken(tok_comma);
        getNextToken();
    }

    getNextToken();
    return std::make_unique<ASTFunctionCall>(name, std::move(args));
}

std::unique_ptr<ASTExpression> Parser::parseIdentifierExpr() {
    validateToken(tok_identifier);
    std::string identifier_str = m_Lexer.identifierStr();
    getNextToken();

    if (m_CurrTok == tok_leftParenthesis)
        return parseFunctionCall(identifier_str);
    if (m_CurrTok == tok_leftBracket)
        return parseArrayReference(identifier_str);

    auto var_ref = std::make_unique<ASTSingleVarReference>(identifier_str);
    if (m_CurrTok == tok_assign)
        return parseAssign(std::move(var_ref));

    return std::move(var_ref);

}

std::unique_ptr<ASTNumber> Parser::parseNumberExpr() {
    int sign = 1;
    if (m_CurrTok == tok_minus) {
        sign = -1;
        getNextToken();
    }

    validateToken(tok_number);
    getNextToken();

    return std::make_unique<ASTNumber>(sign * m_Lexer.numVal());
}

std::unique_ptr<ASTString> Parser::parseStringExpr() {
    validateToken(tok_string);
    getNextToken();
    return std::make_unique<ASTString>(m_Lexer.strVal());
}

std::unique_ptr<ASTExpression> Parser::parseParenthesisExpr() {

    validateToken(tok_leftParenthesis);
    getNextToken();

    auto res = parseExpression();

    validateToken(tok_rightParenthesis);
    getNextToken();

    return std::move(res);
}

std::unique_ptr<ASTExpression> Parser::parsePrimaryExpr() {
    switch (m_CurrTok) {
        case tok_identifier:
            return parseIdentifierExpr();
        case tok_number:
            return parseNumberExpr();
        case tok_minus:
            return parseNumberExpr();
        case tok_string:
            return parseStringExpr();
        case tok_leftParenthesis:
            return parseParenthesisExpr();
        default:
            throw ("Primary expression beginning with unexpected token");
    }
}

std::unique_ptr<ASTExpression> Parser::parseExpression() {
    auto LHS = parsePrimaryExpr();
    if (!LHS)
        return nullptr;
    return parseBinaryOperatorRHS(1, std::move(LHS));
}

//statements

std::unique_ptr<ASTBody> Parser::parseBody() {

    std::vector<std::unique_ptr<ASTExpression>> content;
    if (m_CurrTok == tok_begin) {
        getNextToken();

        while (m_CurrTok != tok_end && m_CurrTok != tok_eof) {
            content.emplace_back(parseStatement());
            while (m_CurrTok == tok_semicolon) {
                getNextToken();
                if (m_CurrTok == tok_end)
                    break;
                content.emplace_back(parseStatement());
            }
        }
        validateToken(tok_end);
        getNextToken();
    } else {
        content.emplace_back(parseStatement());
    }
    return std::make_unique<ASTBody>(std::move(content));
}

std::unique_ptr<ASTIf> Parser::parseIfStmt() {
    validateToken(tok_if);

    getNextToken();

    auto condition = parseExpression();

    validateToken(tok_then);
    getNextToken();

    auto then_body = parseBody();

    if (m_CurrTok == tok_else) {
        getNextToken();
        auto else_body = parseBody();
        return std::make_unique<ASTIf>(std::move(condition), std::move(then_body), std::move(else_body));
    }
    return std::make_unique<ASTIf>(std::move(condition), std::move(then_body), nullptr);

}


std::unique_ptr<ASTExit> Parser::parseExit() {
    return std::make_unique<ASTExit>();
}

std::unique_ptr<ASTBreak> Parser::parseBreak() {
    return std::make_unique<ASTBreak>();
}


std::unique_ptr<ASTExpression> Parser::parseStatement() {
    switch (m_CurrTok) {
        case tok_identifier:
            return parseIdentifierExpr();
        case tok_if:
            return parseIfStmt();
        case tok_while:
            return parseWhileStmt();
        case tok_for:
            return parseForStmt();
        case tok_break:
            return parseBreak();
        case tok_exit:
            return parseExit();
    }
}

