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
* **h help:** show help
* **d debug:** disable code optimizer
* **r run:** directly run code (using JIT) 

If run is disabled, LLVM-IR for the parsed file is outputed (hello_world.mlang.ir) and also executable (hello_world.exe) is generated.

Language Syntax
============
