/**
 * Copyright 2018 Logimic,s.r.o.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <string>
#include <vector>

namespace shape
{
  class LibLoader
  {
  public:
    LibLoader();
    //LibLoader(const std::string& libraryPath, const std::string& libraryName);
    ~LibLoader();

    const std::string& getLibraryPathName() const { return m_libraryPathFile; }
    
    void* getFunction(const std::string& functionName) const;

    bool findLibrary(const std::string& deploymentPath, const std::string& libraryPath, const std::string& libraryName);
    const std::vector<std::string>& getTriedPaths() const { return m_triedPaths; }

    void openLibrary();
    void closeLibrary();
    bool isOpenedLibrary();

  private:
    void* m_lib = nullptr;
    std::string m_libraryPath;
    std::string m_libraryName;
    std::string m_libraryPathFile;
    std::vector<std::string> m_triedPaths;

  };

}
