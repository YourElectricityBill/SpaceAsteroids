#include <stdlib.h>
#include "Entity.h"
#include "Texture Manager.h"

SDL_Texture* Enemy::m_textureSpriteSheet;
int Enemy::m_numberOfRows;
int Enemy::m_numberOfColumns;
int Enemy::m_spriteWidth;
int Enemy::m_spriteHeight;

Entity::Entity(SDL_Renderer* t_entityRenderer, const char* t_path, const int t_x, const int t_y, const int t_w, const int t_h)
{
	m_destroy = false;
	m_entityRenderer = t_entityRenderer;
	m_entityTexture = TextureManager::LoadTexture(t_path, m_entityRenderer);
	m_entityPosition = { t_x, t_y, t_w, t_h };

}


Entity::~Entity()
{
	SDL_DestroyTexture(m_entityTexture);
}

void Entity::draw()
{
	SDL_RenderCopy(m_entityRenderer, m_entityTexture, NULL, &m_entityPosition);
}

bool Entity::hasCollisionWith(const Entity* someEntity)
{
	return SDL_HasIntersection(&m_entityPosition, &someEntity->m_entityPosition);
}

Player::Player(SDL_Renderer* t_entityRenderer, const char* t_path, const int t_x, const int t_y, const int t_w, const int t_h) : Entity(t_entityRenderer, t_path, t_x, t_y, t_w, t_h)
{
	m_speed = 6;

	m_hp = 15;
}

void Player::update()
{
	
}

void Player::update(std::vector<Enemy*> &t_enemiesVector, const int &t_limitForY)
{

    const Uint8* keyboard = SDL_GetKeyboardState(NULL);

	if ((keyboard[SDL_SCANCODE_UP] || keyboard[SDL_SCANCODE_W]) && (m_entityPosition.y > 0))
	{
		m_entityPosition.y -= m_speed;
        #if _DEBUG_MODE_ == 1
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Ship position at X: %d and Y: %d.", m_entityPosition.x, m_entityPosition.y); //For debugging stuff related to the movement
        #endif 
	}
	if ((keyboard[SDL_SCANCODE_DOWN] || keyboard[SDL_SCANCODE_S]) && (m_entityPosition.y < t_limitForY - getHeight())) // To make sure that the the hitbox won't go under the screen
	{
		m_entityPosition.y += m_speed;
        #if _DEBUG_MODE_ == 1
		SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Ship position at X: %d and Y: %d.", m_entityPosition.x, m_entityPosition.y);
        #endif 
	}


	for (Enemy* someEnemy : t_enemiesVector)
	{
		if (hasCollisionWith(someEnemy))
		{
			--m_hp;
			someEnemy->m_destroy = true;
		}
	}
	if (m_hp <= 0)
	{
		m_destroy = true;
	}
}

void Player::changeFiringStatus(bool t_temp)
{
	m_fireButtonPressed = t_temp;
}

Player::~Player()
{
	SDL_DestroyTexture(m_entityTexture);
}

void Enemy::initialiseTexture(SDL_Renderer* t_renderer, const char* t_path, int t_rows, int t_columns)
{
	m_textureSpriteSheet = TextureManager::LoadTexture(t_path, t_renderer);

	Enemy::m_numberOfRows = t_rows;
	Enemy::m_numberOfColumns = t_columns;

	SDL_QueryTexture(m_textureSpriteSheet, 0, 0, &m_spriteWidth, &m_spriteHeight);

}


