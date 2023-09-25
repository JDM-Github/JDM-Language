#pragma once
#include "defines.hpp"

class HigherObject {
public:
	// struct FunctionCall {
	// 	std::vector<std::string> varNameAccesible;
	// 	std::shared_ptr<Block> blockWillRun = nullptr;
	// 	std::map<std::string, std::pair<DataTypeEnum, std::shared_ptr<HigherObject>>> parameters;
	// };

	bool isString  = false;
	bool isInteger = false;
	bool isDecimal = false;
	bool isBoolean = false;
	bool isFunc    = false;

	std::string stringValue  = "";
	int64_t     integerValue = 0;
	long double doubleValue  = 0.0;
	bool        booleanValue = false;
	// FunctionCall funcValue   = FunctionCall();

	std::vector<HigherObject> listValue           = {};
	std::map<HigherObject, HigherObject> mapValue = {};

public:
	HigherObject(const std::string &value ) : stringValue (value), isString (true) { }
	HigherObject(const int64_t value      ) : integerValue(value), isInteger(true) { }
	HigherObject(const long double value  ) : doubleValue (value), isDecimal(true) { }
	HigherObject(const bool value         ) : booleanValue(value), isBoolean(true) { }
	// HigherObject(const FunctionCall &value) : funcValue   (value), isFunc   (true) { }

	HigherObject(const std::shared_ptr<HigherObject> &obj) {
		if      (obj->isString ) { this->stringValue  = obj->stringValue ; this->castToString (); }
		else if (obj->isInteger) { this->integerValue = obj->integerValue; this->castToInteger(); }
		else if (obj->isDecimal) { this->doubleValue  = obj->doubleValue ; this->castToDecimal(); }
		else if (obj->isBoolean) { this->booleanValue = obj->booleanValue; this->castToBoolean(); }
	}
	virtual ~HigherObject() {}

	const void logValue() {
		if      (isString ) {
			size_t found  = this->stringValue.find("\\n");
    		while (found != std::string::npos) {
        		this->stringValue.replace(found, 2, "\n");
        		found = this->stringValue.find("\\n", found + 1);
    		}
			Log << this->stringValue;
		}
		else if (isInteger) Log << this->integerValue;
		else if (isDecimal) Log << this->doubleValue;
		else if (isBoolean) Log << (booleanValue) ? "jtrue" : "jfalse";
		else                Log << "None";
	}

	const void refalseAll() {
		isString  = false;
		isInteger = false;
		isDecimal = false;
		isBoolean = false;
	}

	const void castToString() {
		if      (isString ) return;
		else if (isInteger) this->stringValue = std::to_string(integerValue);
		else if (isDecimal) this->stringValue = std::to_string(doubleValue);
		else if (isBoolean) this->stringValue = (booleanValue) ? "jtrue" : "jfalse";

		this->refalseAll();
		isString  = true;
	}

	const void castToInteger() {
		if      (isInteger) return;
		else if (isString) {
			try {
				integerValue = std::stoll(stringValue);
			} catch (const std::invalid_argument& e) {
				Log << "Warning: String is not a valid Integer Value.";
				integerValue = 0;
			}
		}
		else if (isDecimal) integerValue = static_cast<int>(doubleValue);
		else if (isBoolean) integerValue = (booleanValue) ? 1 : 0;

		this->refalseAll();
		isInteger = true;
	}

	const void castToDecimal() {
		if      (isDecimal) return;
		else if (isString ) {
			try {
				doubleValue = std::stod(stringValue);
			} catch (const std::invalid_argument& e) {
				Log << "Warning: String is not a valid Double Value.";
				doubleValue = 0;
			}
		}
		else if (isInteger) doubleValue = static_cast<double>(integerValue);
		else if (isBoolean) doubleValue = (booleanValue) ? 1.0 : 0.0;

		this->refalseAll();
		isDecimal = true;
	}

	const void castToBoolean() {
		if      (isBoolean) return;
		else if (isString) booleanValue = !stringValue.empty();
		else if (isInteger) booleanValue = integerValue > 0;
		else if (isDecimal) booleanValue = doubleValue > 0.0;

		this->refalseAll();
		isBoolean = true;
	}
};

