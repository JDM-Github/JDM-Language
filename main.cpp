#include "headers/Lexer.hpp"
#include "headers/Parser.hpp"
#include "headers/Compiler.hpp"

namespace fs = std::filesystem;
using namespace std;

void saveLexing(Arguments args)
{
	auto &filename = args.file_to_change;
	auto &output   = args.output_of_file;
	try
	{
		auto newTokenizer = make_unique<Tokenizer>(filename, StaticFunction::getScriptOnFile(filename));
		newTokenizer->tokenize();
		newTokenizer->saveTokens(StaticFunction::changeFileExtension(output, ".jdmt"));
	}
	catch (const JDMTokenizingHandler& error)
	{
		cerr << error.what() << endl;
		exit(EXIT_SUCCESS);
	}
	catch (const std::runtime_error& error)
	{
		cerr << error.what() << endl;
		exit(EXIT_SUCCESS);
	}
	Log << "Lexing completed successfully and tokens saved to " << output << endl;
	exit(EXIT_SUCCESS);
}

void saveParsing(Arguments args)
{
	auto &filename = args.file_to_change;
	auto &output   = args.output_of_file;
	std::shared_ptr<TokenStruct> tokens;

	try
	{
        if (StaticFunction::getFileExtension(filename) == ".jdmt")
        {
            auto newTokenizer = std::make_shared<Tokenizer>();
            newTokenizer->loadTokens(filename);
            tokens = newTokenizer->getTokens();
        }
        else
        {
            auto newTokenizer = std::make_shared<Tokenizer>(filename, StaticFunction::getScriptOnFile(filename));
            newTokenizer->tokenize();
            tokens = newTokenizer->getTokens();
        }
		auto newParser = make_unique<Parser>(tokens);
		newParser->saveBlock(StaticFunction::changeFileExtension(output, ".jdms"));
	}
	catch (const JDMTokenizingHandler& error)
	{
		cerr << error.what() << endl;
		exit(EXIT_SUCCESS);
	}
	catch (const runtime_error& error)
	{
		cerr << error.what() << endl;
		exit(EXIT_SUCCESS);
	}
	Log << "Parsing completed successfully and ast saved to " << output << endl;
	exit(EXIT_SUCCESS);
}

void compileParsed(Arguments args)
{
	auto start = chrono::high_resolution_clock::now();

	auto &script = args.file_to_compile;
	auto newParser = make_unique<Parser>();
	newParser->loadBlock(script);
	try {
		auto newCompiler = make_unique<Compiler>(newParser->getAST());
	}
	catch (const runtime_error& error) {
		cerr << error.what() << endl;
		exit(EXIT_SUCCESS);
	}

	auto stop = chrono::high_resolution_clock::now();
	auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
	cout << "\nTime taken by function: " << duration.count() << " milliseconds" << endl;
	exit(EXIT_SUCCESS);
}

// void handleCompile(Arguments args)
// {
// 	bool time = args.time_flag || args.debug_flag;
// 	string filename = args.filename;
// 	const string extension = ".jdm";
// 	fs::create_directories("logs");

// 	size_t dotPos = filename.find_last_of(".");
// 	if (dotPos == string::npos)
// 	{
// 		if (!(filename.length() >= extension.length() &&
// 			filename.compare(filename.length() - extension.length(), extension.length(), extension) == 0)) {
// 			filename += ".jdm";
// 		}
// 	}

// 	string fileBuffer;
// 	auto start = chrono::high_resolution_clock::now();
// 	try
// 	{
// 		fileBuffer  = StaticFunction::getScriptOnFile(args.location + "/scripts/__native__.jdm", true);
// 		fileBuffer += StaticFunction::getScriptOnFile(filename);
// 	}
// 	catch (const FileNotExistError &error)
// 	{
// 		cout << error.what() << endl;
// 		exit(EXIT_SUCCESS);
// 	}
// 	unique_ptr<Tokenizer> newTokenizer;
// 	unique_ptr<Parser   > newParser;
// 	unique_ptr<Compiler > newCompiler;

// 	future<void> tokenizerFuture;
// 	future<void> saveTokenFuture;
// 	future<void> parserFuture;

// 	try
// 	{
// 		if (0) {
// 			newTokenizer = make_unique<Tokenizer>();
// 			newTokenizer->loadTokens("saveTokens.bin");
// 		}
// 		else
// 		{
// 			newTokenizer = make_unique<Tokenizer>(filename, fileBuffer);
// 			newTokenizer->tokenize();
// 			if (args.debug_flag)
// 				tokenizerFuture = std::async(std::launch::async, &Tokenizer::analyzeAllTokens, newTokenizer.get(), false);

// 			if (args.save_tokens)
// 				saveTokenFuture = async(launch::async, &Tokenizer::saveTokens, newTokenizer.get(), "saveTokens.bin");
// 		}
// 	}
// 	catch (const JDMTokenizingHandler& error) {
// 		cerr << error.what() << endl;
// 		exit(EXIT_SUCCESS);
// 	} catch (const std::runtime_error& error) {
// 		cerr << error.what() << endl;
// 		exit(EXIT_SUCCESS);
// 	}

// 	try
// 	{
// 		newParser = make_unique<Parser>(newTokenizer->getTokens());
// 		if (args.debug_flag)
// 		{
// 			std::string logFilePath = "logs/parser.log";
// 			std::ofstream logFile(logFilePath, std::ios::trunc);
// 			if (!logFile.is_open())
// 			{
// 				std::cerr << "Unable to open log file: " << logFilePath << std::endl;
// 				exit(EXIT_SUCCESS);
// 			}
// 			logFile.close();
// 			parserFuture = std::async(std::launch::async, [&]() {
// 				newParser->analyzeAST(newParser->getAST());
// 			});
// 		}
// 	}
// 	catch (const runtime_error& error) {
// 		cerr << error.what() << endl;
// 		exit(EXIT_SUCCESS);
// 	}

// 	try {
// 		newCompiler = make_unique<Compiler>(newParser->getAST());
// 	}
// 	catch (const runtime_error& error) {
// 		cerr << error.what() << endl;
// 		exit(EXIT_SUCCESS);
// 	}

// 	if (time)
// 	{
// 		auto stop = chrono::high_resolution_clock::now();
// 		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
// 		cout << "\nTime taken by function: " << duration.count() << " milliseconds" << endl;
// 	}

// 	if (args.debug_flag)
// 	{
// 		if (saveTokenFuture.valid()) saveTokenFuture.get();
// 		if (tokenizerFuture.valid()) tokenizerFuture.get();
// 		if (parserFuture   .valid()) parserFuture   .get();
// 	}
// }

int main(int argc, char* argv[])
{
	Arguments args = StaticFunction::parse_arguments(argc, argv);
	if (args.save_tokens) saveLexing(args);
	if (args.save_ast   ) saveParsing(args);
	if (args.compile_ast) compileParsed(args);
	if (argc <= 1 || args.version_flag || args.filename.compare("") == 0)
	{
		cout << " >> JDM Language Version 1.0" << endl;
		cout << "Executable path: " << args.location << "\\JDM.exe" << endl;
		exit(EXIT_SUCCESS);
	}
	// handleCompile(args);
}
