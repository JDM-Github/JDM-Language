#include "utils/Setup.hpp"
#include "instruction/Instructions.hpp"

CEREAL_REGISTER_TYPE(FunctionObjects)
CEREAL_REGISTER_TYPE(IntegerObjects)
CEREAL_REGISTER_TYPE(LambdaObjects)
CEREAL_REGISTER_TYPE(ListObject)
CEREAL_REGISTER_TYPE(MapObject)
CEREAL_REGISTER_TYPE(VariableObjects)
CEREAL_REGISTER_TYPE(StringObjects)

CEREAL_REGISTER_POLYMORPHIC_RELATION(VarObjects, FunctionObjects)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VarObjects, IntegerObjects)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VarObjects, LambdaObjects)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VarObjects, ListObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VarObjects, MapObject)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VarObjects, VariableObjects)
CEREAL_REGISTER_POLYMORPHIC_RELATION(VarObjects, StringObjects)