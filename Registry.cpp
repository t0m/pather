#include <string>
#include <memory>
#include <Windows.h>
#include "Registry.h"
using namespace std;


//
// Open the registry, fetch our path and store the original string and a vector of 
// the string split on ';'
//
Registry::Registry() {
    LPCSTR envKey = "System\\CurrentControlSet\\Control\\Session Manager\\Environment";
    long openResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, envKey, NULL, KEY_WRITE|KEY_READ, &hkey);
    if (openResult != ERROR_SUCCESS) {
        throw "Unable to open registry";
    }

    // first query the size of the PATH env var (excluding \0)
    DWORD sizeOfRegKey = 1;
    long sizeRes = RegGetValue(hkey, NULL, "Path", RRF_RT_ANY, NULL, NULL, &sizeOfRegKey);
    if (sizeRes != ERROR_SUCCESS) {
        throw "Unable to query for path size";
    }

    // then fetch it into our buffer
    unique_ptr<char> keyBuf(new char[(size_t) ++sizeOfRegKey]);
    long queryRes = RegGetValue(hkey, NULL, "Path", RRF_RT_ANY, NULL, keyBuf.get(), &sizeOfRegKey);
    if (queryRes != ERROR_SUCCESS) {
        throw "Unable to query registry key";
    }

    path = string(keyBuf.get());
}

string Registry::getPath() {
    return path;
}

Registry::~Registry() { 
    RegCloseKey(hkey); 
}

void Registry::savePath(const string& path) {
    long saveResult = RegSetValueEx(hkey, "Path", 0, REG_SZ, (const BYTE*) path.c_str(), path.size() + 1);
    if (saveResult != ERROR_SUCCESS) {
        throw "Unable to save registry key";
    }
}

const string path;