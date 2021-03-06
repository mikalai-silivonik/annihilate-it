#include "msGL.h"


#ifdef IOS_GL

#include "CoreFoundation/CoreFoundation.h"

void msGetExecutableDir(string &path)
{
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
    char path1[PATH_MAX];

    if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8 *)path1, PATH_MAX))
    {
        CFRelease(resourcesURL);
        
        path = path1;
    }    
}

#elif defined WINDOWS_GL
                         
#include <windows.h>

void msGetExecutableDir(string &path)
{
    char buf[MAX_PATH];
    GetModuleFileName(0, buf, MAX_PATH);
    path = buf;

    // Extract directory
    path = path.substr(0, path.rfind("\\"));
}

#endif

// modifies given fileName string to contains full path
void msMapDataFileName(string &fileName)
{
    string executablePath = "";
    msGetExecutableDir(executablePath);
    
    //printf(executablePath.c_str());

    fileName = executablePath.append(fileName);
    
    //printf(fileName.c_str());
}


