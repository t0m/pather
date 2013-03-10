#include <string>
#include "Registry.h"
using namespace std;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int cmdShow) {
    string targetDir = string(lpCmdLine);
    Registry reg;

    // If the string is in the path, prompt to remove it
    if (reg.contains(targetDir)) {
        int remove = MessageBox(NULL, "That directory is already in your path, would you like to remove it?", 
                                      "That directory is already in your path!", 
                                      MB_YESNO);
        if (remove == IDYES) {
            reg.remove(targetDir);
            reg.save();
        }
        return 0;
    }
    
    reg.add(targetDir);
    reg.save();
    return 0;
}

