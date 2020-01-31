#include "Header Files.h"
#include "Game.h"
#include "Texture Manager.h"
#include "Entity.h"
#include "Animation.h"


void RunGame(Game* currentGame);

int main(int agrc, char *args[])
{
	Game* myGame = new Game("SpaceAsteroids", 1280, 760, "Images/Texture/Logo.png");

	RunGame(myGame); //A game loop

	delete myGame;

	return 0;
}

void RunGame(Game* currentGame)
{
	srand(time(NULL));

    #if _DEBUG_MODE_ == 0
	currentGame->initialiseMenu("Images/Texture/Menu.jpg");

	while (!currentGame->m_choiceIsMade)
	{
		currentGame->showMenu();


	}
	currentGame->cleanUpMenu();
    #endif

	SDL_Event gameEvent;

	Background* backgroundImage = new Background(currentGame->getRenderer(), "Images/Texture/Background.jpg", 1, currentGame->getWindowWidth(), currentGame->getWindowHeight());


	Player* PlayerShip = new Player(currentGame->getRenderer(), "Images/Texture/Player.png", 100, currentGame->getWindowHeight() / 2 - 64, 128, 128);

	std::vector<Enemy*> Enemies;
	Enemy::initialiseTexture(currentGame->getRenderer(), "Images/Texture/Enemies.png", 12, 12);
	std::vector<Bullet*> Bullets;
	std::vector<Animation*> Explosions;

	currentGame->playBackgroundMusic();

	currentGame->changeDifficulty("Easy");

    #if _DEBUG_MODE_ == 0
	for (int i = 0; i < (currentGame->getNumberToSpawn() * 3.5f); ++i) //Emplace Back seem to be faster than Push Back so I decided to stick to them
	{
	  Enemies.emplace_back(new Enemy(currentGame->getRenderer(), currentGame->getWindowWidth() + (rand() % 1500 ), rand() % (currentGame->getWindowHeight() - 128), 128, 128, currentGame->getDifficulty(), 3));
	}
    #else
	bool enemyVectorIsEmpty = true;
    #endif 

	Mix_VolumeChunk(currentGame->getShootingSound(), 30);
	Mix_VolumeChunk(currentGame->getExplosionSound(), 35);
	Mix_VolumeMusic(70);

	const int frameRate = 60;
	const int delayFrame = 1000 / frameRate; // Maxixmum time between frames, assuming no lags occuring
	int shotTimer = 0, enemyIncrease = 6;



	Uint32 frameStart;
	int timePassedSinceFrameStart;


    #if _DEBUG_MODE_ == 1
	std::cout << "Initial game difficulty is: " << currentGame->getDifficulty() << "\n";
    #endif

	while (currentGame->m_isRunning)
	{
        frameStart = SDL_GetTicks();

		SDL_RenderClear(currentGame->getRenderer());

		// We need to make sure we clean ANY nullptr at the very beginning to make sure no function will try to access deallocated (freed) sections of memory
        Bullets.erase(std::remove(Bullets.begin(), Bullets.end(), nullptr), Bullets.end());
        #if _DEBUG_MODE_ == 0 || enemyVectorIsEmpty == 0
		Enemies.erase(std::remove(Enemies.begin(), Enemies.end(), nullptr), Enemies.end()); 
        #endif 
		Explosions.erase(std::remove(Explosions.begin(), Explosions.end(), nullptr), Explosions.end());

		backgroundImage->draw();
		PlayerShip->draw();

		if (currentGame->m_isPaused == false)
		{
			backgroundImage->update();
			PlayerShip->update(Enemies, currentGame->getWindowHeight());
		}

		if (PlayerShip->m_destroy)
			{
				Explosions.emplace_back(new Animation(currentGame->getRenderer(), "Images/Texture/Explosion.png", PlayerShip->getPosition(), 9, 9, 120));
				Mix_PlayChannel(0, currentGame->getExplosionSound(), 0);
				delete PlayerShip;
				currentGame->m_isRunning = false;
				break;
			}

		if (shotTimer < 0 && PlayerShip->checkFiringStatus())
			{
				shotTimer = 10;
				Bullets.emplace_back(new Bullet(currentGame->getRenderer(), "Images/Texture/Bullet.png", PlayerShip->getPositionX() + (PlayerShip->getWidth()), PlayerShip->getPositionY() + (PlayerShip->getWidth() / 2), 30, 5));
				Mix_PlayChannel(-1, currentGame->getShootingSound(), 0);
				PlayerShip->changeFiringStatus(false); // Sometimes "MouseButtonDown" function is not working properly so I made this to make sure the ship stops fire after you click down your button
			}
#if _DEBUG_MODE_ == 0 || enemyVectorIsEmpty == 0 // Zero evaluates to false

			for (auto &a : Enemies)
			{
				a->draw();

				if (currentGame->m_isPaused == false)
				{
					a->update(Bullets);
				}
				if (a->m_destroy == true && !a->hasCollisionWith(PlayerShip))
				{
					currentGame->addScore();
					Explosions.emplace_back(new Animation(currentGame->getRenderer(), "Images/Texture/Explosion.png", a->getPosition(), 9, 9, 120));
					Mix_PlayChannel(0, currentGame->getExplosionSound(), 0);
					delete a;
					a = nullptr;
				}
				else if ((a->m_destroy == true && a->hasCollisionWith(PlayerShip)))
				{
					Explosions.emplace_back(new Animation(currentGame->getRenderer(), "Images/Texture/Explosion.png", a->getPosition(), 9, 9, 120));
					Mix_PlayChannel(0, currentGame->getExplosionSound(), 0);
					delete a;
					a = nullptr;
				}
				else if (a->getPositionX() < 0 - a->getWidth())
				{
					delete a;
					a = nullptr;
				}
			}
			
#endif 

#if _DEBUG_MODE_ == 1
			if (Enemies.empty())
			{
				enemyVectorIsEmpty = true;
			}
#endif

			--shotTimer;

			for (auto &temp : Bullets) //I use reference so the variable has the memory address of an actual object in container, so we will remove the actual memory of an object and not of temp variable
			{
				temp->draw();

				if (currentGame->m_isPaused == false)
				{
					temp->update();
				}
				if (temp->getLifeTime() < 0 || temp->m_destroy == true)
				{
					delete temp;
					temp = nullptr;
				}
				else if (temp->getPositionX() > (currentGame->getWindowWidth() + 20)) // I don't want to delete it right on the border of the window, hence + 20
				{
					delete temp;
					temp = nullptr;
				}
			}
	

			for (auto &temp : Explosions)
			{
				temp->playAnimation();
				temp->updateAnimation();
				if (temp->m_destroy == true)
				{
					delete temp;
					temp = nullptr;
				}
			}

		while (SDL_PollEvent(&gameEvent))
		{
			if (gameEvent.type == SDL_QUIT)
			{
				currentGame->m_isRunning = false;
				break;
			}
			else if (gameEvent.type == SDL_KEYDOWN)
			{
				switch (gameEvent.key.keysym.sym)
				{
				case SDLK_x:
					currentGame->m_isRunning = false;
						break;
				case SDLK_p:
						if (!Mix_PausedMusic()) //If not paused, then pause
						{
							Mix_PauseMusic();
							break;
						}
						else 
						{
							Mix_ResumeMusic();
							break;
						}
				case SDLK_ESCAPE:
				{
					currentGame->m_isPaused = true; //Pause the whole game
					currentGame->initialiseMenu("Images/Texture/Menu.jpg");
					while (!currentGame->m_choiceIsMade)
					{
						currentGame->showMenu();
						Mix_PauseMusic();
					}
					currentGame->m_isPaused = false; //If player decides to return, unpause the game
					currentGame->cleanUpMenu();
					Mix_ResumeMusic();
				}
                #if _DEBUG_MODE_ == 1
				case SDLK_s: 
					enemyVectorIsEmpty = false;
					Enemies.emplace_back(new Enemy(currentGame->getRenderer(), currentGame->getWindowWidth() + (rand() % 1500), rand() % (currentGame->getWindowHeight() - 128), 128, 128, currentGame->getDifficulty(), 3)	);
				case SDLK_e:
					Explosions.emplace_back(new Animation(currentGame->getRenderer(), "Images/Texture/Explosion.png", PlayerShip->getPosition(), 9, 9, 120));
                #endif
				default: break;
				}
			}
			else if (gameEvent.type == SDL_MOUSEBUTTONDOWN)
			{
				PlayerShip->changeFiringStatus(true);
			}
			else if (gameEvent.type == SDL_MOUSEBUTTONUP) //To prevent player from taking advantage of rapid fire by holding mouse button
			{
				PlayerShip->changeFiringStatus(false);
			}
		}

		currentGame->displayScore();
		currentGame->displayDifficulty();
		currentGame->displayPlayerHP(PlayerShip->getHP());

		SDL_RenderPresent(currentGame->getRenderer());

		currentGame->cleanUpText();

        #if _DEBUG_MODE_ == 0
		if (Enemies.size() < 20)
		{
			for (int i = 0; i < currentGame->getNumberToSpawn(); ++i) //Spawn new asteroids, so our game will feel like it's going on forever, the bigger the score, the more enemies will spawn
			{
				Enemies.emplace_back(new Enemy(currentGame->getRenderer(), currentGame->getWindowWidth() + (rand() % 1400), rand() % (currentGame->getWindowHeight() - 128), 128, 128, currentGame->getDifficulty(), 3));
			}
		}
        #endif 
		if (currentGame->getNumberToSpawn() % 20 == 0 && currentGame->getNumberToSpawn() != 0) //Each 20 kills will increase the spawn rate of our enemies
		{
			currentGame->increaseNumberOfEnemies(enemyIncrease);
			currentGame->addScore();
			// This is not a good way, but I done it to prevent bugs
		}

		if (currentGame->getGameScore() > 20 && currentGame->getGameScore() < 40)
		{
			currentGame->changeDifficulty("Medium");
		}

		else if (currentGame->getGameScore() > 40)
		{
			currentGame->changeDifficulty("Hard");
		}

		timePassedSinceFrameStart = SDL_GetTicks() - frameStart;
		if (delayFrame > timePassedSinceFrameStart)
		{
			SDL_Delay(delayFrame - timePassedSinceFrameStart);
		}

	}
	delete backgroundImage;
}
