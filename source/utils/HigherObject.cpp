#include "utils/HigherObject.hpp"

JDM_DLL
HigherObject::HigherObject() {}

JDM_DLL
HigherObject::HigherObject(const std::string &value)
	: stringValue (value)
{
	currActive = ACTIVE_STRING;
	
	size_t found  = this->stringValue.find("\\n");
	while (found != std::string::npos)
	{
		this->stringValue.replace(found, 2, "\n");
		found = this->stringValue.find("\\n", found + 1);
	}
}

JDM_DLL
HigherObject::HigherObject(const int64_t value)
	: integerValue(value)
{
	currActive = ACTIVE_INTEGER;
}

JDM_DLL
HigherObject::HigherObject(const long double value)
	: doubleValue (value)
{
	currActive = ACTIVE_DECIMAL;
}

JDM_DLL
HigherObject::HigherObject(const bool value)
	: booleanValue(value)
{
	currActive = ACTIVE_BOOLEAN;
}

JDM_DLL
HigherObject::HigherObject(const std::shared_ptr<FunctionCall> &value)
	: funcValue(value)
{
	currActive = ACTIVE_FUNCTION;
}

JDM_DLL
HigherObject::HigherObject(const std::shared_ptr<ClassObject> &value)
	: objectValue(value)
{
	currActive = ACTIVE_OBJECT;
}

JDM_DLL
HigherObject::HigherObject(
	const std::vector<std::shared_ptr<HigherObject>> &value)
	: listValue(value)
{
	currActive = ACTIVE_LIST;
}

JDM_DLL
HigherObject::HigherObject(
	const std::unordered_map<std::shared_ptr<HigherObject>,
	std::shared_ptr<HigherObject>> &value)
	: mapValue(value)
{
	currActive = ACTIVE_MAP;
}

JDM_DLL
HigherObject::HigherObject(const std::shared_ptr<HigherObject> &obj)
{
	if (obj)
		this->setHigherObject(obj);
	else
	{
		this->integerValue = 0;
		currActive = ACTIVE_INTEGER;
	}
}

JDM_DLL
const void HigherObject::setHigherObject(const std::shared_ptr<HigherObject> &obj)
{
	this->isForcedConstraint = obj->isForcedConstraint;
	this->isConstant         = obj->isConstant;

	this->currActive = obj->getCurrActive();
			int index = 0;

	switch (this->currActive)
	{
		case ACTIVE_STRING  : this->stringValue  = obj->stringValue ; return;
		case ACTIVE_INTEGER : this->integerValue = obj->integerValue; return;
		case ACTIVE_DECIMAL : this->doubleValue  = obj->doubleValue ; return;
		case ACTIVE_BOOLEAN : this->booleanValue = obj->booleanValue; return;
		case ACTIVE_FUNCTION: this->funcValue    = obj->funcValue   ; return;
		case ACTIVE_OBJECT  : this->objectValue  = obj->objectValue ; return;
		case ACTIVE_LIST    :
			this->listValue.clear();
			for (const auto &item : obj->listValue)
			{
				if (item->getIsReferenced())
					this->listValue.push_back(item);
				else
					this->listValue.push_back(std::make_shared<HigherObject>(*item));
			}
			return;
		case ACTIVE_MAP     :
			this->mapValue.clear();
			for (const auto &pair : obj->mapValue)
			{
				auto newKey = std::make_shared<HigherObject>(*pair.first);
				auto newValue = std::make_shared<HigherObject>(*pair.second);
				this->mapValue[newKey] = newValue;
			}
			return;
	}
	std::runtime_error("Runtime Error: Invalid HigherObject...");
}

JDM_DLL
const DataTypeEnum HigherObject::getDatatypeEnum()
{
	switch (this->currActive)
	{
		case ACTIVE_STRING  : return DataTypeEnum::DATA_STRING;
		case ACTIVE_INTEGER : return DataTypeEnum::DATA_INTEGER;
		case ACTIVE_DECIMAL : return DataTypeEnum::DATA_DOUBLE;
		case ACTIVE_BOOLEAN : return DataTypeEnum::DATA_BOOLEAN;
		case ACTIVE_FUNCTION: return DataTypeEnum::DATA_LAMBDA;
		case ACTIVE_OBJECT  : return DataTypeEnum::DATA_OBJECT;
		case ACTIVE_LIST    : return DataTypeEnum::DATA_LIST;
		case ACTIVE_MAP     : return DataTypeEnum::DATA_MAP;
	}
	return DataTypeEnum::DATA_ANY;
}

