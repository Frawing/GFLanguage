#include <fstream>
#include <iostream>
#include <cstring>
#include "include/Error.h"
#include "include/Generator.h"

int main(int argc, char* argv[])
{
    std::string file_name = "";
    std::string out_file_path = "./";
    bool print_tokens = false;

    for(int i = 1; i < argc; i++)
    {
        if(std::strcmp(argv[i], "-i") == 0){ // Input (file)
            file_name = argv[++i];
        }
        else if(std::strcmp(argv[i], "-op") == 0){ // Output Path
            out_file_path = argv[++i];
        }
        else if(std::strcmp(argv[i], "-dt") == 0) // Debug Tokens
        {
            i++;
            if (std::strcmp(argv[i], "true") == 0) {
                print_tokens = true;
            } else if (std::strcmp(argv[i], "false") == 0) {
                print_tokens = false;
            } else {
                std::cerr << "The Debug_Token parameter must be a boolean!" << std::endl;
                exit(1);
            }
        }
        else{
            std::cerr << "This parameter is invalid : " << argv[i] << "!" << std::endl;
            exit(1);
        }
    }

    std::fstream file(file_name, std::ios::in);
    if (!file.is_open() || file.fail()) {
        std::cerr << "Errore nell'apertura del file!" << std::endl;
        return 1;
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::cout << file_content << std::endl << std::endl;

    std::cout << std::endl;
    std::cout << "GFL FILE     : " << file_name << std::endl;
    std::cout << "OUTPUT DIR   : " << out_file_path << std::endl;
    std::cout << "PRINT TOKENS : " << print_tokens << std::endl << std::endl;

    Tokenizer tokenizer(file_content);
    std::vector<Token> tokens = tokenizer.tokenize();
    
    if(tokens.size() == 0){
        std::cerr << "The file is blank or" << std::endl;
        std::cerr << "there is some problem with the compiler. ";
        std::cerr << "Please retry some times..." << std::endl << std::endl;
        exit(1);
    }

    if(print_tokens){
        std::cout << "TOKENS : " << std::endl;
        for(auto t : tokens){
            std::cout << ToString(t.type) << std::endl;
        }
        std::cout << std::endl;
    }
    
    Parser parser(tokens);
    std::optional<NodeProg> tree = parser.parse_program();

    Generator generator(tree.value());
    std::fstream out_file(out_file_path + "out.asm", std::ios::out);
    out_file << generator.gen_prog();
    return 0;
}