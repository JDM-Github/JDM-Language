#include "Compiler.hpp"

// This is the return value on what you call. Example the return value of function is list.
// Then you can still do operation because. you know, you can?.
JDM_DLL
const std::shared_ptr<HigherObject> Compiler::manageEndCall(
	const std::shared_ptr<CallObjects> &callObj,
	std::shared_ptr<HigherObject> &returnVal,
	const std::shared_ptr<Expression> &expressionAssign)
{
	// Setup new return
	std::shared_ptr<HigherObject> newReturn;
	auto newCallObj = callObj->nextObject;

	// If there is no call object, just return the value
	if (newCallObj == nullptr)
		return std::make_shared<HigherObject>(returnVal);

	// The chance of this happening is low
	// because If my lexer and parser do their job properly, it will work flawlessly.
	if (returnVal == nullptr)
		throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");

	// Get the next object
	auto newCurrObj = newCallObj->currObject;
	auto tok        = newCurrObj->getToken();
	if (returnVal->getCurrActive() == ACTIVE_FUNCTION)
	{
		if (tok != JDM::TokenType::FUNCTIONS)
			// This will happen because, example a function has been return
			// the you do this
			/*
			jfunc test() => {
				jreturn () => { $logn => "TEST"; }; 
			}
			$logn => test().test();
			*/
			throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");
		// Get the function object of the newCallObject
		auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
		if (functionObj->returnStringValue() != "(")
			throw std::runtime_error("Runtime Error: Return Value is Function. Not a class.");

		this->isAssigning = true;
		return this->runFunction(returnVal->funcValue, this->getVectorHigherObject(functionObj->arguments));
	}
	// This thing is still not properly coded or implemented.
	// I will do this bit later or next time
	// TO DO
	else if (returnVal->getCurrActive() == ACTIVE_OBJECT)
	{
		if (tok == JDM::TokenType::FUNCTIONS)
		{
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
			auto classFunc   = this->nativeClassMap.find(returnVal->objectValue->className);
			if (classFunc != this->nativeClassMap.end())
			{
				// Will never be true, as recursively call, will handle it, but just in case.
				if (functionObj->returnStringValue() == "(")
					newReturn = classFunc->second->constructor(returnVal, this->getVectorHigherObject(functionObj->arguments));
				else
				{
					// This still not really cannot do anything.
					auto func = classFunc->second->mapFunctions.find(functionObj->returnStringValue());
					if (func == classFunc->second->mapFunctions.end())
						throw std::runtime_error("Runtime Error: This method is not a member of class '" + returnVal->objectValue->className + "'.");

					this->isAssigning = true;
					newReturn = classFunc->second->manageFunction(func->second, returnVal, this->getVectorHigherObject(functionObj->arguments));
				}
			}
		}
		// This will be used when the object next is a variable.
		// I will get the value of the next obj, and call the manageEnd again.
		// TO DO
		else if (tok == JDM::TokenType::VARIABLE)
		{
			
		}
	}
	// If the return value is string, do something, make it act like a string
	else if (returnVal->getCurrActive() == ACTIVE_STRING)
	{
		if (tok == JDM::TokenType::EXPRESSION)
			// If the next token is like this
			/*
			jfunc test() => {
				jreturn "TEST"; 
			}
			$log => test()[0];
			*/
			newReturn = this->manageCallBrackets(newCallObj, returnVal, expressionAssign);

		else if (tok == JDM::TokenType::FUNCTIONS)
		{
			// If the next token is something like this
			/*
			jfunc test() => {
				jreturn "Hello World";
			}
			$logn => test().split();
			*/
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
			if (functionObj->returnStringValue() == "(")
				// If the return value is string then you call it.
				/*
				jfunc test() => {
					jreturn "test";
				}
				$logn => test()();
				*/
				throw std::runtime_error("Runtime Error: A String cannot be called.");

			auto func = StringHigherFunctions::stringFunctions.find(functionObj->returnStringValue());
			if (func == StringHigherFunctions::stringFunctions.end())
				// If the return value is string then you call it and the function is not member or valid func.
				/*
				jfunc test() => {
					jreturn "test";
				}
				$logn => test().test();
				*/
				throw std::runtime_error("Runtime Error: This function is not a member of class 'jstring'.");

			this->isAssigning = true;
			newReturn = StringHigherFunctions::manageFunction(
				func->second, returnVal,
				this->getVectorHigherObject(functionObj->arguments)
			);
		}
	}
	else if (returnVal->getCurrActive() == ACTIVE_LIST || returnVal->getCurrActive() == ACTIVE_MAP)
	{
		if (tok == JDM::TokenType::EXPRESSION)
		{
			// This will manage when you want to know the index of something
			newReturn = this->manageCallBrackets(newCallObj, returnVal, expressionAssign);
		}

		else if (tok == JDM::TokenType::FUNCTIONS)
		{
			auto functionObj = std::dynamic_pointer_cast<FunctionObjects>(newCurrObj);
			if (returnVal->getCurrActive() == ACTIVE_LIST)
			{
				if (functionObj->returnStringValue() == "(")
					throw std::runtime_error("Runtime Error: A 'jlist' cannot be called.");

				// jlist native function. Acts like a class.
				auto func = ListHigherFunctions::listFunctions.find(functionObj->name);						
				if (func == ListHigherFunctions::listFunctions.end())
					// If the function is not a member of jlist
					/*
					jfunc test() => {
						jreturn [1, 2, 3, 4, 5]; 
					}
					$log => test().test();
					*/
					throw std::runtime_error("Runtime Error: This function is not a member of class 'jlist'.");

				this->isAssigning = true;
				newReturn = ListHigherFunctions::manageFunction(
					func->second, returnVal,
					this->getVectorHigherObject(functionObj->arguments)
				);
			}
			else // Return Value is Map
			{
				if (functionObj->returnStringValue() == "(")
					throw std::runtime_error("Runtime Error: A 'jmap' cannot be called.");

				// jmap native function. Acts like a class.
				auto func = MapHigherFunctions::mapFunctions.find(functionObj->name);						
				if (func == MapHigherFunctions::mapFunctions.end())
					// If the function is not a member of jmap
					/*
					jfunc test() => {
						jreturn { "test" => 10 }; 
					}
					$log => test().test();
					*/
					throw std::runtime_error("Runtime Error: This function is not a member of class 'jmap'.");

				this->isAssigning = true;
				newReturn = MapHigherFunctions::manageFunction(
					func->second, returnVal,
					this->getVectorHigherObject(functionObj->arguments)
				);
			}
		}
	}
	else
		// When the return value is not what you expect.
		// This soon will be remove, because after I add native for other data type
		// Example I add on integer, float or boolean
		throw std::runtime_error("Runtime Error: Return Value cannot be called or act like class.");

	// Return the new Value calculated above
	return this->manageEndCall(newCallObj, newReturn);
}
