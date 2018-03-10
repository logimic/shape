import sys, os
import pathlib
import shutil
from subprocess import call

mainDir  = ""

def del_evenReadonly(action, name, exc):
    os.chmod(name, stat.S_IWRITE)
    os.remove(name)

def copytree(src, dst, symlinks=False, ignore=None):
    if not os.path.exists(dst):
        os.makedirs(dst)
    for item in os.listdir(src):
        s = os.path.join(src, item)
        d = os.path.join(dst, item)
        if os.path.isdir(s):
            copytree(s, d, symlinks, ignore)
        else:
            if not os.path.exists(d) or os.stat(s).st_mtime - os.stat(d).st_mtime > 1:
                shutil.copy2(s, d)    

# Updates CPP file - comments two lines...
def updateCpp (path):
    #ok
    f = open(os.path.normpath(path),'r')
    filedata = f.read()
    f.close()

    newdata = filedata.replace("#include \"StaticComponents.h\"","//#include \"StaticComponents.h\"")
    newdata2 = newdata.replace("staticInit();","//staticInit();")

    f = open(os.path.normpath(path),'w')
    f.write(newdata2)
    f.close()    


def userMakefile (path, name):
    os.chdir(path)
    
    print("Creating Makefile.txt for project.." + name)
    fo = open("CMakeLists.txt", "w")

    fo.write ("project(" + name + ")")

    fo.write ("\n\ncmake_minimum_required(VERSION 3.0)")

    fo.write ("\n\n#set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)")
    fo.write ("\n#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)")
    fo.write ("\n#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)")

    fo.write ("\n\nif(NOT CMAKE_BUILD_TOOL MATCHES \"(msdev|devenv|nmake|MSBuild)\")")
  
    fo.write ("\n  include(CheckCXXCompilerFlag)")
    fo.write ("\n  CHECK_CXX_COMPILER_FLAG(\"-std=c++11\" COMPILER_SUPPORTS_CXX11)")
    fo.write ("\n  CHECK_CXX_COMPILER_FLAG(\"-std=c++0x\" COMPILER_SUPPORTS_CXX0X)")
    fo.write ("\n  if(COMPILER_SUPPORTS_CXX11)")
    fo.write ("\n    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -fPIC -std=c++11\")")
    fo.write ("\n  elseif(COMPILER_SUPPORTS_CXX0X)")
    fo.write ("\n    set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS} -fPIC -std=c++0x\")")
    fo.write ("\n  else()")
    fo.write ("\n    message(STATUS \"The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.\")")
    fo.write ("\n  endif()")
    fo.write ("\nendif()")

    fo.write ("\n\nFIND_PACKAGE(shape REQUIRED)")

    fo.write ("\n\nmessage(STATUS \"CMAKE_MODULE_PATH: ${CMAKE_MODULE_PATH}\")")
    fo.write ("\ninclude(${CMAKE_MODULE_PATH}/ShapeComponentDeclaration.cmake)")

    fo.write ("\n\ninclude_directories(${shape_INCLUDE_DIRS})")
    fo.write ("\ninclude_directories(${CMAKE_SOURCE_DIR}/include)")

    fo.write ("\n\nadd_subdirectory(startup)")
    fo.write ("\nadd_subdirectory(Example1_Thread)")

    fo.close()

def makeBat (path, projName):
    os.chdir(path)

    print("Creating build32.bat for " + projName)
    fo = open("build32.bat", "w")

    fo.write ("set project = " + projName)

    fo.write ("\nrem //expected build dir structure")
    fo.write ("\nset buildexp=build\\VS14_2015")

    fo.write ("\nset currentdir=%cd%")
    fo.write ("\nset builddir=.\\\%buildexp%")
    fo.write ("\nset libsdir=..\\\..")

    fo.write ("\nmkdir %builddir%")

    fo.write ("\nrem //get path to to Shape libs")
    fo.write ("\nset shape=..\\\shape\\\%buildexp%")
    fo.write ("\npushd %shape%")
    fo.write ("\nset shape=%cd%")
    fo.write ("\npopd")

    fo.write ("\nrem //launch cmake to generate build environment")
    fo.write ("\npushd %builddir%")
    fo.write ("\ncmake -G \"Visual Studio 15 2017\" -Dshape_DIR:PATH=%shape% %currentdir%")
    fo.write ("\npopd")

    fo.write ("\nrem //build from generated build environment")
    fo.write ("\ncmake --build %builddir%")

    fo.close()

# Generate project to folder 'name' on the same level as 'shape' folder
def generateProject(name):

    dirname = os.path.basename(os.getcwd())

    if dirname == "shape":
        print ("I am generating project.." + name)

        # Create project folder
        os.chdir("..")
        projFolder = os.path.normpath(os.getcwd() + "/" + name)    
        pathlib.Path(projFolder).mkdir(parents=True, exist_ok=True)

        # Copy script    
        shutil.copy2(os.path.normpath(mainDir + "/shape.py"),
                os.path.normpath(projFolder + "/shape.py"))    

        # Copy example
        copytree(os.path.normpath(mainDir + "/examples/Example1_Thread"),
                os.path.normpath(projFolder + "/Example1_Thread"))
        # Copy startup
        copytree(os.path.normpath(mainDir + "/startup"),
                os.path.normpath(projFolder + "/startup"))        

        # Create makefile
        userMakefile(projFolder, name)     

        # Create BAT file
        makeBat (projFolder, name)

        # Patch CPP
        updateCpp(os.path.normpath(projFolder + "/startup/main.cpp"))

    else:
        print("You must be in shape folder...")

        

