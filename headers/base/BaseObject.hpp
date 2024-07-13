#pragma once

class VarObjects
{
protected:
	size_t row, col;
	JDM::TokenType varType = JDM::TokenType::UNDEFINED;

protected:
	inline VarObjects(JDM::TokenType type, size_t r = 0, size_t c = 0)
		: varType(type), row(r), col(c) {}
	virtual ~VarObjects() {}

public:
	inline const char* getStringToken()
	{
		return JDM::tokenTypeToString(this->varType);
	}

	inline const void setToken(JDM::TokenType varType)
	{
		this->varType = varType;
	}

	inline const JDM::TokenType getToken()
	{
		return this->varType;
	}

	virtual const std::string returnStringValue()
	{
		return "INVALID";
	};
};
