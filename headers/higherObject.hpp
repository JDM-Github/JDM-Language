#pragma once
#include "instruction.hpp"

class HigherObject {
public:
	struct FunctionCall {
		std::string funcName;
		std::shared_ptr<Block> blockWillRun;
		std::vector<std::string> varNameAccesible;
		std::vector<std::shared_ptr<HigherObject>> preArgs;
		std::unordered_map<
			std::string, std::pair<DataTypeEnum,
			std::shared_ptr<HigherObject>>> parameters;
	};

	struct ClassObject {
		std::string className;
		std::unordered_map<std::string, std::shared_ptr<HigherObject>> members;
		std::unordered_map<std::string, std::shared_ptr<FunctionCall>> methods;
		bool fromMainSource = true;
	};

	enum ActiveDataType {
		NON_NONE,
    	ACTIVE_STRING,
    	ACTIVE_INTEGER,
    	ACTIVE_DECIMAL,
    	ACTIVE_BOOLEAN,
    	ACTIVE_FUNCTION,
    	ACTIVE_LIST,
    	ACTIVE_MAP,
    	ACTIVE_OBJECT
	};

	bool isListSorted = false;

	bool isString  = false;
	bool isInteger = false;
	bool isDecimal = false;
	bool isBoolean = false;
	bool isFunc    = false;
	bool isList    = false;
	bool isMap     = false;
	bool isObject  = false;

	ActiveDataType currActive = NON_NONE;
	bool isForcedConstraint = false;
	bool isConstant         = false;

	std::vector<DataTypeEnum> listConstraint = {};
	std::vector<DataTypeEnum> mapConstraint  = {};

	std::string stringValue  = "";
	int64_t     integerValue = 0;
	long double doubleValue  = 0.0;
	bool        booleanValue = false;

	std::shared_ptr<FunctionCall> funcValue;
	std::shared_ptr<ClassObject > objectValue;

	std::vector<std::shared_ptr<HigherObject>> listValue = {};
	std::unordered_map<std::shared_ptr<HigherObject>,
		std::shared_ptr<HigherObject>> mapValue  = {};

public:
	HigherObject() { }
	HigherObject(const std::string &value ) : stringValue (value), isString (true) { currActive = ACTIVE_STRING;  }
	HigherObject(const int64_t value      ) : integerValue(value), isInteger(true) { currActive = ACTIVE_INTEGER; }
	HigherObject(const long double value  ) : doubleValue (value), isDecimal(true) { currActive = ACTIVE_DECIMAL; }
	HigherObject(const bool value         ) : booleanValue(value), isBoolean(true) { currActive = ACTIVE_BOOLEAN; }
	HigherObject(const std::shared_ptr<FunctionCall> &value) : funcValue(value), isFunc(true) { currActive = ACTIVE_FUNCTION; }
	HigherObject(const std::shared_ptr<ClassObject > &value) : objectValue(value), isObject(true) { currActive = ACTIVE_OBJECT; }
	HigherObject(const std::vector<std::shared_ptr<HigherObject>> &value) : listValue(value), isList(true) { currActive = ACTIVE_LIST; }
	HigherObject(const std::unordered_map<std::shared_ptr<HigherObject>, std::shared_ptr<HigherObject>> &value)
		: mapValue(value), isMap(true) { currActive = ACTIVE_MAP; }

	HigherObject(const std::shared_ptr<HigherObject> &obj)
	{
		this->setHigherObject(obj);
	}
	virtual ~HigherObject() {}