# Generate componenct to project
# The command must be called from project folder, not shape folder
def generateComponent(name):
    print ("I am generating component..." + name)

    dirname = os.path.basename(os.getcwd())

    if dirname == "shape":
        print("Please call this command from you project folder, not shape folder!")
    else: 
        # print ("cur proj name is:" + str(dirname))

        compPath = os.path.normpath(mainDir + "/" + name)

        # Create folder
        if not os.path.exists(compPath):
            os.makedirs(compPath)

        exampleFolder = mainDir + "/Example1_Thread"

        # Copy and update H file
        f = open(os.path.normpath(exampleFolder + "/Example1_Thread.h"),'r')
        filedata = f.read()
        f.close()

        newdata = filedata.replace("Example1_Thread",name)

        f = open(os.path.normpath(compPath + "/" + name + ".h"),'w')
        f.write(newdata)
        f.close()  

        # Copy and update CPP file
        f = open(os.path.normpath(exampleFolder + "/Example1_Thread.cpp"),'r')
        filedata = f.read()
        f.close()

        newdata = filedata.replace("Example1_Thread",name)

        f = open(os.path.normpath(compPath + "/" + name + ".cpp"),'w')
        f.write(newdata)
        f.close()   

        # Copy and update CMake file
        f = open(os.path.normpath(exampleFolder + "/CMakeLists.txt"),'r')
        filedata = f.read()
        f.close()

        newdata = filedata.replace("Example1_Thread",name)

        f = open(os.path.normpath(compPath + "/CMakeLists.txt"),'w')
        f.write(newdata)        
        f.close()     

        ## Add to main CMake.txt     
        f = open(os.path.normpath(mainDir + "/CMakeLists.txt"),'r')
        filedata = f.read()
        f.close()

        if filedata.find("add_subdirectory(" + name + ")") == -1:
            filedata2 = filedata + "\nadd_subdirectory(" + name + ")"

            f = open(os.path.normpath(mainDir + "/CMakeLists.txt"),'w')
            f.write(filedata2)        
            f.close()     

# Build project
def build(generator):

    dirname = os.path.basename(os.getcwd())

    if dirname == "shape":
        print ("I am building shape...")      

        buildDir = os.path.normpath(mainDir + "/build/VS14_2015") 

        # Create build dir
        if not os.path.exists(buildDir):
            os.makedirs(buildDir)  

        os.chdir(buildDir)

        if generator == "default":
            command = "cmake " + " " + mainDir
        else:
            command = "cmake -G \"" + generator +  "\" " + mainDir

        print("command: " + command)  
        call(command) 

        call("cmake --build " + buildDir) 

    else: 
        print ("I am building project...")          

        os.chdir("..")
        shape = os.path.normpath(os.getcwd() + "/shape/build/VS14_2015")    
        buildDir = os.path.normpath(mainDir + "/build/VS14_2015") 
        os.chdir(mainDir)

        # print("Shape: " + shape)  
        # print("buildDir: " + buildDir)  
        # print("cur Dir: " + os.getcwd())  

        # Create build dir
        if not os.path.exists(buildDir):
            os.makedirs(buildDir)    

        os.chdir(buildDir)

        if generator == "default":
            command = "cmake -Dshape_DIR:PATH=" + shape + " " + mainDir
        else:
            command = "cmake -G \"" + generator + "\" -Dshape_DIR:PATH=" + shape + " " + mainDir

        print("command: " + command)  

        call(command)    

        call("cmake --build " + buildDir) 


# Run project
def run():

    dirname = os.path.basename(os.getcwd())

    if dirname == "shape":
        print("Please call this command from you project folder, not shape folder!")
    else: 
        print ("I am running...")          

        startupDir = os.path.normpath(mainDir + "/" + "build/VS14_2015/startup")
        startup = os.path.normpath(startupDir + "/Debug/startup.exe")
        config = os.path.normpath( "configuration/config.json")
        command = startup + " " + config

        os.chdir(startupDir)
        # print("Current directory is : " + os.getcwd())    
        # print("Call: " + startup)    
        # print("Command: " + command)    

        call(command)    

def main(argv):
    total = len(sys.argv) #number of args
    cmdargs = str(sys.argv) #arg list

    #print ("Params:" + sys.argv[0] + " total:" + str(total))

    if total < 2:
        print ("No parameter!")
    else:
        cmd=sys.argv[1]        
        #print ("Parameter:" + cmd)    
        if cmd == "--newProject":
            if total > 2:  
                generateProject(sys.argv[2])
            else: 
                print ("Command parameter missing...")
        elif cmd == "--newComponent":
            if total > 2:  
                generateComponent(sys.argv[2])
            else: 
                print ("Command parameter missing2...")          
        elif cmd == "--run":
                run()  
        elif cmd == "--build":
            if total > 2:                
                build(sys.argv[2])  
            else:                               
                build("default")
        elif cmd == "--help":
            print ("\n--- SHAPE help ---")
            print ("\npy shape.py [command] [options]\n")
            print ("Commands:")
            print ("\t--help: shows help")
            print ("\t--newProject: Creates new projects. You have to call this command from shape folder.")
            print ("\t--newComponent: Creates new projects. You have to call this command from your project folder.")
            print ("\t--run: Runs project.")
            print ("\nExamples:")
            print ("\tpy shape.py --newProject MyProject")  
        else:
            print ("Unknown command, use --help")

if __name__ == "__main__":
    mainDir  = os.getcwd()
    main(sys.argv)


