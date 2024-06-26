#pragma once
#include "../higherObject.hpp"

class JDM_DLL Comparator
{
public:
	JDM_DLL static const bool DoOperationString(
		const std::string &first,
		const std::string &second,
		const std::string &operation="");

	JDM_DLL static const bool DoOperationList(
		std::shared_ptr<HigherObject> &firstVal,
		const std::shared_ptr<HigherObject> &secondVal = nullptr,
		const std::string &operation = "");

	JDM_DLL static const bool DoOperationMap(
		std::shared_ptr<HigherObject> &firstVal,
		const std::shared_ptr<HigherObject> &secondVal = nullptr,
		const std::string &operation = "");

	JDM_DLL static const bool DoOperationFunc(
		std::shared_ptr<HigherObject> &firstVal,
		const std::shared_ptr<HigherObject> &secondVal = nullptr,
		const std::string &operation = "");

	JDM_DLL static const bool DoOperationObject(
		std::shared_ptr<HigherObject> &firstVal,
		const std::shared_ptr<HigherObject> &secondVal = nullptr,
		const std::string &operation = "");

	JDM_DLL static const bool DoOperationNumber(
		double firstVal,
		double secondVal,
		const std::string &operation = "");
};
