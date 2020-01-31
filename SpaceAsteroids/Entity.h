#ifndef _ENTITY_H_
#define _ENTITY_H_

#include "Header Files.h"

class Entity
{
public:
	Entity(SDL_Renderer* t_entityRenderer, const char* t_path, const int t_x, const int t_y, const int t_w, const int t_h);
	Entity() = default;
	virtual ~Entity();
    bool hasCollisionWith(const Entity* someEntity);
	virtual void draw();
	virtual void update() = 0; //Has to be moved to derived classes, makes this class abstract and impossible to instantiate on its own
	int getPositionX() const { return m_entityPosition.x; }
	int getPositionY() const { return m_entityPosition.y; }
	SDL_Rect& getPosition() { return m_entityPosition; }
	int getWidth() const { return m_entityPosition.w; }
	int getHeight() const { return m_entityPosition.h; }
	int getSpeed() const { return m_speed; }
	int getHP() const { return m_hp; }


	bool m_destroy, m_spawnMoreEnemies;

protected:
	SDL_Rect m_entityPosition;
	SDL_Texture* m_entityTexture;
	SDL_Renderer* m_entityRenderer;
	int m_hp, m_speed;
	//int m_numberOfRows, m_numberOfColumns;

private:
	int m_textureWidth, m_textureHeight; // Isn't supposed to be derived, hence private
	
};



class Bullet final : public Entity //Final means can't be derived further
{
public:
	Bullet(SDL_Renderer* t_entityRenderer, const char* t_path, const int t_x, const int t_y, const int t_w, const int t_h);
	virtual ~Bullet();
	void update() final override; //Serves similar purpose as with final keyword in class declaration
	int getLifeTime() const { return m_lifeTime; }
private:
	int m_lifeTime;
};

class Enemy final : public Entity
{
public:

	Enemy(SDL_Renderer* t_enemyRenderer, const int t_x, const int t_y, const int t_w, const int t_h, std::string t_difficulty, const int t_types);
    virtual ~Enemy();
	void draw() final override;
	void update() final override;
	void increaseSpeed();
	void update(std::vector<Bullet*>& t_someEntity); // for moving our enemies
	static void initialiseTexture(SDL_Renderer* t_renderer, const char* t_path, int t_rows, int t_columns); //The same for every instance
	
private:
	int m_enemyWidth, m_enemyHeight, m_numberOfTexture;
	SDL_Rect m_sourceRect;
	static SDL_Texture* m_textureSpriteSheet; //It's same sheet for everyone. Making it static saves up the memory
	static int m_numberOfRows;
	static int m_numberOfColumns;
	static int m_spriteWidth;
	static int m_spriteHeight;

};

class Player final : public Entity 
{
	
public:
	Player(SDL_Renderer* t_EntityRenderer, const char* t_path, const int t_x, const int t_y, const int t_w, const int t_h);
    virtual ~Player();
	void update() final override; 
	void update(std::vector<Enemy*> &t_enemiesVector, const int &t_limitForY); 
	bool checkFiringStatus() const { return m_fireButtonPressed; }
	void changeFiringStatus(bool t_temp);
	
private:
	bool m_fireButtonPressed;

};

#endif 
