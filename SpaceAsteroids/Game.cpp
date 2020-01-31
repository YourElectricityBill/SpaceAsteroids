#include <iostream>
#include <string>
#include <chrono>
#include "SDL.h"
#include "SDL_image.h"
#include "SDL_mixer.h"
#include "SDL_ttf.h"
#include "Texture Manager.h"
#include "Debug Mode.h"
#include "Game.h"


Game::Game(const char* t_ourName, const int t_windowWidth, const int t_windowHeight, const char* t_logoPath)
{
	const char* m_gameName = t_ourName;
	m_gameWindowWidth = t_windowWidth;
	m_gameWindowHeight = t_windowHeight;
	m_gameScore = 0;

	SDL_Init(SDL_INIT_EVERYTHING);
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		std::cerr << " Failed to initialize SDL : " << SDL_GetError() << "\n"; //I don't want to flush the buffer so I choosed \n instead of endl.
		m_isRunning = false;
		exit(0);
	}

	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

	TTF_Init();

	m_gameWindow = SDL_CreateWindow(m_gameName, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, t_windowWidth, t_windowHeight, SDL_WINDOW_SHOWN);
	if (m_gameWindow == nullptr)
	{
		std::cerr << "Game Window Creation Error: " << SDL_GetError() << "\n";
		m_isRunning = false;
		exit(1);
	}

	m_gameRenderer = SDL_CreateRenderer(m_gameWindow, -1, SDL_RENDERER_ACCELERATED);
	if (m_gameRenderer == nullptr)
	{
		std::cerr << "Game Renderer Creation Error: " << SDL_GetError() << "\n";
		m_isRunning = false;
		exit(2);
	}

	m_gameFonts = TTF_OpenFont("Fonts/arial.ttf", 25);
	if (m_gameFonts == nullptr)
	{
		std::cerr << "Game Fonts Creation Error: " << SDL_GetError() << "\n";
		m_isRunning = false;
		exit(3);
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cerr << "Game Audio Creation Error: " << Mix_GetError() << "\n";
		m_isRunning = false;
	}

	m_backgroundMusic = Mix_LoadMUS("Audio/Explore.wav");
	if (m_backgroundMusic == nullptr)
	{
		std::cerr << "Background Music Creation Error: " << Mix_GetError() << "\n";
		m_isRunning = false;
		exit(4);
	}
	m_shootSound = Mix_LoadWAV("Audio/Laser.wav");
	if (m_shootSound == nullptr)
	{
		std::cerr << "Shooting Sound Creation Error: " << Mix_GetError() << "\n";
		m_isRunning = false;
		exit(5);
	}
	m_explosionSound = Mix_LoadWAV("Audio/Explosion.wav");
	if (m_explosionSound == nullptr)
	{
		std::cerr << "Explosion Sound Creation Error: " << Mix_GetError() << "\n";
		m_isRunning = false;
		exit(6);
	}

	m_gameLogo = IMG_Load(t_logoPath);
	SDL_SetWindowIcon(m_gameWindow, m_gameLogo);

	m_fontColor = { 255, 255, 255 };

	m_textWidth1 = m_textHeight1 = m_textWidth2 = m_textHeight2 = m_textWidth3 = m_textHeight3 = 0;

	// First Rectangle

	m_fontSurface1 = TTF_RenderText_Solid(m_gameFonts, (std::string("Player Score: ") + std::to_string(m_gameScore)).c_str(), m_fontColor);

	m_fontTexture1 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface1);

	SDL_FreeSurface(m_fontSurface1); // We don't need it anymore

	SDL_QueryTexture(m_fontTexture1, 0, 0, &m_textWidth1, &m_textHeight1); // To supply the dimensions occupied by the text

	m_textRect0 = { 0, 0, m_textWidth1, m_textHeight1 };

	// Second Rectangle

	m_fontSurface2 = TTF_RenderText_Solid(m_gameFonts, (std::string("Difficulty: ") + m_gameDifficulty).c_str(), m_fontColor);

	m_fontTexture2 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface2);

	SDL_FreeSurface(m_fontSurface2);

	SDL_QueryTexture(m_fontTexture2, 0, 0, &m_textWidth2, &m_textHeight2);

	m_textRect2 = { 0, m_textHeight1 * 2, m_textWidth2 * 2, m_textHeight2 + 1 };

	// Third Rectangle

	m_fontSurface3 = TTF_RenderText_Solid(m_gameFonts, "Player HP: 0", m_fontColor);

	m_fontTexture3 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface3);

	SDL_FreeSurface(m_fontSurface3);

	SDL_QueryTexture(m_fontTexture3, 0, 0, &m_textWidth3, &m_textHeight3);

	m_textRect3 = { 0, m_textHeight1, m_textWidth2 * 2, m_textHeight3 };

	m_enemiesToSpawn = 10;

	m_isPaused = false;
}

