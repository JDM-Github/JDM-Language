#pragma once

class ListObject : public VarObjects
{
public:
	std::vector<std::shared_ptr<Expression>> list;

public:

	inline ListObject(const std::vector<std::shared_ptr<Expression>> &_list = {})
	:
		list(_list),
		VarObjects(JDM::TokenType::LIST)
	{}

	inline const std::string returnStringValue()
	{
		return "LIST";
	}

	inline const std::vector<std::shared_ptr<Expression>> returnValue()
	{
		return this->list;
	}
};
