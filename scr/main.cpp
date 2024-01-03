#include <fstream>
<<<<<<< HEAD
=======

>>>>>>> a3f0de8441e8d9e3af138c650e042a61a8dc3bc0
#include "include/Generator.h"

Error errorHandler;

int main(int argc, char* argv[]){

    // if(argc < 2){
    //     std::cerr << "Specificare il file '.gfl'!" << std::endl;
    //     exit(1);
    // }
<<<<<<< HEAD
    std::string file_name = "main.gfl";
    // std::cin >> file_name;

    std::fstream file(file_name, std::ios::in);

    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file." << std::endl;
        return 1;
    }

    std::string file_content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
=======
    std::string file_name = "../GFL/main.gfl";
    // std::cin >> file_name;

    std::string file_content;
    std::stringstream content_stream;
    std::fstream file(file_name, std::ios::in);
    content_stream << file.rdbuf();
    file_content = content_stream.str();
>>>>>>> a3f0de8441e8d9e3af138c650e042a61a8dc3bc0

    Tokenizer tokenizer(file_content);
    std::vector<Token> tokens = tokenizer.tokenize(errorHandler);

<<<<<<< HEAD
    for(auto t : tokens){
        std::cout << t.type << std::endl;
    }

=======
>>>>>>> a3f0de8441e8d9e3af138c650e042a61a8dc3bc0
    Parser parser(tokens);
    std::optional<NodeProg> tree = parser.parse_program(errorHandler);

    Generator generator(tree.value());

    std::fstream out_file("./out.asm", std::ios::out);
    out_file << generator.gen_prog();

    return 0;
}