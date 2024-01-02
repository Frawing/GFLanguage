#pragma once

#include <iostream>
#include <string>

enum class ErrorType{
    INVALID_SYNTAX,
    INVALID_EXPRESSION,
    MISSED,
    EXITCODE_MISSED,
};

struct Error{
    
    std::string value = "";

    inline void print_error(ErrorType type){
        if(type == ErrorType::INVALID_SYNTAX){
            if(value != ""){
                std::cerr << "Invalid syntax : " << std::endl;
                std::cerr << " => " << value << std::endl;
            }
            else{
                std::cerr << "Invalid syntax!" << std::endl;
            }
        }
        else if(type == ErrorType::INVALID_EXPRESSION){
            if(value != ""){
                std::cerr << "Invalid expression : " << std::endl;
                std::cerr << " => " << value << std::endl;
            }else{
                std::cerr << "Invalid expression!" << std::endl;
            }
        }

        else if(type == ErrorType::MISSED && value != ""){
            std::cerr << "Missed '" << value << "'" << std::endl;
        }
        else if(type == ErrorType::EXITCODE_MISSED){
            std::cerr << "Declare exit code!" << std::endl;
        }
    }

};