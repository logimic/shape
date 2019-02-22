# 
# Copyright 2018 Logimic,s.r.o.
# www.logimic.com
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

import sys, os
import pathlib
import shutil
import argparse
import json
from subprocess import call

mainDir  = ""

def main():

    parser = argparse.ArgumentParser(description='Building script for Shape.')
    parser.add_argument('-C','--json', help='Configuration JSON file', required=False)
    # parser.add_argument('-G','--gen', help='Generator. If you do not know call command \"cmake -G\"', required=False)
    # parser.add_argument('-D', action='append', nargs='*', help='Description for bar argument', required=False)    
    args = parser.parse_args()

    # Check arguments
    if not args.json:
        print("No configuration file... e.g. -C myCfg.json")
        return None  

    # Open config file
    try:
        with open(args.json) as f:
            data = json.load(f) 
    except IOError:
        print("Wrong path or config JSON file.!!!")
        return None      

    # Check keys
    if ('generator' not in data):
        print("\generator\" key is not specified in JSON file.")
        return None

    if ('parameters' not in data):
        print("\parameters\" key is not specified in JSON file.")
        return None        

    # Set parametres
    gen = data["generator"]
    params = data["parameters"]
    
    buildFolder = "Unknown_Builder"

    if gen:
        print("Generator:" + gen)
        if gen == "\"Unix Makefiles\"":
            buildFolder = "Unix_Makefiles"
        elif gen == "\"Visual Studio 15 2017 Win64\"":
            buildFolder = "VS15_2017_x64"  
        elif gen == "\"Visual Studio 15 2017\"":            
            buildFolder = "VS15_2017"                      

    buildDir = os.path.normpath(mainDir + "/build/" + buildFolder) 
    
    # Create build dir
    if not os.path.exists(buildDir):
        os.makedirs(buildDir)  

    # Generator
    generator = " -G " + gen

    # Parameters      
    dParams = ""

    for param in params:
        print(param) 
        if "-DSHAPE_DEPLOY:PATH=" in param:
            index = param.index('=')
            dDir = param[index + 1:]       
 
            if dDir == './' or dDir == '.' or dDir == '' or dDir == ' ': # Shape internal deploy                
                deployDir = os.path.normpath(mainDir + "/deploy/" + buildFolder)
            else:
                deployDir = os.path.normpath(dDir + '/' + buildFolder)
            
            if not os.path.exists(deployDir):
                os.makedirs(deployDir)

            param = '-DSHAPE_DEPLOY:PATH=' + deployDir
            print('Final param: ' + param)                

        dParams = dParams + " " + param

    # Building
    if gen == "\"Unix Makefiles\"":
 
        command = "cmake" + generator + " " + dParams + " -DCMAKE_BUILD_TYPE=Debug " + mainDir        

        print("command: " + command)  
        os.chdir(buildDir)   
        os.system(command)    

        os.chdir(mainDir)
        command = "cmake --build " + buildDir + " --config Debug --target install"
        os.system(command)  

        command = "cmake" + generator + " " + dParams + " -DCMAKE_BUILD_TYPE=Release " + mainDir          

        print("command: " + command)  
        os.chdir(buildDir)   
        os.system(command)          

        command = "cmake --build " + buildDir + " --config Release --target install"
        os.system(command)     

    else:
        command = "cmake" + generator + " " + dParams + " " + mainDir        

        print("command: " + command)  
        os.chdir(buildDir)   
        os.system(command)    

        os.chdir(mainDir)
        command = "cmake --build " + buildDir + " --config Debug --target install"
        os.system(command)  

        command = "cmake --build " + buildDir + " --config Release --target install"
        os.system(command)


if __name__ == "__main__":
    mainDir  = os.getcwd()
    main()