Enemy::Enemy(SDL_Renderer* t_enemyRenderer, const int t_x, const int t_y, const int t_w, const int t_h, std::string t_difficulty, const int t_types)
{
	m_destroy = false;
	m_entityRenderer = t_enemyRenderer;

	//Takes width and height of the whole image and divides by the number of rows and columns respectively to "split up" the texture
	m_sourceRect.w = m_spriteWidth / m_numberOfRows; 
	m_sourceRect.h = m_spriteHeight / m_numberOfColumns; 
	//Finally, chooses a random image from our sprite sheet to later use it as a texture of the object
	//m_sourceRect.x = ((rand() % m_numberOfRows) * m_sourceRect.w);
	//m_sourceRect.y = ((rand() % m_numberOfColumns) * m_sourceRect.h);

	
    #if _DEBUG_MODE_ == 1
	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Width and height wariable before quering: Width %d, Height %d", m_enemyWidth, m_enemyHeight);

	SDL_QueryTexture(Enemy::m_textureSpriteSheet, NULL, NULL, &m_enemyWidth, &m_enemyHeight);

	SDL_LogMessage(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO, "Width and height wariable after quering: Width %d, Height %d", m_enemyWidth, m_enemyHeight);
    #endif
	
	
	m_entityPosition = { t_x, t_y, t_w, t_h }; // Our destination rectangle. It will be renderer in our game renderer later

	if (t_difficulty == "Easy" || t_difficulty == "easy")
	{
		m_sourceRect.x = ((rand() % m_numberOfRows) * m_sourceRect.w);
		m_sourceRect.y = ((rand() % (m_numberOfColumns / t_types)) * m_sourceRect.h);

	}
	else if (t_difficulty == "Medium" || t_difficulty == "medium")
	{
		m_sourceRect.x = ((rand() % m_numberOfRows) * m_sourceRect.w);
		m_sourceRect.y = ((rand() % ((m_numberOfColumns / 2)) * m_sourceRect.h));

	}
	else if (t_difficulty == "Hard" || t_difficulty == "hard")
	{
		m_sourceRect.x = ((rand() % m_numberOfRows) * m_sourceRect.w);
		m_sourceRect.y = ((rand() % m_numberOfColumns) * m_sourceRect.h);

	}
	else
	{
		std::cerr << "Invalid difficulty: " << t_difficulty << "\nTerminating the program.";
		exit(7);
	}

	if (m_sourceRect.y < 520)
	{
		if (t_difficulty == "Easy" || t_difficulty == "easy")
		{
			m_speed = 3;
            m_hp = 1;
		}
		else if (t_difficulty == "Medium" || t_difficulty == "medium")
		{
			m_speed = 5;
			m_hp = 2;
		}
		else
		{
			m_speed = 6;
			m_hp = 3;
		}
	}
	else if (m_sourceRect.y >= 520 && m_sourceRect.y < 1040)
	{
		
		if (t_difficulty == "Medium" || t_difficulty == "Medium" || t_difficulty == "Easy" || t_difficulty == "easy")
		{
			m_speed = 7;
			m_hp = 1;
		}
		else
		{
			m_speed = 8;
			m_hp = 2;
		}
	}
	else if (m_sourceRect.y >= 1040)
	{
		m_speed = 4;
		m_hp = 6;
	}

}

void Enemy::draw()
{
	SDL_RenderCopy(m_entityRenderer, m_textureSpriteSheet, &m_sourceRect, &m_entityPosition);
}

void Enemy::update()
{
	m_entityPosition.x -= m_speed;
}

void Enemy::increaseSpeed()
{
	m_speed += 1;
}

void Enemy::update(std::vector<Bullet*>& t_entityVector)
{
	m_entityPosition.x -= m_speed;
	for (Bullet* someEntity : t_entityVector)
	{
		if (hasCollisionWith(someEntity))
		{
			--m_hp;
			someEntity->m_destroy = true;
		}
		if (m_hp <= 0)
		{
			m_destroy = true;
		}
	}
}

Enemy::~Enemy()
{
	//SDL_DestroyTexture(m_entityTexture);
}

//Maybe not too clearly readable but I wanted to have as less lines as possible so I decided to stick to ember minitializer lists
Bullet::Bullet(SDL_Renderer* t_entityRenderer, const char* t_path, const int t_x, const int t_y, const int t_w, const int t_h) : Entity(t_entityRenderer, t_path, t_x, t_y, t_w, t_h)
{
	m_destroy = false;
	m_lifeTime = 100;

	m_speed = 30;
}

void Bullet::update()
{
	m_entityPosition.x += m_speed;
	--m_lifeTime;
}

Bullet::~Bullet()
{
	SDL_DestroyTexture(m_entityTexture);
}