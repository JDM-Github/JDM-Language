# JDM Language

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
# Arguments

- ***Nothing***: Default. Check the JDM Language version.
```cmd
jdm
```
- **--version--**: Check the JDM Language version.
```cmd
jdm __version__
```
- **-d**: Debug mode. It will generate logs (token, parser ast, and compiler steps).
```cmd
jdm -d <filename>
```
- **-t**: Time compile time execution.
```cmd
jdm -t <filename>
```
- **-f**: Filename of script to compile
```cmd
jdm -f <filename>
```

- **-lex**: Takes 2 arguments file from and file to. Tokenize the script and stored in binary
```cmd
jdm -lex <from> <to>
```
- **-parse**: Takes 2 arguments file from and file to. Parse the script or tokenize binary. Create a "JDMS"
```cmd
jdm -parse <from> <to>
```

- **-compile**: Takes 1 arguments file from. Compile the script or "JDMS"
```cmd
jdm -compile <from> <to>
```