JDM_DLL
const std::string HigherObject::getType()
{
	switch (this->currActive)
	{
		case ACTIVE_STRING  : return "STRING";
		case ACTIVE_INTEGER : return "INTEGER";
		case ACTIVE_DECIMAL : return "DECIMAL";
		case ACTIVE_BOOLEAN : return "BOOLEAN";
		case ACTIVE_FUNCTION: return "FUNCTION";
		case ACTIVE_OBJECT  : return "OBJECT";
		case ACTIVE_LIST    : return "LIST";
		case ACTIVE_MAP     : return "MAP";
	}
	return "UNKNOWN";
}

JDM_DLL
const void HigherObject::logValue()
{
	Log << this->_getStringValue();
}

JDM_DLL
const bool HigherObject::isNumber()
{
	auto active = this->currActive;
	return active == ACTIVE_INTEGER || active == ACTIVE_DECIMAL || active == ACTIVE_BOOLEAN;
}

JDM_DLL
const std::string HigherObject::_getStringValue()
{
	std::ostringstream oss;
	if (this->currActive == ACTIVE_STRING)
	{
		size_t found  = this->stringValue.find("\\n");
		while (found != std::string::npos)
		{
			this->stringValue.replace(found, 2, "\n");
			found = this->stringValue.find("\\n", found + 1);
		}
		oss << this->stringValue;
	}
	else if (this->currActive == ACTIVE_INTEGER)
	{
		oss << this->integerValue;
	}
	else if (this->currActive == ACTIVE_DECIMAL)
	{
		oss << this->doubleValue;
	}
	else if (this->currActive == ACTIVE_BOOLEAN)
	{
		oss << ((booleanValue) ? "jtrue" : "jfalse");
	}
	else if (this->currActive == ACTIVE_FUNCTION)
	{
		if (this->funcValue == nullptr)
			oss << "Function: Invalid.";
		else
			oss << "Function: " << funcValue;
	}
	else if (this->currActive == ACTIVE_OBJECT)
	{
		if (this->objectValue == nullptr)
			oss << "Class: Invalid.";
		else
			oss << "Class: " << objectValue;
	}
	else if (this->currActive == ACTIVE_LIST)
	{
		oss << "[ ";
		int index = 0;
		for (const auto &value : this->listValue)
		{
			oss << value->_getStringValue();
			if (index++ < this->listValue.size() - 1)
				oss << ", ";
		}
		oss << " ]";
	}
	else if (this->currActive == ACTIVE_MAP)
	{
		oss << "{ ";
		int index = 0;
		for (const auto &value : this->mapValue)
		{
			oss << value.first->_getStringValue();
			oss << " => ";
			oss << value.second->_getStringValue();
			if (index++ < this->mapValue.size() - 1)
				oss << ", ";
		}
		oss << " }";
	}
	else
	{
		oss << "None";
	}
	return oss.str();
}

JDM_DLL
const std::shared_ptr<HigherObject> HigherObject::isInMap(
	const std::shared_ptr<HigherObject> &obj,
	const bool getFirst)
{
	for (auto it = this->mapValue.begin(); it != this->mapValue.end(); ++it)
	{
		if (it->first->compareHigherObject(obj))
		{
			if (getFirst)
				return it->first;
			else
				return it->second;
		}
	}
	return nullptr;
}

JDM_DLL
const bool HigherObject::isInList(const std::shared_ptr<HigherObject> &obj)
{
	for (auto it = this->listValue.begin(); it != this->listValue.end(); ++it)
	{
		if (it->get()->compareHigherObject(obj))
			return true;
	}
	return false;
}

