#include "headers/lexer.hpp"
#include "headers/parser.hpp"
#include "headers/compiler.hpp"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;

class FileNotExistError : public runtime_error {
public:
    FileNotExistError(const string& filename, bool important)
        : runtime_error(((important) ? string("[ IMPORTANT FILE MISSING ]: ") : string(""))
            + "File does not exist: " + filename) {}
};

const string getScriptOnFile(const string &filename, bool important=false) {
    ifstream inputFile(filename);
    if (!inputFile) throw FileNotExistError(filename, important);

    string line;
    ostringstream fileString;
    while (getline(inputFile, line))
        fileString << line << '\n';

    inputFile.close();
    return fileString.str();
}

struct Arguments
{
    string location = "";
    string filename = "";
    bool time_flag = false;
    bool debug_flag = false;
    bool version_flag = false;
};

Arguments parse_arguments(int argc, char* argv[])
{
    Arguments args;
    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "__version__") == 0)
            args.version_flag = true;

        else if (strcmp(argv[i], "-t") == 0)
            args.time_flag = true;

        else if (strcmp(argv[i], "-d") == 0)
            args.debug_flag = true;

        else if (strcmp(argv[i], "-f") == 0)
        {
            ++i;
            args.location = argv[i];
        }
        else if (args.filename.compare("") == 0)
            args.filename = argv[i];

    }
    return args;
}

int main(int argc, char* argv[])
{
    Arguments args = parse_arguments(argc, argv);
    if (argc <= 1 || args.version_flag || args.filename.compare("") == 0)
    {
        cout << " >> JDM Language Version 1.0" << endl;
        cout << "Executable path: " << args.location << "\\JDM.exe" << endl;
        exit(EXIT_SUCCESS);
    }

    bool time = args.time_flag || args.debug_flag;
    string filename = args.filename;
    const string extension = ".jdm";
    fs::create_directories("logs");

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
        fileBuffer  = getScriptOnFile(args.location + "/scripts/__native__.jdm", true);
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
        if (args.debug_flag)
            newTokenizer->analyzeAllTokens(false);
    }
    catch (const JDMTokenizingHandler& error)
    {
        cerr << error.what() << endl;
        exit(EXIT_SUCCESS);
    }

    try
    {
        newParser = make_unique<Parser>(newTokenizer->getTokens());
        if (args.debug_flag)
        {
            std::string logFilePath = "logs/parser.log";
            std::ofstream logFile(logFilePath, std::ios::trunc);
            if (!logFile.is_open())
            {
                std::cerr << "Unable to open log file: " << logFilePath << std::endl;
                exit(EXIT_SUCCESS);
            }
            logFile.close();
            newParser->analyzeAST(newParser->getAST());
        }
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

    if (time)
    {
        auto stop = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
        cout << "\nTime taken by function: " << duration.count() << " milliseconds" << endl;
    }
}

