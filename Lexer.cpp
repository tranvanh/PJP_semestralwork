#include "Lexer.hpp"

/*
 * Function to return the next token from standard input
 * the variable 'm_IdentifierStr' is set there in case of an identifier,
 * the variable 'm_NumVal' is set there in case of a number.
 */

Lexer::Lexer(const std::string &input_file) {
    is = std::ifstream(input_file, std::ifstream::in);
    is.clear();
}

Token Lexer::getToken() {

    static int current_char = ' ';

    while (isspace(current_char)) //skipping whitespaces
        current_char = is.get();

    // Identifier: starting with letter, follower by letters and numbers
    if (isalpha(current_char)) {
        m_IdentifierStr = current_char;

        while (isalnum(current_char = is.get()) || current_char == '_')
            m_IdentifierStr += current_char;

        auto it = key_words.find(m_IdentifierStr);
        return (it == key_words.end() ? tok_identifier : it->second);
    }

    // Decimal number
    if (isdigit(current_char)) {
        std::string number;
        number += current_char;
        while (isdigit(current_char = is.get()))
            number += current_char;

        m_NumVal = stoi(number);
        return tok_number;
    }

    // Octal number
    if (current_char == '&') {
        std::string number;
        while (isdigit(current_char = is.get())
               && (current_char - '0') < 8)
            number += current_char;

        m_NumVal = (int) strtol(number.c_str(), NULL, 8);
        return tok_number;
    }

    // Hexadecimal number
    if (current_char == '$') {
        std::string number;
        while (isdigit(current_char = is.get()))
            number += current_char;

        m_NumVal = (int) strtol(number.c_str(), NULL, 16);
        return tok_number;
    }

    // String
    if (current_char == '\'') {

        m_StrVal = "";
        while ((current_char = is.get() != '\'')) {
            if (current_char == EOF)
                return tok_eof;
            m_StrVal += current_char;
        }

        current_char = is.get();
        return tok_string;
    }

    // Comments (1 line)
    if (current_char == '#') {
        while (current_char != EOF
               && current_char != '\n'
               && current_char != '\r')
            current_char = is.get();

        if (current_char != EOF)
            return getToken();
    }

    // Tokens
    Token res_token = tok_error; // Incase of unidentificable token

    switch (current_char) {
        case '+' :
            res_token = tok_plus;
            break;
        case '-' :
            res_token = tok_minus;
            break;
        case '*' :
            res_token = tok_multiply;
            break;
        case '=' :
            res_token = tok_equal;
            break;
        case '<' :
            current_char = is.get();
            if (current_char == '>')
                res_token = tok_notEqual;
            else if (current_char == '=')
                res_token = tok_lessEqual;
            else {
                res_token = tok_less;
                is.putback(current_char);
            }
            break;
        case '>' :
            current_char = is.get();
            if (current_char == '=')
                res_token = tok_greaterEqual;
            else {
                res_token = tok_greater;
                is.putback(current_char);
            }
            break;
        case '(' :
            res_token = tok_leftParenthesis;
            break;
        case ')' :
            res_token = tok_rightParenthesis;
            break;
        case '[' :
            res_token = tok_leftBracket;
            break;
        case ']' :
            res_token = tok_rightBracket;
            break;
        case ':' :
            current_char = is.get();
            if (current_char == '=')
                res_token = tok_assign;
            else {
                res_token = tok_colon;
                is.putback(current_char);
            }
            break;
        case ',' :
            res_token = tok_comma;
            break;
        case ';' :
            res_token = tok_semicolon;
            break;
        case '.' :
            res_token = tok_dot;
            break;
    }


    if (current_char == EOF)
        return tok_eof;


    current_char = is.get(); //
    return res_token;
}

