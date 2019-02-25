
# Shape

![](docs/images/logimic_shape.png)

C++ modular and scalable framework.

## Table of Contents

* [Prerequisites](#prerequisites)
* [Usage](#usage)
* [Generate Project](#generate-project)
* [Add Component](#add-component)
* [Build Project](#build-project)
* [Run Project](#run-project)
* [How It Works](#how-it-works)

## Prerequisites

- Git
- Cmake
- Pyhon 3.x

## Build

```batch
$ git clone https://github.com/logimic/shape.git
$ cd shape
```

Then run Python building sript:

```batch
$ python3 build.py                        #for Linux, Raspberry Pi
$ py build.py                             #for Win
```

Building parameters are specified in **bcfgWin.json** and **bcfgLin.json** files consumed by the building script.

## Generate Project

```bash
$ cd shape
$ py shape.py --newProject MyProject
```
Project has been generated to **MyProject** folder on the same level as **shape** folder.

## Add Component

```bash
$ cd ./path/MyProject
$ py shape.py --newComponent MyComponent
```
Each component is alone folder in project directory.

## Build Project

```bash
$ cd ./path/MyProject
$ py shape.py --build
```
You can also specify generator
```bash
$ py shape.py --build "Visual Studio 15 2017"
```

## Run Project

```bash
$ cd ./path/MyProject
$ py shape.py --run
```
**Note:** Before launch please specify proper  ```MyProject/startup/configuration/config.json``` structure - [see below](#5-launch-configuration).

## How it works

* [Main Function](#main-function)
* [Components](#components)
* [Building](#building)
* [Run](#run)
* [Launch configuration](#launch-configuration)
* [Terms definition](#terms-definition)
* [Naming convention](#naming-convention)

```batch
|- shape              (shape modular framework)
|- MyProject          (your generated project)
    |- build          (built binaries)
    |- startup        (main function and configuration)
    |- MyComponent1   (User Component)  
    |- MyComponent2   (User Component)   
    |- shape.py       (operation script)  
    |- CMakeLists.txt (cmake script)  
```
**MyProject** user project and **shape** framework are on the same folder level.

### 1 Main Function

```batch
|-MyProject           (your generated project)
    |-..
    |-startup         (main function and configuration)
        |- ..     
        |- main.cpp   (main function)
        |- ..

```

Main function is **main.cpp** located in **startup** directory. This starts **shape** plug-in framework and other project components. Any other code shall be located in components.

```cpp
#include <Shaper.h>
#include <Trace.h>
#include <iostream>

TRC_INIT_MNAME("app-start");

int main(int argc, char** argv)
{
  std::cout << "startup ... " << std::endl;
  shapeInit(argc, argv);
  shapeRun();
  return 0;
}
```

### 2 Components

```batch
|-MyProject                 (your generated project)
    |-..
    |-..
    |-MyComponent1          (User Component)   
        |- MyComponent1.h   (header file)  
        |- MyComponent1.cpp (cpp file)
        |- CMakeLists.h     (cmake script)
```

Main components files are **<component_name>.h** and **<component_name>.cpp**.

```cpp
namespace shapeExpl { //namespace as declared in CMake
  class ComponentTemplate : //component class name
    //implemented provided interfaces (multiple inheritance in case of more interfaces)    
    public ITemplateService
  {
  public:
    ComponentTemplate();
    virtual ~ComponentTemplate();

    //example of interface service function
    std::string doService(const std::string & str) const override;

    void activate(const shape::Properties *props = 0); //activate function
    void deactivate();                                 //deactivate function
    void modify(const shape::Properties *props);       //modify function

    //repeated for all required interfaces   
    void attachInterface(shape::ITraceService* iface); //attach required interface
    void detachInterface(shape::ITraceService* iface); //detach required interface

  private:
  };
}
```
If you create component manually you can use [ComponentTemplate](https://github.com/logimic/shape/tree/master/ComponentTemplate).

#### Shape Component Requirements and Recommendations
Shape framework is not magic. Component Instances are not isolated and runs in one process, so one bug can block or kill all process, so besides general C++ good practice please follow these:

- It is required to provide default constructor
 - Dependency are satisfied in attachInterface(\*iface) functions
 - Parameters are delivered in activate(\*props) function
- Store Provided Interface in attachInterface() and postpone some initialization actions to activate(), just to keep everything smoothly running at Start Up
- Don't call blocking functions in activate(), start dedicated thread instead, see example: [Example1_Thread](https://github.com/logimic/shape/tree/master/examples/Example1_Thread)
- Catch all exceptions generated in a thread within this thread
- Document thrown exceptions in Interfaces
- All threads started in activate() join in deactivate()
- Set Interface to nullptr in detach function
- If Required Interface is not MANDATORY it has to be checked for nullptr and protected in thread by mutex to avoid calling invalid Interface pointer
- Use Tracing as much as possible
- Add file and line to exception trace and catch (use Shape Trace features for it)
- Be careful with pPassing Class instances via Interfaces via shared_ptr<>. The instances has to be destroyed in correct order as unloading shared library before calling destructor may lead to crash during shutdown.
- Shared libraries with components has to be built wit the same tool set as it is allowed to pass C++ objects via interfaces. It is assured by Meta Class during Start Up via Compiler identification. It may be problem with pre-build shared libraries

### 3 Building

#### Automatic Building

Shape and project use **CMake** for building. You can build it automatically via these commands:

```bash
$ cd ./shape
$ py shape.py --build
$ cd ./MyProject
$ py shape.py --build
```

These take your default compiler or you can specify it as an option:

```bash
$ py shape.py --build "Visual Studio 15 2017 Win64"
```

#### Manual building

Build **shape** in its directory:
```
rem //launch from <build_dir>
cmake -G "Visual Studio 15 2017" <path_to_root_CMakeLists.txt>
rem //build from generated build environment
cmake --build <build_dir>
```
Build your project **MyProjects** in its directory:
```
rem //launch from <build_dir>
cmake -G <generator> -Dshape_DIR:PATH=<path_to_Shape>  <path_to_root_CMakeLists.txt>
rem //build from generated build environment
cmake --build <build_dir>
```

#### Configure CMake for project

```batch
|-MyProject           (your generated project)
    |- ..
    |- CMakeLists.txt  (main function and configuration)
    |- ..
```

Please check necessary dependencies in root **CMakeLists.txt**:

```
FIND_PACKAGE(shape REQUIRED)
message(STATUS "CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}")
include(${CMAKE_MODULE_PATH}/ShapeComponentDeclaration.cmake)
include_directories(${shape_INCLUDE_DIRS})

```

Each component has its own **CMakeLists.txt**:

```batch
|-MyProject                 (your generated project)
    |- ..
    |- ..
    |- MyComponent1         (User Component)   
        |- ..
        |- CMakeLists.h     (cmake script)
```


The Components has to provide special Meta Class allowing Shape framework to create Component Instances, provide strong type check and bind all Provided and Required Instances. It is declared via auto-generated header file created by CMake. To declare the Meta Class put to CMakeLists.txt these lines:

```
set(COMPONENT shapeExpl::ComponentTemplate)
DeclareShapeComponent(${COMPONENT})
AddShapeProvidedInterface(${COMPONENT} shapeExpl::ITemplateService)
AddShapeRequiredInterface(${COMPONENT} shape::ITraceService MANDATORY MULTIPLE)
ConfigureShapeComponent(${COMPONENT} COMPONENT_HXX)
```
- **DeclareShapeComponent** declare Component Meta Class
- **AddShapeProvidedInterface** declare implemented Provided Interface. Repeated for all interfaces
- **AddShapeRequiredInterface** declare placeholder for Required Interface. Repeated for all interfaces
- **UNREQUIRED | MANDATORY** declares optionality
- **SINGLE | MULTIPLE** declares cardinality
- **ConfigureShapeComponent** auto-generated header file

Auto-generated file is named (from example) e.g: **shapeExpl__ComponentTemplate.hxx** and it is stored in \${CMAKE_CURRENT_BINARY_DIR} directory. This file has to be included in Component's implementation file **cpp**.

### 4 Run project

When you run project, **shape** framework is started automatically. You can run project via script:

```bash
$ cd ./MyProject
$ py shape.py --run
```

Or manually via **startup.exe** with one parameter which is path to **config.json**

```batch
|-MyProject                     (your generated project)
    |- build                    (building folder)
        |- VS14_2015  
        |- ..
        |- startup              (folder with main)     
            |- ..
            |- configuration   
            |   |- config.json  (configuration file)   
            |   |- ..  
            |
            |- Debug  
                |- startup.exe  (start exe file)
                |- ..  
```

Example:

```bash
$ cd ./MyProject/build/VS14_2015/startup
$ ./Debug/startup.exe  ./configuration/config.json

Win:
> cd .\MyProject\build\VS14_2015\startup
> .\Debug\startup.exe  .\configuration\config.json  
```

### 5 Launch configuration

Launch configuration is defined in **config.json** file:

```json
{
  "applicationName": "ShapeTest",
  "resourceDir": "",
  "dataDir": "",
  "cacheDir": "cache",
  "userDir": "",
  "configurationDir": "configuration",
  "deploymentDir": "../../..",
  "components": [
    {
      "name": "shape::TraceFormatService",
      "libraryPath": "shape/bin",
      "libraryName": "TraceFormatService",
      "enabled": true,
      "startlevel": -4
    },
    {
      "name": "shape::TraceFileService",
      "libraryPath": "shape/bin",
      "libraryName": "TraceFileService",
      "enabled": true,
      "startlevel": -3
    },
    {
      "name": "shape::LauncherService",
      "libraryPath": "",
      "libraryName": "",
      "enabled": false,
      "startlevel": -2
    },
    {
      "name": "shape::Example1_Thread",
      "libraryPath": "shape/bin",
      "libraryName": "Example1_Thread",
      "enabled": true,
      "startlevel": 0
    },
    {
      "name": "shape::MyComponent",
      "libraryPath": "shape/bin",
      "libraryName": "MyComponent",
      "enabled": true,
      "startlevel": 0
    }
  ]
}

```
Lets postpone details explanation now. The most important for launching is:
- **configurationDir** directory with components configuration files relative to working directory.
- **deploymentDir** path to deployment directory relative to working directory.
- **components[]** declaring used components in this configuration where
  - **name** is component name (may be different from library name)\
  - **libraryPath** is path to shared library implementing component relative  from deploymentDir.
  - **libraryName** is shared library name. Platform dependent prefix or suffix (lib, sh, dll) is added automaticaly
  - **enabled** is on/off flag if the component shall be loaded or ignored.
  - **startlevel** is start order (lower - more prio), Note it is not important to keep specific order, but it may be appropriate in some cases (e.g. more dependent components started at the end)

#### Component Instances configuration
Components are factories creating Instances according delivered configurations.
The configurations are JSON files and groupped in a directory specified in **Start Up Configuration**
The files names are not important but it must have at least two mandatory items:

- **component** has to corelate with **name** in in **Start Up Configuration**
- **instance** is name of an instance to be created. This name has to be unique.

At least one valid **Component Instance Configuration** has to exists else none instance is created.

If we have declared Components in **Start Up Configuration** e.g:
```json
    {
      "name": "shape::TraceFileService",
      "libraryPath": "",
      "libraryName": "TraceFileService",
      "enabled": true,
      "startlevel": -3
    }
```
Then valid **Component Instance Configuration** can be declared in these files with content:

*./configuration/Trace1.json*
```json
{
  "component": "shape::TraceFileService",
  "instance": "shape::TraceFileService-Instance1",
  "...": "..."
}
```
*./configuration/Trace2.json*
```json
{
  "component": "shape::TraceFileService",
  "instance": "shape::TraceFileService-Instance2",
  "...": "..."
}
```
*./configuration/Trace3.json*
```json
{
  "component": "shape::TraceFileService",
  "instance": "shape::TraceFileService-Instance3",
  "...": "..."
}
```

#### Component Instances Properties
- **activate** Component Instance gets its Properties via **activate(*props)** function. It can be used to read parameters necessary for runtime.
- **modify** Component Instance gets modified Properties via **modify(*props)** function. It can be used to read parameters necessary for runtime.

Note, Shape interface allowing usage of **modify** during runtime is still in developement.


### 6 Terms definition

**Module** is distributable software in form of shared library. Includes components data classes and logically merges related SW parts.

**Interface** is pure virtual (abstract) class containing methods (functions). Used parameters should be general not exposing implementation details.

**Component** is class which implements Interface. There can be more components implementing the same Interface. E.g. component logging to file and component logging into database. Both can implement the same Interface. Component may use other Interfaces implemented by other Components.

**Component Instance** is an object of a component created by the component constructor. Number of created Component Instances depends on an application configuration. E.g. file logging component can have three instances, one for critical errors, second for debug level and third for a specific component.

**Instance Properties** is an object of properties created form component configuration.

**Provided Interface** is running Component Instance implementing the Interface published to be used.

**Required Interface** is published placeholder for  required Interface. If a Component Instance wants to use an Interface it publish the placeholder.

**Attach Interface** is delivering Provided Interface to Required Interface placeholder.

**Detach Interface** is withdrawal of Provided Interface to Required Interface placeholder.

**Interface Cardinality** is SINGLE or MULTIPLE. It means Required Interface instance can be attached just once or all instantiated Interfaces can be attached.

**Interface Optionality** is UNREQUIRED or MANDATORY. It means all MANDATORY Interfaces have to be attached.

**Interface Target** is narrowing of Required Interface to be delivered a component

**Activate** is a Component Instance state when all MANDATORY Required Interfaces were attached and the instance is set by Shape to normal operation. Component properties are delivered via activate.

**Dectivate** is a Component Instance state when some of MANDATORY Required Interface was detached and the instance is set by Shape to stop normal operation.

**Service** is represented by its Interface. Provides service like doing command, send data, parse data, calculate data, registering call-back, etc. Service is a published Interface (Provided or Required) within Shape framework.

### 7 Naming convention
* Name of Interface class shall begin with **I** e.g. IChannel and contains pure virtual methods.
* Name of Interface class declaring Service shall end with **Service, Srvc or S** e.g. **ISchedulerDataSrvc**
* Name of component shall begin with **Cm** e.g. **CmSchedulerData**.
