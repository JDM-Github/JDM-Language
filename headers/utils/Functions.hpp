#pragma once

template <class _TypeClass>
	CBool isInVec(const _TypeClass &e, const std::vector<_TypeClass> eList)
{
	for (SizeT i = 0; i < eList.size(); i++) if (e == eList[i])
		return true;
	return false;
}
