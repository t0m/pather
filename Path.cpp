#include "Path.h"
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
using namespace std;

//
// Uses a map for quick add/remove/lookup and preserves the original order 
// of the string by saving the position as the map value
//
Path::Path(const string& pathStr) {
    setPath(pathStr);
}

//
// Split the path string on ';' and build our map
//
void Path::setPath(const string& pathStr) {
    string buf = "";
    char delim = ';';
    position = 0;

    for (char chr : pathStr) {
        if (chr == delim) {
            if (buf != "") {
                add(buf);
            }
            buf = "";
        } else {
            buf += chr;
        }
    }

    if (buf.length() > 0) {
        add(buf);
    }
}

bool Path::contains(const string& str) {
    return pathMap.find(str) != pathMap.end();
}

void Path::remove(const string& str) {
    pathMap.erase(str);
}

void Path::add(const string& str) {
    pathMap[str] = position++;
}

//
// Rebuild the path string by sorting the values in the map based
// on the key (position) and imploding 
//
string Path::get() {
    vector<pair<string, int>> ordered;
    for (auto keyval : pathMap) {
        ordered.push_back(keyval);
    }

    auto sortByPosition = [](const pair<string, int>& lhs, const pair<string, int>& rhs) {
        return lhs.second < rhs.second;
    };
    sort(ordered.begin(), ordered.end(), sortByPosition);
    
    string imploded;
    for (auto pair : ordered) {
        imploded.append(pair.first).append(";");
    }
    return imploded;
}

map<string, int, CaseInsensitive> pathMap; 
int position;
