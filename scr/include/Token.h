#pragma once

#include <iostream>
#include <optional>
#include <string>
#include <vector>

enum TokenType
{
    LET,
    IDENT,

    IF,
    ELIF,
    ELSE,

    EXIT,
    PRINT,
    PRINTLN,
    INPUT,

    OPEN_PAREN,
    CLOSE_PAREN,
    OPEN_CURLY,
    CLOSE_CURLY,

    PLUS,
    MINUS,
    STAR,
    SLASH,

    MAJOR,
    MINOR,
    EQUAL,
    DBL_EQUAL,
    NOT_EQUAL,
    SEMI,

    INTEGER,
    STRING
};

struct Token
{
    TokenType type;
    int line;
    std::optional<std::string> value;

    bool operator==(const Token &other){
        if(type == other.type && value == other.value){
            return true;
        }else{
            return false;
        }
    }
};

const inline std::string ToString(TokenType type)
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
        case TokenType::ELIF:
            return "ELIF";
            break;
        case TokenType::ELSE:
            return "ELSE";
            break;

        case TokenType::EXIT:
            return "EXIT";
            break;
        case TokenType::PRINT:
            return "PRINT";
            break;
        case TokenType::PRINTLN:
            return "PRINTLN";
            break;
        case TokenType::INPUT:
            return "INPUT";
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
        

        case TokenType::MAJOR:
            return "MAJOR";
            break;
        case TokenType::MINOR:
            return "MINOR";
            break;
        case TokenType::EQUAL:
            return "EQUAL";
            break;
        case TokenType::DBL_EQUAL:
            return "DOUBLE EQUAL";
            break;
        case TokenType::NOT_EQUAL:
            return "NOT EQUAL";
            break;
        case TokenType::SEMI:
            return "SEMI";
            break;
        
        case TokenType::INTEGER:
            return "INTEGER";
            break;
        case TokenType::STRING:
            return "STRING";
            break;

        default:
            return "UNKNOW";
            break;
    }
}

const inline std::optional<int> bin_prec(TokenType type)
{
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

template<typename T>
bool compareVectors(std::vector<T> vecA, std::vector<T> vecB)
{
    if(vecA.size() != vecB.size())
    {
        return false;
    }

    for(size_t i = 0; i < vecA.size(); i++)
    {
        if(vecA[i] != vecB[i]){
            return false;
        }
    }
  
    return true;
}

class Tokenizer
{
    public:

        inline explicit Tokenizer(const std::string p_src) : src(p_src) {}
        std::vector<Token> tokenize();

    private:

        const inline std::optional<char> peek(int offset = 0)
        {
            if((index + offset) >= src.length()){
                return {};
            }else{
                return src.at(index + offset);
            }
        }
        const inline char consume(){
            return src.at(index++);
        }

        const std::string src;
        size_t index;
};