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
* flex and bison
* CMake 3.17

Build
-----

Getting Started
============

Hello World
-----
hello_world.mlang
```
func main(): Int {
  println("Hello world")
  return 0
}
```

Usage
-----
```
mlang hello_world.mlang -h -d -r
```
* **h help:** show help
* **d debug:** disable code optimizer
* **r run:** directly run code (using JIT) 

If run is disabled, LLVM-IR for the parsed file is outputed (hello_world.mlang.ir) and also executable (hellow_world.exe) is generated.

Language Syntax
============
