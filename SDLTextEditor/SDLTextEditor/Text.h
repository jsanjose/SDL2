#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Config.h"

class Text {
private:
	TTF_Font* fontOut;
	SDL_Surface* surfaceOutText;
	SDL_Texture* textureOutText;
	SDL_Rect destOutText;
protected:
	SDL_Surface* surfaceInputText;
	SDL_Texture* textureInputText;

	std::string mainBuffer;
	TTF_Font* fontInput;
	SDL_Rect destInputText;
	bool outText(SDL_Renderer*& renderer, const char* message, const char* styleText,
		int size, SDL_Color color, int x, int y);
	bool inputText(const char* styleText);
	void saveText();
};

