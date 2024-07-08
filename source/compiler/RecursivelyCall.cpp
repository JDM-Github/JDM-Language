#include "compiler.hpp"

JDM_DLL
const std::shared_ptr<HigherObject> Compiler::recursivelyCall(
	const std::shared_ptr<CallObjects> &callObj,
	const std::shared_ptr<Expression> &expressionAssign)
{
	auto tok = callObj->currObject->getToken();
	std::shared_ptr<HigherObject> returnValue = nullptr;

	if (tok == JDM::TokenType::FUNCTIONS)
	{
		auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(callObj->currObject);
		std::shared_ptr<HigherObject::FunctionCall> newFunc = nullptr;
		auto variables = this->variable->variables  .find(functionObj->returnStringValue());
		auto function  = this->variable->functionMap.find(functionObj->returnStringValue());

		if (function != this->variable->functionMap.end())
			newFunc = function->second;

		// Used to run a lambda
		else if (variables != this->variable->variables.end() && variables->second.second->isFunc)
			newFunc = variables->second.second->funcValue;

		else if (variables != this->variable->variables.end() && variables->second.second->isObject)
		{
			for (auto &e : this->allInclude)
			{
				if (e.compare(variables->second.second->objectValue->className) == 0)
				{
					if (!variables->second.second->objectValue->fromMainSource)
						throw std::runtime_error("Runtime Error: This object is not callable.");
					auto newObj = std::make_shared<HigherObject>();

					newObj->castToObject();
					newObj->objectValue->className = variables->second.second->objectValue->className;
					newObj->objectValue->members   = variables->second.second->objectValue->members;
					newObj->objectValue->methods   = variables->second.second->objectValue->methods;
					newObj->objectValue->fromMainSource = false;
					returnValue = this->nativeClassMap[e]->constructor(newObj,
						this->getVectorHigherObject(functionObj->arguments));
					return this->manageEndCall(callObj, returnValue, expressionAssign);
				}
			}
			throw std::runtime_error("Runtime Error: No CONSTRUCTOR implemented yet.");
		}
		else
		{
			auto nativeFunc = NativeFunction::allNativeFunction.find(functionObj->returnStringValue());
			if (nativeFunc != NativeFunction::allNativeFunction.end())
			{
				if (nativeFunc->second == NativeFunction::NativeFunctionEnum::NATIVE_REFERENCE)
				{
					if (functionObj->arguments.size() != 1)
						throw std::runtime_error("Runtime Error: Reference expect L value.");
					// if (callObj->nextObject != nullptr)
					// 	throw std::runtime_error("Runtime Error: Reference can't have recursive call.");

					return getVariableObject(functionObj->arguments[0]);
				}
				else
					returnValue = this->handleNativeFunction(nativeFunc->second,
						this->getVectorHigherObject(functionObj->arguments));
				return this->manageEndCall( callObj, returnValue, expressionAssign );
			}
			else throw std::runtime_error("Runtime Error: Function is not declared.");
		}
		if (newFunc == nullptr)
			throw std::runtime_error("Runtime Error: Invalid Function.");

		returnValue = this->runFunction(newFunc, this->getVectorHigherObject(functionObj->arguments));
	}
	else if (tok == JDM::TokenType::EXPRESSION)
	{
		// Get the expression object
		auto exprObj = std::dynamic_pointer_cast<ExpressionObjects>(callObj->currObject);
		auto variables = this->variable->variables.find(exprObj->returnStringValue());
		if (variables == this->variable->variables.end())
			throw std::runtime_error("Runtime Error: Variable is not declared.");

		auto varList = variables->second.second;
		if (varList->isList || varList->isMap || varList->isString)
			returnValue = this->manageCallBrackets(callObj, varList, expressionAssign);
		else
			throw std::runtime_error("Runtime Error: Variable is not a String, List or a Map.");
	}
	else if (tok == JDM::TokenType::VARIABLE)
	{
		auto var = std::dynamic_pointer_cast<VariableObjects>(callObj->currObject);
		auto variables = this->variable->variables.find(var->returnStringValue());
		if (variables == this->variable->variables.end())
			throw std::runtime_error("Runtime Error: Variable is not declared.");

		returnValue = variables->second.second;
	}
	else if (tok == JDM::TokenType::STRING)
		returnValue = std::make_shared<HigherObject>(callObj->currObject->returnStringValue());

	return this->manageEndCall(callObj, returnValue, expressionAssign);
}
