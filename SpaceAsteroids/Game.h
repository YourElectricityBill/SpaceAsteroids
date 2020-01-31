#ifndef _GAME_H_
#define _GAME_H_
#include "Texture Manager.h"


class Game final //Has no inheritance
{
public:
	Game(const char* t_ourName, const int t_windowWidth, const int t_windowHeight, const char* t_logoPath);
	~Game();

	bool m_choiceIsMade, m_isRunning, m_isPaused;
	void displayScore(), cleanUpText(), playBackgroundMusic(), addScore(), increaseNumberOfEnemies(const int& t_number), changeDifficulty(const char* t_newDifficulty), 
		displayDifficulty(), displayPlayerHP(int t_hp), initialiseMenu(const char* t_iamgePath),
		cleanUpMenu();
	void showMenu(), scene3(); //I had in mind that scene1 is the menu, and the actual game being scene2

	//getters. Const is because we are not going to modify the values of those variables
	SDL_Window* getWindow() const { return m_gameWindow; }
	SDL_Renderer* getRenderer() const { return m_gameRenderer; }
	TTF_Font* getGameFonts() const { return m_gameFonts; }
	SDL_Surface* getLogo() const { return m_gameLogo; }
	SDL_Surface* getFontSurface1() const { return m_fontSurface1; }
	SDL_Surface* getFontSurface2() const { return m_fontSurface2; }
	SDL_Texture* getFontTexture() const { return m_fontTexture1; }
	SDL_Rect getFontRectangle1() const { return m_textRect1; }
	SDL_Rect getFontRectangle2() const { return m_textRect2; }
	Mix_Music* getBackgroundMusic() const { return m_backgroundMusic; }
	Mix_Chunk* getShootingSound() const { return m_shootSound; }
	Mix_Chunk* getExplosionSound() const { return m_explosionSound; }
	SDL_Color getFontColor() const { return m_fontColor; }
	const char* getGameName() const { return m_gameName; }
	std::string getDifficulty() const { return m_gameDifficulty; }
	int getWindowWidth() const { return m_gameWindowWidth; }
	int getWindowHeight() const { return m_gameWindowHeight; }
	int getGameScore() const { return m_gameScore; }
	int getNumberToSpawn() const { return m_enemiesToSpawn; }
	int getTextWidth1() const { return m_textWidth1; }
	int getTextHeight1() const { return m_textHeight1; }
	int getTextWidth2() const { return m_textWidth2; }
	int getTextHeight2() const { return m_textHeight2; }

private:
	SDL_Window* m_gameWindow;
	SDL_Renderer* m_gameRenderer;
	TTF_Font* m_gameFonts, *m_menuFonts;
	SDL_Surface *m_gameLogo, *m_fontSurface1, *m_fontSurface2, *m_fontSurface3, *m_buttonSurface;
	SDL_Texture* m_fontTexture1, *m_fontTexture2, *m_fontTexture3, *m_menuTexture, *m_gameNameTexture, *m_gameNameFrame, 
		*m_menuButton1, *m_menuButton2, *m_menuButton3, *m_menuButton4, *m_buttonFrame, *m_aboutGame;
	SDL_Rect m_textRect0, m_textRect1, m_textRect2, m_textRect3, m_sourceRect1, m_destinationRect1, m_textRect4,
		m_buttonRect2, m_textRect5, m_buttonRect3, m_textRect6, m_buttonRect4, m_textRect7,
		m_buttonRect5;
	Mix_Music* m_backgroundMusic;
	Mix_Chunk* m_shootSound, *m_explosionSound;
	SDL_Color m_fontColor;
	const char* m_gameName;
	std::string m_gameDifficulty;
	int m_gameWindowWidth, m_gameWindowHeight, m_gameScore, m_enemiesToSpawn, m_textWidth1, m_textHeight1, 
		m_textWidth2, m_textHeight2, m_textWidth3, m_textHeight3, m_menuTextWidth, m_menuTextHeight, 
		m_choice;

};

class Background
{
public:
	Background(SDL_Renderer* t_displayRenderer, const char* t_filePath, const int t_scrollSpeed, const int t_width, const int t_height);
	void draw(), update();
private:
	SDL_Texture* m_image;
	SDL_Renderer* m_backgroundRenderer;
	SDL_Rect m_destinationRect;
	int m_scrollSpeed;
	int m_screenPosX; //Our background changes only along the X axis so I see no point in creating variable for Y;
};
#endif 