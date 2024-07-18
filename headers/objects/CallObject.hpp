#pragma once

class CallObjects : public VarObjects
{
public:
	std::shared_ptr<VarObjects > currObject;
	std::shared_ptr<CallObjects> prevObject;
	std::shared_ptr<CallObjects> nextObject;
	std::string operation = "";

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<VarObjects>(this));
		archive(currObject, prevObject, nextObject, operation);
	}

	inline CallObjects(
		const std::shared_ptr<VarObjects > &_currObject = nullptr,
		const std::shared_ptr<CallObjects> &_prevObject = nullptr
	)
	:
		VarObjects(JDM::TokenType::CALL_OBJ),
		currObject(_currObject),
		prevObject(_prevObject)
	{}
	inline CallObjects(const std::shared_ptr<CallObjects > &_callObj)
	:
		VarObjects(JDM::TokenType::CALL_OBJ),
		currObject(_callObj->currObject),
		prevObject(_callObj->prevObject),
		nextObject(_callObj->nextObject)
	{}

	inline const std::string returnStringValue()
	{
		return "CALL";
	}
};