void Game::initialiseMenu(const char* t_imagePath)
{
	m_menuTexture = TextureManager::LoadTexture(t_imagePath, m_gameRenderer);

	m_menuFonts = TTF_OpenFont("Fonts/neuropolitical.ttf", 45);

	m_fontSurface1 = TTF_RenderText_Solid(m_menuFonts, "SpaceAsteroids", m_fontColor);

	TTF_CloseFont(m_menuFonts);

	m_menuTextWidth = m_menuTextHeight = 0;

	m_gameNameTexture = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface1);

	SDL_FreeSurface(m_fontSurface1);

	SDL_QueryTexture(m_gameNameTexture, 0, 0, &m_menuTextWidth, &m_menuTextHeight);

	// Computer calculates the position of every button based on the window's resolution and width/length of the text

	m_textRect1.x = (m_gameWindowWidth / 2) - (m_menuTextWidth / 2); // We substract width divided by 2 in order to allign it more perfectly
	m_textRect1.y = m_gameWindowHeight * 0.1f;
	m_textRect1.w = m_menuTextWidth;
	m_textRect1.h = m_menuTextHeight;

	m_gameNameFrame = TextureManager::LoadTexture("Images/Texture/Frames.png", m_gameRenderer);
	SDL_QueryTexture(m_gameNameFrame, 0, 0, &m_menuTextWidth, &m_menuTextHeight);
	m_sourceRect1 = { 0, 0, m_menuTextWidth / 2, m_menuTextHeight / 3 }; //We use source rectangle since we use a sheet of frames
	m_destinationRect1.w = m_textRect1.w * 1.25f;
	m_destinationRect1.h = m_textRect1.h * 3;
	m_destinationRect1.x = (m_gameWindowWidth / 2) - (m_destinationRect1.w / 2);   
	m_destinationRect1.y = m_textRect1.y - m_textRect1.h * 1.5f;

	m_menuFonts = TTF_OpenFont("Fonts/orena.ttf", 25);

	if (m_isPaused == false)
	{
		m_fontSurface1 = TTF_RenderText_Solid(m_menuFonts, "Play Game!", m_fontColor);
	}
	else
	{
		m_fontSurface1 = TTF_RenderText_Solid(m_menuFonts, "Resume", m_fontColor);
	}

	m_menuButton1 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface1);

	SDL_FreeSurface(m_fontSurface1);

	SDL_QueryTexture(m_menuButton1, 0, 0, &m_menuTextWidth, &m_menuTextHeight);

	m_textRect4.x = (m_gameWindowWidth / 2) - (m_menuTextWidth / 2);
	m_textRect4.y = m_textRect1.y * 3.25f;
	m_textRect4.w = m_menuTextWidth;
	m_textRect4.h = m_menuTextHeight;


	m_fontSurface1 = TTF_RenderText_Solid(m_menuFonts, "About", m_fontColor);

	m_menuButton2 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface1);

	SDL_FreeSurface(m_fontSurface1);

	SDL_QueryTexture(m_menuButton2, 0, 0, &m_menuTextWidth, &m_menuTextHeight);

	m_textRect5.x = (m_gameWindowWidth / 2) - (m_menuTextWidth / 2);
	m_textRect5.y = m_textRect4.y + 75;
	m_textRect5.w = m_menuTextWidth;
	m_textRect5.h = m_menuTextHeight;

	m_fontSurface1 = TTF_RenderText_Solid(m_menuFonts, "Exit", m_fontColor);

	TTF_CloseFont(m_menuFonts);

	m_menuButton4 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface1);

	SDL_FreeSurface(m_fontSurface1);

	SDL_QueryTexture(m_menuButton4, 0, 0, &m_menuTextWidth, &m_menuTextHeight);

	m_textRect7.x = (m_gameWindowWidth / 2) - (m_menuTextWidth / 2);
	m_textRect7.y = m_textRect5.y + 75;
	m_textRect7.w = m_menuTextWidth;
	m_textRect7.h = m_menuTextHeight;

	m_buttonRect2.w = m_buttonRect3.w = m_buttonRect4.w = m_buttonRect5.w = m_textRect4.w * 1.25f; //The same dimensions for every button looks more pleasing
	m_buttonRect2.h = m_buttonRect3.h = m_buttonRect4.h = m_buttonRect5.h = m_textRect4.h * 2;
	m_buttonRect2.x = (m_gameWindowWidth / 2) - (m_buttonRect2.w / 2);
	m_buttonRect2.y = m_textRect4.y - m_textRect4.h * 0.6f;

	m_buttonRect3.x = (m_gameWindowWidth / 2) - (m_buttonRect3.w / 2);
	m_buttonRect3.y = m_textRect5.y - m_textRect5.h * 0.6f;

	m_buttonRect5.x = (m_gameWindowWidth / 2) - (m_buttonRect5.w / 2);
	m_buttonRect5.y = m_textRect7.y - m_textRect7.h * 0.6f;

	m_buttonFrame = TextureManager::LoadTexture("Images/Texture/Button.png", m_gameRenderer);


	m_choiceIsMade = false;
}

