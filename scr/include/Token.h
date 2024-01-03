#pragma once

#include <optional>
#include <string>
#include <vector>

#include "Errors.h"

enum TokenType
{
    LET,
    IDENT,
    EXIT,

    OPEN_PAREN,
    CLOSE_PAREN,
    SEMI,
    EQUAL,
    PLUS,
    STAR,

    INTEGER
};

struct Token
{
    TokenType type;
    std::optional<std::string> value;
};

class Tokenizer
{

    public:

        inline explicit Tokenizer(const std::string p_src) : src(p_src) {}

        inline std::vector<Token> tokenize(Error errorHandler){
            std::vector<Token> tokens;
            std::string buf;
            
            while(peek().has_value()){
                if(std::isalpha(peek().value())){
                    buf.push_back(consume());

                    while(peek().has_value() && std::isalnum(peek().value())){
                        buf.push_back(consume());
                    }

                    if(buf == "exit"){
                        tokens.push_back({.type = TokenType::EXIT});
                        buf.clear();
                    }

                    else if(buf == "let"){
                        tokens.push_back({.type = TokenType::LET});
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

                else if(peek().value() == '='){
                    consume();
                    tokens.push_back({.type = TokenType::EQUAL});
                    buf.clear();
                }

                else if(peek().value() == '+'){
                    consume();
                    tokens.push_back({.type = TokenType::PLUS});
                    buf.clear();
                }

                else if(peek().value() == '*'){
                    consume();
                    tokens.push_back({.type = TokenType::STAR});
                    buf.clear();
                }

                else if(peek().value() == ';'){
                    consume();
                    tokens.push_back({.type = TokenType::SEMI});
                    buf.clear();
                }

                else if(peek().value() == '('){
                    consume();
                    tokens.push_back({.type = TokenType::OPEN_PAREN});
                    buf.clear();
                }

                else if(peek().value() == ')'){
                    consume();
                    tokens.push_back({.type = TokenType::CLOSE_PAREN});
                    buf.clear();
                }

                else if(std::isspace(peek().value())){
                    consume();
                }

                else{
                    errorHandler.value = peek().value();
                    errorHandler.print_error(ErrorType::INVALID_SYNTAX);
                    exit(1);
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