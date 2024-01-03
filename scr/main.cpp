#include <fstream>
#include "include/Generator.h"

Error errorHandler;
int main(int argc, char* argv[]){

    // if(argc < 2){
    //     std::cerr << "Specificare il file '.gfl'!" << std::endl;
    //     exit(1);
    // }
    
    // std::cin >> file_name;

    std::string file_name = "main.gfl";

    std::fstream file(file_name, std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return 1;
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    Tokenizer tokenizer(file_content);
    std::vector<Token> tokens = tokenizer.tokenize(errorHandler);

    for(auto t : tokens){
        std::cout << t.type << std::endl;
    }

    Parser parser(tokens);
    std::optional<NodeProg> tree = parser.parse_program(errorHandler);

    Generator generator(tree.value());
    std::fstream out_file("./out.asm", std::ios::out);
    out_file << generator.gen_prog();
    return 0;
}