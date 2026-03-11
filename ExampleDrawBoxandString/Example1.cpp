#include "FailedDevDeps.h"
#include <Windows.h>

Drawer Drew;


int main() {
    Drew.USEFAILEDDEV(RGB(0,0,0));
    while (true) {
        Drew.DrawBox(100, 100, 50, 50, 26, RGB(255, 0, 0), 1);
        Drew.DrawString(1000, 115,10,1,10,"HELLO!", RGB(255, 255, 255));
        Drew.Present();
    }
    return 0;
}