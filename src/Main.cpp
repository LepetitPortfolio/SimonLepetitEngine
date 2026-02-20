#pragma once
#include "System/Renderer.h"



int main() {
    
	Renderer renderer;

	renderer.OpenWindow(1600, 900, "Test Window");

	while (renderer.Run())
	{

	}

    return 0;
}
