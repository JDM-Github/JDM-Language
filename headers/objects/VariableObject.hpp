#pragma once

class VariableObjects : public VarObjects
{
public:
	std::string name;

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<VarObjects>(this));
		archive(name);
	}

	VariableObjects() = default;
	inline explicit VariableObjects(const std::shared_ptr<TokenStruct> &tok)
	:
		VarObjects(JDM::TokenType::VARIABLE, std::get<2>(tok->token),
		std::get<3>(tok->token)),
		name(std::get<0>(tok->token))
	{}
	inline const std::string returnStringValue() { return this->name; }
};
