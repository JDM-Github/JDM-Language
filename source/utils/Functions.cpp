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
const std::string StaticFunction::createNewFilename(const std::string &extension)
{
	std::string newFilename;
	std::cout << "Enter new filename or filepath: ";
	std::cin >> newFilename;
	return StaticFunction::changeFileExtension(newFilename, extension);
}

__declspec(dllexport)
const std::string StaticFunction::handleFileExists(
	const std::string &filename,
	const std::string &extension,
	const bool autoOverwrite)
{
	if (autoOverwrite)
	{
		if (!std::filesystem::remove(filename))
			throw std::runtime_error("Failed to delete the existing file: " + filename);
		return filename;
	}

	char choice;
	std::cout << "File already exists. Choose an option:\n"
		<< "1. Overwrite\n"
		<< "2. Change path\n"
		<< "3. Cancel\n"
		<< "Enter your choice (1/2/3): ";
	std::cin >> choice;

	switch (choice)
	{
	case '1':
		if (!std::filesystem::remove(filename))
			throw std::runtime_error("Failed to delete the existing file: " + filename);
		return filename;
	case '2':
		return StaticFunction::createNewFilename(extension);
	case '3':
		std::cout << "Operation canceled.\n";
		exit(EXIT_SUCCESS);
	default:
		std::cout << "Invalid choice. Operation canceled.\n";
		exit(EXIT_SUCCESS);
	}
}

__declspec(dllexport)
Arguments StaticFunction::parse_arguments(
	int argc,
	char* argv[])
{
	Arguments args;
	args.argc = argc;
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
		else if (std::strcmp(argv[i], "/O") == 0)
		{
			args.overwrite = true;
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

