#pragma once

class JDM_DLL Comparator
{
public:
	JDM_DLL StaticCBool DoOperationString(CStringRef first, CStringRef second, CStringRef operation="");
	JDM_DLL StaticCBool DoOperationList  (SharedPtrRef<HigherObject> firstVal, CSharedPtrRef<HigherObject> secondVal = nullptr, CStringRef operation = "");
	JDM_DLL StaticCBool DoOperationMap   (SharedPtrRef<HigherObject> firstVal, CSharedPtrRef<HigherObject> secondVal = nullptr, CStringRef operation = "");
	JDM_DLL StaticCBool DoOperationFunc  (SharedPtrRef<HigherObject> firstVal, CSharedPtrRef<HigherObject> secondVal = nullptr, CStringRef operation = "");
	JDM_DLL StaticCBool DoOperationObject(SharedPtrRef<HigherObject> firstVal, CSharedPtrRef<HigherObject> secondVal = nullptr, CStringRef operation = "");
	JDM_DLL StaticCBool DoOperationNumber(double firstVal, double secondVal, CStringRef operation = "");
};