void Game::showMenu()
{
	int xMousePosition, yMousePosition;

	SDL_RenderClear(m_gameRenderer);

	SDL_RenderCopy(m_gameRenderer, m_menuTexture, NULL, NULL);
	SDL_RenderCopy(m_gameRenderer, m_gameNameFrame, &m_sourceRect1, &m_destinationRect1);
	SDL_RenderCopy(m_gameRenderer, m_buttonFrame, NULL, &m_buttonRect2);
	SDL_RenderCopy(m_gameRenderer, m_buttonFrame, NULL, &m_buttonRect3);
	SDL_RenderCopy(m_gameRenderer, m_buttonFrame, NULL, &m_buttonRect5);
	SDL_RenderCopy(m_gameRenderer, m_gameNameTexture, NULL, &m_textRect1);
	SDL_RenderCopy(m_gameRenderer, m_menuButton1, NULL, &m_textRect4);
	SDL_RenderCopy(m_gameRenderer, m_menuButton2, NULL, &m_textRect5);
	SDL_RenderCopy(m_gameRenderer, m_menuButton4, NULL, &m_textRect7);

	SDL_Event menuEvent;

	while (SDL_PollEvent(&menuEvent))
	{
		if(menuEvent.type == SDL_QUIT)
		{
			m_isRunning = false;
			m_choiceIsMade = true;
			break;
		}
		//if (menuEvent.type == SDL_MOUSEMOTION || menuEvent.type == SDL_MOUSEBUTTONDOWN || menuEvent.type == SDL_MOUSEBUTTONUP)

		if (menuEvent.type == SDL_MOUSEBUTTONDOWN)
		{
			SDL_GetMouseState(&xMousePosition, &yMousePosition);

			if (!(xMousePosition < m_buttonRect2.x) && 
				!(xMousePosition > m_buttonRect2.x + m_buttonRect2.w) && 
				!(yMousePosition < m_buttonRect2.y) && 
				!(yMousePosition > m_buttonRect2.y + m_buttonRect2.h))
			{
				std::cout << "New Game!\n";
				m_isRunning = true;
				m_choiceIsMade = true;
				break;
			}

			else if (!(xMousePosition < m_buttonRect3.x) &&
				!(xMousePosition > m_buttonRect3.x + m_buttonRect3.w) &&
				!(yMousePosition < m_buttonRect3.y) &&
				!(yMousePosition > m_buttonRect3.y + m_buttonRect3.h))
			{
				scene3();
			}

			else if (!(xMousePosition < m_buttonRect4.x) &&
				!(xMousePosition > m_buttonRect4.x + m_buttonRect4.w) &&
				!(yMousePosition < m_buttonRect4.y) &&			 
				!(yMousePosition > m_buttonRect4.y + m_buttonRect4.h))
			{
				std::cout << "Options!\n";
			}

			else if (!(xMousePosition < m_buttonRect5.x) &&
				!(xMousePosition > m_buttonRect5.x + m_buttonRect5.w) &&
				!(yMousePosition < m_buttonRect5.y) &&			 
				!(yMousePosition > m_buttonRect5.y + m_buttonRect5.h))
			{
				m_isRunning = false;
				m_choiceIsMade = true;
				break;
			}

		}
	}

	SDL_RenderPresent(m_gameRenderer);

}

void Game::scene3()
{
	bool exit = false;
	SDL_Event sceneEvent;
	
	m_aboutGame = TextureManager::LoadTexture("Images/Texture/About.png", m_gameRenderer);

	while (!exit)
	{
		SDL_RenderClear(m_gameRenderer);

		while (SDL_PollEvent(&sceneEvent))
		{
			switch (sceneEvent.type)
			{
			case SDL_KEYDOWN:
				switch (sceneEvent.key.keysym.sym)
				{
				case SDLK_ESCAPE: 
					exit = true;
					break;
				}
				break;
			default: break;
			}
		}

		SDL_RenderCopy(m_gameRenderer, m_aboutGame, NULL, NULL);
		SDL_RenderPresent(m_gameRenderer);
	}
	SDL_DestroyTexture(m_aboutGame);
}

