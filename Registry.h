#pragma once
#include <string>
#include <Windows.h>
#include "Path.h"
using namespace std;

class Registry {
public:
    Registry();
    ~Registry();

    void save();
    bool contains(const string& dir);
    void remove(const string& dir);
    void add(const string& dir);


private:
    HKEY hkey;
    Path path;
    string originalPath;
};