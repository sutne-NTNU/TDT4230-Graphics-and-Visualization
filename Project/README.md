# Light reflection and refraction using dynamic cubemaps






## Using the application

### Options

For easy configuration i placed a few options in the *src/options.hpp* file, change these according to preference.

### Buttons

- WASD + LShift + Space: Move Camera in all directions
- Q: decrease camera sensitivity and speed
- E: increase camera sensitivity and speed
- R: reset camera sensitivity and speed
- T: Pause/Start rotation of the bust
- L: Swap Scene (skybox)
- M: Change Material of the bust
- N: Change Material of all the shapes
- X: Take a Screenshot
- UP / DOWN: Increase/decrease reflection resoution


## Installation and Execution


### Linux

Make sure you have installed the following:

- Git
- Python3
- [OpenAL](https://www.openal.org/downloads/)
- C++ compiler
- CMake

To build, compile and run the program simply use:

```txt
make run
```

you can also use:

```txt
make help
```
To get a list of other available commands.

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

#### Building with CMake

From the root of the project do

```txt
cd build 
cmake ..
```
#### Compile and run

##### Visual Studio

If you are using using Visual Studio, you should be able to build and execute the project from there by selecting **TDT4230_Project**. (i have not tested this myself, as i have only been using the VSCode method)

##### VSCode

###### Adding *MSBuild.exe* to path

> This is an optional step, if you specifically want to use VSCode.

After all the installations you need to add *MSBuild.exe* to your environment path. This is a part of the Visual Studio Community program, but when added to the path it can be easily used from any terminal. Simply add the following (or equivalent) to *Path*:

```txt
C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin
```

###### Compile and Run

I included some handy *tasks* for VSCode in the **.vscode/tasks.json** file, these can be used by innstalling the *Task Runner* extension and enables building and running the project with a single buttonpress. The tasks also list all the commands so you can enter them manually.

To build and run the project simply perform the task *Build and Run*. If you have forgotten to use *CMake* there is a handy task for that as well.

If you want to perform it manually you, while inside the **build** folder, create the executable with:

```txt
MSBuild.exe .\TDT4230_Project.sln
```

And run it with:

```txt
Debug\TDT4230_Project.exe
```
