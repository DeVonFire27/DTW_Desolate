#pragma once
#include "IGameState.h"
#include <string>

class BitmapFont;

enum difficulty {standard, difficult, veryhard, impossible};

class NewGameState : public IGameState
{
public:
	static NewGameState* GetInstance(void);

	// IGameState Interface:
	virtual void	Enter(void)				override;
	virtual void	Exit(void)				override;

	virtual bool	Input(void)				override;
	virtual void	Update(float elapsedTime)	override;
	virtual void	Render(void)				override;
	void SavePlayerInfo();
	string getCharacterClass() { return characterClass; }

	string getDifficulty() { return difficulty; }
	void SetDifficulty(difficulty newDifficulty) { difficulty = newDifficulty; }

	string gunslingerInfo;
	string sniperInfo;
	string brawlerInfo;
	string medicInfo;
	string cyborgInfo;
	string gunslingerStatInfo;
	string sniperStatInfo;
	string brawlerStatInfo;
	string medicStatInfo;
	string cyborgStatInfo;
private:
	NewGameState() = default;
	~NewGameState() = default;

	NewGameState(const NewGameState&) = delete;	// copy constructor
	NewGameState& operator= (const NewGameState&) = delete;	// assignment operator
	SGD::Point mouseposition;
	SGD::HTexture newGameScreen;
	SGD::HTexture newGameScreen_selected;
	SGD::Rectangle backRect, beginRect;

	int characterCursor = 0;
	int difficultyCursor = 0;
	int section = 0;
	bool usingKeyboardInput = true;
	string characterClass;
	string difficulty;
};