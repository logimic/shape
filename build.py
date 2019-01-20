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
from subprocess import call

mainDir  = ""

def main():

    parser = argparse.ArgumentParser(description='Building script for Shape.')
    parser.add_argument('-G','--gen', help='Generator', required=False)
    # parser.add_argument('-D', action='append', nargs='*', help='Description for bar argument', required=False)    
    args = parser.parse_args()

    if args.gen:
        print("Generator:" + args.gen)    

    buildexp=os.path.normpath("/build/VS15_2017_x64") 
    buildDir = os.path.normpath(mainDir + buildexp) 
    
    # Create build dir
    if not os.path.exists(buildDir):
        os.makedirs(buildDir)  

    deployDir = os.path.normpath(mainDir + "/deploy/VS15_2017_x64") 
    
    # Create build dir
    if not os.path.exists(deployDir):
        os.makedirs(deployDir)           

    generator = ""
    if args.gen:
        generator = " -G \"" + args.gen +  "\""   

    dTesting = "-DBUILD_TESTING:BOOL=true"    
    dShapeDeploy= "-DSHAPE_DEPLOY:PATH=" + deployDir
        
    parameters = " " + dTesting + " " + dShapeDeploy

    command = "cmake" + generator + parameters + " " + mainDir        

    print("command: " + command)  
    os.chdir(buildDir)   
    call(command)    

    os.chdir(mainDir)
    command = "cmake --build " + buildDir + " --config Debug --target install"
    call(command)  

    command = "cmake --build " + buildDir + " --config Release --target install"
    call(command)      

if __name__ == "__main__":
    mainDir  = os.getcwd()
    main()
