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
    const char *filename = "script.jdm";
	std::unique_ptr<Tokenizer> newTokenizer = std::make_unique<Tokenizer>(filename, getScriptOnFile(filename));
	newTokenizer->printAllTokens(true, true);
}
