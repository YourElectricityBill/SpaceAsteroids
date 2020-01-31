#include "Header Files.h"
#include "Menu.h"
#include "Texture Manager.h"

Menu::Menu(SDL_Window* t_gameWindow, const char* t_imagePath)
{

	m_menuRenderer = SDL_CreateRenderer(m_gameWindow, -1, SDL_RENDERER_ACCELERATED);
	if (m_menuRenderer == nullptr)
	{
		std::cerr << "Game Renderer Creation Error: " << SDL_GetError() << "\n";
		exit(2);
	}

	m_menuFonts = TTF_OpenFont("Fonts/plom praeng.ttf", 25);
	if (m_menuFonts == nullptr)
	{
		std::cerr << "Game Fonts Creation Error: " << SDL_GetError() << "\n";
		exit(3);
	}

	m_fontColor = { 255, 255, 255 };

	m_menuImage = TextureManager::LoadTexture(t_imagePath, m_menuRenderer);

}

int Menu::showMenu()
{
	SDL_RenderClear(m_menuRenderer);

	SDL_RenderPresent(m_menuRenderer);

	return m_choice;
}


Menu::~Menu()
{
}
