#include <iostream>
#include <string>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;

class LTexture {
public:
	LTexture() {
		mTexture = NULL;
		mWidth = 0;
		mHeight = 0;
		mPixels = NULL;
		mPitch = 0;
	};

	~LTexture() {
		freeTexture();
	};

	bool loadFromFile(string path) {
		freeTexture();

		SDL_Texture* newTexture = NULL;

		SDL_Surface* loadedSurface = IMG_Load(path.c_str());
		if (loadedSurface == NULL)
		{
			cout << "¡No se puede cargar la imagen " << path << "! SDL_image ERROR: " << IMG_GetError() << endl;
		}
		else
		{
			SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
			if (formattedSurface == NULL)
			{
				cout << "¡No se puede convertir la imagen cargada al formato de visualización! SDL ERROR: " << SDL_GetError() << endl;
			}
			else
			{
				newTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
				if (newTexture == NULL)	{
					cout << "¡No se puede crear una textura vacía! SDL ERROR: " << SDL_GetError() << endl;
				}
				else {
					SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);
					SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &mPixels, &mPitch);

					memcpy(mPixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

					mWidth = formattedSurface->w;
					mHeight = formattedSurface->h;

					// mapa de pixels en formato editable
					Uint32* pixels = (Uint32*)mPixels;
					int pixelCount = (mPitch / 4) * mHeight;

					// color para transparencia, en este caso: R=00, G=FF, B=FF	(cian)
					Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
					Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 0x00, 0xFF, 0xFF, 0x00);

					// hace transparentes los pixels con el color indicado
					for (int i = 0; i < pixelCount; ++i) {
						if (pixels[i] == colorKey) {
							pixels[i] = transparent;
						}
					}

					SDL_UnlockTexture(newTexture);
					mPixels = NULL;
				}
				SDL_FreeSurface(formattedSurface);
			}
			SDL_FreeSurface(loadedSurface);
		}

		mTexture = newTexture;
		return mTexture != NULL;
	};

	void freeTexture() {
		if (mTexture != NULL) {
			SDL_DestroyTexture(mTexture);
			mTexture = NULL;
			mWidth = 0;
			mHeight = 0;
			mPixels = NULL;
			mPitch = 0;
		}
	};

	void setColor(Uint8 red, Uint8 green, Uint8 blue) {
		SDL_SetTextureColorMod(mTexture, red, green, blue);
	};

	void setBlendMode(SDL_BlendMode blending) {
		SDL_SetTextureBlendMode(mTexture, blending);
	};

	void setAlpha(Uint8 alpha) {
		SDL_SetTextureAlphaMod(mTexture, alpha);
	};

	void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL,
		SDL_RendererFlip flip = SDL_FLIP_NONE) {
		// espacio de renderizado
		SDL_Rect renderQuad = { x, y, mWidth, mHeight };

		//Set clip rendering dimensions
		if (clip != NULL) {
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		// renderizarf a pantalla
		SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
	};

	int getWidth() {
		return mWidth;
	};
	
	int getHeight() {
		return mHeight;
	};

	bool lockTexture() {
		bool success = true;

		// la textura ya está bloqueada
		if (mPixels != NULL) {
			printf("Texture is already locked!\n");
			cout << "¡La textura ya está bloqueada!" << endl;
			success = false;
		}
		else {
			if (SDL_LockTexture(mTexture, NULL, &mPixels, &mPitch) != 0) {
				cout << "¡No se pudo bloquear la textura! ERROR: " << SDL_GetError() << endl;
				success = false;
			}
		}

		return success;
	};

	bool unlockTexture() {
		bool success = true;

		// la textura no está bloqueada
		if (mPixels == NULL) {
			cout << "¡Textura no bloqueada!" << endl;
			success = false;
		}
		else {
			SDL_UnlockTexture(mTexture);
			mPixels = NULL;
			mPitch = 0;
		}

		return success;
	};
	
	void* getPixels() {
		return mPixels;
	};
	
	int getPitch() {
		return mPitch;
	};
	
	Uint32 getPixel32(unsigned int x, unsigned int y) {
		// convierte los pixels a 32 bits
		Uint32* pixels = (Uint32*)mPixels;

		// devuelve el pixel
		return pixels[(y * (mPitch / 4)) + x];
	};

private:
	SDL_Texture* mTexture;
	void* mPixels;
	int mPitch;
	int mWidth, mHeight;
};

