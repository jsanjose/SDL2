#include "Text.h"

bool Text::outText(SDL_Renderer*& renderer, const char* message, const char* styleText,
	int size, SDL_Color color, int x, int y) {
	if (TTF_Init() == -1) {
		std::cout << "SDL_TTF_Init Error: " << TTF_GetError() << std::endl;
		return false;
	}
	fontOut = TTF_OpenFont(styleText, size);
	surfaceOutText = TTF_RenderUTF8_Solid(fontOut, message, color);
	textureOutText = SDL_CreateTextureFromSurface(renderer, surfaceOutText);

	int w = 0, h = 0;
	TTF_SizeUTF8(fontOut, message, &w, &h);
	destOutText.x = x;
	destOutText.y = y;
	destOutText.w = w;
	destOutText.h = h;

	SDL_RenderCopy(renderer, textureOutText, NULL, &destOutText);
	return true;
}

bool Text::inputText(const char* styleText) {
	if (TTF_Init() == -1) {
		std::cout << "SDL_TTF_Init Error: " << TTF_GetError() << std::endl;
		return false;
	}
	fontInput = TTF_OpenFont(styleText, config::fontSize);
}

void Text::saveText() {
	std::ofstream save;
	save.open("ourFile.txt");
	save << mainBuffer;
	save.close();
}