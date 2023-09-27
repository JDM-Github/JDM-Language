#pragma once
#include "defines.hpp"

class HigherObject {
public:
	struct FunctionCall {
		std::string funcName;
		std::vector<std::string> varNameAccesible;
		std::shared_ptr<Block> blockWillRun;
		std::unordered_map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> parameters;
	};
	enum ActiveDataType {
		NON_NONE,
    	ACTIVE_STRING,
    	ACTIVE_INTEGER,
    	ACTIVE_DECIMAL,
    	ACTIVE_BOOLEAN,
    	ACTIVE_FUNCTION,
    	ACTIVE_LIST,
    	ACTIVE_MAP
	};
	bool isString  = false;
	bool isInteger = false;
	bool isDecimal = false;
	bool isBoolean = false;
	bool isFunc    = false;
	bool isList    = false;
	bool isMap     = false;

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

	std::vector<std::shared_ptr<HigherObject>>                             listValue = {};
	std::unordered_map<std::shared_ptr<HigherObject>, std::shared_ptr<HigherObject>> mapValue  = {};

public:
	HigherObject(const std::string &value ) : stringValue (value), isString (true) { currActive = ACTIVE_STRING; }
	HigherObject(const int64_t value      ) : integerValue(value), isInteger(true) { currActive = ACTIVE_INTEGER; }
	HigherObject(const long double value  ) : doubleValue (value), isDecimal(true) { currActive = ACTIVE_DECIMAL; }
	HigherObject(const bool value         ) : booleanValue(value), isBoolean(true) { currActive = ACTIVE_BOOLEAN; }
	HigherObject(const std::shared_ptr<FunctionCall> &value) : funcValue(value), isFunc(true) { currActive = ACTIVE_FUNCTION; }
	HigherObject(const std::vector<std::shared_ptr<HigherObject>> &value) : listValue(value), isList(true) { currActive = ACTIVE_LIST; }
	HigherObject(const std::unordered_map<std::shared_ptr<HigherObject>, std::shared_ptr<HigherObject>> &value)
		: mapValue(value), isMap(true) { currActive = ACTIVE_MAP; }

	HigherObject(const std::shared_ptr<HigherObject> &obj) {
		this->refalseAll();
		if      (obj->isString ) { this->stringValue  = obj->stringValue ; this->isString  = true; currActive = ACTIVE_STRING; }
		else if (obj->isInteger) { this->integerValue = obj->integerValue; this->isInteger = true; currActive = ACTIVE_INTEGER; }
		else if (obj->isDecimal) { this->doubleValue  = obj->doubleValue ; this->isDecimal = true; currActive = ACTIVE_DECIMAL; }
		else if (obj->isBoolean) { this->booleanValue = obj->booleanValue; this->isBoolean = true; currActive = ACTIVE_BOOLEAN; }
		else if (obj->isFunc   ) { this->funcValue    = obj->funcValue   ; this->isFunc    = true; currActive = ACTIVE_FUNCTION; }
		else if (obj->isList   ) { this->listValue    = obj->listValue   ; this->isList    = true; currActive = ACTIVE_LIST; }
		else if (obj->isMap    ) { this->mapValue     = obj->mapValue    ; this->isMap     = true; currActive = ACTIVE_MAP; }
	}
	virtual ~HigherObject() {}

