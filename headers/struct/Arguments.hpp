#pragma once

struct Arguments
{
	int argc = 0;
	std::string file_to_change  = "";
	std::string output_of_file  = "";
	std::string file_to_compile = "";

	std::string location = "";
	std::string filename = "";

	bool time_flag    = false;
	bool debug_flag   = false;
	bool version_flag = false;

	bool overwrite    = false;
	bool save_tokens  = false;
	bool save_ast     = false;
	bool compile_ast  = false;
};
