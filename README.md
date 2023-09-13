# JDM-Language
JDM Language, Own Language created in C++

```
|> JDM Language : Single Line Comment
|->  : Block Comment
JDM Language 
Creator : JDM
<-|

Declare:
jany anyInt  = 99;          |> Integer
jany anyStrD = "Test";      |> String Double Qoute
jany anyStrS = 'Test';      |> String Single Qoute

jany anyList = [ ... ];     |> List, Note: 'jany' can only detect a list when it is in brackets.

Explicitly Use Type:
jmap

jint    int    = 99;          |> Integer
jdouble double = 99.45;       |> Double
jstring str    = "'test'";    |> String

jlist   listB  = [ ... ];                       |> List Brackets
jlist   listNB = 1, "t", 3.4;                   |> List no Brackets
jmap    map    = [ 'key' => 'value', 3 => 2.3]; |> Map
jlambda func   = () => { ... };                 |> Lambda, can accept parameters, like function

jlist   list[jint]          = [ ... ];  |> List that accept only int
jlist   list[jint, jstring] = [ ... ];  |> List that accept only int or string

jlambda func   = () => jint => { ... }; |> Explicitly telling that this function will return int

Note: If you add constraint and didn't follow it. It will throw an error.

Example:
jlist list[int] = ["string"]; |> Will throw error, as it only accept int

Note: List and Map can accept any value. (String, Integer, Double, Boolean, List, Map, Function, etc.)


Function Usage

jfunc simpleFunc() => { ... }         |> Simple Function
jfunc lineFunc  () => jint test = 34; |> 1 Line Function

|> Simple Then Func with Param
jfunc thenFunc(jint test) => {
	...
}.then() => {
	|> Then Function will only run when the thenFunc successfully RUN
}

|> Overloaded Then Func
jfunc thenFunc() => {
	...
	jreturn 10;
}.then(jint funcReturnVal) => {
	|> If function return matches the parameter of then, this then will be called
}.then => {
	|> This then will only be called if it matches the return, In this case. It will not be called
}

|> Lambda with Then
jlambda func   = () => jint => { ... }.then() => { ... }

Function With Any parameters
jfunc anyParam(param1, param2) => {
	|> If the parameter doesn't have value, it will automatically turn into jany data type.
}

Note: What function return will be the return, even if 'then' uses it.

Example:
|> if func return integer, example 10, and then uses that return, it will still return 10

jany thenInAssignment = func().then(jint test) => { ... } + 34;

$log => thenInAssignment; |> 44, Since func return 10, even if then uses it, then add by 34

|> $log is the way to print the output or something in the terminal

Class Usage:

jclass NewClass => { ... } |> Simple Class Declaration

jclass Base    => { ... }              |> Define Class Base
jclass Derived => Base, ... => { ... } |> Derive a class, can add more. Polymorphism

jclass ScopeTest => {
	jpublic => {
		|> All public method, member will be here
	}

	jprotected => {
		|> All protected method, member will be here
	}

	jprivate => {
		|> All private method, member will be here
	}

	|->
	Note: After you declared the [jpublic, jprotected, jprivate], you cannot declare it again.
	Example:
	...
	jpublic => { }
	jpublic => { }

	Two jpublic declaration will throw error
	...
	<-|
}

Declaring, Using method, member in Class and more FEATURES

Declaring method, member on Class:

jpublic => {
	jany testMember = [ 1, 2.0, '3' ];

	joverride jfunc testMethod() => jnone => { |> jnone means it doesn't return anything, returning anything will result to error.

		super.baseFunction(); |> Super with base class function. If function doesn't exist, will throw error

		|-> 
		Since we are in testMethod, And it is override, we can use this super() or super.testMethod()
		super();              |> Super without specifying what will super, super will automatically super the same method on Base

		Using super() and the method is not override, will result on error.
		<-|

		jthis=>testMember = 45; |> Remember this is any, You can change it's value.

		Note: Any Variable is just like Python Variable
	}
}

```