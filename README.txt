This is a simple 1 Header lib using Windows GDI to do simple tasks.

How do i use it?

-----------------------------------------------

#include "FailedDevDeps.h"
#include <Windows.h>

Drawer Drew; //Drew is just an example name
Drawer USEFAILEDDEV(); //This creates an Transparent Overlay
-----------------------------------------------

Thats it, now lets move on to the syntax!


DrawString -> This function expects a X coordinate, a Y coordiante, the width of the String, the depth, the height, a char with the desired text to be displayed and the color. 
DrawBox -> This function expects a X coordinate, a Y coordiante, the width of the String, the depth, the height, the color and how thick the box should be.
