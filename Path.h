#pragma once
#include <string>
#include <map>
using namespace std;

struct CaseInsensitive { 
    bool operator() (const std::string& lhs, const std::string& rhs) const {
        return _stricmp(lhs.c_str(), rhs.c_str()) < 0;
    }
};

class Path {
public:
    Path() { };
    Path(const string& pathStr);

    bool contains(const string& str);
    void remove(const string& str);
    void add(const string& str);
    string get();

private:
    void setPath(const string& pathStr);
    map<string, int, CaseInsensitive> pathMap; 
    int position;
};