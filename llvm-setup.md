# How To Setup LLVM and Clang

**1. Download llvm project source codes**

https://releases.llvm.org/download.html

**2. Generate Visual Studio solution**

```
mkdir build
cd build
cmake -DLLVM_ENABLE_PROJECTS=clang -G "Visual Studio 16 2019" -A x64 -Thost=x64 .. 
```

**3. Open llvm.sln and build ALL_BUILD project**

Note I recommend release mode, whichs build is significantly smaller then debug.

**4. Add global variables**

```
LLVM_ROOT=path_to_llvm\build
LLVM_DIR=path_to_llvm\build
```

**5. Add Clang to path variable**
```
path_to_llvm\build\Release\bin
```

**5. Continue to [README.md](README.md)**
