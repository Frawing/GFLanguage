#pragma once

#include <iostream>

const inline void error_expected(const std::string msg, const int line = 0)
{
    if(line == 0){
        std::cerr << "[Parser Error] Expected `" << msg << "`" << line << std::endl;
    }else{
        std::cerr << "[Parser Error] Expected `" << msg << "` at line : " << line << std::endl;
    }
    exit(1);
}

const inline void error_invalid(const std::string msg, const int line = 0)
{
    if(line == 0){
        std::cerr << "[Parser Error] Invalid `" << msg << "`" << std::endl;
    }else{
        std::cerr << "[Parser Error] Invalid `" << msg << "` at line : " << line << std::endl;
    }
    exit(1);
}