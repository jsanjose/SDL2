#include <iostream>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Connection.h"
#include "Interface.h"

void startApplication() {
	Interface* startInterface = new Interface;
	ConnectionSDL* startConnect = new ConnectionSDL;
	startInterface->displayMain();
	startInterface->processingEvent();
	delete startInterface;
}

int main(int argc, char** args)
{
	startApplication();
	return 0;
}