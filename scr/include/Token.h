#pragma once

#include <optional>
#include <string>
#include <vector>

enum TokenType
{
    LET,
    IDENT,

    IF,

    EXIT,

    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_CURLY,
    CLOSE_CURLY,

    PLUS,
    MINUS,
    STAR,
    SLASH,

    EQUAL,
    SEMI,

    INTEGER
};

inline const std::string ToString(TokenType type)
{
    switch (type)
    {
        case TokenType::LET:
            return "LET";
            break;
        case TokenType::IDENT:
            return "IDENT";
            break;

        case TokenType::IF:
            return "IF";
            break;

        case TokenType::EXIT:
            return "EXIT";
            break;
        
        case TokenType::OPEN_PAREN:
            return "OPEN PAREN";
            break;
        case TokenType::CLOSE_PAREN:
            return "CLOSE PAREN";
            break;
        case TokenType::OPEN_CURLY:
            return "OPEN CURLY";
            break;
        case TokenType::CLOSE_CURLY:
            return "CLOSE CURLY";
            break;

        case TokenType::PLUS:
            return "PLUS";
            break;
        case TokenType::MINUS:
            return "MINUS";
            break;
        case TokenType::STAR:
            return "STAR";
            break;
        case TokenType::SLASH:
            return "SLASH";
            break;
        
        case TokenType::EQUAL:
            return "EQUAL";
            break;
        case TokenType::SEMI:
            return "SEMI";
            break;
        
        case TokenType::INTEGER:
            return "INTEGER";
            break;

        default:
            return "UNKNOW";
            break;
    }
}

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};

std::optional<int> bin_prec(TokenType type){
    switch (type)
    {
        case TokenType::STAR:
            return 1;
        case TokenType::SLASH:
            return 1;
        case TokenType::PLUS:
            return 0;
        case TokenType::MINUS:
            return 0;
        default:
            return {};
    }
}

class Tokenizer
{

    public:

        inline explicit Tokenizer(const std::string p_src) : src(p_src) {}

        inline std::vector<Token> tokenize(){
            std::vector<Token> tokens;
            std::string buf;
            
            while(peek().has_value()){
                if(std::isalpha(peek().value())){
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
                
                else if(std::isdigit(peek().value())){
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

                else{
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

                        case '=':
                            consume();
                            tokens.push_back({.type = TokenType::EQUAL});
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

    private:

        inline std::optional<char> peek(int offset = 0) const {
            if((index + offset) >= src.length()){
                return {};
            }else{
                return src.at(index + offset);
            }
        }

        inline char consume(){
            return src.at(index++);
        }

        const std::string src;
        size_t index;

};