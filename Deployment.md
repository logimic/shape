# Deployment

Automatic deployment is implemented via Cmake install. The deployment path has to be provided as Cmake variable **SHAPE_DEPLOY** to configuration  phase, e.g.:

```
cmake -G "Visual Studio 15 2017 Win64" -DSHAPE_DEPLOY:PATH=<PATH> . 
``` 

The PATH can be arbitrary absolute path. If not provided it is set to `<SHAPE_PATH>/build/deploy` by default.
As all projects depend on Shape and uses Cmake command `find_package(shape)` to get it, the deploy directory is provided to each component configuration via **shape_DEPLOY** global variable. The variable can be referred in component's CMakeLists.txt to be deployed by install. It is recommended to use generator specific name in deploy path to avoid mixing of binaries provided by different compilators. For example: `C:/shape/deploy/VS15_2017_x64`

Deployment can be shared by more projects based on Shape or can be dedicated just for one project. It is suitable e.g. if a project or projects group depends on specific Shape version. 

## Deploy CMake support functions

To structure Deploy in uniform way use predefined CMake functions in `shape/cmake/ShapeComponentDeclaration.cmake`

- `function(DeployShapeComponent componentName)` to deploy component's binary
- `function(DeployShapeConfiguration cfgName)` to deploy runtime configuration
- `function(DeployShapeConfigurationStartUp executable cfgName)` to deploy StartUp script
- `function(ConfigureMsvcProject executable cfgName)` to modify MSVC project user setup. It applies only if `vcxproj.user` 
doesn't already exists to avoid previous manual setup overwrite.
- `DeployShapeConfigurationTest(executable cfgName)` to deploy test based on cfgName configuration

## Deploy tree structure
Typical deployment build by recommended Cmake functions looks like:

deploy
- **generator1** used cmake generator
  - **Debug** build configuration
    - **project1** project name
     - **bin** shared libs
     - **lib** static libs
     - **runcfg** predefined runtime configurations
       - **cfg1** configuration
- generator2
 - ...

For example
```
deploy:.
├───VS15_2017
    └───Debug
        ├───shape
        │   ├───bin
        │   │       Example1_Thread.dll
        │   │       startup.exe
        │   │       TraceFileService.dll
        │   │       TraceFormatService.dll
        │   │
        │   ├───lib
        │   │       Example1_Thread.lib
        │   │       TraceFileService.lib
        │   │       TraceFormatService.lib
        │   │
        │   └───runcfg
        │       └───testStartup
        │           │   StartUp.bat
        │           │
        │           └───configuration
        │                   config.json
        │                   shape__ConfigurationService.json
        │                   shape__Example1_Thread.json
        │                   shape__LauncherService.json
        │                   shape__TraceFileService-ATest.json
        │                   shape__TraceFileService-CommonTrace.json
        │                   shape__TraceFileService-MyTrace.json
        │                   shape__TraceFormatService-Standard.json
        │
        └───shapeware
            ├───bin
            │       CommandLineService.dll
            │       CommandService.dll
            │       CppRestService.dll
            │       CurlRestApiService.dll
            │       MqMessageService.dll
            │       startup.exe
            │       TestCppRestService.dll
            │       TestSimulationIRestApiService.dll
            │       TestWebsocketService.dll
            │       WebsocketCppClientService.dll
            │       WebsocketCppService.dll
            │       WebsocketService.dll
            │
            ├───lib
            │       CommandLineService.lib
            │       CommandService.lib
            │       CppRestService.lib
            │       CurlRestApiService.lib
            │       MqMessageService.lib
            │       TestCppRestService.lib
            │       TestSimulationIRestApiService.lib
            │       TestWebsocketService.lib
            │       WebsocketCppClientService.lib
            │       WebsocketCppService.lib
            │       WebsocketService.lib
            │
            └───runcfg
                ├───startup
                │   │   StartUp.bat
                │   │
                │   └───configuration
                │           config.json
                │           shape__CommandLineService.json
                │           shape__CommandService.json
                │           shape__LauncherService.json
                │           shape__TraceFileService.json
                │           shape__TraceFormatService-Standard.json
                │
                └───TestWebsocketService
                    │   StartUp.bat
                    │
                    └───configuration
                            config.json
                            shape__LauncherService.json
                            shape__TestWebsocketService.json
                            shape__TraceFileService.json
                            shape__TraceFormatService-Standard.json
                            shape__WebsocketCppClientService-1.json
                            shape__WebsocketCppClientService-2.json
                            shape__WebsocketCppService-1339.json
                            shape__WebsocketCppService-1340.json
```
