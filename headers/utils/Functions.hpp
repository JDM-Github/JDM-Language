#pragma once
#include "struct/Arguments.hpp"

class __declspec(dllexport) StaticFunction
{
public:
	__declspec(dllexport)
	static const std::string changeFileExtension(const std::string& filename, const std::string& newExtension);

	__declspec(dllexport)
	static Arguments parse_arguments(int argc, char* argv[]);

	__declspec(dllexport)
	static const std::string getScriptOnFile(const std::string& filename, bool important=false);

	__declspec(dllexport)
	static const std::string getFileExtension(const std::string& filename);
};
