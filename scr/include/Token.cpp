#include "Token.h"

std::vector<Token> Tokenizer::tokenize()
{
    std::vector<Token> tokens;
    std::string buf;
    
    while(peek().has_value())
    {
        if(std::isalpha(peek().value()))
        {
            buf.push_back(consume());

            while(peek().has_value() && std::isalnum(peek().value())){
                buf.push_back(consume());
            }

            if(buf == "let"){
                tokens.push_back({.type = TokenType::LET});
                buf.clear();
            }
            
            else if(buf == "if"){
                tokens.push_back({.type = TokenType::IF});
                buf.clear();
            }

            else if(buf == "exit"){
                tokens.push_back({.type = TokenType::EXIT});
                buf.clear();
            }

            else{
                tokens.push_back({.type = TokenType::IDENT, .value = buf});
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
            
            tokens.push_back({.type = TokenType::INTEGER, .value = buf});
            buf.clear();
        }
        else if(std::isspace(peek().value())){
            consume();
        }

        else
        {
            switch (peek().value())
            {
                case '(':
                    consume();
                    tokens.push_back({.type = TokenType::OPEN_PAREN});
                    buf.clear();
                    break;

                case ')':
                    consume();
                    tokens.push_back({.type = TokenType::CLOSE_PAREN});
                    buf.clear();
                    break;
                
                case '{':
                    consume();
                    tokens.push_back({.type = TokenType::OPEN_CURLY});
                    buf.clear();
                    break;

                case '}':
                    consume();
                    tokens.push_back({.type = TokenType::CLOSE_CURLY});
                    buf.clear();
                    break;

                case '+':
                    consume();
                    tokens.push_back({.type = TokenType::PLUS});
                    buf.clear();
                    break;

                case '*':
                    consume();
                    tokens.push_back({.type = TokenType::STAR});
                    buf.clear();
                    break;

                case '-':
                    consume();
                    tokens.push_back({.type = TokenType::MINUS});
                    buf.clear();
                    break;

                case '/':
                    consume();
                    tokens.push_back({.type = TokenType::SLASH});
                    buf.clear();
                    break;

                case '=':
                    consume();
                    tokens.push_back({.type = TokenType::EQUAL});
                    buf.clear();
                    break;

                case ';':
                    consume();
                    tokens.push_back({.type = TokenType::SEMI});
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