#include <string>
#include "Registry.h"
#include "Path.h"
using namespace std;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int cmdShow) {
    string targetDir = string(lpCmdLine);
    Registry reg;
    Path path(reg.getPath());

    // If the string is in the path, prompt to remove it
    if (path.contains(targetDir)) {
        int remove = MessageBox(NULL, "That directory is already in your path, would you like to remove it?", 
                                      "That directory is already in your path!", 
                                      MB_YESNO);
        if (remove == IDYES) {
            path.remove(targetDir);
            reg.savePath(path.get());
        }
        return 0;
    }
    
    path.add(targetDir);
    reg.savePath(path.get());
    return 0;
}