JDM_DLL
const void HigherObject::sortList()
{
	if (this->currActive != ACTIVE_LIST || this->isListSorted)
		return;

	std::vector<std::shared_ptr<HigherObject>> stringObject;
	std::vector<std::shared_ptr<HigherObject>> decimalObject;
	std::vector<std::shared_ptr<HigherObject>> listObject;
	std::vector<std::shared_ptr<HigherObject>> mapObject;
	std::vector<std::shared_ptr<HigherObject>> otherObject;
	for (const auto &obj : this->listValue)
	{
		auto cActive = obj->getCurrActive();
		if (cActive == ACTIVE_STRING )
		{
			stringObject.push_back(obj);
		}
		else if (cActive == ACTIVE_INTEGER || cActive == ACTIVE_DECIMAL || cActive == ACTIVE_BOOLEAN)
		{
			obj->castToDecimal();
			decimalObject.push_back(obj);
		}
		else if (cActive == ACTIVE_LIST)
		{
			listObject.push_back(obj);
		}
		else if (cActive == ACTIVE_MAP)
		{
			mapObject .push_back(obj);
		}
		else
		{
			otherObject.push_back(obj);
		}
	}
	std::sort(mapObject.begin(), mapObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
	{
		return a->mapValue.size() < b->mapValue.size();
	});
	std::sort(listObject.begin(), listObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
	{
		return a->listValue.size() < b->listValue.size();
	});
	std::sort(stringObject.begin(), stringObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
	{
		return a->stringValue < b->stringValue;
	});
	std::sort(decimalObject.begin(), decimalObject.end(),
		[](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
	{
		return a->doubleValue < b->doubleValue;
	});
	this->listValue.clear();
	this->listValue.insert(this->listValue.end(), mapObject    .begin(), mapObject    .end());
	this->listValue.insert(this->listValue.end(), listObject   .begin(), listObject   .end());
	this->listValue.insert(this->listValue.end(), stringObject .begin(), stringObject .end());
	this->listValue.insert(this->listValue.end(), decimalObject.begin(), decimalObject.end());
	this->listValue.insert(this->listValue.end(), otherObject  .begin(), otherObject  .end());
	this->isListSorted = true;
}

JDM_DLL
const bool HigherObject::compareHigherObject(const std::shared_ptr<HigherObject> &obj)
{
	if (this->currActive == ACTIVE_STRING && this->currActive == obj->currActive)
	{
		return this->stringValue == obj->stringValue;
	}
	else if (this->currActive == ACTIVE_INTEGER)
	{
		obj->castToDecimal();
		return this->integerValue == obj->doubleValue;
	}
	else if (this->currActive == ACTIVE_DECIMAL)
	{
		obj->castToDecimal();
		return this->doubleValue == obj->doubleValue;
	}
	else if (this->currActive == ACTIVE_BOOLEAN)
	{
		obj->castToDecimal();
		return this->booleanValue == obj->doubleValue;
	}
	else if (this->currActive == ACTIVE_FUNCTION && this->currActive == obj->currActive)
	{
		return this->funcValue == obj->funcValue && this->funcValue != nullptr;
	}
	else if (this->currActive == ACTIVE_OBJECT && this->currActive == obj->currActive)
	{
		return this->objectValue == obj->objectValue && this->objectValue != nullptr;
	}
	else if (this->currActive == ACTIVE_LIST && this->currActive == obj->currActive)
	{
		if (this->listValue.size() != obj->listValue.size())
			return false;

		for (int i = 0; i < this->listValue.size(); i++)
		{
			if (!this->listValue[i]->compareHigherObject( obj->listValue[i] ))
				return false;
		}
		return true;
	}
	else if (this->currActive == ACTIVE_MAP && this->currActive == obj->currActive)
	{
		if (this->mapValue.size() != obj->mapValue.size())
			return false;

		for (int i = 0; i < this->mapValue.size(); i++)
		{
			auto it1 = this->mapValue.begin();
			auto it2 =  obj->mapValue.begin();
			std::advance(it1, i);
			std::advance(it2, i);
			if (!it1->first ->compareHigherObject ( it2->first  )
			 || !it1->second->compareHigherObject( it2->second ))
				return false;
		}
		return true;
	}
	return false;
}

JDM_DLL
const void HigherObject::castToString()
{
	if (this->currActive == ACTIVE_STRING)
		return;

	else if (this->currActive == ACTIVE_INTEGER)
		this->stringValue = std::to_string(integerValue);
	else if (this->currActive == ACTIVE_DECIMAL)
		this->stringValue = std::to_string(doubleValue);
	else if (this->currActive == ACTIVE_BOOLEAN)
		this->stringValue = (booleanValue) ? "jtrue" : "jfalse";
	else if (this->currActive == ACTIVE_FUNCTION)
		this->stringValue = this->_getStringValue();
	else if (this->currActive == ACTIVE_OBJECT)
		this->stringValue = this->_getStringValue();
	else if (this->currActive == ACTIVE_LIST)
		this->stringValue = this->_getStringValue();
	else if (this->currActive == ACTIVE_MAP)
		this->stringValue = this->_getStringValue();

	this->currActive = ACTIVE_STRING;
}

JDM_DLL
const void HigherObject::castToInteger()
{
	if (this->currActive == ACTIVE_INTEGER)
		return;

	else if (this->currActive == ACTIVE_STRING)
	{
		try {
			this->integerValue = std::stoll(stringValue);
		} catch (const std::invalid_argument& e) {
			this->integerValue = 0;
		}
	}
	else if (this->currActive == ACTIVE_DECIMAL)
		this->integerValue = static_cast<int>(doubleValue);
	else if (this->currActive == ACTIVE_BOOLEAN)
		this->integerValue = (booleanValue) ? 1 : 0;
	else if (this->currActive == ACTIVE_FUNCTION)
		this->integerValue = (this->funcValue   != nullptr) ? 1 : 0;
	else if (this->currActive == ACTIVE_OBJECT)
		this->integerValue = (this->objectValue != nullptr) ? 1 : 0;
	else if (this->currActive == ACTIVE_LIST)
		this->integerValue = this->listValue.size();
	else if (this->currActive == ACTIVE_MAP)
		this->integerValue = this->mapValue.size();

	this->currActive = ACTIVE_INTEGER;
}

JDM_DLL
const void HigherObject::castToDecimal()
{
	if (this->currActive == ACTIVE_DECIMAL)
		return;

	else if (this->currActive == ACTIVE_STRING)
	{
		try {
			this->doubleValue = std::stod(stringValue);
		}
		catch (const std::invalid_argument& e) {
			this->doubleValue = 0;
		}
	}
	else if (this->currActive == ACTIVE_INTEGER)
		this->doubleValue = static_cast<double>(integerValue);
	else if (this->currActive == ACTIVE_BOOLEAN)
		this->doubleValue = (booleanValue) ? 1.0 : 0.0;
	else if (this->currActive == ACTIVE_FUNCTION)
		this->doubleValue = (this->funcValue   != nullptr) ? 1.0 : 0.0;
	else if (this->currActive == ACTIVE_OBJECT)
		this->doubleValue = (this->objectValue != nullptr) ? 1.0 : 0.0;
	else if (this->currActive == ACTIVE_LIST)
		this->doubleValue = this->listValue.size();
	else if (this->currActive == ACTIVE_MAP)
		this->doubleValue = this->mapValue.size();

	this->currActive = ACTIVE_DECIMAL;
}

JDM_DLL
const void HigherObject::castToBoolean()
{
	if (this->currActive == ACTIVE_BOOLEAN)
		return;

	else if (this->currActive == ACTIVE_STRING)
		this->booleanValue = !stringValue.empty();
	else if (this->currActive == ACTIVE_INTEGER)
		this->booleanValue = integerValue > 0;
	else if (this->currActive == ACTIVE_DECIMAL)
		this->booleanValue = doubleValue > 0.0;
	else if (this->currActive == ACTIVE_FUNCTION)
		this->booleanValue = (this->funcValue   != nullptr);
	else if (this->currActive == ACTIVE_OBJECT)
		this->booleanValue = (this->objectValue != nullptr);
	else if (this->currActive == ACTIVE_LIST)
		this->booleanValue = this->listValue.empty();
	else if (this->currActive == ACTIVE_MAP)
		this->booleanValue = this->mapValue.empty();

	this->currActive = ACTIVE_BOOLEAN;
}

JDM_DLL
const void HigherObject::castToFunction()
{
	if (this->currActive == ACTIVE_FUNCTION)
		return;

	this->funcValue = std::make_shared<FunctionCall>();
	this->funcValue->blockWillRun = std::make_shared<Block>();
	this->currActive = ACTIVE_FUNCTION;
}

JDM_DLL
const void HigherObject::castToObject()
{
	if (this->currActive == ACTIVE_OBJECT)
		return;

	this->objectValue = std::make_shared<ClassObject>();
	this->currActive = ACTIVE_OBJECT;
}

JDM_DLL
const void HigherObject::castToList()
{
	if (this->currActive == ACTIVE_LIST)
		return;

	this->listValue.clear();
	if (this->currActive == ACTIVE_STRING)
		this->listValue.push_back(std::make_shared<HigherObject>(stringValue));
	else if (this->currActive == ACTIVE_INTEGER)
		this->listValue.push_back(std::make_shared<HigherObject>(integerValue));
	else if (this->currActive == ACTIVE_DECIMAL)
		this->listValue.push_back(std::make_shared<HigherObject>(doubleValue));
	else if (this->currActive == ACTIVE_BOOLEAN)
		this->listValue.push_back(std::make_shared<HigherObject>(booleanValue));
	else if (this->currActive == ACTIVE_FUNCTION)
		this->listValue.push_back(std::make_shared<HigherObject>(funcValue));
	else if (this->currActive == ACTIVE_OBJECT)
		this->listValue.push_back(std::make_shared<HigherObject>(objectValue));
	else if (this->currActive == ACTIVE_MAP)
	{
		for (auto it = this->mapValue.begin(); it != this->mapValue.end(); ++it)
			this->listValue.push_back(it->first);
	}
	this->currActive = ACTIVE_LIST;
}

JDM_DLL
const void HigherObject::castToMap()
{
	if (this->currActive == ACTIVE_MAP)
		return;

	this->mapValue.clear();
	this->currActive = ACTIVE_MAP;
}