	const void setHigherObject(const std::shared_ptr<HigherObject> &obj)
	{
		this->refalseAll();
		if      (obj->isString ) { this->stringValue  = obj->stringValue ; this->isString  = true; currActive = ACTIVE_STRING;   }
		else if (obj->isInteger) { this->integerValue = obj->integerValue; this->isInteger = true; currActive = ACTIVE_INTEGER;  }
		else if (obj->isDecimal) { this->doubleValue  = obj->doubleValue ; this->isDecimal = true; currActive = ACTIVE_DECIMAL;  }
		else if (obj->isBoolean) { this->booleanValue = obj->booleanValue; this->isBoolean = true; currActive = ACTIVE_BOOLEAN;  }
		else if (obj->isFunc   ) { this->funcValue    = obj->funcValue   ; this->isFunc    = true; currActive = ACTIVE_FUNCTION; }
		else if (obj->isObject ) { this->objectValue  = obj->objectValue ; this->isObject  = true; currActive = ACTIVE_OBJECT;   }
		else if (obj->isList   ) { this->listValue    = obj->listValue   ; this->isList    = true; currActive = ACTIVE_LIST;     }
		else if (obj->isMap    ) { this->mapValue     = obj->mapValue    ; this->isMap     = true; currActive = ACTIVE_MAP;      }
		this->isForcedConstraint = obj->isForcedConstraint;
		this->isConstant         = obj->isConstant;
	}

