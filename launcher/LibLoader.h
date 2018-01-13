#pragma once

#include <string>

namespace shape
{
  class LibLoader
  {
  public:
    LibLoader();
    LibLoader(const std::string& libraryPath, const std::string& libraryName);
    ~LibLoader();

    const std::string& getLibraryPathName() const { return m_libraryPathFile; }
    
    void* getFunction(const std::string& functionName) const;

    void openLibrary();
    void closeLibrary();
    bool isOpenedLibrary();

  private:
    void* m_lib = nullptr;
    std::string m_libraryPath;
    std::string m_libraryName;
    std::string m_libraryPathFile;
  };

}
