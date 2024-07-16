#pragma once

struct TokenCurrentLink
{
	std::shared_ptr<TokenCurrentLink> next;
	std::shared_ptr<TokenCurrentLink> prev;
	SharedTokenStruct current;
	std::vector<SharedTokenStruct> currTokens;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(next, prev, current, currTokens);
	}
};
