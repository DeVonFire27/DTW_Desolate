/***************************************************************
|	File:		GameplayState.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|	Course:		
|	Purpose:	GameplayState class initializes & runs the game logic
***************************************************************/

#ifndef GAMEPLAYSTATE_H
#define GAMEPLAYSTATE_H


#include "IGameState.h"						// uses IGameState
#include "../SGD Wrappers/SGD_Handle.h"		// uses HTexture & HAudio
#include "../SGD Wrappers/SGD_AudioManager.h"

enum Tutorial { Shoot, WeaponSwap, Trap, Active, Knockback, HealthKit, Recruitment, PartySkills };

/**************************************************************/
// Forward class declaration
//	- tells the compiler that the type exists
//	- allows us to create pointers or references
class Entity;
class Player;
class NPC;
class EntityManager;
class World;
class HealthPickup;
class Weapon;
//class Dialogue;

#include "../SGD Wrappers/SGD_Declarations.h"
#include <string>
#include <map>
#include <sstream>
#include "Particle System/Emitter.h"

class ObjectiveList;
class Dialogue;

//Bucket Enums
enum Buckets { PARTICLES, PICKUPSandHAZARDS, CHARACTERS, BULLETS, SKILLS };
/**************************************************************/
// GameplayState class
//	- runs the game logic
//	- SINGLETON! (Static allocation, not dynamic)
class GameplayState : public IGameState
{
public:
	
	/**********************************************************/
	// Public members
	float sinceLastTutorial;

	// testing members
	float fBaseDamageTaken, fPassiveDamageTaken;

	float fBaseDamageDealt, fPassiveDamageDealt;

	float fPassiveKillLifeGain;

	float fNoFireTimer, fPassiveTimer;

	int nMyChance, nTheOdds, nRiccochetOdds;

	bool effectRemoved = false;

	bool enterReleased = false;

	Player* player = nullptr;
	vector <NPC*> npcs;
	NPC *newPartyMember = nullptr;

	bool pausedSave = false;
	bool pausedSaveNew = false;
	std::stringstream pausedSaveInput;

	bool tutorial = true;
	bool tutorialChosen = false;
	int currentTutorialObjective = 0;
	vector<string> tutorialDialogue;

	NPC * npcBeingSpokenTo = nullptr;
	bool inRecruitmentDialogue = false;
	float sinceFailedRecruiting = 3.0f;
	float sinceRecruitment = 3.0f;

	EntityManager*			m_pEntities = nullptr;
	vector<Emitter*> saveEmitters;
	//SGD::Color saveEmitterStartColor;
	//SGD::Color saveEmitterEndColor;

	Dialogue * gameplayDialogue = nullptr;

	stringstream weaponCompare;

	/**********************************************************/
	// Save screen
	void EnterSaveScreen();
	SGD::HTexture GetCharClassPortrait(string);
	SGD::HTexture GetCharClassPortrait(int charType);
	SGD::HTexture GetCharActive(int charType);
	std::vector<string> saveFiles;
	std::vector<string> saveCurrObjective;
	std::vector<string> saveCharClass;
	std::vector<string> saveTime;
	std::vector<string> saveDifficulty;
	std::vector<string> saveLevel;

	/**********************************************************/
	// Singleton Accessor:
	static GameplayState* GetInstance( void );
	
	// World Accessor
	World* GetWorld() { return world; }

	/**********************************************************/
	// IGameState Interface:
	virtual void	Enter	( void )				override;	// load resources
	virtual void	Exit	( void )				override;	// unload resources
					
	virtual bool	Input	( void )				override;	// handle user input
	virtual void	Update	( float elapsedTime )	override;	// update game entities / animations
	virtual void	Render	( void )				override;	// render game entities / menus
	bool IsGamePaused() { return (paused || pausedSave); }
	bool GetPaused();
	void SetPaused(bool p);
	void SetCharacterSelect(bool cs) { chooseCharacterScreen = cs; }
	bool GetSwapParty() { return swapParty; }
	void SetSwapParty(bool sp) { swapParty = sp; }
	bool GetPartyWasAltered() { return partyWasAltered; }
	void SetPartyWasAltered(bool altered) { partyWasAltered = altered; }
	bool GetLose() { return lose; }
	bool GetWin() { return win; }
	Emitter *CreateEmitter(std::string file, bool addEntityManager = true);
	Emitter *CreateEmitter(std::string file, Character* anchor);
	Emitter *LoadEmitterFile(std::string file);
	void RenderPauseMenu();
	void RenderSaveMenu();
	void RenderStatsScreen();
	void RenderLoseScreen();
	void RenderWinScreen();
	void ShowCharacterSelectScreen();
	void CheckCursorCollisionWithCharacterPortraits();
	void CheckIfCharacterIsCloseEnoughToAnotherCharacterToBeConsideredACollision();
	void SaveProgress(string file);
	HealthPickup* GetHealthPickup() { return hp; }

	bool GetFuckTheseStupidFuckingNPCs() { return fuckthesestupidfuckingnpcs; }
	void SetFuckTheseStupidFuckingNPCs(bool ftsfnpcs) { fuckthesestupidfuckingnpcs = ftsfnpcs; }

	void SwapWeapons(Weapon* w);
	void CheckWeaponCollision();

	void SetWeaponCompare(Weapon* pWep);
	void RenderWeaponCompare();
	bool comparingWeapons = false;

