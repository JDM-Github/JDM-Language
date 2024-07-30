#include "utils/HigherObject.hpp"
#include "library/classes/File.hpp"

JDM_DLL
FileClass::FileClass()
{
	this->mapFunctions["open"    ] = FileEnum::FFILE_OPEN; 
	this->mapFunctions["close"   ] = FileEnum::FFILE_CLOSE;
	this->mapFunctions["read"    ] = FileEnum::FFILE_READ;
	this->mapFunctions["write"   ] = FileEnum::FFILE_WRITE;
	this->mapFunctions["append"  ] = FileEnum::FFILE_APPEND;
	this->mapFunctions["truncate"] = FileEnum::FFILE_TRUNCATE;
	this->mapFunctions["remove"  ] = FileEnum::FFILE_REMOVE;
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::constructor(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (objects.size() > 1)
		throw std::runtime_error("Runtime Error: 'File' expects only one argument (file path).");

	obj1->objectValue->members["class" ] = std::make_shared<HigherObject>("File");
	obj1->objectValue->members["file"  ] = std::make_shared<HigherObject>("");
	obj1->objectValue->members["opened"] = std::make_shared<HigherObject>(false);
	if (objects.size() == 1)
		return this->openFile(obj1, objects, true);

	return obj1;
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::manageFunction(
	int funcType,
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	FileEnum newfuncType = static_cast<FileEnum>(funcType);
	if (newfuncType == FileClass::FileEnum::FFILE_OPEN    ) return this->openFile    (obj1, objects);
	if (newfuncType == FileClass::FileEnum::FFILE_CLOSE   ) return this->closeFile   (obj1, objects);
	if (newfuncType == FileClass::FileEnum::FFILE_READ    ) return this->readFile    (obj1, objects);
	if (newfuncType == FileClass::FileEnum::FFILE_REMOVE  ) return this->removeFile  (obj1, objects);
	if (newfuncType == FileClass::FileEnum::FFILE_WRITE   ) return this->writeFile   (obj1, objects);
	if (newfuncType == FileClass::FileEnum::FFILE_APPEND  ) return this->appendFile  (obj1, objects);
	if (newfuncType == FileClass::FileEnum::FFILE_APPEND  ) return this->appendFile  (obj1, objects);
	if (newfuncType == FileClass::FileEnum::FFILE_TRUNCATE) return this->truncateFile(obj1, objects);

	throw std::runtime_error("Runtime Error: This function is not a member of class 'File'.");
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::openFile(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects,
	bool fromConstructor)
{
	if (!fromConstructor && obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'File.open' method is not static.");

	if (objects.size() != 1)
		throw std::runtime_error("Runtime Error: 'File.open' expects one argument (file path).");

	if (objects[0]->getCurrActive() != ACTIVE_STRING)
		throw std::runtime_error("Runtime Error: 'File.open' expects the first argument to be file path.");

	std::string filePath = std::filesystem::absolute(objects[0]->stringValue).string();
	if (!std::filesystem::exists(filePath))
	{
		std::fstream fileStream(filePath, std::ios::out);
		fileStream.close();
	}

	if (obj1->objectValue->members["opened"]->booleanValue)
		throw std::runtime_error("Runtime Error: 'File' already opened '"
			+ obj1->objectValue->members["file"  ]->stringValue
			+ "'. Close the file first.");

	obj1->objectValue->members["file"  ]->stringValue  = filePath;
	obj1->objectValue->members["opened"]->booleanValue = true;
	return obj1;
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::closeFile(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
		throw std::runtime_error("Runtime Error: The 'File.close' method is not static.");

	if (objects.size() != 0)
		throw std::runtime_error("Runtime Error: 'File.close' expects no arguments for closing.");

	if (!obj1->objectValue->members["opened"]->booleanValue)
		return std::make_shared<HigherObject>(false);

	obj1->objectValue->members["opened"]->booleanValue = false;
	return std::make_shared<HigherObject>(true);
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::removeFile(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (obj1->objectValue->fromMainSource)
	{
		if (objects.size() != 1)
			throw std::runtime_error("Runtime Error: 'File.remove' expects one argument (file path).");

		if (objects[0]->getCurrActive() != ACTIVE_STRING)
			throw std::runtime_error("Runtime Error: 'File.remove' expects the argument to be a file path.");

		std::string filePath = std::filesystem::absolute(objects[0]->stringValue).string();
		if (!std::filesystem::exists(filePath))
			return std::make_shared<HigherObject>(false);

		if (std::remove(filePath.c_str()) != 0) 
			throw std::runtime_error("Runtime Error: Failed to remove file: " + filePath);

		return std::make_shared<HigherObject>(true);
	}

	if (objects.size() != 0)
		throw std::runtime_error("Runtime Error: 'File.remove' expects no arguments for removing the current file.");

	if (!obj1->objectValue->members["opened"]->booleanValue)
		throw std::runtime_error("Runtime Error: No file is currently open to remove.");

	std::string filePath = obj1->objectValue->members["file"]->stringValue;
	if (!std::filesystem::exists(filePath))
		return std::make_shared<HigherObject>(false);

	if (std::remove(filePath.c_str()) != 0) 
		throw std::runtime_error("Runtime Error: Failed to remove file: " + filePath);

	obj1->objectValue->members["opened"]->booleanValue = false;
	return std::make_shared<HigherObject>(true);
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::readFile(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (!obj1->objectValue->members["opened"]->booleanValue)
		throw std::runtime_error("Runtime Error: No file is currently open to read.");

	if (objects.size() > 1)
        throw std::runtime_error("Runtime Error: 'File.read' expects at least one argument (optional seek position).");

	std::string filePath = obj1->objectValue->members["file"]->stringValue;
	if (!std::filesystem::exists(filePath))
		throw std::runtime_error("Runtime Error: 'File.read' cannot read a file that does not exists.");

	std::fstream fileStream(filePath, std::ios::in);
	if (!fileStream.is_open())
		throw std::runtime_error("Runtime Error: File stream is not open.");

	if (objects.size() == 1)
    {
        if (objects[0]->getCurrActive() != ACTIVE_INTEGER)
            throw std::runtime_error("Runtime Error: 'File.read' optional seek argument must be an integer.");

        int64_t seekPosition = objects[0]->integerValue;
        fileStream.seekg(seekPosition, std::ios::beg);
        if (fileStream.fail())
            throw std::runtime_error("Runtime Error: Failed to seek to position " + std::to_string(seekPosition) + " in file.");
    }

	std::stringstream buffer;
	buffer << fileStream.rdbuf();
	fileStream.close();

	return std::make_shared<HigherObject>(buffer.str());
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::writeFile(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (!obj1->objectValue->members["opened"]->booleanValue)
		throw std::runtime_error("Runtime Error: No file is currently open to write.");

	if (objects.size() != 1)
		throw std::runtime_error("Runtime Error: 'File.write' expects one argument (data to write).");

	std::string filePath = obj1->objectValue->members["file"]->stringValue;
	std::ofstream outFile(filePath, std::ios::out | std::ios::trunc);
	if (!outFile.is_open())
		throw std::runtime_error("Runtime Error: Failed to open file for writing: " + filePath);

	std::string data = objects[0]->_getStringValue();
	outFile << data;
	outFile.close();
	return std::make_shared<HigherObject>(true);
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::appendFile(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (!obj1->objectValue->members["opened"]->booleanValue)
		throw std::runtime_error("Runtime Error: No file is currently open to write.");

	if (objects.size() < 1 || objects.size() > 2)
		throw std::runtime_error("Runtime Error: 'File.append' expects one or two arguments (data to append, optional seek position).");

	std::string filePath = obj1->objectValue->members["file"]->stringValue;
	if (!std::filesystem::exists(filePath))
		throw std::runtime_error("Runtime Error: 'File.append' cannot append a file that does not exists.");

	std::fstream fileStream(filePath, std::ios::in | std::ios::out | std::ios::app);
	if (!fileStream.is_open())
	{
		fileStream.open(filePath, std::ios::out);
		fileStream.close();
		fileStream.open(filePath, std::ios::in | std::ios::out | std::ios::app);
		if (!fileStream.is_open())
			throw std::runtime_error("Runtime Error: Failed to open or create file: " + filePath);
	}

	if (objects.size() == 2)
	{
		if (objects[1]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: 'File.append' expects the second argument to be an integer (seek position).");

		int64_t seekPosition = objects[1]->integerValue;
		fileStream.seekp(seekPosition, std::ios::beg);
		if (fileStream.fail())
			throw std::runtime_error("Runtime Error: Failed to seek to position " + std::to_string(seekPosition) + " in file.");
	}
	else
		fileStream.seekp(0, std::ios::end);

	fileStream << objects[0]->_getStringValue();
	if (fileStream.fail())
		throw std::runtime_error("Runtime Error: Failed to append to file.");
	fileStream.close();

	return std::make_shared<HigherObject>(true);
}

JDM_DLL
std::shared_ptr<HigherObject> FileClass::truncateFile(
	std::shared_ptr<HigherObject> &obj1,
	const std::vector<std::shared_ptr<HigherObject>> &objects)
{
	if (!obj1->objectValue->members["opened"]->booleanValue)
		throw std::runtime_error("Runtime Error: No file is currently open to truncate.");

	if (objects.size() > 1)
		throw std::runtime_error("Runtime Error: 'File.truncate' expects at least one argument (size to truncate).");

	std::string filePath = obj1->objectValue->members["file"]->stringValue;
	if (!std::filesystem::exists(filePath))
		throw std::runtime_error("Runtime Error: File '" + filePath + "' does not exist.");

	int64_t newSize = 0;
	if (objects.size() == 1)
	{
		if (objects[0]->getCurrActive() != ACTIVE_INTEGER)
			throw std::runtime_error("Runtime Error: 'File.truncate' expects the size argument to be an integer.");
		newSize = objects[0]->integerValue;
		if (newSize < 0) newSize = 0;
	}
	std::filesystem::resize_file(filePath, newSize);
	return std::make_shared<HigherObject>(true);
}
