#pragma once

struct TokenStruct
{
	std::tuple<TokenStr, JDM::TokenType, size_t, size_t> token =
		std::make_tuple("", JDM::TokenType::UNDEFINED, 0, 0);
	std::vector<std::shared_ptr<TokenStruct>> child = {};

    template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(token, child);
	}
};
