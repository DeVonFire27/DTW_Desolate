#pragma once
#include "IGameState.h"
class Sniper;

class CreditsState : public IGameState
{
public:
	static CreditsState* GetInstance(void);

	// IGameState Interface:
	virtual void	Enter(void)				override;
	virtual void	Exit(void)				override;

	virtual bool	Input(void)				override;
	virtual void	Update(float elapsedTime)	override;
	virtual void	Render(void)				override;
	
private:
	CreditsState() = default;
	~CreditsState() = default;

	CreditsState(const CreditsState&) = delete;	// copy constructor
	CreditsState& operator= (const CreditsState&) = delete;	// assignment operator
	float yposition = 600;
	SGD::HTexture creditsScreen;
	SGD::HAudio creditsMusic;
};