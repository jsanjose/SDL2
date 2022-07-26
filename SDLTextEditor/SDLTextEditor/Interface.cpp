#include <iostream>

#include <SDL.h>
#include <SDL_ttf.h>

#include "Interface.h"

void Interface::displayMain() {
	createWindow(windowMain, "Main", 360, 35, config::editWidth, config::editHeight);
	createRenderer(windowMain, rendererMain);
	SDL_SetRenderDrawColor(rendererMain, 173, 216, 230, 255);
	SDL_RenderClear(rendererMain);
	SDL_SetRenderDrawColor(rendererMain, 255, 255, 255, 255);
	rectMain = { 240, 0, 600, 820 };
	SDL_RenderFillRect(rendererMain, &rectMain);
	SDL_RenderPresent(rendererMain);
	inputText(config::sansFont.c_str());
	//saveText();
}

void Interface::processingEvent() {
	int x = config::editX, y = config::editY;
	SDL_StartTextInput();
	while (run) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == SDLK_ESCAPE) {
					run = false;
				}
				if (event.key.keysym.sym == SDLK_RETURN) {
					mainBuffer.push_back('\n');
				}
				if (event.key.keysym.sym == SDLK_BACKSPACE && mainBuffer.length() > 0) {
					pop_utf8(mainBuffer);
				}
				else if (event.key.keysym.sym == SDLK_c && SDL_GetModState() & KMOD_CTRL) {
					SDL_SetClipboardText(mainBuffer.c_str());
				}
				else if (event.key.keysym.sym == SDLK_v && SDL_GetModState() & KMOD_CTRL) {
					mainBuffer = SDL_GetClipboardText();
				}
			}
			if (event.type == SDL_TEXTINPUT) {
				if (!(SDL_GetModState() & KMOD_CTRL && (event.text.text[0] == 'c' || event.text.text[0] == 'C' || event.text.text[0] == 'v' || event.text.text[0] == 'V'))) {
					mainBuffer += event.text.text;
				}
			}
		}

		mainBuffer.push_back(config::cursor);

		surfaceInputText = TTF_RenderUTF8_Blended_Wrapped(fontInput, mainBuffer.c_str(), color->black, config::editWidth - (config::editX * 2));
		textureInputText = SDL_CreateTextureFromSurface(rendererMain, surfaceInputText);
		
		destInputText.x = x;
		destInputText.y = y;
		destInputText.h = surfaceInputText->h;
		destInputText.w = surfaceInputText->w;

		SDL_SetRenderDrawColor(rendererMain, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(rendererMain);
		SDL_RenderCopy(rendererMain, textureInputText, NULL, &destInputText);
		SDL_RenderPresent(rendererMain);

		if (!mainBuffer.empty()) pop_utf8(mainBuffer);

	}
	SDL_StopTextInput();
}

bool Interface::is_leading_utf8_byte(char c)
{
	auto first_bit_set = (c & 0x80) != 0;
	auto second_bit_set = (c & 0x40) != 0;
	return !first_bit_set || second_bit_set;
}

void Interface::pop_utf8(string& s)
{
	while (!is_leading_utf8_byte(s.back()))
		s.pop_back();
	s.pop_back();
}
