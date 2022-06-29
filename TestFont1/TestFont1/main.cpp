#include <iostream>
#include <string>
#include <vector>
#include <crtdbg.h>
#include <SDL.h>
#include <SDL_ttf.h>
using namespace std;

class EngineSDL {
private:
	int width, height;
	SDL_Renderer* renderer = nullptr;
	SDL_Window* window = nullptr;
	bool fullScreen = false;
	string fontPath;
	vector<TTF_Font*> fonts;
	int selectedFont = -1;

public:
	EngineSDL(int window_width, int window_height, string window_title) : width(window_width), height(window_height)  {
		SDL_Init(SDL_INIT_VIDEO);
		TTF_Init();

		window = SDL_CreateWindow(window_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, 0);
		renderer = SDL_CreateRenderer(window, -1, 0);
	}

	~EngineSDL() {
		for (TTF_Font* fnt : fonts) {
			TTF_CloseFont(fnt);
		}
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		TTF_Quit();
		SDL_Quit();
	}

	// Devuelve el ancho de la ventana.
	int getWidth() {
		return width;
	}

	// Devuelve el alto de la ventana.
	int getHeight() {
		return height;
	}

	// Cambia de modo ventana a pantalla completa, y viceversa.
	void switchFullScreen() {
		fullScreen = !fullScreen;
		if (fullScreen) {
			SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
		}
		else {
			SDL_SetWindowFullscreen(window, 0);
		}
	}

	// Ruta a los ficheros con los tipos de letra (fonts)
	void setFontPath(string path) {
		fontPath = path;
	}

	// Carga un fichero de tipo de letra, con un tamaño dado.
	// Devuelve el 'fontId'
	int loadFont(string fontFileName, int fontSize) {
		if (!fontPath.empty()) {
			TTF_Font *fnt = TTF_OpenFont((fontPath + fontFileName).c_str(), fontSize);
			if (fnt != nullptr) {
				fonts.push_back(fnt);
				return (int)(fonts.size() - 1);	// fontId
			}
			else return -1;
		}
		else return -1;
	}

	// Selecciona un tipo de letra (font) por su 'fontId'
	void setFont(int fontId) {
		if (fonts.size() > 0) {
			if (fontId >= 0 && fontId < fonts.size()) {
				selectedFont = fontId;
			}
			else selectedFont = -1;
		}
	}

	// Imprime un texto en la ventana, con un color determinado.
	// Usa el tipo de letra seleccionado.
	// 'textWidth' indica la máxima anchura antes de que el texto salte de línea.
	// 'textY' es la posición Y donde se imprimirá el texto.
	int printText(string text, SDL_Color color, int textWidth, int textY) {
		if (selectedFont < fonts.size()) {
			TTF_Font* font = fonts.at(selectedFont);

			SDL_Surface* surface = TTF_RenderText_Solid_Wrapped(font, text.c_str(), color, textWidth);
			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

			int textureW = 0, textureH = 0;

			SDL_QueryTexture(texture, NULL, NULL, &textureW, &textureH);
			SDL_Rect dstRect = { 0, textY, textureW, textureH };

			SDL_RenderCopy(renderer, texture, NULL, &dstRect);
			SDL_RenderPresent(renderer);

			SDL_DestroyTexture(texture);
			SDL_FreeSurface(surface);

			return (textY + textureH);
		}
		else return textY;
	}
};



// ----------
// MAIN
// ----------
int main(int argc, char* argv[])
{
	bool quit = false;
	SDL_Event event;
	EngineSDL engine(800, 600, "Test font 1");

	// TTF test
	engine.setFontPath("C:\\Temp\\SDL2\\ttf\\");
	vector<string> fonts = {
		"zx_spectrum.ttf", "zx_spectrum_bold.ttf", "vt323.ttf", "flexi_ibm_vga.ttf", "notalot35.ttf"
	};
	int fontSize = engine.getWidth() / 40;
	int fontId;
	for (string fontFileName : fonts) {
		fontId = engine.loadFont(fontFileName, fontSize);
	}
	vector<SDL_Color> colors = {
		{ 255, 255, 255 }, {255, 240, 150 }, { 230, 200, 145 }, { 0, 195, 255 }, { 240, 50, 210 }
	};
	const char* text = "En un lugar de La Mancha, de cuyo nombre no quiero acordarme, hace tiempo que vivía un hidalgo caballero...";

	while (!quit) {
		//SDL_WaitEvent(&event);
		int posY = 0;
		engine.setFont(0);
		posY = engine.printText(text, colors.at(0), engine.getWidth(), posY);
		engine.setFont(1);
		posY = engine.printText(text, colors.at(1), engine.getWidth(), posY);
		engine.setFont(2);
		posY = engine.printText(text, colors.at(2), engine.getWidth(), posY);
		engine.setFont(3);
		posY = engine.printText(text, colors.at(3), engine.getWidth(), posY);
		engine.setFont(4);
		posY = engine.printText(text, colors.at(4), engine.getWidth(), posY);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYDOWN:
					if (event.key.keysym.scancode == SDL_SCANCODE_F) {
						engine.switchFullScreen();
					}
					break;
			}
		}
	}

	return 0;
}
