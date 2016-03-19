#pragma once
#include "IGameState.h"

class InstructionsState : public IGameState
{
public:
	static InstructionsState* GetInstance(void);

	// IGameState Interface:
	virtual void	Enter(void)				override;
	virtual void	Exit(void)				override;

	virtual bool	Input(void)				override;
	virtual void	Update(float elapsedTime)	override;
	virtual void	Render(void)				override;
private:
	InstructionsState() = default;
	~InstructionsState() = default;

	InstructionsState(const InstructionsState&) = delete;
	InstructionsState& operator=(const InstructionsState&) = delete;

	SGD::Rectangle nextRect;
	SGD::Rectangle backRect;
	bool page2 = false;
	SGD::HTexture pcControlsScreen, arcadeControlsScreen, consoleControlsScreen;
	SGD::HTexture characterClassesScreen;
	int m_nCursor = 0;
};