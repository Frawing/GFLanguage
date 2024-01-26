#include "Token.h"

std::vector<Token> Tokenizer::tokenize()
{
    std::vector<Token> tokens;
    std::string buf;
    int line_count = 1;

    while(peek().has_value())
    {
        /* Strings */
        if(peek().value() == '"'){
            consume();
            while (peek().has_value() && peek().value() != '"')
            {
                buf.push_back(consume());
            }
            consume();
            tokens.push_back({ TokenType::STRING, line_count, buf });
            buf.clear();
        }

        /* New Line */
        else if(peek().value() == '\n'){
            line_count++;
            consume();
        }
        
        /* KeyWords */
        else if(std::isalpha(peek().value()))
        {
            buf.push_back(consume());

            while(peek().has_value() && (std::isalnum(peek().value()) || peek().value() == '_')){
                buf.push_back(consume());
            }

            if(buf == "let"){
                tokens.push_back({ TokenType::LET, line_count });
                buf.clear();
            }
            
            else if(buf == "if"){
                tokens.push_back({ TokenType::IF, line_count });
                buf.clear();
            }
            else if(buf == "elif"){
                tokens.push_back({ TokenType::ELIF, line_count });
                buf.clear();
            }
            else if(buf == "else"){
                tokens.push_back({ TokenType::ELSE, line_count });
                buf.clear();
            }

            else if(buf == "exit"){
                tokens.push_back({ TokenType::EXIT, line_count });
                buf.clear();
            }
            else if(buf == "println"){
                tokens.push_back({ TokenType::PRINTLN, line_count });
                buf.clear();
            }
            else if(buf == "print"){
                tokens.push_back({ TokenType::PRINT, line_count });
                buf.clear();
            }
            else if(buf == "input"){
                tokens.push_back({ TokenType::INPUT, line_count });
                buf.clear();
            }

            else{
                tokens.push_back({ TokenType::IDENT, line_count, buf});
                buf.clear();
            }
        }
        
        /* Numbers */
        else if(std::isdigit(peek().value()))
        {
            buf.push_back(consume());

            while(peek().has_value() && std::isdigit(peek().value()))
            {
                buf.push_back(consume());
            }
            
            tokens.push_back({ TokenType::INTEGER, line_count, buf});
            buf.clear();
        }

        /* Ignore */
        else if(std::isspace(peek().value())){
            consume();
        }

        /* Comments */
        else if(peek().value() == '#')
        {
            if(peek(1).has_value() && peek(1).value() == '!')
            {
                consume();
                consume();
                
                while(peek().has_value() && peek().value() != '\n')
                {
                    consume();
                }
            }
        }
        else if(peek().value() == '&')
        {
            if(peek(1).has_value() && peek(1).value() == '*')
            {
                consume();
                consume();
                
                while(peek().has_value())
                {
                    if(peek().value() == '*' && peek(1).has_value() && peek(1).value() == '&'){
                        break;
                    }
                    consume();
                }
                if(peek().has_value()){
                    consume();
                }
                if(peek().has_value()){
                    consume();
                }
            }
        }

        /* Chars */
        else
        {
            switch (peek().value())
            {
                case '>':
                    consume();
                    tokens.push_back({ TokenType::MAJOR, line_count });
                    buf.clear();
                    break;

                case '<':
                    consume();
                    tokens.push_back({ TokenType::MINOR, line_count });
                    buf.clear();
                    break;

                case '(':
                    consume();
                    tokens.push_back({ TokenType::OPEN_PAREN, line_count });
                    buf.clear();
                    break;

                case ')':
                    consume();
                    tokens.push_back({ TokenType::CLOSE_PAREN, line_count });
                    buf.clear();
                    break;
                
                case '{':
                    consume();
                    tokens.push_back({ TokenType::OPEN_CURLY, line_count });
                    buf.clear();
                    break;

                case '}':
                    consume();
                    tokens.push_back({ TokenType::CLOSE_CURLY, line_count });
                    buf.clear();
                    break;

                case '+':
                    consume();
                    tokens.push_back({ TokenType::PLUS, line_count });
                    buf.clear();
                    break;

                case '*':
                    consume();
                    tokens.push_back({ TokenType::STAR, line_count });
                    buf.clear();
                    break;

                case '-':
                    consume();
                    tokens.push_back({ TokenType::MINUS, line_count });
                    buf.clear();
                    break;

                case '/':
                    consume();
                    tokens.push_back({ TokenType::SLASH, line_count });
                    buf.clear();
                    break;

                case '=':
                    consume();
                    if(peek().has_value() && peek().value() == '='){
                        consume();
                        tokens.push_back({ TokenType::DBL_EQUAL, line_count });
                        buf.clear();
                        break;
                    }
                    tokens.push_back({ TokenType::EQUAL, line_count });
                    buf.clear();
                    break;

                case ';':
                    consume();
                    tokens.push_back({ TokenType::SEMI, line_count });
                    buf.clear();
                    break;

                case '!':
                    if(peek(1).has_value() && peek(1).value() == '='){
                        consume();
                        consume();
                        tokens.push_back({ TokenType::NOT_EQUAL, line_count });
                        buf.clear();
                        break;
                    }

                default:
                    std::cerr << "Invalid syntax : '" << peek().value() << "'!" << std::endl;
                    exit(1);
            }
        }
    }

    index = 0;
    return tokens;
}