class LBitmapFont {
public:
	LBitmapFont() {
		mBitmap = NULL;
		mNewLine = 0;
		mSpace = 0;
	};

	bool buildFont(LTexture* bitmap) {
		bool success = true;

		if (!bitmap->lockTexture()) {
			cout << "¡No se puede bloquear el bitmap de la fuente!" << endl;
			success = false;
		}
		else {
			// color de fondo
			Uint32 bgColor = bitmap->getPixel32(0, 0);

			// tamaño de las celdas
			int cellW = bitmap->getWidth() / 16;
			int cellH = bitmap->getHeight() / 16;

			// para cambio de línea
			int top = cellH;
			int baseA = cellH;

			// el caracter actual
			int currentChar = 0;

			// recorremos las filas
			for (int rows = 0; rows < 16; ++rows) {
				// recorremos las columnas
				for (int cols = 0; cols < 16; ++cols) {
					// posición del caracter
					mChars[currentChar].x = cellW * cols;
					mChars[currentChar].y = cellH * rows;

					// tamaño del caracter
					mChars[currentChar].w = cellW;
					mChars[currentChar].h = cellH;

					// buscamos lado izquierdo
					// recorremos las columnas de pixels
					for (int pCol = 0; pCol < cellW; ++pCol) {
						// recorremos las filas de pixels
						for (int pRow = 0; pRow < cellH; ++pRow) {
							// posición del pixel
							int pX = (cellW * cols) + pCol;
							int pY = (cellH * rows) + pRow;

							// si el pixel no es de color de fondo
							if (bitmap->getPixel32(pX, pY) != bgColor) {
								// posición X
								mChars[currentChar].x = pX;

								// sale de los bucles
								pCol = cellW;
								pRow = cellH;
							}
						}
					}

					// buscamos lado derecho
					// recorremos las columnas de pixels
					for (int pColW = cellW - 1; pColW >= 0; --pColW) {
						// recorremos las filas de pixels
						for (int pRowW = 0; pRowW < cellH; ++pRowW) {
							// posición del pixel
							int pX = (cellW * cols) + pColW;
							int pY = (cellH * rows) + pRowW;

							// si el pixel no es del color de fondo
							if (bitmap->getPixel32(pX, pY) != bgColor) {
								// anchura
								mChars[currentChar].w = (pX - mChars[currentChar].x) + 1;

								// sale de los bucles
								pColW = -1;
								pRowW = cellH;
							}
						}
					}

					// buscamos la parte superior
					// recorremos las filas de pixels
					for (int pRow = 0; pRow < cellH; ++pRow) {
						// recorremos las columnas de pixels
						for (int pCol = 0; pCol < cellW; ++pCol) {
							// posición del pixel
							int pX = (cellW * cols) + pCol;
							int pY = (cellH * rows) + pRow;

							// si el pìxel no es del color de fondo
							if (bitmap->getPixel32(pX, pY) != bgColor) {
								// nos quedamos con la posición más alta posible
								if (pRow < top)	{
									top = pRow;
								}

								// sale de los bucles
								pCol = cellW;
								pRow = cellH;
							}
						}
					}

					// buscamos la parte baja de 'A'
					if (currentChar == 'A')	{
						// recorremos las filas de pixels
						for (int pRow = cellH - 1; pRow >= 0; --pRow) {
							// recorremos las columnas de pixels
							for (int pCol = 0; pCol < cellW; ++pCol) {
								// posición del pixel
								int pX = (cellW * cols) + pCol;
								int pY = (cellH * rows) + pRow;

								// si el pixel no es de color del fondo
								if (bitmap->getPixel32(pX, pY) != bgColor) {
									// hemos encontrado la parte baja de 'A'
									baseA = pRow;

									// sale de los bucles
									pCol = cellW;
									pRow = -1;
								}
							}
						}
					}

					// siguiente caracter
					++currentChar;
				}
			}

			// calculamos el espacio
			mSpace = cellW / 2;

			// calculamos nueva linea
			mNewLine = baseA - top;

			// saltamos el exceso de pixels superiores
			for (int i = 0; i < 256; ++i) {
				mChars[i].y += top;
				mChars[i].h -= top;
			}

			bitmap->unlockTexture();
			mBitmap = bitmap;
		}

		return success;
	};

