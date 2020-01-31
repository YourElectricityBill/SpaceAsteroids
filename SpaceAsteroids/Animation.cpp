#include "Animation.h"

Animation::Animation(SDL_Renderer* t_currentRenderer, const char* t_path, SDL_Rect t_position, const int t_rows, const int t_columns, const int t_playSpeed)
{
	m_animationRenderer = t_currentRenderer;
	m_animationSheet = TextureManager::LoadTexture(t_path, t_currentRenderer);
	m_destinationRect = t_position;
	m_rowsInSheet = t_rows;
	m_columnsInSheet = t_columns;
	m_animationSpeed = t_playSpeed;

	m_currentRow = 0;
	m_currentColumn = 0;

	m_sheetWidth = m_sheetHeight = 0;

	SDL_QueryTexture(m_animationSheet, 0, 0, &m_sheetWidth, &m_sheetHeight);

	m_sourceRect = { m_currentRow, m_currentColumn, m_sheetWidth / m_rowsInSheet , m_sheetHeight / m_columnsInSheet };
}

void Animation::playAnimation()
{
	m_sourceRect.x = m_currentRow * m_sourceRect.w;
	m_sourceRect.y = m_currentColumn * m_sourceRect.h;

	SDL_RenderCopy(m_animationRenderer, m_animationSheet, &m_sourceRect, &m_destinationRect);
}

void Animation::updateAnimation()
{
	++m_currentRow;
	if (m_currentRow >= m_rowsInSheet)
	{
		m_currentRow = 0;
		++m_currentColumn;
		if (m_currentColumn >= m_columnsInSheet)
		{
			m_destroy = true;
		}
	}
	m_destinationRect.x -= 3; // To give feeling that the animation is not "static"
}

Animation::~Animation()
{
	SDL_DestroyTexture(m_animationSheet);
}
