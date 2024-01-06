#include "Token.h"

std::vector<Token> Tokenizer::tokenize()
{
    std::vector<Token> tokens;
    std::string buf;
    int line_cout = 1;

    while(peek().has_value())
    {
        if(std::isalpha(peek().value()))
        {
            buf.push_back(consume());

            while(peek().has_value() && std::isalnum(peek().value())){
                buf.push_back(consume());
            }

            if(buf == "let"){
                tokens.push_back({ TokenType::LET, line_cout });
                buf.clear();
            }
            
            else if(buf == "if"){
                tokens.push_back({ TokenType::IF, line_cout });
                buf.clear();
            }
            else if(buf == "elif"){
                tokens.push_back({ TokenType::ELIF, line_cout });
                buf.clear();
            }
            else if(buf == "else"){
                tokens.push_back({ TokenType::ELSE, line_cout });
                buf.clear();
            }

            else if(buf == "exit"){
                tokens.push_back({ TokenType::EXIT, line_cout });
                buf.clear();
            }

            else{
                tokens.push_back({ TokenType::IDENT, line_cout, buf});
                buf.clear();
            }
        }
        
        else if(std::isdigit(peek().value()))
        {
            buf.push_back(consume());

            while(peek().has_value() && std::isdigit(peek().value()))
            {
                buf.push_back(consume());
            }
            
            tokens.push_back({ TokenType::INTEGER, line_cout, buf});
            buf.clear();
        }

        else if(std::isspace(peek().value())){
            consume();
        }

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

        else
        {
            switch (peek().value())
            {
                case '(':
                    consume();
                    tokens.push_back({ TokenType::OPEN_PAREN, line_cout });
                    buf.clear();
                    break;

                case ')':
                    consume();
                    tokens.push_back({ TokenType::CLOSE_PAREN, line_cout });
                    buf.clear();
                    break;
                
                case '{':
                    consume();
                    tokens.push_back({ TokenType::OPEN_CURLY, line_cout });
                    buf.clear();
                    break;

                case '}':
                    consume();
                    tokens.push_back({ TokenType::CLOSE_CURLY, line_cout });
                    buf.clear();
                    break;

                case '+':
                    consume();
                    tokens.push_back({ TokenType::PLUS, line_cout });
                    buf.clear();
                    break;

                case '*':
                    consume();
                    tokens.push_back({ TokenType::STAR, line_cout });
                    buf.clear();
                    break;

                case '-':
                    consume();
                    tokens.push_back({ TokenType::MINUS, line_cout });
                    buf.clear();
                    break;

                case '/':
                    consume();
                    tokens.push_back({ TokenType::SLASH, line_cout });
                    buf.clear();
                    break;

                case '=':
                    consume();
                    tokens.push_back({ TokenType::EQUAL, line_cout });
                    buf.clear();
                    break;

                case ';':
                    consume();
                    tokens.push_back({ TokenType::SEMI, line_cout });
                    buf.clear();
                    break;

                default:
                    std::cerr << "Invalid syntax : " << peek().value() << "!" << std::endl;
                    exit(1);
            }
        }
    }

    index = 0;
    return tokens;
}