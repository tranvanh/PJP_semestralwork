#include "Parser.hpp"

//Use tutorials in: https://llvm.org/docs/tutorial/

int main(int argc, char *argv[]) {

    std::string input_file;
    std::string output_file = "output.o";

    if(argc != 2){
        printf("Usage: %s <input_file>\n", argv[0]);
        return 2;
    }

    input_file = argv[1];


    Parser parser(input_file);

    try {
        std::unique_ptr<ASTProgram> parsed_program(parser.Parse());
        parsed_program -> runCodegen(output_file);
    } catch (const char *exception) {
        printf("Error while compiling %s\n", input_file.c_str());
        printf("%s\n", exception);
        return 2;
    }


    return 0;
}
