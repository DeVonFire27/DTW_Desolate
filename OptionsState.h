#pragma once
#include "IGameState.h"

class BitmapFont;

class OptionsState : public IGameState
{
public:
	static OptionsState* GetInstance(void);

	// IGameState Interface:
	virtual void	Enter(void)				override;
	virtual void	Exit(void)				override;

	virtual bool	Input(void)				override;
	virtual void	Update(float elapsedTime)	override;
	virtual void	Render(void)				override;

	int getMusicVolume() { return musicVolume; }
	void setMusicVolume(int mv) { musicVolume = mv; }
	int getSFXVolume() { return SFXVolume; }
	void setSFXVolume(int sfxv) { SFXVolume = sfxv; }
	bool getFullScreen() { return fullScreen; }
	void setFullScreen(bool fs) { fullScreen = fs; }

private:
	OptionsState() = default;
	~OptionsState() = default;

	OptionsState(const OptionsState&) = delete;	// copy constructor
	OptionsState& operator= (const OptionsState&) = delete;	// assignment operator

	// Cursor Index
	int	m_nCursor = 0;
	int musicVolume = 50;	//two volume int that will be between 0 and 100;
	int SFXVolume = 50;
	bool fullScreen = false;

	SGD::Rectangle fullscreenRect, backRect, sfxRect, musicRect;
	SGD::Point mouseLocation;
	SGD::HTexture emptyCheckBox;
	SGD::HTexture checkedBox;
	SGD::HAudio backgroundMusic, gunshotSFX;
};