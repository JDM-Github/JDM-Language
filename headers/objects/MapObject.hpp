#pragma once

class MapObject : public VarObjects
{
public:
	std::vector<std::shared_ptr<MapStruct>> map;

public:
	template<class Archive>
	inline void serialize(Archive & archive)
	{
		archive(cereal::base_class<VarObjects>(this));
		archive(map);
	}

	inline MapObject(const std::vector<std::shared_ptr<MapStruct>> &_map = {})
	:
		map(_map),
		VarObjects(JDM::TokenType::MAP)
	{}

	inline const std::string returnStringValue()
	{
		return "MAP";
	}

	inline const std::vector<std::shared_ptr<MapStruct>> returnValue()
	{
		return this->map;
	}
};
