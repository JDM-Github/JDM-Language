# JDM Language

JDM Language is langauge purely made in c++, This language is inspired in python where you can create a dynamic variable that can be
changed depend on what you want but in here you can create this in more secure manner.

---

### DataType
- **jint** - ***INTEGER***
- **jdouble** - ***DOUBLE***
- **jstring** - ***STRING***
- **jboolean** - ***BOOLEAN***
- **jlist** - ***LIST***
- **jmap** - ***MAP***
- **jlambda** - ***FUNCTION***

---

### Keywords 
- **jconst** - Used to declare variables as constants.
- **jforce** - Used to enforce a specific datatype, preventing type errors (e.g., jforce jint test = "test"; results in an error).
- **jcforce** - Combines forcing a specific datatype with declaring it as a constant.
- **jreverse**: Used to reverse the order of elements, typically in arrays or lists.
- **jtrue**: Represents the boolean value `true`.
- **jfalse**: Represents the boolean value `false`

---

### Control Flow
- **jif** - Used for conditional statements.
- **jelseif** - Used for additional conditional statements following an `jif` block.
- **jelse** - Used to define an alternative block of code in case the condition of the preceding `jif` or `jelseif` statement is false.
- **jfor** - Used to start a loop that iterates a specified number of times.
- **jforeach** - Used to iterate over elements in a collection, such as arrays or lists.
- **jwhile** - Used to create a loop that executes a block of code as long as a specified condition is true.
- **jswitch** - Used to execute one out of multiple possible blocks of code based on the value of an expression.
- **jcase** - Used within a `jswitch` block to define a specific case.
- **jdefault** - Used within a `jswitch` block to define the default case, executed when no other `jcase` matches.
- **jfunc** - Used to define a function or method.
- **jreturn** - Used to exit from a function and optionally return a value to the caller.
- **jbreak**: Used to exit from a loop or switch statement.
- **jcontinue**: Used to skip the current iteration of a loop and proceed to the next iteration.

---

### Functions
- **$log**: Outputs a log message.
- **$logn**: Outputs a log message with a newline.
- **$clear**: Clears the console or output.
- **$sleep**: Pauses execution for a specified duration.
- **$import**: Imports external files or modules.
- **$include**: Include native package JDM provided.

---

### Comments

- **|>** - Used to comment 1 line.
- **|->** - Used to open a comment block.
- **<-|** - Used to close a comment block.

---

# Usage
### Example Usage for Control Flow Statements

#### Example for `jif` Statement
```jdm
jif (condition) => {
    |> Code block to execute if condition is true
}
jelseif (condition) => {
    |> Code block to execute if previous conditions were false and this condition is true
}
jelse => {
    |> Code block to execute if all previous conditions were false
}
```

#### Example for `jfor` Statement
```jdm
|> Syntax: jfor variable => [start, stop, step] => { code block }

|> Example 1: Start from 1, stop at 45, step by 1
jfor i => [1, 45, 1] => {
    |> Code to execute inside the loop with variable i
}

|> Example 2: Start from 1, stop at 45 (step defaults to 1)
jfor i => 1, 45 => {
    |> Code to execute inside the loop with variable i
}

|> Example 3: Stop at 45 (start defaults to 0, step defaults to 1)
jfor i => 45 => {
    |> Code to execute inside the loop with variable i
}
```

#### Example for `jwhile` Statement, `jbreak` and `jcontinue`
```jdm
jint count = 0;
jwhile (count < 10) =>
{
    |> Code block to execute while count is less than 10
    jif count == 5 => jcontinue;
    jif count >= 8 => jbreak;
    count += 1;
}
```

#### Example for `jforeach` Statement
```jdm
jlist testList = ["TESDT", 0, 4, "test", 4.56, 2];
jmap  testMap  = {1 => "Hello", 2 => "World", 1 => "TEST"};

$logn => "FOREACH KEY ONLY";
$logn => "MAP:";
jforeach k => testMap => {
	$log => "Key: " => k => '\n';
}
$logn => "LIST:";
jforeach v => testList => {
	$log => "Value: " => v => '\n';
}
```

#### Example for `jfunc` Statement, `jreturn` and `casting` 
```jdm
jfunc testFunction(jint arg1, arg2) => {
	$logn => arg1;
	$logn => gettype(arg2);
	jreturn arg1 + jint(arg2);
}
$logn => testFunction(20, "50");
```

---

## Native Function

#### Testing some of the Native Function
```jdm
jfunc testFilterFunction( arg )        => jreturn arg % 2 == 0;
jfunc testReduceFunction( arg1, arg2 ) => jreturn arg1 + arg2;
jfunc testSortIfFunction( arg1, arg2 ) => jreturn arg1 < arg2;

jlist testList = [1, 2, 3, 4, 5];
$logn
	=> "input    : " + jstring(input    ("Test Input: "))
	=> "split    : " + jstring(split    ("Test-Split", "-"))
	=> "len      : " + jstring(len      (testList))
	=> "abs      : " + jstring(abs      (-436))
	=> "ceil     : " + jstring(ceil     (34.3))
	=> "floor    : " + jstring(floor    (34.3))
	=> "max      : " + jstring(max      (testList))
	=> "min      : " + jstring(min      (testList))
	=> "reverse  : " + jstring(reverse  (testList))
	=> "round    : " + jstring(round    (34.76))
	=> "sum      : " + jstring(sum      (testList))
	=> "join     : " + jstring(join     (testList, " -> "))
	=> "index    : " + jstring(index    (testList, 4))
	=> "randint  : " + jstring(randint  (100, 200))
	=> "randfloat: " + jstring(randfloat(10.0, 20.0))
	=> "filter   : " + jstring(filter   (testList, testFilterFunction))
	=> "reduce   : " + jstring(reduce   (testList, testReduceFunction))
	=> "sort_if  : " + jstring(sort_if  (testList, testSortIfFunction))
	=> "chain    : " + jstring(chain    (testList, testList, testList))
	=> "map      : " + jstring(map      (testList, ( arg ) => { jreturn arg ** 2 } )) |> Use anonymous function
	=> "gettype  : " + jstring(gettype  (testFilterFunction))
;
```

#### Testing some of the `jstring` function
```jdm
jstring test = "Hello World";
$logn => "Size   : " + test.size();
$logn => "Replace: " + test.rreplace("o", "oo");
$logn => "Concat : " + test.rconcat("!");
$logn => "Upper  : " + test.rupper();
$logn => "Lower  : " + test.rlower();
$logn => "Reverse: " + test.rreverse();
$logn => "Compare: " + test.compare("Hello, World");
$logn => "Substr : " + test.rsubstr(7, 5);
$logn => "Find   : " + test.find("llo");
$logn => "Count  : " + test.count("o");
$logn => "Empty  : " + test.empty();
$logn => "StartsW: " + test.startsw("Hello");
$logn => "EndsW  : " + test.endsw("World");
$logn => "Insert : " + test.rinsert(7, "JD's ");
$logn => "Erase  : " + test.rerase(7, 5);
$logn => "Trim   : " + test.rtrim(2, 2);
$logn => "Token  : " + test.tokenize("\\s+|,\\s*");
````
---

## Referencing Variable

#### Example of Referencing Variable using `ref`
```jdm
jint test = 10;
jany test2 = ref(test);

test = 20;
$logn => test2;
```

#### Example of Referencing Variable in `jfunc` using `ref`
```jdm
jfunc testFunc(arg) => {
	arg += 10;
}
jint test = 10;
testFunc(ref(test));
$logn => test;
```