void Game::cleanUpMenu()
{
	SDL_RenderClear(m_gameRenderer);
	SDL_DestroyTexture(m_aboutGame);
	SDL_DestroyTexture(m_buttonFrame);
	SDL_DestroyTexture(m_gameNameTexture);
	SDL_DestroyTexture(m_menuButton1);
	SDL_DestroyTexture(m_menuButton2);
	SDL_DestroyTexture(m_menuButton3);
	SDL_DestroyTexture(m_menuButton4);
	SDL_DestroyTexture(m_menuTexture);
	SDL_DestroyTexture(m_gameNameFrame);
}

void Game::displayScore()
{
	m_fontSurface1 = TTF_RenderText_Solid(m_gameFonts, (std::string("Player Score: ") + std::to_string(m_gameScore)).c_str(), m_fontColor);
	m_fontTexture1 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface1);

	SDL_RenderCopy(m_gameRenderer, m_fontTexture1, 0, &m_textRect0);
}

void Game::displayDifficulty()
{

	m_fontSurface2 = TTF_RenderText_Solid(m_gameFonts, (std::string("Difficulty: ") + m_gameDifficulty).c_str(), m_fontColor);
	m_fontTexture2 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface2);

	SDL_RenderCopy(m_gameRenderer, m_fontTexture2, 0, &m_textRect2);
}

void Game::displayPlayerHP(int t_hp)
{
	m_fontSurface3 = TTF_RenderText_Solid(m_gameFonts, (std::string("Player HP: ") + std::to_string(t_hp)).c_str(), m_fontColor);
	m_fontTexture3 = SDL_CreateTextureFromSurface(m_gameRenderer, m_fontSurface3);

	SDL_RenderCopy(m_gameRenderer, m_fontTexture3, 0, &m_textRect3);
}

void Game::changeDifficulty(const char* t_newDifficulty)
{
	m_gameDifficulty = t_newDifficulty;
}

void Game::addScore()
{
	++m_gameScore;
}

void Game::increaseNumberOfEnemies(const int& t_number)
{
	m_enemiesToSpawn += t_number;
}

void Game::cleanUpText() // So our memory won't overflow with unused textures
{
	SDL_FreeSurface(m_fontSurface1);
	SDL_FreeSurface(m_fontSurface2);
	SDL_FreeSurface(m_fontSurface3);
    SDL_DestroyTexture(m_fontTexture1);
	SDL_DestroyTexture(m_fontTexture2); 
	SDL_DestroyTexture(m_fontTexture3);
}

void Game::playBackgroundMusic()
{
	Mix_PlayMusic(m_backgroundMusic, -1);
}

Game::~Game()
{
	m_gameLogo = nullptr;
	m_backgroundMusic = nullptr;
	Mix_FreeChunk(m_shootSound);
	m_shootSound = nullptr;
	Mix_FreeChunk(m_explosionSound);
	m_explosionSound = nullptr;
	/*
	SDL_FreeSurface(m_fontSurface1); 
	m_fontSurface1 = nullptr;
	SDL_FreeSurface(m_fontSurface2);
	m_fontSurface2 = nullptr;
	*/
	SDL_DestroyTexture(m_fontTexture1);
	m_fontTexture1 = nullptr;
	TTF_CloseFont(m_gameFonts);
	m_gameFonts = nullptr;
	SDL_DestroyRenderer(m_gameRenderer);
	m_gameRenderer = nullptr;
	TTF_Quit();
	Mix_Quit();
	IMG_Quit();
	SDL_Quit();
}

Background::Background(SDL_Renderer* t_displayRenderer, const char* t_filePath, const int t_scrollSpeed, const int t_width, const int t_height)
{
	m_screenPosX = 0;
	m_destinationRect.x = 0;
	m_destinationRect.y = 0;
	m_destinationRect.w = t_width;
	m_destinationRect.h = t_height;

	m_backgroundRenderer = t_displayRenderer;
	m_image = TextureManager::LoadTexture(t_filePath, m_backgroundRenderer);
	m_scrollSpeed = t_scrollSpeed;
}

void Background::draw()
{
	m_destinationRect.x = m_screenPosX;
	SDL_RenderCopy(m_backgroundRenderer, m_image, 0, &m_destinationRect);

	m_destinationRect.x = m_screenPosX + m_destinationRect.w;
	SDL_RenderCopy(m_backgroundRenderer, m_image, 0, &m_destinationRect);
}

void Background::update()
{
	m_screenPosX -= m_scrollSpeed;
	if (m_screenPosX < 0 - m_destinationRect.w)
	{
		m_screenPosX += m_destinationRect.w;
	}
}