	void renderText(int x, int y, string text) {
		if (mBitmap != NULL) {
			int curX = x, curY = y;

			for (int i = 0; i < text.length(); ++i)	{
				// si es un espacio, avanzamos el tamaño del espacio
				if (text[i] == ' ')	{
					curX += mSpace;
				}
				// si es un salto de línea, nos posicionamos en la siguiente
				else if (text[i] == '\n') {
					curY += mNewLine;
					curX = x;
				}
				else {
					int ascii = (unsigned char)text[i];
					mBitmap->render(curX, curY, &mChars[ascii]);
					curX += mChars[ascii].w + 1;
				}
			}
		}
	};

private:
	LTexture* mBitmap;
	SDL_Rect mChars[256];
	int mNewLine, mSpace;
};


LTexture gBitmapTexture;
LBitmapFont gBitmapFont;


bool init() {
	bool success = true;

	// inicializar SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		cout << "¡No se puede inicializar SDL! SDL ERROR: " << SDL_GetError() << endl;
		success = false;
	}
	else {
		// filtrado lineal de texturas
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
			cout << "¡Aviso! Filtrado lineal de texturas no está activado." << endl;
		}

		// inicializa generador de números aleatorios
		srand(SDL_GetTicks());

		// crea la ventana
		gWindow = SDL_CreateWindow("Test font 2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL) {
			cout << "¡No se ha podido crear la ventana! SDL ERROR: " << SDL_GetError() << endl;
			success = false;
		}
		else {
			// renderer de la ventana
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
			if (gRenderer == NULL) {
				cout << "¡No se ha podido crear el renderer! SDL ERROR: " << SDL_GetError() << endl;
				success = false;
			}
			else 			{
				// inicializa el color de dibujado del renderer
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// inicializa carga de PNGs
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					cout << "¡No se ha podido inicializar SDL_image! SDL_image ERROR: " << IMG_GetError() << endl;
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	bool success = true;

	// cargar textura con la fuente
	if (!gBitmapTexture.loadFromFile("C:\\Temp\\SDL2\\ttf\\lazyfont.png")) {
		cout << "¡Error al cargar el fichero con la fuente!" << endl;
		success = false;
	}
	else {
		// construimos la fuente desde la textura
		gBitmapFont.buildFont(&gBitmapTexture);
	}

	return success;
}

void close() {
	gBitmapTexture.freeTexture();

	// destruimos la ventana	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	// finalizamos los subsistemas de SDL
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[]) {

	if (!init()) {
		cout << "¡Fallo al inicializar!" << endl;
	}
	else {
		if (!loadMedia()) {
			cout << "¡Fallo al cargar los recursos!" << endl;
		}
		else {
			bool quit = false;
			SDL_Event e;

			while (!quit) {
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT)	{
						quit = true;
					}
				}

				// limpiamops la pantalla
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
				SDL_RenderClear(gRenderer);

				// dibujamos el texto de prueba
				gBitmapFont.renderText(0, 0, "Bitmap Font:\nABDCEFGHIJKLMNOPQRSTUVWXYZ\nabcdefghijklmnopqrstuvwxyz\n0123456789\nÁÉÍÓÚÑÄÜÇ\náéíóúñäüç\nEn un lugar de La Mancha, de cuyo nombre no quiero acordarme, hace tiempo que vivía un hidalgo caballero...");

				// actualizamos la pantalla
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	close();

	return 0;
}