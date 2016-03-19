/***************************************************************
|	File:		Game.h
|	Author:
|	Course:
|	Purpose:	Game class controls the SGD wrappers
|				& runs the game state machine
***************************************************************/

#ifndef GAME_H
#define GAME_H


/**************************************************************/
// Forward class declaration
//	- tells the compiler that the type exists
//	- allows us to create pointers or references
class BitmapFont;
class IGameState;
class Entity;
#include "../SGD Wrappers/SGD_Declarations.h"
#include "../SGD Wrappers/SGD_AudioManager.h"
#include "Weapons\Weapon.h"
#include <list>
using namespace std;
//class Weapon;
/**************************************************************/
// Game class
//	- runs the game logic
//	- controls the SGD wrappers
//	- SINGLETON!
//		- there is only ONE instance
//		- global access method (GetInstance)
class Game
{
public:
	SGD::HTexture background = SGD::INVALID_HANDLE;

	/**********************************************************/
	// Singleton Accessors:
	static Game*	GetInstance(void);
	static void		DeleteInstance(void);

	/**********************************************************/
	// Input:
	bool IsMenuUpInput();
	bool IsMenuDownInput();
	bool IsMenuLeftInput();
	bool IsMenuRightInput();
	bool IsMenuConfirmInput();
	bool IsMenuBackInput();

	/**********************************************************/
	// Setup, Play, Cleanup:
	bool Initialize(int width, int height);
	int	 Main(void);
	void Terminate(void);

	SGD::Color col;
	/**********************************************************/
	// Screen Size Accessors:
	int	GetScreenWidth(void) const	{ return m_nScreenWidth; }
	int	GetScreenHeight(void) const	{ return m_nScreenHeight; }

	void SetScreenWidth(int set) { m_nScreenWidth = set; }
	void SetScreenHeight(int set) { m_nScreenHeight = set; }

	IGameState* GetCurrentState() { return m_pCurrState; }

	HAudio GetMusic() { return backgroundMusic; }
	void SetMusic(HAudio newAudio, bool useFade = false);

	//Accessor and Mutator for full screen mode
	bool getFullScreen() { return fullScreen; }
	void setFullScreen(bool full) { fullScreen = full; }

	// Font Accessor:
	BitmapFont* GetFont(void) const	{ return m_pFontThick; }
	BitmapFont* GetFontThin(void) const	{ return m_pFont; }
	BitmapFont* GetFontTitle(void) const	{ return m_pFontTitle; }
	BitmapFont* GetFontButton(void) const	{ return m_pFontButton; }
	//list<Weapon*> GetWeaponsList() { return weapons; }

	bool LoadSave = false;
	string loadFile = "";
	string playerClass = "";
	int playerDifficulty = 0;

	Weapon* CreateWeapon(SGD::Point p, Weapon::GunType gt, int gunLevel, bool world = true, int Gun = -1);
		/**********************************************************/
	// Game State Machine:
	//	- can ONLY be called by the state's Input, Update, or Render methods!!!
	void ChangeState(IGameState* pNewState);

	void InteruptState(IGameState* newState);

	bool GetStateInterupted(void) const { return m_pCurrState != nullptr; }

	// Game-global audio
	SGD::HAudio creditsMusic;
	SGD::HAudio fightMusic;
	SGD::HAudio mainMenuMusic;
	SGD::HAudio townMusic1;
	SGD::HAudio townMusic2;
	SGD::HAudio townMusic3;
	SGD::HAudio explorationMusic1;

	float opacity = 1.0f;
	bool fading = true;
	bool showFPS = false;

private:
	/**********************************************************/
	// Singleton Object:
	static Game*	s_pInstance;

	Game(void) = default;		// default constructor
	~Game(void) = default;		// destructor

	Game(const Game&) = delete;	// copy constructor
	Game& operator= (const Game&) = delete;	// assignment operator

	// Globally-used changing music variable
	SGD::HAudio backgroundMusic;

	/**********************************************************/
	// SGD Wrappers
	SGD::AudioManager*		m_pAudio = nullptr;
	SGD::GraphicsManager*	m_pGraphics = nullptr;
	SGD::InputManager*		m_pInput = nullptr;


	/**********************************************************/
	// Screen Size
	int						m_nScreenWidth = 1;
	int						m_nScreenHeight = 1;

	/**********************************************************/
	// Menu Background
	Emitter *backParticles = nullptr;

	SGD::Vector joystick_prev;
	SGD::Vector joystick2_prev;

	/**********************************************************/
	// Game Font
	BitmapFont*				m_pFont = nullptr;
	BitmapFont*				m_pFontButton = nullptr;
	BitmapFont*				m_pFontTitle = nullptr;
	BitmapFont*				m_pFontThick = nullptr;

	/**********************************************************/
	// reticule and cursor textures
	SGD::HTexture reticuleTex = SGD::INVALID_HANDLE;
	SGD::HTexture cursorTex = SGD::INVALID_HANDLE;

	/**********************************************************/
	// Current Game State
	IGameState*				m_pCurrState = nullptr;
	// the state that has been interupted
	IGameState* interuptedState = nullptr;

	bool fullScreen = false;

	float musicFadeVolume = 1.f;

	/**********************************************************/
	// Game Time
	unsigned long long			m_ulGameTime = 0;
	int frames = 0;
	int fps = 0;
	float sinceFPS = 0.0f;
};

#endif //GAME_H
