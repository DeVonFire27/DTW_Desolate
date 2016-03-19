#include "globals.h"
#include "IGameState.h"
#include "BitmapFont.h"

void IGameState::DrawButton(SGD::Point position, BitmapFont& font, const char * text, SGD::Color backColor)
{
	bulletButton = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/bulletbutton.png");

	//TODO: replace drawRectangle with draw texture of bullet once we get the art asset
	//SGD::GraphicsManager::GetInstance()->DrawRectangle(buttonPosition, backColor);
	//SGD::GraphicsManager::GetInstance()->DrawTexture(bulletButton, position, 0.0f, {}, {}, {1.6f, 1.6f});
	SGD::Point centeredPosition = { position.x, position.y };

	//SGD::Point centeredPosition = { (position.x + 25), position.y + 5 };
	font.Draw(text, (int)centeredPosition.x, (int)centeredPosition.y, 1.f, SGD::Color::White);

	SGD::GraphicsManager::GetInstance()->UnloadTexture(bulletButton);
}
