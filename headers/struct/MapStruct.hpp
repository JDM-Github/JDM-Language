#pragma once

struct MapStruct
{
	std::shared_ptr<Expression> key;
	std::shared_ptr<Expression> value;

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(key, value);
	}
};
