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
**1. Clone this repository**
```
git clone https://github.com/MFori/mlang.git
```
**2. Create build directory**
```
cd mlang
mkdir build
```
**3. Change to build directory and run cmake (tested with Visual Studio)**
```
cd build
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
func main(): Int {
  println("Hello World")
  return 0
}
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
### Int
A 64 bit integer
```
val myInt = 1
```

### Double
A 64 bit floating point real 
```
val myDouble = 1.5
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
join
stack x heap
rm

### Void

Arrays
-----
rm, validation

Functions
-----
return (optional)

Control Expressions
-----
### If-else

### While

### Do-While

### For
range

### ForEach

Entry Point
-----
In mlang any code can be outside of functions and is automatically executed on startup. However if you want use c-like ```main``` function, there is possibility for that. Just create ```main``` function like below. If you have created ```main``` function and at the same time have some code outside of functions, at first the code outside is perfomed and after that ```main``` function is executed.
```
println("Outside 1") // 1.

func main() {
  // is called automatically
  println("Hello World") // 3.
}

println("Outside 2") // 2.
```

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
