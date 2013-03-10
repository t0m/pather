#pragma once
#include <string>
#include <Windows.h>
#include "Path.h"
using namespace std;

class Registry {
public:
    Registry();
    ~Registry();

    void savePath(const string& path);
    string getPath();

private:
    HKEY hkey;
    string path;
};