	void SetTimeStep(float step) { timeStep = step; }

	// Compass
	SGD::HTexture compass = SGD::INVALID_HANDLE;
	SGD::HTexture compassArrow = SGD::INVALID_HANDLE;

	// All musics
	SGD::HAudio grunt;
	SGD::HAudio grunt2;
	SGD::HAudio grunt3;

	// projectile weapons
	SGD::HAudio pistolFiring;
	SGD::HAudio shotgunFiring;
	SGD::HAudio sniperRifleFiring;
	SGD::HAudio bulletImpact;

	SGD::HAudio flameThrowerFiring;
	SGD::HAudio chainSawFiring;
	SGD::HAudio chainSawLauncher;
	SGD::HAudio gatlingGun;
	SGD::HAudio spitAttack;

	// melee weapons
	SGD::HAudio pointyStickSwinging;
	SGD::HAudio knifeSwinging;
	SGD::HAudio knifeImpact;
	SGD::HAudio sledgehammerSwinging;
	SGD::HAudio slash_sfx;
	SGD::HAudio bite_sfx;
	SGD::HAudio allure_sfx;
	SGD::HAudio mortar_sfx;
	SGD::HAudio stampede_sfx;

	//Pickup sfx
	SGD::HAudio weaponPickup = SGD::INVALID_HANDLE;
	SGD::HAudio healthPickupSFX = SGD::INVALID_HANDLE;

	// Success sound effect
	SGD::HAudio success = SGD::INVALID_HANDLE;

	//passive sound effects
	SGD::HAudio trickShot = SGD::INVALID_HANDLE;
	SGD::HAudio bloodSpolsion = SGD::INVALID_HANDLE;
	SGD::HAudio tranfusion = SGD::INVALID_HANDLE;
	SGD::HAudio chain_lightnin = SGD::INVALID_HANDLE;
	SGD::HAudio crit_shot = SGD::INVALID_HANDLE;
	SGD::HAudio shield = SGD::INVALID_HANDLE;
	SGD::HAudio shieldFull = SGD::INVALID_HANDLE;
	SGD::HAudio heals = SGD::INVALID_HANDLE;
	SGD::HAudio knockSFX = SGD::INVALID_HANDLE;


	// win/loose sound effects
	SGD::HAudio loss_sound_1 = SGD::INVALID_HANDLE;
	SGD::HAudio loss_sound_2 = SGD::INVALID_HANDLE;
	SGD::HAudio win_sound_1 = SGD::INVALID_HANDLE;
	SGD::HAudio win_sound_2 = SGD::INVALID_HANDLE;



	//hazard sound effects
	SGD::HAudio geigerCounter = SGD::INVALID_HANDLE;
	SGD::HAudio bearTrapSound = SGD::INVALID_HANDLE;
	SGD::HAudio pitfallSound = SGD::INVALID_HANDLE;

	SGD::HTexture upArrow, downArrow;
	SGD::HTexture gunslingerPortrait, sniperPortrait, medicPortrait, brawlerPortrait, cyborgPortrait;
	SGD::HTexture gunslingerActivePic, sniperActivePic, medicActivePic, brawlerActivePic, cyborgActivePic;

	SGD::HTexture previous, next;
	//int sinceAudioChange = 0;
	list<Weapon*>weapons;

	vector<Emitter *> HUDentities;

	ObjectiveList * objectives = nullptr;
	//Dialogue * dialogue = nullptr; 
	bool fromGameplayState = false;
	bool collidingWithAlly = false;
	bool alreadySetWeaponInfo = false;
	bool newGame = false;
private:

	/**********************************************************/
	// SINGLETON (not-dynamically allocated)
	GameplayState( void )			= default;	// default constructor
	virtual ~GameplayState( void )	= default;	// destructor

	GameplayState( const GameplayState& )				= delete;	// copy constructor
	GameplayState& operator= ( const GameplayState& )	= delete;	// assignment operator

	/**********************************************************/
	// Game Entities

	World*					world				= nullptr;
	std::map<string, Emitter> emitters;
	
	/**********************************************************/
	// Event & Message Managers
	SGD::EventManager*		m_pEvents			= nullptr;
	SGD::MessageManager*	m_pMessages			= nullptr;

	int LoadEmitters(std::string);

	// Message Callback Function:
	static void MessageProc( const SGD::Message* pMsg );
	HealthPickup* hp;
	bool lose = false;
	bool win = false;
	bool paused = false;
	bool checkStats = false;	//bool to be marked as true when you click the stats button in the pause menu to see your current party's stats
	bool chooseCharacterScreen = false;	//bool to be marked as true when your players character is killed. Tells the program to pause the game and render the screen to pick a new character to control
	bool swapParty = false;
	bool partyWasAltered = false;
	SGD::Rectangle resumeRect, instructRect, optionRect, statsRect, exitRect;
	int m_nCursor = 0;
	int selectedSave = 0;
	float timeStep = 1.0f;
	bool fuckthesestupidfuckingnpcs = false;
	bool renderPassiveInfo = false;
	string passiveInfo;
	float fadeInCooldown = 0.0f;

	int alphaEntrySelected = 0;
	bool alphaentry = true;

	float timer = 0;
	BitmapFont* font;

	//-1 means its less than, 0 is equals, 1 is greater than
	int damageCompare = 0;
	int rateOfFireCompare = 0;
};

#endif //GAMEPLAYSTATE_H
