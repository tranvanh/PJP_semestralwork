#include "Parser.hpp"
#include "ast/references/ASTSingleVarReference.hpp"
#include "ast/operators/ASTBinaryOperator.hpp"
#include "ast/data_types/ASTInteger.hpp"
#include "ast/data_types/ASTArray.hpp"

//Parser::Parser()
//        : MilaContext(), MilaBuilder(MilaContext), MilaModule("mila", MilaContext) {}

Parser::Parser(const std::string &file_name): m_Lexer(file_name) {
//        : MilaContext(), MilaBuilder(MilaContext), MilaModule("mila", MilaContext), Lexer(file_name) {

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

int Parser::getPrecedence() {
    auto it = m_precedence_table.find(m_CurrTok);
    if (it == m_precedence_table.end())
        return -1;
    return it->second;

}

/**
 * 'program' name ';'
 * [var_declaration]
 * [const_declaration]
 * [function] [procedure]
 * [main]
 */
std::unique_ptr<ASTProgram> Parser::Parse() {
    getNextToken();

    validateToken(tok_program);
    getNextToken();

    validateToken(tok_identifier);
    std::string program_name = m_Lexer.identifierStr();
    getNextToken();

    validateToken(tok_semicolon);
    getNextToken();

    std::vector<std::unique_ptr<ASTVariableDef>> glob_vars;
    std::vector<std::unique_ptr<ASTFunction>> functs;

    std::unique_ptr<ASTBody> main;

    while (true) {
        if (m_CurrTok == tok_var) {
            auto global_vars = parseVarDeclaration();
            for (auto &v : global_vars)
                glob_vars.push_back(std::move(v));
        } else if (m_CurrTok == tok_const) {
            auto const_vars = parseConstVarDeclaration();
            for (auto &v : const_vars)
                glob_vars.push_back(std::move(v));
        } else if (m_CurrTok == tok_function || m_CurrTok == tok_procedure)
            functs.emplace_back(parseFunction());
        else {
            main = parseBody();
            break;
        }
    }

    validateToken(tok_dot);
    getNextToken();

    return std::make_unique<ASTProgram>(program_name, std::move(glob_vars),
                                        std::move(functs), std::move(main));
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

/**
 * [number]
 * { '-' } tok_number
 */
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

/**
 * '(' expression ')'
 */
std::unique_ptr<ASTExpression> Parser::parseParenthesisExpr() {

    validateToken(tok_leftParenthesis);
    getNextToken();

    auto res = parseExpression();

    validateToken(tok_rightParenthesis);
    getNextToken();

    return res;
//    return std::move(res);
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


std::unique_ptr<ASTExpression> Parser::parseBinaryOperatorRHS(int precedence, std::unique_ptr<ASTExpression> LHS) {
    while (true) {
        int curr_prec = getPrecedence();
        if (curr_prec < precedence)
            return LHS;

        Token bin_op = m_CurrTok;
        getNextToken();

        auto RHS = parsePrimaryExpr();
        if (!RHS)
            return nullptr;

        int next_prec = getPrecedence();
        if (curr_prec < next_prec) {
            RHS = parseBinaryOperatorRHS(curr_prec + 1, std::move(RHS));
            if (!RHS)
                return nullptr;
        }

        LHS = std::make_unique<ASTBinaryOperator>(bin_op, std::move(LHS), move(RHS));
    }


    return std::unique_ptr<ASTExpression>();
}


std::unique_ptr<ASTExpression> Parser::parseExpression() {
    auto LHS = parsePrimaryExpr();
    if (!LHS)
        return nullptr;
    return parseBinaryOperatorRHS(1, std::move(LHS));
}

//statements
/**
 * [body]
 * ('begin' {[line]}* 'end') / ([line])
 */
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

/**
 * [if]
 * 'if' condition
 * 'then' [body]
 * {'else' [body]}
 */
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

/**
 * [while]
 * 'while' condition 'do'
 * [body]
 */
std::unique_ptr<ASTWhile> Parser::parseWhileStmt() {
    validateToken(tok_while);
    getNextToken();

    auto condition = parseExpression();
    validateToken(tok_do);
    getNextToken();
    auto while_body = parseBody();

    return std::make_unique<ASTWhile>(std::move(condition), std::move(while_body));

}

/**
 * [for]
 * 'for' var_name ':=' [number] 'to'/'downto' [number] 'do'
 * [body]
 */
std::unique_ptr<ASTFor> Parser::parseForStmt() {
    validateToken(tok_for);
    getNextToken();

    validateToken(tok_identifier);
    getNextToken();
    std::string ctrl_var = m_Lexer.identifierStr();

    validateToken(tok_assign);
    getNextToken();

    auto start = parseExpression();
    if (!start)
        return nullptr;

    bool downto;
    if (m_CurrTok == tok_to)
        downto = false;
    else if (m_CurrTok == tok_downto)
        downto = true;
    else
        throw "Expected: 'to' or 'downto'. Recieved: " + tokenToStr(m_CurrTok) + ".";

    getNextToken();

    auto end = parseExpression();
    if (!end)
        return nullptr;

    validateToken(tok_do);
    getNextToken();

    auto for_body = parseBody();

    auto step = nullptr;

    return std::make_unique<ASTFor>(ctrl_var, std::move(start), std::move(end), std::move(step), std::move(for_body),
                                    downto);
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
            getNextToken();
            return std::make_unique<ASTBreak>();
        case tok_exit:
            getNextToken();
            return std::make_unique<ASTBreak>();
        default:
            return nullptr;
    }
}

//Assign
/**
 * [var_reference] ':=' expression
 */
std::unique_ptr<ASTAssignOperator> Parser::parseAssign(std::unique_ptr<ASTReference> var_ref) {
    validateToken(tok_assign);
    getNextToken();

    return std::make_unique<ASTAssignOperator>(std::move(var_ref), parseExpression());
}

/**
 * [var_reference] ':=' expression
 */
std::unique_ptr<ASTAssignOperator> Parser::parseAssign(const std::string &var_name) {

    std::unique_ptr<ASTReference> var_ref = nullptr;

    if (m_CurrTok == tok_leftBracket)
        var_ref = parseArrayReference(var_name);
    else
        var_ref = std::make_unique<ASTSingleVarReference>(var_name);

    return parseAssign(std::move(var_ref));
}

/**
 * identifier '[' expression ']'
 */
std::unique_ptr<ASTArrayReference> Parser::parseArrayReference(const std::string &name) {
    validateToken(tok_leftBracket);
    getNextToken();

    auto idx = parseExpression();

    validateToken(tok_rightBracket);
    getNextToken();

    return std::make_unique<ASTArrayReference>(name, std::move(idx));
}


//Variables

/**
 * [type]
 * 'integer' | 'array' '[' [number] '..' [number] ']' 'of' [type]
 */
std::shared_ptr<ASTVariableType> Parser::parseVarType() {
    if (m_CurrTok == tok_integer) {
        getNextToken();
        return std::make_shared<ASTInteger>();
    } else if (m_CurrTok == tok_array) { // array type
        getNextToken();
        validateToken(tok_leftBracket);
        getNextToken();

        auto lower = parseNumberExpr();

        for (int i = 0; i < 2; ++i) {
            validateToken(tok_dot);
            getNextToken();
        }

        auto upper = parseNumberExpr();

        validateToken(tok_rightBracket);
        validateToken(tok_of);
        getNextToken();

        auto type = parseVarType();
        return std::make_shared<ASTArray>(std::move(lower), std::move(upper), type);
    }
    return nullptr;
}

/**
 * [var_declaration]
 * 'var' {identifier {',' identifier}* ':' [type] ';'}+
 */
std::vector<std::unique_ptr<ASTVariable>> Parser::parseVarDeclaration() {
    validateToken(tok_var);
    getNextToken();

    if (m_CurrTok == tok_identifier) {
        std::vector<std::unique_ptr<ASTVariable>> res;
        std::vector<std::string> var_name;

        getNextToken();

        while (m_CurrTok == tok_comma) {
            getNextToken();
            validateToken(tok_identifier);
            var_name.push_back(m_Lexer.identifierStr());
            getNextToken();
        }

        validateToken(tok_colon);
        getNextToken();

        auto type = parseVarType();
        validateToken(tok_semicolon);
        getNextToken();

        for (const std::string &name : var_name)
            res.emplace_back(std::make_unique<ASTVariable>(name, type));
        return res;
    }
    return std::vector<std::unique_ptr<ASTVariable>>();
}

/**
 * [const_declaration]
 * 'const' {identifier '=' value ';'}+
 */
std::vector<std::unique_ptr<ASTConstVariable>> Parser::parseConstVarDeclaration() {

    validateToken(tok_const);
    getNextToken();

    std::vector<std::unique_ptr<ASTConstVariable>> res;

    do {
        validateToken(tok_identifier);
        std::string name = m_Lexer.identifierStr();
        getNextToken();

        validateToken(tok_equal);
        getNextToken();

        validateToken(tok_number);
        res.emplace_back(std::make_unique<ASTConstVariable>(name, m_Lexer.numVal()));
        getNextToken();

        validateToken(tok_semicolon);
        getNextToken();
    } while (m_CurrTok == tok_identifier);


    return std::vector<std::unique_ptr<ASTConstVariable>>();
}

//Function
/**
 * [function_proto]
 * 'function' function_name '(' {var_name ':' [type] ';'}* ')' ':' [type] ';'
 */
std::unique_ptr<ASTFunctionPrototype> Parser::parseFunctionPrototype() {

    if (m_CurrTok != tok_function && m_CurrTok != tok_procedure)
        return nullptr;
    bool is_funct = m_CurrTok == tok_function ? true : false; // else proccedure, no return type
    getNextToken();

    validateToken(tok_identifier);
    std::string funct_name = m_Lexer.identifierStr();
    getNextToken();

    validateToken(tok_leftParenthesis);
    getNextToken();

    std::vector<std::unique_ptr<ASTVariable>> params;

    //pars params
    while (m_CurrTok == tok_identifier) {
        std::string param_name = m_Lexer.identifierStr();
        getNextToken();

        validateToken(tok_colon);
        auto type = parseVarType();

        params.push_back(std::make_unique<ASTVariable>(param_name, type));

        if (m_CurrTok != tok_semicolon)
            break;

        getNextToken();
    }

    validateToken(tok_rightParenthesis);
    getNextToken();

    std::shared_ptr<ASTVariableType> ret_type = nullptr;
    if (is_funct) {
        validateToken(tok_colon);
        getNextToken();

        ret_type = parseVarType();
    }

    validateToken(tok_semicolon);
    getNextToken();

    return std::make_unique<ASTFunctionPrototype>(funct_name, std::move(params), ret_type);
}

/**
 * [function_proto] {'forward' ';'}
 * [var_declaration]
 * [body_begin_end]
 */
std::unique_ptr<ASTFunction> Parser::parseFunction() {
    auto prototype = parseFunctionPrototype();

    std::vector<std::unique_ptr<ASTVariable>> loc_var;

    if (m_CurrTok == tok_forward) {
        getNextToken();
        validateToken(tok_semicolon);
        getNextToken();

        return std::make_unique<ASTFunction>(std::move(prototype), std::move(loc_var), nullptr);
    }

    while (m_CurrTok == tok_var) {
        auto vars = parseVarDeclaration();
        for (auto &v : vars)
            loc_var.push_back(std::move(v));
    }
    validateToken(tok_begin);

    auto body = parseBody();

    validateToken(tok_semicolon);
    getNextToken();

    return std::make_unique<ASTFunction>(std::move(prototype), std::move(loc_var), std::move(body));
}

