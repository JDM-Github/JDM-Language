#include "headers/Lexer.hpp"
#include "headers/Parser.hpp"
#include "headers/Compiler.hpp"

namespace fs = std::filesystem;
using namespace std;
namespace JDM
{
	Tokenizer *tokenize(const std::string &filename, const std::string &output="", bool willSave=false, bool overwrite=false)
	{
		Tokenizer *tokenizer = nullptr;
		if (StaticFunction::getFileExtension(filename) == ".jdmt")
		{
			if (willSave)
			{
				Log << "File is already a JDMT. Please tokenize JDM script.";
				exit(EXIT_SUCCESS);
			}
			tokenizer = new Tokenizer();
			tokenizer->loadTokens(filename);
			return tokenizer;
		}
		tokenizer = new Tokenizer(filename, StaticFunction::getScriptOnFile(filename));
		tokenizer->tokenize();
		if (willSave) tokenizer->saveTokens(StaticFunction::changeFileExtension(output, ".jdmt"), overwrite);
		return tokenizer;
	}

	Parser *parse(const std::string &filename, const std::string &output="", bool willSave=false, bool overwrite=false)
	{
		Parser *parser = nullptr;
		if (StaticFunction::getFileExtension(filename) == ".jdms")
		{
			if (willSave)
			{
				Log << "File is already a JDMS. Please parse JDM script.";
				exit(EXIT_SUCCESS);
			}
			parser = new Parser();
			parser->loadBlock(filename);
			return parser;
		}
		Tokenizer *tokenizer = tokenize(filename);
		parser = new Parser(tokenizer->getTokens());
		parser->analyzeAST(parser->getAST());
		if (willSave) parser->saveBlock(StaticFunction::changeFileExtension(output, ".jdms"), overwrite);

		delete tokenizer;
		return parser;
	}

	void compile(const std::string &filename)
	{
		const string extension = ".jdm";

		auto newFile = filename;
		size_t dotPos = newFile.find_last_of(".");
		if (dotPos == string::npos)
		{
			if (!(newFile.length() >= extension.length()
			   && newFile.compare(newFile.length() - extension.length(),
			   	  extension.length(), extension) == 0))
				newFile += ".jdm";
		}

		Parser *parser = parse(newFile);
		auto ast = parser->getAST();
		delete parser;

		Compiler compiler = Compiler(ast);
	}

	void handleArgs(const Arguments &args)
	{
		try
		{
			if (args.save_tokens)
				tokenize(args.file_to_change, args.output_of_file, true, args.overwrite);
			else if (args.save_ast)
				parse(args.file_to_change, args.output_of_file, true, args.overwrite);
			else if (args.compile_ast)
				compile(args.file_to_compile);

			else if (args.argc <= 1 || args.version_flag || args.filename.compare("") == 0)
			{
				cout << " >> JDM Language Version 1.0" << endl;
				cout << "Executable path: " << args.location << "\\JDM.exe" << endl;
				exit(EXIT_SUCCESS);
			}
			else compile(args.filename);
		}
		catch (const JDMTokenizingHandler& error)
		{
			cerr << error.what() << endl;
			exit(EXIT_SUCCESS);
		}
		catch (const FileNotExistError &error)
		{
			cout << error.what() << endl;
			exit(EXIT_SUCCESS);
		}
		catch (const runtime_error& error)
		{
			cerr << error.what() << endl;
			exit(EXIT_SUCCESS);
		}
	}

	void startTimer(const Arguments &args)
	{
		auto start = chrono::high_resolution_clock::now();
		handleArgs(args);

		auto stop = chrono::high_resolution_clock::now();
		auto duration = chrono::duration_cast<chrono::milliseconds>(stop - start);
		cout << "\nTime taken by function: " << duration.count() << " milliseconds" << endl;
	}	
}

int main(int argc, char* argv[])
{
	fs::create_directories("logs");
	Arguments args = StaticFunction::parse_arguments(argc, argv);
	if (args.time_flag || args.debug_flag)
		JDM::startTimer(args);
	else
		JDM::handleArgs(args);
}
