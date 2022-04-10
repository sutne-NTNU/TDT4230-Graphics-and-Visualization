# Dynamic Reflections and Refractions

This project utilizes dynamic/buffered cubemaps created in FrameBUffers to emulate realistic reflections and refraction of light.


![High Quality GIF](report/images/refraction/4-chromatic-aberration.png)


---

# Using the application

## Options

For easy configuration i placed a few options in the *src/options.hpp* file, change these according to preference.

## Buttons

### The Camera
- WASD + LShift + Space: Move Camera in all directions
- Mouse Movement: Look around the scene
- Mouse Scroll: Zoom in and out (increase/decrease FOV)
- Q: decrease camera sensitivity and speed
- E: increase camera sensitivity and speed
- R: reset camera sensitivity and speed
  
### The Scene

- T: Pause/Start rotation of the bust
- L: Swap Skybox
- M: Change Material of the bust
- N: Change Material of all the shapes
- UP / DOWN: Increase/decrease reflection resolution
- X: Take a Screenshot

---

# Installation and Execution


## Linux

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

---

## Windows

### Installations

Make sure you have installed the following:

- Git
- Python3
- [OpenAL](https://www.openal.org/downloads/)
- Visual Studio Community. Make sure to include:
  - C++ Desktop Development, with:
    - CMake for windows
    - C++ Clang Tools (optional, but recommended)

### Running in Visual Studio

If you are using using Visual Studio, you should be able to build and execute the project from there by selecting **TDT4230_Project**. (i have not tested this myself, as i have only been using the VSCode method)

### Running in VSCode / from the command line

To enable easy building and running outside of Visual Studio, an extra step is needed.

#### Adding *MSBuild.exe* to path

After all the installations you need to add *MSBuild.exe* to your environment path. This is part of the Visual Studio Community application, but when added to the path it can be easily used from the command line. This executable is what builds the project and creates the executable project file. Simply add the following (or equivalent) to *Path*:

```txt
C:\Program Files\Microsoft Visual Studio\2022\Community\Msbuild\Current\Bin
```

#### CMake

From the root of the project do

```txt
cd build 
cmake ..
```

#### Compile and Run

I included some handy *tasks* for VSCode in the **.vscode/tasks.json** file, these can be used by innstalling the [Task Runner extension](https://marketplace.visualstudio.com/items?itemName=SanaAjani.taskrunnercode) and enables building and running the project with a single buttonpress. The tasks also list all the commands so you can enter them manually.

To build and run the project simply perform the task *Build and Run*. If you have forgotten to use *CMake* there is a handy task for that as well.

If you want to build/run manually from the command line you can that as well. While inside the **build** folder, create the executable with:

```txt
MSBuild.exe .\TDT4230_Project.sln
```

And run it with:

```txt
Debug\TDT4230_Project.exe
```
