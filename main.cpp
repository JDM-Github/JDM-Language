#include "headers/lexer.hpp"
#include "headers/parser.hpp"
#include "headers/compiler.hpp"

using namespace std;

class FileNotExistError : public runtime_error {
public:
    FileNotExistError(const string& filename)
        : runtime_error("File does not exist: " + filename) {}
};

const string getScriptOnFile(const string &filename) {
    ifstream inputFile(filename);
    if (!inputFile) throw FileNotExistError(filename);

    string line;
    ostringstream fileString;
    while (getline(inputFile, line))
        fileString << line << '\n';

    inputFile.close();
    return fileString.str();
}

int main(int argc, char* argv[]) {

    if (argc <= 1 || strcmp(argv[1], "__version__") == 0) {
        cout << " >> JDM Language Version 1.0";
        exit(EXIT_SUCCESS);
    }
    string filename = argv[1];
    const string extension = ".jdm";

    size_t dotPos = filename.find_last_of(".");
    if (dotPos == string::npos)
    {
        if (!(filename.length() >= extension.length() &&
            filename.compare(filename.length() - extension.length(), extension.length(), extension) == 0)) {
            filename += ".jdm";
        }
    }

    string fileBuffer;
    auto start = chrono::high_resolution_clock::now();
    try
    {
        fileBuffer  = getScriptOnFile("C:/JDMWeb/JDM-Language/scripts/__native__.jdm");
        fileBuffer += getScriptOnFile(filename);
    }
    catch (const FileNotExistError &error)
    {
        cout << error.what() << endl;
        exit(EXIT_SUCCESS);
    }
    unique_ptr<Tokenizer> newTokenizer;
    unique_ptr<Parser> newParser;
    unique_ptr<Compiler> newCompiler;

    try
    {
        newTokenizer = make_unique<Tokenizer>(filename, fileBuffer);
        // newTokenizer->analyzeAllTokens(true);
    }
    catch (const JDMTokenizingHandler& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_SUCCESS);
    }

    // cout << "\n\nParse AST:\n";
    try
    {
        newParser = make_unique<Parser>(newTokenizer->getTokens());
    }
    catch (const runtime_error& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_SUCCESS);
    }

    // cout << "\nCompiler:\n";
    try
    {
        newCompiler = make_unique<Compiler>(newParser->getAST());
    }
    catch (const runtime_error& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_SUCCESS);
    }

    auto stop = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
    cout << "\n\nTime taken by function: " << duration.count() << " milliseconds" << endl;
}