	const DataTypeEnum getDatatypeEnum() {
		switch (currActive) {
			case ACTIVE_STRING  : return DataTypeEnum::DATA_STRING;
			case ACTIVE_INTEGER : return DataTypeEnum::DATA_INTEGER;
			case ACTIVE_DECIMAL : return DataTypeEnum::DATA_DOUBLE;
			case ACTIVE_BOOLEAN : return DataTypeEnum::DATA_BOOLEAN;
			case ACTIVE_FUNCTION: return DataTypeEnum::DATA_LAMBDA;
			case ACTIVE_LIST    : return DataTypeEnum::DATA_LIST;
			case ACTIVE_MAP     : return DataTypeEnum::DATA_MAP;
		}
		return DataTypeEnum::DATA_ANY;
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
		else if (isFunc   ) oss << "Function: " << funcValue;
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

	// const void addHigherObject(const std::shared_ptr<HigherObject> &obj1) {
	// 	if ()
	// }

	const void refalseAll() {
		isString  = false;
		isInteger = false;
		isDecimal = false;
		isBoolean = false;
		isFunc    = false;
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

	const bool compareHigherObject(const std::shared_ptr<HigherObject> &obj) {
		if (this->currActive != obj->currActive)
			return false;

		     if (this->currActive == ACTIVE_STRING)
			return this->stringValue == obj->stringValue;
		else if (this->currActive == ACTIVE_INTEGER)
			return this->integerValue == obj->integerValue;
		else if (this->currActive == ACTIVE_DECIMAL)
			return this->doubleValue == obj->doubleValue;
		else if (this->currActive == ACTIVE_BOOLEAN)
			return this->booleanValue == obj->booleanValue;
		else if (this->currActive == ACTIVE_FUNCTION)
			return this->funcValue == obj->funcValue;
		else if (this->currActive == ACTIVE_LIST) {
			if (this->listValue.size() != obj->listValue.size())
				return false;

			for (int i = 0; i < this->listValue.size(); i++) {
				if (!this->listValue[i]->compareHigherObject( obj->listValue[i] ))
					return false;
			}
			return true;
		}
		else if (this->currActive == ACTIVE_MAP) {
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

		else if (isFunc   ) this->stringValue = "";
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

		else if (isFunc   ) this->integerValue = 1;
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
		else if (isFunc   ) this->doubleValue = 1;
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

		else if (isFunc   ) this->booleanValue = true;
		else if (isList   ) this->booleanValue = this->listValue.empty();
		else if (isMap    ) this->booleanValue = this->mapValue.empty();

		this->refalseAll();
		isBoolean = true;
		currActive = ACTIVE_BOOLEAN;
	}

	const void castToFunction() {
		if      (isFunc   ) return;
		this->funcValue = nullptr;
		this->refalseAll();
		isFunc = true;
		currActive = ACTIVE_FUNCTION;
	}

	const void castToList() {
		if (isList) return;

		this->listValue.clear();
		     if (isString ) this->listValue.push_back(std::make_shared<HigherObject>(stringValue));
		else if (isInteger) this->listValue.push_back(std::make_shared<HigherObject>(integerValue));
		else if (isDecimal) this->listValue.push_back(std::make_shared<HigherObject>(doubleValue));
		else if (isBoolean) this->listValue.push_back(std::make_shared<HigherObject>(booleanValue));

		else if (isFunc   ) this->listValue.push_back(std::make_shared<HigherObject>(funcValue));
		else if (isMap    ) this->listValue.push_back(std::make_shared<HigherObject>(mapValue));

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


class ListHigherFunctions {
public:
	enum ListFunction {
    	list_sort,
    	list_search,
    	list_insert,
    	list_delete,
    	list_at,
    	list_push_back,
    	list_push_front,
    	list_pop_back,
    	list_pop_front
	};

	static std::unordered_map<std::string, ListFunction> listFunctions;

	static const void sort      (std::shared_ptr<HigherObject> &obj1) {
		if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
	}
	static const int search     (std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2) {
		int index = 0;
		for (const auto &li : obj1->listValue)
			if (li->compareHigherObject(obj2)) return index;
			else index++;
		return -1;
	}
	static const void insert    (std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2, int index = -1) {
		if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
		if (index < 0)   index += obj1->listValue.size();
		if (index < 0 || index >  obj1->listValue.size())
    		throw std::runtime_error("Runtime Error: Index out of bounds.");

		obj1->listValue.insert(obj1->listValue.begin()+index, obj2);
	}
	static const void del    (std::shared_ptr<HigherObject> &obj1, int index) {
		if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
		if (index < 0)   index += obj1->listValue.size();
		if (index < 0 || index >= obj1->listValue.size())
    		throw std::runtime_error("Runtime Error: Index out of bounds.");

		obj1->listValue.erase(obj1->listValue.begin()+index);
	}
	static const std::shared_ptr<HigherObject> at        (std::shared_ptr<HigherObject> &obj1, int index) {
		if (index < 0)   index += obj1->listValue.size();
		if (index < 0 || index >= obj1->listValue.size())
    		throw std::runtime_error("Runtime Error: Index out of bounds.");

    	return obj1->listValue[index];
	}
	static const void push_back (std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2) {
		if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
		obj1->listValue.push_back(obj2);
	}
	static const void push_front(std::shared_ptr<HigherObject> &obj1, const std::shared_ptr<HigherObject> &obj2) {
		if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
		obj1->listValue.insert(obj1->listValue.begin(), obj2);
	}
	static const void pop_back  (std::shared_ptr<HigherObject> &obj1) {
		if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
		if (obj1->listValue.empty()) return;
		obj1->listValue.pop_back();
	}
	static const void pop_front (std::shared_ptr<HigherObject> &obj1) {
		if (obj1->isConstant) throw std::runtime_error("Runtime Error: Variable is Constant.");
		if (obj1->listValue.empty()) return;
		obj1->listValue.erase(obj1->listValue.begin());
	}
};

enum NativeFunction {
	NATIVE_INPUT,
};
std::unordered_map<std::string, NativeFunction> allNativeFunction = {
	{"input", NATIVE_INPUT}
};

std::unordered_map<std::string, ListHigherFunctions::ListFunction> ListHigherFunctions::listFunctions = {
	{"sort"      , ListFunction::list_sort      },
    {"search"    , ListFunction::list_search    },
    {"insert"    , ListFunction::list_insert    },
    {"delete"    , ListFunction::list_delete    },
    {"at"        , ListFunction::list_at        },
    {"push_back" , ListFunction::list_push_back },
    {"push_front", ListFunction::list_push_front},
    {"pop_back"  , ListFunction::list_pop_back  },
    {"pop_front" , ListFunction::list_pop_front }
};