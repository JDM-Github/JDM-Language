#include "headers/lexer.hpp"
#include <memory>
#include <stdexcept>

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

int main() {
    std::string filename;
    while (true) {
        std::cout << " > ";
        std::cin >> filename;
        // try {
            Tokenizer newTokenizer = Tokenizer(filename, getScriptOnFile("scripts/"+filename));
            newTokenizer.printAllTokens(true, true);
        // } catch (const FileNotExistError &error1) {
        //     std::cout << error1.what() << std::endl;
        // } catch (const JDMTokenizingHandler& error2) {
        //     std::cout << error2.what() << std::endl;
        // }
    }
}
