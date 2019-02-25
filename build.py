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
import platform
from subprocess import call

# Date: 25-Feb-2018
# Version:
ver = '1.0.0'

mainDir  = ""

def main():

    parser = argparse.ArgumentParser(description='Building script for Shape.')
    # parser.add_argument('-C','--json', help='Configuration JSON file', required=False)
    # parser.add_argument('-G','--gen', help='Generator. If you do not know call command \"cmake -G\"', required=False)
    # parser.add_argument('-D', action='append', nargs='*', help='Description for bar argument', required=False)    
    args = parser.parse_args()

    # Welcome print
    print('*****************************************************')
    print('Building procedure by Logimic, s.r.o, www.logimic.com')
    print('Version:' + ver)
    print('*****************************************************')

    # Determine config JSON file
    buildCfg = ''
    system = platform.system()

    if system == 'Windows':
        buildCfg = 'bcfgWin.json'
    elif system == 'Linux':
        buildCfg = 'bcfgLin.json'
    else:
        return None

    print('Platform file: ' + buildCfg)

    # Open config file
    try:
        with open(buildCfg) as f:
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
        if gen == "Unix Makefiles":
            buildFolder = "Unix_Makefiles"
        elif gen == "Visual Studio 15 2017 Win64":
            buildFolder = "VS15_2017_x64"  
        elif gen == "Visual Studio 15 2017":            
            buildFolder = "VS15_2017"                      

    buildDir = os.path.normpath(mainDir + "/build/" + buildFolder) 
    
    # Create build dir
    if not os.path.exists(buildDir):
        os.makedirs(buildDir)  

    # Generator
    generator = " -G \"" + gen + "\""

    # Parameters      
    dParams = ""

    for param in params:
        print(param) 
        if "SHAPE_DEPLOY:PATH=" in param:
            index = param.index('=')
            dDir = param[index + 1:]       
 
            if dDir == './' or dDir == '.' or dDir == '' or dDir == ' ': # Shape internal deploy                
                deployDir = os.path.normpath(mainDir + "/deploy/" + buildFolder)
            else:
                deployDir = os.path.normpath(dDir + '/' + buildFolder)
            
            if not os.path.exists(deployDir):
                os.makedirs(deployDir)

            param = 'SHAPE_DEPLOY:PATH=' + deployDir

        if "shape_DIR:PATH=" in param:
            index = param.index('=')
            dDir = param[index + 1:]       
 
            if dDir == './' or dDir == '.' or dDir == '' or dDir == ' ': # Shape internal deploy                                
                os.chdir("..")
                shapeDir = os.path.normpath(os.getcwd() + "/shape/" +  "/build/" + buildFolder)      
                os.chdir(mainDir)          
            else:
                shapeDir = os.path.normpath(dDir + '/' + buildFolder)

            param = 'shape_DIR:PATH=' + shapeDir

        if "shapeware_DIR:PATH=" in param:
            index = param.index('=')
            dDir = param[index + 1:]       
 
            if dDir == './' or dDir == '.' or dDir == '' or dDir == ' ': # Shape internal deploy                                
                os.chdir("..")
                shapewareDir = os.path.normpath(os.getcwd() + "/shapeware/" +  "/build/" + buildFolder)      
                os.chdir(mainDir)          
            else:
                shapewareDir = os.path.normpath(dDir + '/' + buildFolder)

            param = 'shapeware_DIR:PATH=' + shapewareDir            
            
        dParams = dParams + " -D" + param

    print('Final params: ' + dParams)  

    #return None     

    # Building
    if gen == "Unix Makefiles":
 
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
