#pragma once
#include "baseClasses.hpp"


// class TextFileWrapper {
// public:
// 	TextFileWrapper(const std::string& filename, const std::string& mode)
// 		: filename(filename), mode(mode) {
// 		file.open(filename, mode);
// 	}
// 	~TextFileWrapper() {
// 		if (file.is_open()) {
// 			file.close();
// 		}
// 	}
// 	std::ofstream& getFile() {
// 		return file;
// 	}

// 	friend std::ostream& operator<<(std::ostream& os, const TextFileWrapper& wrapper) {
// 		os << "<TextWrapper name='" << wrapper.filename
// 		   << "' mode='" << wrapper.mode << "' encoding='UTF-8'>";
// 		return os;
// 	}
// private:
// 	std::string filename;
// 	std::string mode;
// 	std::ofstream file;
// };

class FileClassFunctions : public BaseNativeClass {
public:
	enum FileFunction {
		file_openFile,
		file_closeFile
	};

	FileClassFunctions() {
		this->mapFunctions["open" ] = FileFunction::file_openFile;  // STATIC
		this->mapFunctions["close"] = FileFunction::file_closeFile; // STATIC
	}

	std::shared_ptr<HigherObject> constructor(
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects)
	{ std::cout << "File Class: " << obj1->objectValue  << std::endl; return obj1; }

	std::shared_ptr<HigherObject> manageFunction(
		int funcType,
		std::shared_ptr<HigherObject> &obj1,
		const std::vector<std::shared_ptr<HigherObject>> &objects) {
		std::shared_ptr<HigherObject> newReturn;

		std::cout << obj1->objectValue << std::endl;
		throw std::runtime_error("Runtime Error: This function is not a member of class FILEEE.");
		return newReturn;
	}

};
