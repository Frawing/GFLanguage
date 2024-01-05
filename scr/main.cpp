#include <fstream>
#include <iostream>
#include <cstring>
#include "include/Arena.h"
#include "include/Generator.h"

int main(int argc, char* argv[]){

    if(argc < 2){
        std::cerr << "Specificare il file '.gfl'!" << std::endl;
        exit(1);
    }

    std::string out_file_path = "./";
    if(argc >= 3){
        out_file_path = argv[2];
    }

    bool print_tokens = false;
    if(argc == 4){
        if (std::strcmp(argv[3], "true") == 0) {
            print_tokens = true;
        } else if (std::strcmp(argv[3], "false") == 0) {
            print_tokens = false;
        } else {
            std::cerr << "The third param must be a boolean!" << std::endl;
            exit(1);
        }
    }

    std::string file_name = argv[1];

    std::fstream file(file_name, std::ios::in);
    if (!file.is_open() || file.fail()) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return 1;
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    std::cout << std::endl;
    std::cout << "GFL FILE     : " << file_name << std::endl;
    std::cout << "OUTPUT DIR   : " << out_file_path << std::endl;
    std::cout << "PRINT TOKENS : " << print_tokens << std::endl << std::endl;

    Tokenizer tokenizer(file_content);
    std::vector<Token> tokens = tokenizer.tokenize();

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