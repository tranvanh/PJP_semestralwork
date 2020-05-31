#ifndef PJPPROJECT_LEXER_HPP
#define PJPPROJECT_LEXER_HPP

#include <iostream>
#include <fstream>
#include <map>


/*
 * Lexer returns tokens [0-255] if it is an unknown character,
 * otherwise one of these for known things.
 * Here are all valid tokens:
 */

enum Token {
    tok_eof = -1,

    // numbers and identifiers

    tok_identifier = -2,
    tok_number = -3,

    // keywords

    tok_begin = -4,
    tok_end = -5,
    tok_const = -6,
    tok_procedure = -7,
    tok_forward = -8,
    tok_function = -9,
    tok_if = -10,
    tok_then = -11,
    tok_else = -12,
    tok_program = -13,
    tok_while = -14,
    tok_exit = -15,
    tok_var = -16,
    tok_integer = -17,
    tok_for = -18,
    tok_do = -19,

    // 2-character operators

    tok_notequal = -20,
    tok_lessequal = -21,
    tok_greaterequal = -22,
    tok_assign = -23,
    tok_or = -24,

    // 3-character operators (keywords)

    tok_mod = -25,
    tok_div = -26,
    tok_not = -27,
    tok_and = -28,
    tok_xor = -29,

    // keywords in for loop

    tok_to = -30,
    tok_downto = -31,

    // data types

    tok_string = -32,

    tok_plus = -33,
    tok_minus = -34,
    tok_multiply = -35,
    tok_equal = -36,
    tok_notEqual = -37,
    tok_less = -38,
    tok_lessEqual = -39,
    tok_greater = -40,
    tok_greaterEqual = -41,
    tok_leftParenthesis = -42,
    tok_rightParenthesis = -43,
    tok_leftBracket = -44,
    tok_rightBracket = -45,
    tok_comma = -47,
    tok_semicolon = -48,
    tok_dot = -49,
    tok_colon = -50,

    tok_error = -51
};


class Lexer {
public:
    Lexer() = default;

    Lexer(const std::string &input_file);

    ~Lexer() = default;

    int getToken();

    const std::string &identifierStr() const { return this->m_IdentifierStr; }

    const std::string &strVal() const { return this->m_StrVal; }

    int numVal() { return this->m_NumVal; }

private:
    std::ifstream is;
    std::string m_IdentifierStr; // variable name - tok_identifier
    int m_NumVal;                // tok_number
    std::string m_StrVal;
    std::map<std::string, Token> key_words{
            {"begin", tok_begin},
            {"end", tok_end},
            {"const", tok_const},
            {"procedure", tok_procedure},
            {"forward", tok_forward},
            {"function", tok_function},
            {"if", tok_if},
            {"then", tok_then},
            {"else", tok_else},
            {"program", tok_program},
            {"while", tok_while},
            {"exit", tok_exit},
            {"var", tok_var},
            {"integer", tok_integer},
            {"for", tok_for},
            {"do", tok_do}
    };
};


#endif //PJPPROJECT_LEXER_HPP