	const DataTypeEnum getDatatypeEnum() {
		switch (currActive) {
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

	const std::string getType()
	{
		if      (this->isString ) return "STRING";
		else if (this->isInteger) return "INTEGER";
		else if (this->isDecimal) return "DECIMAL";
		else if (this->isBoolean) return "BOOLEAN";
		else if (this->isFunc   ) return "FUNCTION";
		else if (this->isObject ) return "OBJECT";
		else if (this->isList   ) return "LIST";
		else if (this->isMap    ) return "MAP";
		else                      return "UNKNOWN";
	}

	const void logValue() {
		Log << this->_getStringValue();
	}

	const std::string _getStringValue() {
		std::ostringstream oss;
		if      (isString ) {
			size_t found  = this->stringValue.find("\\n");
    		while (found != std::string::npos) {
        		this->stringValue.replace(found, 2, "\n");
        		found = this->stringValue.find("\\n", found + 1);
    		}
			oss << this->stringValue;
		}
		else if (isInteger) oss << this->integerValue;
		else if (isDecimal) oss << this->doubleValue;
		else if (isBoolean) oss << (booleanValue) ? "jtrue" : "jfalse";
		else if (isFunc   ) {
			if (this->funcValue == nullptr) oss << "Function: Invalid.";
			else oss << "Function: " << funcValue;
		}
		else if (isObject ) {
			if (this->objectValue == nullptr) oss << "Class: Invalid.";
			else oss << "Class: " << objectValue;
		}
		else if (isList   ) {
			oss << "[ ";
			int index = 0;
			for (const auto &value : this->listValue) {
				oss << value->_getStringValue();
				if (index++ < this->listValue.size() - 1) oss << ", ";
			}
			oss << " ]";
		}
		else if (isMap    ) {
			oss << "{ ";
			int index = 0;
			for (const auto &value : this->mapValue){
				oss << value.first->_getStringValue();
				oss << " => ";
				oss << value.second->_getStringValue();
				if (index++ < this->mapValue.size() - 1) oss << ", ";
			}
			oss << " }";
		}
		else                oss << "None";
		return oss.str();
	}

	const void refalseAll() {
		isString  = false;
		isInteger = false;
		isDecimal = false;
		isBoolean = false;
		isFunc    = false;
		isObject  = false;
		isList    = false;
		isMap     = false;
	}

	const std::shared_ptr<HigherObject> isInMap(const std::shared_ptr<HigherObject> &obj, const bool getFirst = false) {
		for (auto it = this->mapValue.begin(); it != this->mapValue.end(); ++it) {
			if (it->first->compareHigherObject(obj))
				if (getFirst) return it->first;
				else return it->second;
		}
		return nullptr;
	}

	const bool isInList(const std::shared_ptr<HigherObject> &obj) {
		for (auto it = this->listValue.begin(); it != this->listValue.end(); ++it) {
			if (it->get()->compareHigherObject(obj))
				return true;
		}
		return false;
	}

	const void sortList() {
		if (!this->isList || this->isListSorted) return;
		std::vector<std::shared_ptr<HigherObject>> stringObject;
		std::vector<std::shared_ptr<HigherObject>> decimalObject;
		std::vector<std::shared_ptr<HigherObject>> listObject;
		std::vector<std::shared_ptr<HigherObject>> mapObject;
		std::vector<std::shared_ptr<HigherObject>> otherObject;

		for (const auto &obj : this->listValue) {
			if      (obj->isString ) stringObject .push_back(obj);
			else if (obj->isInteger || obj->isDecimal || obj->isBoolean) {
				obj->castToDecimal();
				decimalObject.push_back(obj);
			}
			else if (obj->isList   ) listObject.push_back(obj);
			else if (obj->isMap    ) mapObject .push_back(obj);
			else otherObject.push_back(obj);
		}
		std::sort(mapObject.begin(), mapObject.end(), [](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
			{ return a->mapValue.size() < b->mapValue.size(); });
		std::sort(listObject.begin(), listObject.end(), [](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
			{ return a->listValue.size() < b->listValue.size(); });
		std::sort(stringObject.begin(), stringObject.end(), [](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
			{ return a->stringValue < b->stringValue; });
		std::sort(decimalObject.begin(), decimalObject.end(), [](const std::shared_ptr<HigherObject> &a, const std::shared_ptr<HigherObject> &b)
			{ return a->doubleValue < b->doubleValue; });

		this->listValue.clear();
		this->listValue.insert(this->listValue.end(), mapObject    .begin(), mapObject    .end());
		this->listValue.insert(this->listValue.end(), listObject   .begin(), listObject   .end());
		this->listValue.insert(this->listValue.end(), stringObject .begin(), stringObject .end());
		this->listValue.insert(this->listValue.end(), decimalObject.begin(), decimalObject.end());
		this->listValue.insert(this->listValue.end(), otherObject  .begin(), otherObject  .end());

		this->isListSorted = true;
	}

	const bool compareHigherObject(const std::shared_ptr<HigherObject> &obj) {
		if (this->currActive == ACTIVE_STRING && this->currActive == obj->currActive)
			return this->stringValue == obj->stringValue;

		else if (this->currActive == ACTIVE_INTEGER) {
			obj->castToDecimal();
			return this->integerValue == obj->doubleValue;
		}
		else if (this->currActive == ACTIVE_DECIMAL) {
			obj->castToDecimal();
			return this->doubleValue == obj->doubleValue;
		}
		else if (this->currActive == ACTIVE_BOOLEAN) {
			obj->castToDecimal();
			return this->booleanValue == obj->doubleValue;
		}

		else if (this->currActive == ACTIVE_FUNCTION && this->currActive == obj->currActive)
			return this->funcValue == obj->funcValue && this->funcValue != nullptr;

		else if (this->currActive == ACTIVE_OBJECT && this->currActive == obj->currActive)
			return this->objectValue == obj->objectValue && this->objectValue != nullptr;

		else if (this->currActive == ACTIVE_LIST && this->currActive == obj->currActive) {
			if (this->listValue.size() != obj->listValue.size())
				return false;

			for (int i = 0; i < this->listValue.size(); i++) {
				if (!this->listValue[i]->compareHigherObject( obj->listValue[i] ))
					return false;
			}
			return true;
		}
		else if (this->currActive == ACTIVE_MAP && this->currActive == obj->currActive) {
			if (this->mapValue.size() != obj->mapValue.size())
				return false;

			for (int i = 0; i < this->mapValue.size(); i++) {
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

	const void castToString() {
		if      (isString ) return;
		else if (isInteger) this->stringValue = std::to_string(integerValue);
		else if (isDecimal) this->stringValue = std::to_string(doubleValue);
		else if (isBoolean) this->stringValue = (booleanValue) ? "jtrue" : "jfalse";

		else if (isFunc   ) this->stringValue = this->_getStringValue();
		else if (isObject ) this->stringValue = this->_getStringValue();
		else if (isList   ) this->stringValue = this->_getStringValue();
		else if (isMap    ) this->stringValue = this->_getStringValue();
		this->refalseAll();
		isString  = true;
		currActive = ACTIVE_STRING;
	}

	const void castToInteger() {
		if      (isInteger) return;
		else if (isString) {
			try {
				this->integerValue = std::stoll(stringValue);
			} catch (const std::invalid_argument& e) {
				this->integerValue = 0;
			}
		}
		else if (isDecimal) this->integerValue = static_cast<int>(doubleValue);
		else if (isBoolean) this->integerValue = (booleanValue) ? 1 : 0;

		else if (isFunc   ) this->integerValue = (this->funcValue   != nullptr) ? 1 : 0;
		else if (isObject ) this->integerValue = (this->objectValue != nullptr) ? 1 : 0;
		else if (isList   ) this->integerValue = this->listValue.size();
		else if (isMap    ) this->integerValue = this->mapValue.size();

		this->refalseAll();
		isInteger = true;
		currActive = ACTIVE_INTEGER;
	}

	const void castToDecimal() {
		if      (isDecimal) return;
		else if (isString ) {
			try {
				this->doubleValue = std::stod(stringValue);
			} catch (const std::invalid_argument& e) {
				this->doubleValue = 0;
			}
		}
		else if (isInteger) this->doubleValue = static_cast<double>(integerValue);
		else if (isBoolean) this->doubleValue = (booleanValue) ? 1.0 : 0.0;
		else if (isFunc   ) this->doubleValue = (this->funcValue   != nullptr) ? 1.0 : 0.0;
		else if (isObject ) this->doubleValue = (this->objectValue != nullptr) ? 1.0 : 0.0;
		else if (isList   ) this->doubleValue = this->listValue.size();
		else if (isMap    ) this->doubleValue = this->mapValue.size();

		this->refalseAll();
		isDecimal = true;
		currActive = ACTIVE_DECIMAL;
	}

	const void castToBoolean() {
		if      (isBoolean) return;
		else if (isString ) this->booleanValue = !stringValue.empty();
		else if (isInteger) this->booleanValue = integerValue > 0;
		else if (isDecimal) this->booleanValue = doubleValue > 0.0;

		else if (isFunc   ) this->booleanValue = (this->funcValue   != nullptr);
		else if (isObject ) this->booleanValue = (this->objectValue != nullptr);
		else if (isList   ) this->booleanValue = this->listValue.empty();
		else if (isMap    ) this->booleanValue = this->mapValue.empty();

		this->refalseAll();
		isBoolean = true;
		currActive = ACTIVE_BOOLEAN;
	}

	const void castToFunction() {
		if (isFunc) return;
		this->funcValue = std::make_shared<FunctionCall>();
		this->funcValue->blockWillRun = std::make_shared<Block>();
		this->refalseAll();

		isFunc = true;
		currActive = ACTIVE_FUNCTION;
	}

	const void castToObject() {
		if (isObject) return;
		this->objectValue = std::make_shared<ClassObject>();
		this->refalseAll();

		isObject = true;
		currActive = ACTIVE_OBJECT;
	}

	const void castToList() {
		if (isList) return;

		this->listValue.clear();
		     if (isString ) this->listValue.push_back(std::make_shared<HigherObject>(stringValue));
		else if (isInteger) this->listValue.push_back(std::make_shared<HigherObject>(integerValue));
		else if (isDecimal) this->listValue.push_back(std::make_shared<HigherObject>(doubleValue));
		else if (isBoolean) this->listValue.push_back(std::make_shared<HigherObject>(booleanValue));

		else if (isFunc   ) this->listValue.push_back(std::make_shared<HigherObject>(funcValue));
		else if (isObject ) this->listValue.push_back(std::make_shared<HigherObject>(objectValue));
		else if (isMap    ) {
			for (auto it = this->mapValue.begin(); it != this->mapValue.end(); ++it)
				this->listValue.push_back(it->first);
		}
		this->refalseAll();
		isList = true;
		currActive = ACTIVE_LIST;
	}

	const void castToMap() {
		if (isMap) return;

		this->mapValue.clear();
		this->refalseAll();
		isMap = true;
		currActive = ACTIVE_MAP;
	}
};
