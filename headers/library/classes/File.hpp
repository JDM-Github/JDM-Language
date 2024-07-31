#pragma once

class JDM_DLL FileClass : public BaseNativeClass
{
public:
	enum FileEnum
	{
		FFILE_OPEN,
		FFILE_CLOSE,
		FFILE_READ,
		FFILE_WRITE,
		FFILE_APPEND,
		FFILE_TRUNCATE,
		FFILE_REMOVE
	};
	JDM_DLL FileClass();
	JDM_DLL std::shared_ptr<HigherObject> constructor   (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> openFile      (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects, bool fromConstructor = false);
	JDM_DLL std::shared_ptr<HigherObject> closeFile     (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> removeFile    (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> readFile      (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> writeFile     (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> appendFile    (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> truncateFile  (std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
	JDM_DLL std::shared_ptr<HigherObject> manageFunction(int funcType, std::shared_ptr<HigherObject> &obj1, const std::vector<std::shared_ptr<HigherObject>> &objects);
};
