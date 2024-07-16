#include <iostream>
#include <cstring>
#include <string>
#include <filesystem>
#include <fstream>
#include "utils/Functions.hpp"

__declspec(dllexport)
const std::string StaticFunction::changeFileExtension(
	const std::string& filename,
	const std::string& newExtension)
{
	std::filesystem::path filePath(filename);
	
	if (filePath.has_extension())
		filePath.replace_extension(newExtension);
	else
		filePath += newExtension;

	return filePath.string();
}

__declspec(dllexport)
Arguments StaticFunction::parse_arguments(
	int argc,
	char* argv[])
{
	Arguments args;
	for (int i = 1; i < argc; ++i)
	{
		if (std::strcmp(argv[i], "__version__") == 0)
			args.version_flag = true;
		else if (std::strcmp(argv[i], "-t") == 0)
			args.time_flag = true;
		else if (std::strcmp(argv[i], "-d") == 0)
			args.debug_flag = true;
		else if (std::strcmp(argv[i], "-f") == 0)
		{
			++i;
			args.location = argv[i];
		}
		else if (std::strcmp(argv[i], "-lex") == 0)
		{
			if (i + 2 >= argc)
			{
				std::cerr << "Error: Missing arguments for -lex option." << std::endl;
				exit(EXIT_FAILURE);
			}
			++i;
			args.save_tokens = true;
			args.file_to_change = argv[i];
			++i;
			args.output_of_file = argv[i];
			return args;
		}
		else if (std::strcmp(argv[i], "-parse") == 0)
		{
			if (i + 2 >= argc)
			{
				std::cerr << "Error: Missing arguments for -parse option." << std::endl;
				exit(EXIT_FAILURE);
			}
			++i;
			args.save_ast = true;
			args.file_to_change = argv[i];
			++i;
			args.output_of_file = argv[i];
			return args;
		}
		else if (std::strcmp(argv[i], "-compile") == 0)
		{
			if (i + 1 >= argc)
			{
				std::cerr << "Error: Missing arguments for -compile option." << std::endl;
				exit(EXIT_FAILURE);
			}
			++i;
			args.compile_ast = true;
			args.file_to_compile = argv[i];
			return args;
		}
		else if (args.filename.empty())
			args.filename = argv[i];
	}
	return args;
}

__declspec(dllexport)
const std::string StaticFunction::getScriptOnFile(
	const std::string& filename,
	bool important)
{
	std::ifstream inputFile(filename);
	if (!inputFile)
		throw std::runtime_error("File does not exist: " + filename);

	std::string line;
	std::ostringstream fileString;
	while (std::getline(inputFile, line))
		fileString << line << '\n';

	inputFile.close();
	return fileString.str();
}

__declspec(dllexport)
const std::string StaticFunction::getFileExtension(
	const std::string& filename)
{
	std::filesystem::path filePath(filename);
	return filePath.extension().string();
}

