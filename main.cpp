#include "headers/lexer.hpp"
#include "headers/parser.hpp"
#include <memory>
#include <cstring>
#include <stdexcept>
#include <chrono>

class FileNotExistError : public std::runtime_error {
public:
    FileNotExistError(const std::string& filename)
        : std::runtime_error("File does not exist: " + filename) {}
};

const std::string getScriptOnFile(const std::string &filename) {
    std::ifstream inputFile(filename);
    if (!inputFile) throw FileNotExistError(filename);

    std::string line;
    std::ostringstream fileString;
    while (std::getline(inputFile, line))
        fileString << line << '\n';

    inputFile.close();
    return fileString.str();
}

int main(int argc, char* argv[]) {

    if (argc <= 1 || strcmp(argv[1], "__version__") == 0) {
        std::cout << " >> JDM Language Version 1.0";
        std::exit(EXIT_SUCCESS);
    }
    std::string filename = argv[1];
    const std::string extension = ".jdm";

    size_t dotPos = filename.find_last_of(".");
    if (dotPos == std::string::npos) { // If there is no dot
        if (!(filename.length() >= extension.length() &&
            filename.compare(filename.length() - extension.length(), extension.length(), extension) == 0)) {
            filename += ".jdm";
        }
    }

    std::string fileBuffer;
    auto start = std::chrono::high_resolution_clock::now();
    try {
        fileBuffer = getScriptOnFile(filename);
    } catch (const FileNotExistError &error) { // Custom Exception
        std::cout << error.what() << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    std::unique_ptr<Tokenizer> newTokenizer;
    std::unique_ptr<Parser> newParser;

    try {
        newTokenizer = std::make_unique<Tokenizer>(filename, getScriptOnFile(filename));
        newTokenizer->analyzeAllTokens(true);
    } catch (const JDMTokenizingHandler& error) {
        std::cerr << error.what() << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    std::cout << "\n\nParse AST:\n";
    try {
        newParser = std::make_unique<Parser>(newTokenizer->getTokens());
    } catch (const std::runtime_error& error) {
        std::cerr << error.what() << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    // try {
    //     Compiler newCompiler = Compiler(newParser.getAST());
    // } catch (const JDMCompilerHandler& error) {
    //     std::cerr << error.what() << std::endl;
    //     std::exit(EXIT_SUCCESS);
    // }

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Time taken by function: " << duration.count() << " milliseconds" << std::endl;
}
