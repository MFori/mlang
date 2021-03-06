# mlang
Simple programming language using LLVM, flex and bison.

# Table of Contents
<!--ts-->
   * [Installation](#installation)
      * [Dependencies](#dependencies)
      * [Build](#build)
   * [Getting Started](#getting-started)
      * [Hello World](#hello-world)
      * [Usage](#usage)
   * [Language Syntax](#language-syntax)
      * [Identifiers](#identifiers)
      * [Variables](#variables)
      * [Data Types](#data-types)
      * [Arrays](#arrays)
      * [Functions](#functions)
      * [Expressions](#expressions)
      * [Control Expressions](#control-expressions)
      * [Entry Point](#entry-point)  
      * [Import](#import)
      * [Comments](#comments)
      * [Terminators](#terminators)
   * [Build-in Functions](#build-in-functions)
   
<!--te-->

Installation
============

Dependencies
-----
* LLVM 11 
* Clang
* flex and bison
* CMake 3.17

Build
-----
**0. Setup LLVM**

See [this guide](llvm-setup.md)

**1. Clone this repository**
```
git clone https://github.com/MFori/mlang.git
```
**2. Go to build directory**
```
cd mlang
cd build
```
Note: Step 3 and 4 can be skipped by running **build.bat** file.

**3. Run cmake (tested with Visual Studio)**
```
cmake .. -G "Visual Studio 16 2019"
```
**4. Build mlang**

Must be build with the same configuration as your LLVM/Clang build (Debug,Release,...)
* Using Visual Studio

  Open *mlang.sln* (generated in build directory) in Visual Studio and compile it
* From Command Line

```
cd build
cmake --build . --config Release
```

* Other IDE

  Open project with any other IDE with support for cmake and build it using Visual Studio compiler (tested with Clion).
  
**5. Done**

After that **mlang.exe** (and other stuff) is generated. Output path depends on step 4. *build/src/Release* for Visual Studio, *cmake-build-release* for Clion,..

Getting Started
============

Hello World
-----
samples/hello_world.mlang
```
func helloWorld() {
  println("Hello World")
}
helloWorld()
```

Usage
-----
```
mlang.exe hello_world.mlang -h -d -r
```
* **-h (--help):** show help
* **-d (--debug):** disable code optimizer
* **-r (--run):** directly run code (using JIT) 

If run is disabled, LLVM-IR for the parsed file is outputed (hello_world.mlang.ir) and also executable (hello_world.exe) is generated.

Language Syntax
============

Identifiers
-----
Identifier can be any word including underscore and digits and must start with letter (```[a-zA-Z][a-zA-Z0-9_]*```).
Identifiers are used for variable and function names and they are case sensitive.

Variables
-----
Variables can be declared with keywords ```var``` and ```val```.
* **```var```** is used for mutable variables
* **```val```** is used for constants

When declaring, variable must be always initialized. Type of variable is automatically deducted from initializing value.

Variable is only visible in its scope (program block) and can be shadowed by any variable with same name inside sub block.
```
val a = 1 // constant
var b = 2 // reasiggneable

func someFunction(Int x) { // x is var and can be reassigned
  val b = x // shadows first b
  x = 0
}
```

Data Types
-----
There is support for ```Int, Double, Char, Bool, String``` data types. Mlang is strongly typed language with static type check. You can use [buildin cast functions](#cast-functions).

### Int
A 64 bit integer
```
val myInt = 1
```

### Double
A 64 bit floating point real 
```
val myDouble1 = 1.5   // 1.5
val myDouble2 = .1    // 0.1
val myDouble3 = 1.e2  // 100.0
val myDouble4 = .1e-1 // 0.01
```

### Char
An 8 bit character. There are two ways for creating char literals. First using ```@``` and required character or ```@@``` and ascii value of required character.
```
val ch1 = @A
val ch2 = @@65
if (ch1 == ch2) { // always true
}
```

### Bool
A bool value, literals can be ```true``` and ```false```.
```
val b1 = true
val b2 = false
```

### String
String is internally represented as char array and has all features like other [arrays](#arrays). String is enclosed in ```"``` or ```'```. A quote inside string must be escaped by ```\```.

String can be created with literal or by calling ```String(Int size)``` function. If second case string is created on heap and must be freed using ```rm``` keyword.
```
val str1 = "This is a \"string\"".
val str2 = String(10)
rm str2
```
There is special expression for joining mutiple strings. To join string insert them isnside ```(.``` and ```.)``` symbols separed by comma. The resulting stream is created on heap and must be freed.
```
val res = (. str1, " and ", str2 .)
rm res
```
You can access and set Char at index like with any other array.

Arrays
-----
Arrays are available for all data types (except Char, but there is String). Array is created by calling appropriate function passing arrays size.
```
IntArray(10)
DoubleArray(10)
BoolArray(10)
String(10) 
```
To access (or assign) array element at index use ```[index]``` expression. Array size is internally stored asside with array items and during access index is validated against array bounds. To get array size you can use buildin function ```sizeOf(array)```.
```
val arr = IntArray(10)
arr[0] = 1
arr[10] = 1 // runtime error index out of range
val i = arr[1]
```
All arrays are created on heap and must be freed with ```rm``` keyword.
```
rm arr
```

Functions
-----
Functions are declared with keyword ```func``` followed with function name ([identifier](#identifiers)), parameters and return type.
Body of function is inside ```{ }```. Parameters are specified by type and name (parameters are mutable, like var variables).
```
func myFunction(Int param1, DoubleArray param2): Bool {
  return true
}
```
Primitive data types (```Int, Double, Bool, Char```) are passed by value, arrays (and string) are passed by reference.
```
func processArray(IntArray arr) {
  arr[0] = -1
}

val array = IntArray(10)
processArray(arr)
println(toString(arr[0])) // -1
```
Return statement is optional and then is returned last statement.
```
func getInt(): Int {
  1
}
```

Expressions
-----
### Assignment
Assignemnt is via ```=``` symbol. Assignemnt results to expression so multiple assignemnt is possible.
```
var a = b = c = 10
```

### Ternary Operator
C-like ternary operator.
```
bool-expression ? then : else
```
```
val a = b > 10 ? 1.5 : 5.1
```

### Boolean expression
For ```Int```, ```Double```, ```Char``` and ```String```:
```
if (a == b) {...}
if (a != b) {...}
if (a > b) {...}
if (a < b) {...}
if (a >= b) {...}
if (a <= b) {...}
```
For ```Bool```:
```
if (true) {...}
if (not true) {...}
if (b1 and b2) {...}
if (b1 or b2) {...}
```

### Binary operator
For ```Int```, ```Double``` and ```Char```:
```
a + b
a - b
a * b
a / b
```
For ```Bool```:
```
a and b
a or b
```

### Unary operator
For ```Int```, ```Double``` and ```Char```:
```
i++   // return i and then increase i by one
++i   // increase i by one and return i
-i
```
For ```Bool```:
```
not b
```

Control Expressions
-----
### If-else
Else is optional.
```
if (bool-expression) {
  statements
} else {
  statements
}
```

### While
```
while (bool-expression) {
  statements
}
```
```
var i = 10
while (i > 0) {
    println(toString(i))
    i--
}
```

### Do-While
```
do {
  statements
} while (bool-expression)
```
```
do {
    i--
} while (i > 0)
```

### For
For loop works only with Int variable which is automatically created and initialized on start bound of [range](#range). Step size of optional (default is 1) and can be Int literal or Int expression.
```
for (var in range) { statements }
for (var in range step step-size) { statements }
```
#### Range
Range is specified by bounds (can be literals or expressions) and keyword ```to``` or ```until```.
```
for (i in 0 to 10) {...}  // i in [1, 10], 10 is included
for (i in 0 until 10) {...}  // i in [0, 10), 10 is excluded
```
```
val start = 0
val end = 100
val s = 2

for (i in start to end step s) {
  println(toString(i)) // prints 0, 2, 4, .., 100
}
```

### ForEach
ForEach loops through array (or string). Loop variable is automatically created and typed to array element type.
```
for (var in array) { statements }
```
```
val array = IntArray(20)

for (i in array) {
  println(toString(i))
}
```

### Break
```break``` keyword can be used inside loops (for, foreach, while, do-while) and jump right after loop.
```
for (i in 0 to 10) {
  if (i > 5) {
    break
  }
}
```

Entry Point
-----
In mlang any code can be outside of functions and is automatically executed on startup. However if you want use c-like ```main``` function, there is possibility for that. Just create ```main``` function like below. If you have created ```main``` function and at the same time have some code outside of functions, at first the code outside is perfomed and after that ```main``` function is executed.
```
println("Outside 1") // called fist 

func main() {
  // is called automatically
  println("Hello World") // called third
}

println("Outside 2") // called second
```
Note: Passing arguments to ```main``` function is currently not possible.

Import
-----
Importing other file is possible via the ```import``` keyword. Path is relative to current file.
```
import other-file.mlang

import dir/another-file.mlang
```

Comments
-----
There is C-like comments support in mlang.
### One Line
One line comments starts with ```//```. All characters until end of line are ignored. 
### Multi Line 
Multi line comment starts with ```/*``` and ends with ```*/```. All characters between are ignored.

Terminators
-----
Grammar uses semicolons as productions terminators, but lexer is automatically inserting them (like GoLang do).
So semicolons are optional.

Build-in Functions
============
Note: for the functions to work is necessary that file ```buildins.bc``` (which is generated during build) is in same directory as ```mlang.exe```
### print
Works like c's printf. Fist arguments is string with format, then goes arguments for that format.
```
val name = "Martin"
val age = 23
print("Name: %s, age: %d", name, age)
```

### println
Same as ```print```, bud add new line after text.

### read
Reads one character from ```stdin```.
```
val ch = read()
```

### readLine
Reads one line from ```stdin```.
```
val line = readLine()
```

### sizeOf
Returns size of array or string.
```
val arr = IntArray(10)
val size = sizeOf(arr) // 10
```

### len
Returns length of string - number of characters until ```'\0'``` or end of string.
```
val str = "Hello World"
var l = len(str) // 11

val str2 = String(20)
l = len(str2) // 0
val size = sizeOf(str2) // 20
rm str2
```

### Cast functions
There are cast functions available for every datatype:
```
toInt(...)
toDouble(...)
toBool(...)
toChar(...)
toString(...)
```
Note: ```toString``` create String on stack, there is no need to free memory using ```rm``` keyword.
