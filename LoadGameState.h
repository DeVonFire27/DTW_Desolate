#pragma once
#include "IGameState.h"
#include <vector>

class LoadGameState : public IGameState
{
public:
	static LoadGameState* GetInstance(void);

	// IGameState Interface:
	virtual void	Enter(void)				override;
	virtual void	Exit(void)				override;

	virtual bool	Input(void)				override;
	virtual void	Update(float elapsedTime)	override;
	virtual void	Render(void)				override;

private:
	LoadGameState() = default;
	~LoadGameState() = default;

	LoadGameState(const LoadGameState&) = delete;	// copy constructor
	LoadGameState& operator= (const LoadGameState&) = delete;	// assignment operator
	SGD::HTexture GetClassPic(string charClass);
	SGD::Rectangle *selectedGameRect, save1Rect, save2Rect, save3Rect;
	int m_nCursor = 0;
	int selectedSave = 0;
	int totalSaves = 0;

	std::vector<string> files;
	std::vector<string> currObjective;
	std::vector<string> charClass;
	std::vector<string> saveLevel;
	std::vector<string> saveTime;
	std::vector<string> saveDifficulty;

	SGD::HTexture previous, next;
	SGD::HTexture gunslinger = SGD::INVALID_HANDLE, medic = SGD::INVALID_HANDLE, cyborg = SGD::INVALID_HANDLE, sniper = SGD::INVALID_HANDLE, brawler = SGD::INVALID_HANDLE;
};
