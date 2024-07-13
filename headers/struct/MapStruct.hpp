#pragma once

struct MapStruct
{
	std::shared_ptr<Expression> key;
	std::shared_ptr<Expression> value;
};