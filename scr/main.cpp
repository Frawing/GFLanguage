#include <fstream>

#include "include/Generator.h"

Error errorHandler;

int main(int argc, char* argv[]){

    // if(argc < 2){
    //     std::cerr << "Specificare il file '.gfl'!" << std::endl;
    //     exit(1);
    // }
    std::string file_name = "../GFL/main.gfl";
    // std::cin >> file_name;

    std::string file_content;
    std::stringstream content_stream;
    std::fstream file(file_name, std::ios::in);
    content_stream << file.rdbuf();
    file_content = content_stream.str();

    Tokenizer tokenizer(file_content);
    std::vector<Token> tokens = tokenizer.tokenize(errorHandler);

    Parser parser(tokens);
    std::optional<NodeProg> tree = parser.parse_program(errorHandler);

    Generator generator(tree.value());

    std::fstream out_file("./out.asm", std::ios::out);
    out_file << generator.gen_prog();

    return 0;
}