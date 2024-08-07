#pragma once

#include "utils/Includes.hpp"
#include "utils/Defines.hpp"
#include "utils/Using.hpp"

#include "enums/TokenType.hpp"
#include "enums/DataTypeEnum.hpp"
#include "enums/CustomKeywordEnum.hpp"
#include "enums/CustomFunctionEnum.hpp"
#include "enums/ControlFlowEnum.hpp"

#include "struct/TokenStruct.hpp"
#include "struct/TokenCurrentLink.hpp"

#include "utils/Functions.hpp"
#include "utils/Keywords.hpp"

template <class _TypeClass>
CBool isInVec(const _TypeClass &e, const std::vector<_TypeClass> eList)
{
	for (SizeT i = 0; i < eList.size(); i++) if (e == eList[i])
		return true;
	return false;
}
