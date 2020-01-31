#ifndef _ANIMATION_H_
#define _ANIMATION_H_

#include "Header Files.h"
#include "Texture Manager.h"

class Animation final //Can not be derived
{
public:
	Animation(SDL_Renderer* t_currentRenderer, const char* t_path, SDL_Rect t_position, const int t_rows, const int t_columns, const int t_playSpeed);
	void updateAnimation(), playAnimation();

	bool m_destroy;
	~Animation();
private:
	SDL_Renderer* m_animationRenderer;
	SDL_Rect m_sourceRect, m_destinationRect;
	SDL_Texture* m_animationSheet;
	int m_rowsInSheet, m_columnsInSheet, m_currentRow, m_currentColumn, m_animationSpeed, m_sheetWidth, m_sheetHeight;

};


#endif
