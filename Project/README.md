# Project

## Description

<!-- What is the project about and what is it supposed to do -->

## Desired Result

<!-- What exactly should the finished product look like/How should it behave -->


## Installation and Execution


### Linux

Make sure you have installed the following:

- Git
- Python3
- [OpenAL](https://www.openal.org/downloads/)
- C++ compiler
- CMake

To run the program simply make the project with:

```txt
cd build
cmake ..
```

And then build and run it with:

```txt
make
```

### Windows

#### Installations

Make sure you have installed the following:

- Git
- Python3
- [OpenAL](https://www.openal.org/downloads/)
- Visual Studio Community. Make sure to include:
  - C++ Desktop Development, with:
    - CMake for windows
    - C++ Clang Tools

##### How to use VSCode

> This is an optional step, if you specifically want to use VSCode.

After all the installations you need to add **MSBuild.exe** to your environment path. This is a part of the Visual Studio Community program, but when added to the path it can be easily used from any terminal. Simply add the following (or similar) to *Path*:

```txt
C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin
```

#### Building with CMake

From the root of the project do

```txt
cd build 
cmake ..
```
#### Compile and run

##### Visual Studio

If you are using using Visual Studio, you should be able to build and execute the project from there by selecting **glowbox**.

##### VSCode

I included some handy *tasks* for VSCode in the **.vscode/tasks.json** file, these can be used by using/innstalling the *Task Runner* extension and enables building and running the project with a single buttonpress.

If you want to perform it manually you, while still inside the **build** folder, create the executable with:

```txt
MSBuild.exe .\TDT4230_Project.sln
```

And run it with:

```txt
Debug\TDT4230_Project.exe
```