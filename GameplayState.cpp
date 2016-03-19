/***************************************************************
|	File:		GameplayState.cpp
|	Author:		Everyone had a hand in creating this monstrosity
|	Course:		SGP
|	Purpose:	GameplayState class initializes & runs the game logic
|				3000 lines of wtf
***************************************************************/

#include "globals.h"
#include "GameplayState.h"

#include "Weapons\Pistol.h"

#include "Game.h"
#include "MainMenuState.h"
#include "NewGameState.h"
#include "InstructionsState.h"
#include "OptionsState.h"

#include "CreditsState.h"
#include "LoadGameState.h"

#include "BitmapFont.h"

#include "Agents\Jane.h"

#include "Agents\ColliderTest.h"
#include "Entity.h"
#include "EntityManager.h"
#include "Particle System/Emitter.h"
#include "Tile System/World.h"
#include "Hazards\BearTrap.h"
#include "Hazards\Pitfall.h"
#include "Hazards\RadiationPool.h"
#include "HealthPickup.h"
#include "Tile System\World.h"
#include "EventProc\EventColliderTest.h"

#include "Weapons\Bullet.h"
#include "Abilities\Ability.h"
#include "Abilities\LaserBullet.h"

#include "HealthPickup.h"

#include "Animation System\AnimationSystem.h"

#include "Weapons\MeleeWeapon.h"
#include "Weapons\Pistol.h"
#include "Weapons\Shotgun.h"
#include "Weapons\SniperRifle.h"
#include "Weapons\SpecialWeapons\Flamethrower.h"
#include "Weapons\SpecialWeapons\GatlingGun.h"
#include "Weapons\SpecialWeapons\BuzzsawLauncher.h"
#include "Weapons\HitMarker.h"


#include <cstdlib>
#include <cassert>
#include <vector>
#include "EventProc\EventProc.h"

#include "TinyXML/tinyxml.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <Shlobj.h>
#include <Shlwapi.h>
#pragma comment( lib, "shlwapi.lib")

#include "Tile System\World.h"

// Including objectives
#include "Objective System\Objective.h"
#include "Objective System\ObjectiveList.h"

// Including dialogue
#include "Dialogue System\Dialogue.h"
#include "Agents\MUTt.h"
#include "Agents\Razard.h"
#include "Agents\Radzilla.h"
#include "Agents\Radiant.h"
#include "Agents\Radicorn.h"

// Including spawn sysetm
#include "Spawn System\Spawn.h"
#include "Spawn System\SpawnList.h"

#define FADEINCOOLDOWN 2.0f

Emitter* GameplayState::CreateEmitter(std::string file, bool addEntityManager)
{
	Emitter* emitter = new Emitter(emitters[file]);

	if (addEntityManager)
	{
		m_pEntities->AddEntity(emitter, PARTICLES);
		emitter->Release();
	}
	return emitter;
}

Emitter* GameplayState::CreateEmitter(std::string file, Character* anchor)
{
	Emitter* emitter = new Emitter(emitters[file]);
	emitter->SetPosition({ anchor->GetPosition().x + anchor->GetSize().width / 2, anchor->GetPosition().y - anchor->GetSize().height });
	emitter->SetFollowTarget(anchor);

	m_pEntities->AddEntity(emitter, PARTICLES);
	emitter->Release();
	return emitter;
}


/**************************************************************/
// GetInstance
//	- allocate static global instance
//	- return THE instance
/*static*/ GameplayState* GameplayState::GetInstance(void)
{
	static GameplayState s_Instance;	// stored in global memory once
	return &s_Instance;
}

/**************************************************************/
// Enter
//	- reset game
//	- load resources
//	- set up entities
/*virtual*/ void GameplayState::Enter(void)
{
	//Plaster the loading screen before doing this
	SGD::GraphicsManager::GetInstance()->DrawTexture(Game::GetInstance()->background, { Game::GetInstance()->GetScreenWidth() - MENU_BACKIMAGE_WIDTH, Game::GetInstance()->GetScreenHeight() - MENU_BACKIMAGE_HEIGHT });
	Game::GetInstance()->GetFontTitle()->Draw("Loading", 25, Game::GetInstance()->GetScreenHeight() - 75, 1.0f, SGD::Color::White);
	SGD::GraphicsManager::GetInstance()->Update();
	fadeInCooldown = FADEINCOOLDOWN;

	lose = false;
	win = false;
	timer = 0;
	timeStep = 1.0f;

	// Setting tutorial dialogue
	tutorialDialogue.push_back("Left-click to shoot!");																					// 0
	tutorialDialogue.push_back("Weapons will occasionally drop from \nenemies. Approach them and press 'E' \nto swap it with your current one!");			// 1
	tutorialDialogue.push_back("Sometimes traps will be laying on the\n ground. Careful not to touch them!");												// 2
	tutorialDialogue.push_back("There's a bandit up ahead!\nRight-click to use your active skill\nto help you beat him!");				// 3
	tutorialDialogue.push_back("If enemies are too aggressive, \nyou can press 'F' to push\nthem back!");								// 4
	tutorialDialogue.push_back("He dropped a health kit!\nWalk over it to pick it up\nand heal!");										// 5
	tutorialDialogue.push_back("Any friendly NPC can be \nrecruited by pressing 'E', \nif you have the reputation.");					// 6
	tutorialDialogue.push_back("Your partymates have skills, too!\nPress 1/2/3 to use them.");											// 7


	//Initialize rects for buttons in pause menu
	resumeRect = { 300, 110, 470, 160 };
	instructRect = { 300, 210, 470, 260 };
	optionRect = { 300, 310, 470, 360 };
	statsRect = { 300, 410, 470, 460 };
	exitRect = { 300, 510, 470, 560 };

	// store a pointer to message manager
	m_pMessages = SGD::MessageManager::GetInstance();
	m_pMessages->Initialize(MessageProc);

	// load font
	font = new BitmapFont("resources\\graphics\\fonts\\fontThick.fnt");

	//Load all emitters from file
	LoadEmitters("resources/Particles/*");

	AnimationSystem::GetInstance()->LoadAnimation("BrawlerAnim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("MedicAnim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("GunAnim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("SniperAnim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("CyborgAnim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("Jane.xml");
	AnimationSystem::GetInstance()->LoadAnimation("MUTts_Anim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("RADiant_Anim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("Radzard_Anim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("Radzilla_Anim.xml");
	AnimationSystem::GetInstance()->LoadAnimation("RADicorn.xml");


	AnimationSystem::GetInstance()->LoadAnimation("Medic_Active.xml");
	AnimationSystem::GetInstance()->LoadAnimation("Sniper_Active.xml");
	AnimationSystem::GetInstance()->LoadAnimation("Cyborg_Active.xml");
	AnimationSystem::GetInstance()->LoadAnimation("Jane_Active.xml");



	// Loading in the compass
	compass = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/compassBG.png");
	compassArrow = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/compassArrow.png");

	// Loading in all audios
#pragma region Load Audio
	grunt = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/HumanGrunt1.wav");
	grunt2 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/HumanGrunt2.wav");
	grunt3 = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/HumanGrunt3.wav");

	success = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/coin.wav");

	pistolFiring = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/pistol.wav");
	shotgunFiring = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/shotgun.wav");
	sniperRifleFiring = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/sniper.wav");
	bulletImpact = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/thud1.wav");

	flameThrowerFiring = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/flameThrower.wav");
	chainSawFiring = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/chainsaw.wav");
	chainSawLauncher = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/chainsawlauncher.wav");
	gatlingGun = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/pistol.wav");
	spitAttack = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/spitting.wav");

	pointyStickSwinging = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/stickSwinging.wav");
	knifeSwinging = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/knifeSwing.wav");
	knifeImpact = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/knifeStab.wav");
	sledgehammerSwinging = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/sledgeSwinging2.wav");

	trickShot = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/trick_shot.wav");
	bloodSpolsion = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/blood_splosion.wav");
	tranfusion = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/blood_transfusion.wav");
	chain_lightnin = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/chain_lightning.wav");
	crit_shot = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/crit_shot.wav");
	shield = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/dead_shield.wav");
	shieldFull = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/shield_full.wav");
	heals = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/heals.wav");

	geigerCounter = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/GeigerCounterv2.wav");
	bearTrapSound = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/BearTrap.wav");
	pitfallSound = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/pitfallCollision.wav");

	weaponPickup = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/rifleReloadSFX.wav");
	healthPickupSFX = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/healthPickupSFX.wav");
	knockSFX = SGD::AudioManager::GetInstance()->LoadAudio("resources/audio/Sound Effects/knockback.wav");

	SGD::AudioManager* pAudio = SGD::AudioManager::GetInstance();
	win_sound_1 = pAudio->LoadAudio("resources/audio/Sound Effects/WinLoose/win.wav");
	win_sound_2 = pAudio->LoadAudio("resources/audio/Sound Effects/WinLoose/win2.wav");
	loss_sound_1 = pAudio->LoadAudio("resources/audio/Sound Effects/WinLoose/loss.wav");
	loss_sound_2 = pAudio->LoadAudio("resources/audio/Sound Effects/WinLoose/loss2.wav");
	//bite_sfx = pAudio->LoadAudio("resources/audio/Sound Effects/Bite.wav");
	slash_sfx = pAudio->LoadAudio("resources/audio/Sound Effects/Slash.wav");
	allure_sfx = pAudio->LoadAudio("resources/audio/Sound Effects/Allure.wav");
	mortar_sfx = pAudio->LoadAudio("resources/audio/Sound Effects/Mortar.wav");
	stampede_sfx = pAudio->LoadAudio("resources/audio/Sound Effects/Stampede.wav");

#pragma endregion

	gunslingerPortrait = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Gunslinger.png");
	sniperPortrait = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Sniper.png");
	medicPortrait = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Medic.png");
	brawlerPortrait = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Brawler.png");
	cyborgPortrait = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Cyborg.png");

	gunslingerActivePic = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Gunslinger Active Icon.png");
	sniperActivePic = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Sniper Active Icon.png");
	medicActivePic = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Medic Active Icon.png");
	brawlerActivePic = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Brawler Active Icon.png");
	cyborgActivePic = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/cyborg active.png");

	// Initialize the Event Manager
	m_pEvents = SGD::EventManager::GetInstance();
	m_pEvents->Initialize();

	//load textures
	next = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/next.png");
	previous = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/previous.png");
	upArrow = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/upArrow.png");
	downArrow = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/downArrow.png");

	// Allocate the Entity Manager
	m_pEntities = new EntityManager;

	world = World::GetInstance();

	//Get save folder path.  Make directory if does not exist
	wchar_t *appDataPath;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &appDataPath);
	wstring path = appDataPath;
	path.append(L"\\Desolate\\");
	if (!PathFileExists(path.c_str())) CreateDirectory(path.c_str(), NULL);

	//Add filename
	wchar_t buffer[100];
	mbstowcs_s(nullptr, buffer, Game::GetInstance()->loadFile.c_str(), 200);
	path.append(buffer);
	path.append(L".xml");

	gameplayDialogue = new Dialogue(nullptr);
	gameplayDialogue->StopDrawing();

	// Initializing the objectives 
	objectives = ObjectiveList::GetInstance();
	objectives->sinceNewObjective = 3.0f;

	// Loading the world
	char pathbuffer[100];
	wcstombs_s(nullptr, pathbuffer, path.c_str(), 200);
	if (Game::GetInstance()->LoadSave) world->LoadWorld(pathbuffer);
	else world->LoadWorld();

	//Load all emitters from file
	LoadEmitters("resources/Particles/*");

	// loop through the world collision layer and create particle effects for "save" event tiles
	if (true)//World::GetInstance()->world)
	{
		for (unsigned i = 0; i < World::GetInstance()->world[1]->tiles.size(); i++)
		{
			vector<TileLayer*> tempWorld = World::GetInstance()->world;

			// search for save events
			if (tempWorld[1]->tiles[i]->event == "Save")
			{
				// create a new emitter using the savePoint XML
				Emitter* e = GameplayState::GetInstance()->CreateEmitter("savePoint");

				// set the position == the tiles position
				e->SetPosition({
					float(i % tempWorld[1]->layerColumns * tempWorld[1]->GetTileSize().width) + tempWorld[1]->GetTileSize().width / 2,
					float(i / tempWorld[1]->layerColumns * tempWorld[1]->GetTileSize().height) + tempWorld[1]->GetTileSize().height / 2 });

					// set particle scale & spawnRate
					e->SetParticleSize(0, .2f);
					e->spawnRate = 0;

					// save emitter location for future use
					saveEmitters.push_back(e);
			}
		}
	}

#if _DEBUG

	Flamethrower* flamethrower = new Flamethrower();
	flamethrower->SetPosition(player->GetCharacter()->GetPosition());
	weapons.push_back(flamethrower);

#endif

#if SPAWN_GATLING_GUN

	GatlingGun* ggun = new GatlingGun();
	ggun->SetPosition(player->GetCharacter()->GetPosition());
	weapons.push_back(ggun);

#endif
	//74x64  340x64
	if (newGame)
	{
	Weapon::GunType gunType;
	int type = rand() % 4;
	if (type == 0)
		gunType = Weapon::GunType::Pistol;
	else if (type == 1)
		gunType = Weapon::GunType::SniperRifle;
	else if (type == 2)
		gunType = Weapon::GunType::meleeWeapon;
	else if (type == 3)
		gunType = Weapon::GunType::Shotgun;

	Weapon *w = Game::GetInstance()->CreateWeapon({ 4736, 21760 }, gunType, 0);
	weapons.push_back(w);
	}
	// Set Music
	Game::GetInstance()->SetMusic(Game::GetInstance()->explorationMusic1);
	SGD::AudioManager::GetInstance()->SetMasterVolume(SGD::AudioGroup::SoundEffects, OptionsState::GetInstance()->getSFXVolume());
}

/**************************************************************/
// Exit
//	- deallocate entities
//	- unload resources
/*virtual*/ void GameplayState::Exit(void)
{
	AnimationSystem::GetInstance()->UnloadAnimation();

	// detach weapons/abilities
	EventProc::GetInstance()->Dispatch("GameExit");

	SGD::GraphicsManager::GetInstance()->UnloadTexture(previous);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(next);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(upArrow);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(downArrow);

	for (auto i = saveEmitters.begin(); saveEmitters.size() > 0;)
	{
		saveEmitters.erase(i);
	}

	// Deallocate the Entity Manager
	m_pEntities->RemoveAll();
	delete m_pEntities;
	m_pEntities = nullptr;

	// release the reference to the Character
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}

	world->Exit();
	world = nullptr;

	// delete dialog
	delete gameplayDialogue;

	//Delete HUDEntities (cooldown particles)
	for (unsigned int i = 0; i < HUDentities.size(); i++)
	{
		delete HUDentities[i];
	}
	HUDentities.clear();

	// Terminate & deallocate the SGD wrappers
	m_pEvents->Terminate();
	m_pEvents = nullptr;
	SGD::EventManager::DeleteInstance();

	m_pMessages->Terminate();
	m_pMessages = nullptr;
	SGD::MessageManager::DeleteInstance();

	//Unloading the compass
	SGD::GraphicsManager::GetInstance()->UnloadTexture(compass);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(compassArrow);

	SGD::AudioManager::GetInstance()->UnloadAudio(success);

	SGD::AudioManager::GetInstance()->UnloadAudio(pistolFiring);
	SGD::AudioManager::GetInstance()->UnloadAudio(shotgunFiring);
	SGD::AudioManager::GetInstance()->UnloadAudio(sniperRifleFiring);
	SGD::AudioManager::GetInstance()->UnloadAudio(bulletImpact);

	SGD::AudioManager::GetInstance()->UnloadAudio(flameThrowerFiring);
	SGD::AudioManager::GetInstance()->UnloadAudio(chainSawFiring);
	SGD::AudioManager::GetInstance()->UnloadAudio(chainSawLauncher);
	SGD::AudioManager::GetInstance()->UnloadAudio(gatlingGun);
	SGD::AudioManager::GetInstance()->UnloadAudio(spitAttack);

	SGD::AudioManager::GetInstance()->UnloadAudio(pointyStickSwinging);
	SGD::AudioManager::GetInstance()->UnloadAudio(knifeSwinging);
	SGD::AudioManager::GetInstance()->UnloadAudio(knifeImpact);
	SGD::AudioManager::GetInstance()->UnloadAudio(sledgehammerSwinging);

	SGD::AudioManager::GetInstance()->GetInstance()->UnloadAudio(grunt);
	SGD::AudioManager::GetInstance()->GetInstance()->UnloadAudio(grunt2);
	SGD::AudioManager::GetInstance()->GetInstance()->UnloadAudio(grunt3);

	SGD::AudioManager::GetInstance()->UnloadAudio(trickShot);
	SGD::AudioManager::GetInstance()->UnloadAudio(bloodSpolsion);
	SGD::AudioManager::GetInstance()->UnloadAudio(tranfusion);
	SGD::AudioManager::GetInstance()->UnloadAudio(chain_lightnin);
	SGD::AudioManager::GetInstance()->UnloadAudio(crit_shot);
	SGD::AudioManager::GetInstance()->UnloadAudio(shield);
	SGD::AudioManager::GetInstance()->UnloadAudio(shieldFull);
	SGD::AudioManager::GetInstance()->UnloadAudio(heals);

	SGD::AudioManager::GetInstance()->UnloadAudio(geigerCounter);
	SGD::AudioManager::GetInstance()->UnloadAudio(bearTrapSound);
	SGD::AudioManager::GetInstance()->UnloadAudio(pitfallSound);

	SGD::AudioManager::GetInstance()->UnloadAudio(weaponPickup);
	SGD::AudioManager::GetInstance()->UnloadAudio(healthPickupSFX);

	SGD::GraphicsManager::GetInstance()->UnloadTexture(gunslingerPortrait);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(sniperPortrait);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(medicPortrait);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(brawlerPortrait);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(cyborgPortrait);

	SGD::GraphicsManager::GetInstance()->UnloadTexture(gunslingerActivePic);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(sniperActivePic);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(medicActivePic);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(brawlerActivePic);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(cyborgActivePic);

	SGD::AudioManager* pAudio = SGD::AudioManager::GetInstance();
	pAudio->UnloadAudio(win_sound_1);
	pAudio->UnloadAudio(win_sound_2);
	pAudio->UnloadAudio(loss_sound_1);
	pAudio->UnloadAudio(loss_sound_2);


	GameplayState::GetInstance()->weapons.clear();

	// clean up template emitters
	emitters.clear();

	// Unload font
	font->Terminate();

	// Removing objectives
	objectives->RemoveAll();

	// Clearing the weapon vector
	for (auto i = weapons.begin(); weapons.size() > 0; i = weapons.begin())
	{
		(*i)->Release();

		weapons.remove((*i));
	}

	// Clearing the NPC vector
	for (unsigned int i = 0; i < npcs.size(); i++)
	{
		if (!npcs[i]->GetIsEnemy()) // Enemies are deleted in the spawn list
		{
			delete npcs[i];

			_ASSERTE(_CrtCheckMemory());
			npcs[i] = nullptr;
		}
	}

	// temporarly comment this out to stop crashes
	SpawnList::GetInstance()->DeleteAllSpawns();

	npcBeingSpokenTo = nullptr;

	npcs.clear();

	delete font;
	font = nullptr;
}

void GameplayState::SwapWeapons(Weapon* w)
{
	//go through list of weapons, remove your new weapon from the list then add your old weapon to it
	for (auto i = weapons.begin(); i != weapons.end(); ++i)
	{
		if ((*i) == w)
		{
			player->GetCharacter()->GetWeapon()->SetPosition((*i)->GetPosition());
			weapons.remove((*i));

			//RemoveWeaponMsg *msg = new RemoveWeaponMsg(*i);
			//msg->QueueMessage();

			weapons.push_back(player->GetCharacter()->GetWeapon());
			player->GetCharacter()->GetWeapon()->AddRef();
			break;
		}
	}

	//Swap your weapon with the weapon you passed in
	Weapon* tempWeapon = nullptr;
	tempWeapon = player->GetCharacter()->GetWeapon();
	player->GetCharacter()->SetWeapon(w);
	w = tempWeapon;
	w->SetOwner(nullptr);
	w->SetNotWorld(true);
	player->SetOtherWeapon(nullptr);

	if (GameplayState::GetInstance()->currentTutorialObjective == Tutorial::WeaponSwap)
	{
		GameplayState::GetInstance()->currentTutorialObjective++;
		sinceLastTutorial = 0.0f;
	}
}

/**************************************************************/
// Input
//	- handle user input
/*virtual*/ bool GameplayState::Input(void)
{
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();
	static SGD::Vector joystick_prev = SGD::InputManager::GetInstance()->GetLeftJoystick(0);
	SGD::Point mouse_location = pInput->GetMousePosition();

	if (!tutorialChosen && !paused)
	{
		Dialogue::DialogueResult thisResult = gameplayDialogue->Input();

		if (thisResult == Dialogue::YES)
		{
			tutorialChosen = true;
			tutorial = true;
		}
		else if (thisResult == Dialogue::NO)
		{
			tutorialChosen = true;
			tutorial = false;
		}
	}
	else
	{
		/*
		if (pInput->IsKeyPressed(SGD::Key::LeftAlt))
		{
		SetFuckTheseStupidFuckingNPCs(!GetFuckTheseStupidFuckingNPCs());
		}*/
		if (!inRecruitmentDialogue)
		{
			//Input for when you lose the game
			if (lose && timer >= 3)
			{
				if ((ARCADEBUILD == 1 && pInput->GetLeftJoystick(0).y > 0.8f) || pInput->IsKeyPressed(SGD::Key::Down) || pInput->IsDPadPressed(0, SGD::DPad::Down) || pInput->IsKeyPressed(SGD::Key::S))
				{
					++m_nCursor;
					if (m_nCursor > 1)
						m_nCursor = 0;
				}
				else if ((ARCADEBUILD == 1 && pInput->GetLeftJoystick(0).y < -0.8f) || pInput->IsKeyPressed(SGD::Key::Up) || pInput->IsDPadPressed(0, SGD::DPad::Up) || pInput->IsKeyPressed(SGD::Key::W))
				{
					--m_nCursor;
					if (m_nCursor < 0)
						m_nCursor = 1;
				}
				if ((ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || ((ARCADEBUILD == 0) && pInput->IsKeyPressed(SGD::Key::Enter) || pInput->IsButtonPressed(0, 0)))
				{
					if (m_nCursor == 0)
						Game::GetInstance()->ChangeState(LoadGameState::GetInstance());
					else if (m_nCursor == 1)
						Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
				}
				if (((pInput->IsKeyDown(SGD::Key::LButton)) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || ((ARCADEBUILD == 0) && (pInput->IsButtonPressed(0, 0)))))
				{
					if (pInput->GetMousePosition().IsWithinRectangle({ 330, 250, 500, 300 }))
					{
						//Go to load game screen
						Game::GetInstance()->ChangeState(LoadGameState::GetInstance());
					}
					else if (pInput->GetMousePosition().IsWithinRectangle({ 330, 350, 500, 400 }))
					{
						//Go to main menu
						Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
					}
					return true;
				}
			}

			if (win && timer >= 3)
			{

				if ((pInput->IsAnyKeyPressed() || pInput->IsButtonPressed(0, 0)))
				{
					Game::GetInstance()->ChangeState(CreditsState::GetInstance());
					return true;
				}
			}

			if (player != nullptr)
				player->Input();

			// Press Escape to return to Main Menu
			if (paused == true && checkStats == true)
			{

				if (((ARCADEBUILD == 1) && pInput->IsKeyPressed(SGD::Key::RButton)) || pInput->IsKeyDown(SGD::Key::Escape) || pInput->IsButtonDown(0, 2))
				{
					checkStats = false;
					swapParty = false;
				}
				if (pInput->GetMousePosition().IsWithinRectangle({ 650, 570, 750, 595 }) && (pInput->IsKeyDown(SGD::Key::LButton) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 1))))
				{
					checkStats = false;
					swapParty = false;
				}
				//65, 490, 125, 550	
				//265, 490, 325, 550
				//465, 490, 525, 550
				//665, 490, 725, 550
				for (unsigned int i = 0; i <= player->party.size(); i++)
				{
					if (i == 0)
					{
						if (pInput->GetMousePosition().IsWithinRectangle({ 65, 490, 125, 550 }))
						{
							renderPassiveInfo = true;
							passiveInfo = player->GetCharacter()->GetPassiveDiscription().str();
							return true;
							//display party[i] passive info being passed in by deshon
						}
					}
					if (pInput->GetMousePosition().IsWithinRectangle({ 65 + 200.0f * i, 490, 125 + 200.0f * i, 550 }))
					{
						renderPassiveInfo = true;
						passiveInfo = player->party[i - 1]->getCharacter()->GetPassiveDiscription().str();
						return true;
						//display party[i] passive info being passed in by deshon
					}
					else
						renderPassiveInfo = false;
				}
				return true;
			}

			//press escape to activate the pause menu
			if (!win && !lose)
			{
				if (((pInput->IsKeyPressed(SGD::Key::Escape)) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 6)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 7))) && !pausedSave)
				{
					//When escape is pressed the pause menu should appear
					paused = !paused;
					m_nCursor = 0;
					return true;
				}
			}
			if (checkStats && (pInput->IsKeyPressed(SGD::Key::Escape) || (ARCADEBUILD == 1 && pInput->IsKeyPressed(SGD::Key::RButton)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 2))))
				checkStats = false;
			//If you are on the character select screen
			if (paused == true && chooseCharacterScreen == true)
			{
				unsigned int index = 0;
				for (auto iterator = player->party.begin(); iterator != player->party.end(); ++index)
				{
					if (swapParty && (pInput->IsKeyDown(SGD::Key::Escape) || (ARCADEBUILD == 1 && pInput->IsKeyPressed(SGD::Key::RButton)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 2))))
					{
						swapParty = false;
					}
					if (pInput->GetMousePosition().IsWithinRectangle({ 100.0f + 150.0f * index, 150, 230.0f + 150.0f * index, 280 }) && (pInput->IsKeyPressed(SGD::Key::LButton) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 1))))
					{
						//If you are switching party members
						if (swapParty)
						{
							//make sure the character doesn't continue to follow you around
							(*iterator)->anchor = nullptr;
							//delete the party member from your party
							player->party.erase(iterator);
							(*iterator)->getCharacter()->SetInParty(false);
							//partyWasAltered = true;
							newPartyMember->getCharacter()->SetInParty(true);
							newPartyMember->SetAnchor(GameplayState::GetInstance()->player->GetCharacter());
							GameplayState::GetInstance()->player->party.push_back(newPartyMember);
							GameplayState::GetInstance()->SetPartyWasAltered(false);
							swapParty = false;
							chooseCharacterScreen = false;
							paused = false;
							sinceRecruitment = 0.f;
							return true;
						}
						else
						{
							//If the player has chosen a new character to control and clicks on the continue button, 
							//switch their character with the one selected and remove their previous character from the party and the list of entities
							//Have the character drop his weapon
							player->GetCharacter()->GetWeapon()->SetOwner(nullptr);
							player->GetCharacter()->GetWeapon()->SetPosition(player->GetCharacter()->GetPosition());
							weapons.push_back(player->GetCharacter()->GetWeapon());


							//delete the character from the party and the entity manager
							Character* tempCharacter = player->GetCharacter();
							player->SetCharacter(player->party[index]->character);
							player->party[index]->SetCharacter(tempCharacter);
							m_pEntities->RemoveEntity(player->party[index]->character);
							(*iterator)->getCharacter()->SetInParty(false);
							player->party.erase(iterator);

							for (unsigned int i = 0; i < player->party.size(); i++)
							{
								player->party[i]->SetAnchor(player->GetCharacter());
							}

							chooseCharacterScreen = false;
							paused = false;
						}
						break;
					}
					else
						++iterator;
				}

			}

			//pause menu input
			if (!win && !lose && paused == true && checkStats == false && chooseCharacterScreen == false)
			{
				RenderPauseMenu();
				fromGameplayState = true;

				if (pInput->GetMousePosition().y > 100 && pInput->GetMousePosition().y < 160)
					m_nCursor = 0;
				else if (pInput->GetMousePosition().y > 200 && pInput->GetMousePosition().y < 260)
					m_nCursor = 1;
				else if (pInput->GetMousePosition().y > 300 && pInput->GetMousePosition().y < 360)
					m_nCursor = 2;
				else if (pInput->GetMousePosition().y > 400 && pInput->GetMousePosition().y < 460)
					m_nCursor = 3;
				else if (pInput->GetMousePosition().y > 500 && pInput->GetMousePosition().y < 560)
					m_nCursor = 4;

				if (pInput->IsKeyPressed(SGD::Key::LButton) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 1)))
				{
					if (pInput->GetMousePosition().IsWithinRectangle(resumeRect))
					{
						paused = false;
						fromGameplayState = false;
					}
					else if (pInput->GetMousePosition().IsWithinRectangle(instructRect))
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(InstructionsState::GetInstance());
						return true;
					}
					else if (pInput->GetMousePosition().IsWithinRectangle(optionRect))
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(OptionsState::GetInstance());
						return true;
					}
					else if (pInput->GetMousePosition().IsWithinRectangle(statsRect))
					{
						checkStats = true;
					}
					else if (pInput->GetMousePosition().IsWithinRectangle(exitRect))
					{
						fromGameplayState = false;
						paused = false;
						Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
						return true;
					}
				}
				if ((pInput->GetLeftJoystick(0).y >= .8f && joystick_prev.y < .8f) || pInput->IsKeyPressed(SGD::Key::Down) || pInput->IsDPadPressed(0, SGD::DPad::Down) || pInput->IsKeyPressed(SGD::Key::S))
				{
					m_nCursor++;
					if (m_nCursor > 4)
						m_nCursor = 0;
				}
				if ((pInput->GetLeftJoystick(0).y <= -.8f && joystick_prev.y > -.8f) || pInput->IsKeyPressed(SGD::Key::Up) || pInput->IsDPadPressed(0, SGD::DPad::Up) || pInput->IsKeyPressed(SGD::Key::W))
				{
					m_nCursor--;
					if (m_nCursor < 0)
						m_nCursor = 4;
				}
				if (pInput->IsKeyPressed(SGD::Key::Enter) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || (pInput->IsKeyPressed(SGD::Key::LButton)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 0)))
				{
					if (m_nCursor == 0)
					{
						fromGameplayState = false;
						paused = false;
					}
					if (m_nCursor == 1)
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(InstructionsState::GetInstance());
					}
					if (m_nCursor == 2)
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(OptionsState::GetInstance());
					}
					if (m_nCursor == 3)
					{
						checkStats = true;
					}
					if (m_nCursor == 4)
					{
						fromGameplayState = false;
						MainMenuState::GetInstance()->setPlayMenu(false);
						paused = false;
						Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
					}
				}
			}
		}
		// If we're inside of recruitment dialogue
		else
		{
			//pause menu input
			if (paused == true && checkStats == false && chooseCharacterScreen == false)
			{
				RenderPauseMenu();
				fromGameplayState = true;
				if (pInput->GetMousePosition().y > 100 && pInput->GetMousePosition().y < 160)
					m_nCursor = 0;
				else if (pInput->GetMousePosition().y > 200 && pInput->GetMousePosition().y < 260)
					m_nCursor = 1;
				else if (pInput->GetMousePosition().y > 300 && pInput->GetMousePosition().y < 360)
					m_nCursor = 2;
				else if (pInput->GetMousePosition().y > 400 && pInput->GetMousePosition().y < 460)
					m_nCursor = 3;
				else if (pInput->GetMousePosition().y > 500 && pInput->GetMousePosition().y < 560)
					m_nCursor = 4;

				if (pInput->IsKeyPressed(SGD::Key::LButton) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 0)))
				{
					if (m_nCursor == 0)
					{
						paused = false;
						fromGameplayState = false;
					}
					else if (m_nCursor == 1)
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(InstructionsState::GetInstance());
						return true;
					}
					else if (m_nCursor == 2)
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(OptionsState::GetInstance());
						return true;
					}
					else if (m_nCursor == 3)
					{
						checkStats = true;
					}
					else if (m_nCursor == 4)
					{
						fromGameplayState = false;
						paused = false;
						Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
						return true;
					}
				}
				if ((pInput->GetLeftJoystick(0).y >= .8f && joystick_prev.y < .8f) || pInput->IsDPadPressed(0, SGD::DPad::Down) || pInput->IsKeyPressed(SGD::Key::S))
				{
					m_nCursor++;
					if (m_nCursor > 4)
						m_nCursor = 0;
				}
				if ((pInput->GetLeftJoystick(0).y <= -.8f && joystick_prev.y > -.8f) || pInput->IsKeyPressed(SGD::Key::Up) || pInput->IsKeyPressed(SGD::Key::W))
				{
					m_nCursor--;
					if (m_nCursor < 0)
						m_nCursor = 4;
				}
				if (pInput->IsKeyPressed(SGD::Key::Enter) || (ARCADEBUILD == 1 && pInput->IsButtonPressed(0, 0)) || (pInput->IsKeyPressed(SGD::Key::LButton)) || (ARCADEBUILD == 0 && pInput->IsButtonPressed(0, 1)))
				{
					if (m_nCursor == 0)
					{
						fromGameplayState = false;
						paused = false;
					}
					if (m_nCursor == 1)
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(InstructionsState::GetInstance());
					}
					if (m_nCursor == 2)
					{
						fromGameplayState = true;
						Game::GetInstance()->InteruptState(OptionsState::GetInstance());
					}
					if (m_nCursor == 3)
					{
						checkStats = true;
					}
					if (m_nCursor == 4)
					{
						fromGameplayState = false;
						MainMenuState::GetInstance()->setPlayMenu(false);
						paused = false;
						Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
					}
				}
			}
			else
			{
				Dialogue::DialogueResult thisResult = npcBeingSpokenTo->GetDialogue()->Input();

				if (thisResult == Dialogue::YES)
				{
					// Check to make sure the player has enough reputation to recruit the enemy
					if (npcBeingSpokenTo->GetRepRequirement() <= player->GetReputation())
					{
						// If the party is not yet full
						if (player->party.size() < (unsigned int)3 - Game::GetInstance()->playerDifficulty)
						{
							npcBeingSpokenTo->SetAnchor(player->GetCharacter());
							npcBeingSpokenTo->getCharacter()->SetInParty(true);
							player->party.push_back(npcBeingSpokenTo);
							sinceRecruitment = 0.f;
						}
						// If the party is full
						else
						{
							//ask player if they would like to switch a character from their party for the new character
							SetPaused(true);
							SetCharacterSelect(true);
							SetSwapParty(true);
							newPartyMember = npcBeingSpokenTo;
						}

						inRecruitmentDialogue = false;
					}
					else
					{
						inRecruitmentDialogue = false;
						sinceFailedRecruiting = 0.0f;
					}
				}
				else if (thisResult == Dialogue::NO)
					inRecruitmentDialogue = false;
			}
		}
	}
	joystick_prev = pInput->GetLeftJoystick(0);
	return true;	// keep playing
}

// Render
//	- render the game entities
/*virtual*/ void GameplayState::Render(void)
{
	//tutorial = false;
	//Render gameplay elements
	if (!GameplayState::GetInstance()->IsGamePaused())
	{
		world->Render();

		for (auto i = weapons.begin(); i != weapons.end(); ++i)
		{
			Weapon *wep = dynamic_cast<Weapon*>((*i));
			if (wep && wep->GetType() == Entity::EntityType::ENT_WEAPON)
			{
				if (wep)
					(*i)->Render();
			}
		}

		// Render the entities
		m_pEntities->RenderAll();

		D3DXMATRIX identity;
		D3DXMatrixIdentity(&identity);
		SGD::GraphicsManager::GetInstance()->SetTransform(identity);

		if (player != nullptr && player->GetCharacter()->GetCurrHealth() > 0)
		{
			//check the collisions between the player and the weapons in the weapons list

			CheckWeaponCollision();
		}

		RenderWeaponCompare();
		// Drawing the currently-selected string
		gameplayDialogue->DrawString();
		Game::GetInstance()->GetFont()->DrawStoredString(210, (int)(SCREENHEIGHT)-140, 1.0f);

		if (lose)
			RenderLoseScreen();
		//if win condition has been met, game over, you win! go to credits state.

		else if (objectives->GetCurrentObjectiveNumber() >= objectives->GetNumberOfObjectives())
		{
			RenderWinScreen();
		}
		else
		{
			if (player->GetCharacter()->GetCurrHealth() > 0)
				player->RenderHUD();
			CheckCursorCollisionWithCharacterPortraits();

			// Rendering recruitment dialogue
			if (inRecruitmentDialogue)
				npcBeingSpokenTo->GetDialogue()->DisplayRecruitmentDialogue();

			// Rendering the failed recruitment dialogue
			if (sinceFailedRecruiting < 5.0f && npcBeingSpokenTo != nullptr && !inRecruitmentDialogue)
				npcBeingSpokenTo->GetDialogue()->DisplayLackingRepDialogue();

			// Rendering random chat
			for (unsigned int i = 0; i < npcs.size(); i++)
			{
				npcs[i]->GetDialogue()->Render();
			}

			if (player->currentLocation != "" && player->sinceLocationReached <= 5.0f)
			{
				SGD::GraphicsManager::GetInstance()->DrawRectangle(
					SGD::Rectangle(
					SCREENWIDTH / 2 - 150, 100, SCREENWIDTH / 2 + 150, 150), SGD::Color(120, 0, 0, 0));

				font->Draw(player->currentLocation.c_str(), (int)(SCREENWIDTH) / 2 - ((int)font->MeasureString(player->currentLocation.c_str()) / 2), 110, 1.0f, SGD::Color(255, 255, 255));
			}



			// Drawing all of the objectives 
			if (player->GetCharacter()->GetCurrHealth() > 0)
				objectives->Render();

			// Rendering the compass/////////////////////
			SGD::GraphicsManager::GetInstance()->DrawTexture(compass, SGD::Point(80, SCREENHEIGHT - 135), {}, {}, {}, { 2.f, 2.f });

			// Finding the vector to the current objective, to point the compass in the right direction
			SGD::Vector toObjective =
				SGD::Vector(objectives->GetObjectivesList()[objectives->GetCurrentObjectiveNumber()]->GetLocX() * World::GetInstance()->GetTileLayers()[1]->GetTileSize().width,
				objectives->GetObjectivesList()[objectives->GetCurrentObjectiveNumber()]->GetLocY() * World::GetInstance()->GetTileLayers()[1]->GetTileSize().height)
				- SGD::Vector(player->GetPosition().x, player->GetPosition().y);

			// Default rotation vector
			SGD::Vector orientation = { 0, -1 };

			// Computing the angle
			float compassRotation = orientation.ComputeAngle(toObjective);

			//rotate the character towards the cursor
			if (orientation.ComputeSteering(toObjective) < 0.0f)
				compassRotation = -compassRotation;

			// Rendering the compass arrow
			SGD::GraphicsManager::GetInstance()->DrawTexture(compassArrow, SGD::Point(80 + (19 * 2), SCREENHEIGHT - 135 + (9 * 2)), compassRotation, { 3, 13 }, {}, { 2.f, 2.f });
			///////////////////////////////////////////////


			// If tutorial
			if (tutorial && tutorialChosen)
			{
				//gameplayDialogue->SetStringToDisplay(tutorialDialogue[currentTutorialObjective].c_str());
				SGD::GraphicsManager * gManager = SGD::GraphicsManager::GetInstance();
				gManager->DrawRectangle(SGD::Rectangle(200, SCREENHEIGHT - 150, SCREENWIDTH - 225, SCREENHEIGHT - 50), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);
				font->Draw(tutorialDialogue[currentTutorialObjective].c_str(), 210, (int)(SCREENHEIGHT)-140, 1.0f, SGD::Color(255, 255, 255));
			}
			else if (!tutorialChosen)
			{
				gameplayDialogue->DisplayRecruitmentDialogue();
			}
		}
	}
	else if (checkStats)
	{
		RenderStatsScreen();
	}
	else if (swapParty)
	{
		ShowCharacterSelectScreen();
	}

	if (pausedSave)
		RenderSaveMenu();

	// Georges Passives Tests
#if _DEBUG
	stringstream passiveDamageTaken;
	stringstream baseDamageTaken;

	stringstream healthregen;

	stringstream baseDamageDealt;
	stringstream passiveDamageDealt;

	stringstream passiveKillLifeGain;

	stringstream myChances, theOdds;

	stringstream noFireTimer, passiveTimer;

	stringstream riccochetOdds;

	if (player)
	{
		Brawler* tempBrawler = dynamic_cast<Brawler*>(player->GetCharacter());
		Sniper* tempSniper = dynamic_cast<Sniper*>(player->GetCharacter());
		Gunslinger* tempGunSlinger = dynamic_cast<Gunslinger*>(player->GetCharacter());
		theOdds << nRiccochetOdds;

		SGD::GraphicsManager::GetInstance()->DrawString(theOdds.str().c_str(), { 300, 200 });
	}
#endif

	if (!paused && !lose && !pausedSave)
		m_pEntities->RenderMinimap();

	//Render cooldown effects
	for (unsigned int i = 0; i < HUDentities.size(); i++)
	{
		HUDentities[i]->Render();
	}

	//Fade in from black
	if (fadeInCooldown > 0.0f)
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle(
		{ 0.0f, 0.0f, (float)Game::GetInstance()->GetScreenWidth(), (float)Game::GetInstance()->GetScreenHeight() },
		SGD::Color(255 * (unsigned char)(fadeInCooldown / FADEINCOOLDOWN), 0, 0, 0)
		);
	}
}

void GameplayState::RenderPauseMenu()
{
	D3DXMATRIX identity;

	D3DXMatrixIdentity(&identity);

	GraphicsManager::GetInstance()->SetTransform(identity);

	Game::GetInstance()->GetFont()->Draw("Paused", 350, 50, 1.f, { 255, 255, 255 });
	DrawButton({ 300, 110 }, *font, "Resume");
	DrawButton({ 300, 210 }, *font, "Instructions");
	DrawButton({ 300, 310 }, *font, "Options");
	DrawButton({ 300, 410 }, *font, "Stats");
	DrawButton({ 300, 510 }, *font, "Exit");
	Game::GetInstance()->GetFontTitle()->Draw("I", 270, 100 + 100 * m_nCursor, 0.8f, { (unsigned char)(Game::GetInstance()->opacity * 255), 255, 255, 255 });

	//SGD::GraphicsManager::GetInstance()->DrawString("--->", { 270, 120.0f + 100 * m_nCursor }, { 124, 255, 14 });
}

SGD::HTexture GameplayState::GetCharClassPortrait(string charClass)
{
	if (charClass == "Gunslinger") return gunslingerPortrait;
	if (charClass == "Sniper") return sniperPortrait;
	if (charClass == "Medic") return medicPortrait;
	if (charClass == "Brawler") return brawlerPortrait;
	if (charClass == "Cyborg") return cyborgPortrait;
	else return SGD::INVALID_HANDLE;
}

SGD::HTexture GameplayState::GetCharClassPortrait(int charType)
{
	switch (charType)
	{
	case Entity::ENT_BRAWLER:
		return brawlerPortrait;
	case Entity::ENT_CYBORG:
		return cyborgPortrait;
	case Entity::ENT_GUNSLINGER:
		return gunslingerPortrait;
	case Entity::ENT_SNIPER:
		return sniperPortrait;
	case Entity::ENT_MEDIC:
		return medicPortrait;
	default:
		return SGD::INVALID_HANDLE;
	}
}

SGD::HTexture GameplayState::GetCharActive(int charType)
{
	switch (charType)
	{
	case Entity::ENT_BRAWLER:
		return brawlerActivePic;
	case Entity::ENT_CYBORG:
		return cyborgActivePic;
	case Entity::ENT_GUNSLINGER:
		return gunslingerActivePic;
	case Entity::ENT_SNIPER:
		return sniperActivePic;
	case Entity::ENT_MEDIC:
		return medicActivePic;
	default:
		return SGD::INVALID_HANDLE;
	}
}

void GameplayState::EnterSaveScreen()
{
	//Populate list of files
	//Go through Saves folder
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	saveCurrObjective.clear();
	saveFiles.clear();
	saveCharClass.clear();
	saveDifficulty.clear();
	saveLevel.clear();
	saveTime.clear();

	SGD::InputManager::GetInstance()->Update();

	int totalSaves = 0;

	alphaentry = false;
	alphaEntrySelected = 0;

	//Get save folder path.  Make directory if does not exist
	wchar_t *appDataPath;
	SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &appDataPath);
	wstring saveFile = appDataPath;
	saveFile.append(L"\\Desolate\\");
	if (!PathFileExists(saveFile.c_str())) CreateDirectory(saveFile.c_str(), NULL);
	char appPath[200];
	wcstombs_s(nullptr, appPath, saveFile.c_str(), 200);
	saveFile.append(L"*");

	hFind = FindFirstFile(saveFile.c_str(), &FindFileData);
	do {
		// ignore directories
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			wstring name = FindFileData.cFileName;
			if (name[name.find_last_of(L".") + 1] == 'x') //Is it an xml?
			{
				char newName[200];
				wcstombs_s(nullptr, newName, name.c_str(), 200);

				totalSaves++;

				string truncName = newName;
				truncName.erase(truncName.find_first_of("."));

				//Load the save file
				saveFiles.push_back(truncName);

				string newPath = appPath;
				newPath.append(newName);

				//Get current objective
				TiXmlDocument doc(newPath.c_str());
				doc.LoadFile();
				TiXmlElement *xRoot = doc.FirstChildElement();
				saveCurrObjective.push_back(xRoot->Attribute("currentObjective"));

				//Get the class
				TiXmlElement *xSpawns = xRoot->FirstChildElement("Spawns");
				TiXmlElement *xSpawn = xSpawns->FirstChildElement("Spawn");
				for (; strcmp(xSpawn->Attribute("SpawnType"), "Player") != 0; xSpawn = xSpawn->NextSiblingElement("Spawn")); //iterate xSpawn until Player tag
				saveCharClass.push_back(xSpawn->Attribute("Class"));

				//Get other info
				string tempLevel = "Level: ";
				tempLevel.append(xSpawn->Attribute("Level"));
				saveLevel.push_back(tempLevel);

				int diff;
				xSpawn->Attribute("Difficulty", &diff);
				string difficuty;
				if (diff == 0)
					difficuty = "Easy";
				else if (diff == 1)
					difficuty = "Medium";
				else if (diff == 2)
					difficuty = "Hard";
				saveDifficulty.push_back(difficuty);

				doc.Clear();

				//retreive last write of file
				FILETIME lastWrite;
				HANDLE myfile = CreateFileA(newPath.c_str(), 0x00, 0x00, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
				GetFileTime(myfile, NULL, NULL, &lastWrite);
				SYSTEMTIME lastWriteSys;
				FileTimeToSystemTime(&lastWrite, &lastWriteSys);

				//Format it
				char buffer[200];
				GetDateFormatA(LOCALE_CUSTOM_DEFAULT, DATE_SHORTDATE, &lastWriteSys, NULL, buffer, 200);
				string tempDateTime = buffer;
				tempDateTime.append(" ");
				GetTimeFormatA(LOCALE_CUSTOM_DEFAULT, TIME_NOSECONDS, &lastWriteSys, NULL, buffer, 200);
				tempDateTime.append(buffer);

				saveTime.push_back(tempDateTime);
			}
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	//Add new to the list
	saveFiles.push_back("New Save");
	saveCharClass.push_back("");
	saveCurrObjective.push_back("");
	saveDifficulty.push_back("");
	saveTime.push_back("");
	saveLevel.push_back("");
}

void GameplayState::RenderSaveMenu()
{
	GraphicsManager *graphics = GraphicsManager::GetInstance();
	int winWidth = Game::GetInstance()->GetScreenWidth();
	int winHeight = Game::GetInstance()->GetScreenHeight();
	InputManager *pInput = InputManager::GetInstance();
	//Input
	if (Game::GetInstance()->IsMenuBackInput())
	{
		pausedSave = false;
	}
	if (!pausedSaveNew)
	{
		graphics->DrawTextureSection(next, { 650, 450 }, { 20, 10, 100, 115 });
		graphics->DrawTextureSection(previous, { 50, 450 }, { 20, 10, 100, 115 });
		if (selectedSave > 0 && pInput->IsKeyPressed(SGD::Key::LButton) && pInput->GetMousePosition().IsWithinRectangle({ 0, 0, 200, WINDOW_HEIGHT }))
			selectedSave--;
		else if (selectedSave < (int)saveFiles.size() - 1 && pInput->IsKeyPressed(SGD::Key::LButton) && pInput->GetMousePosition().IsWithinRectangle({ 600, 0, WINDOW_WIDTH, WINDOW_HEIGHT }))
			selectedSave++;
		else if (Game::GetInstance()->IsMenuLeftInput() && selectedSave > 0) selectedSave--;
		else if (Game::GetInstance()->IsMenuRightInput() && selectedSave < (int)saveFiles.size() - 1) selectedSave++;
		else if (Game::GetInstance()->IsMenuConfirmInput() || pInput->GetMousePosition().IsWithinRectangle(SGD::Rectangle({ winWidth / 4.0f, winHeight / 4.0f, (winWidth / 4.0f) * 3, (winHeight / 4.0f) * 3 })) && pInput->IsKeyPressed(SGD::Key::LButton))
		{
			if (saveFiles[selectedSave] != "New Save")
			{
				//Get save folder path.  Make directory if does not exist
				wchar_t *appDataPath;
				SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &appDataPath);
				wstring path = appDataPath;
				path.append(L"\\Desolate\\");
				if (!PathFileExists(path.c_str())) CreateDirectory(path.c_str(), NULL);

				//Add filename
				wchar_t buffer[100];
				mbstowcs_s(nullptr, buffer, saveFiles[selectedSave].c_str(), 200);
				path.append(buffer);
				path.append(L".xml");

				// Loading the world
				char pathbuffer[100];
				wcstombs_s(nullptr, pathbuffer, path.c_str(), 200);
				SaveProgress(pathbuffer);

				pausedSave = false;
			}
			else
			{
				pausedSaveInput.str("");
				pausedSaveNew = true;
			}
		}
	}
	else
	{
		//Capture keypresses
		wchar_t letter = pInput->GetAnyCharPressed();
		if (letter != 0 && IsCharAlphaNumeric(letter))
			if (pausedSaveInput.str().length() < 16)
				pausedSaveInput << (char)letter;

		// Handling backspace
		if (InputManager::GetInstance()->IsKeyPressed(Key::Backspace))
		{
			string usedToErase = pausedSaveInput.str();

			int usedToEraseLength = usedToErase.length();
			if ((usedToEraseLength - 1) > -1)
				usedToErase.erase(usedToErase.length() - 1);

			pausedSaveInput.str("");
			pausedSaveInput << usedToErase;
		}

		//Do on screen text entry stuff
		if (ARCADEBUILD)
		{
			if (Game::GetInstance()->IsMenuRightInput()) alphaEntrySelected++;
			else if (Game::GetInstance()->IsMenuLeftInput()) alphaEntrySelected--;
			else if (Game::GetInstance()->IsMenuConfirmInput() && pausedSaveInput.str().length() < 16 && alphaentry) pausedSaveInput << char(alphaEntrySelected + 65);

			if (alphaEntrySelected < 0) alphaEntrySelected = 25;
			else if (alphaEntrySelected > 25) alphaEntrySelected = 0;

			if (Game::GetInstance()->IsMenuDownInput() || Game::GetInstance()->IsMenuUpInput()) alphaentry = !alphaentry;
		}

		if (Game::GetInstance()->IsMenuConfirmInput() && (!alphaentry || !ARCADEBUILD))
		{
			pausedSave = false;
			pausedSaveNew = false;

			//Get save folder path.  Make directory if does not exist
			wchar_t *appDataPath;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &appDataPath);
			wstring path = appDataPath;
			path.append(L"\\Desolate\\");
			if (!PathFileExists(path.c_str())) CreateDirectory(path.c_str(), NULL);

			//Add filename
			wchar_t buffer[100];
			mbstowcs_s(nullptr, buffer, pausedSaveInput.str().c_str(), 200);
			path.append(buffer);
			path.append(L".xml");

			// Loading the world
			char pathbuffer[100];
			wcstombs_s(nullptr, pathbuffer, path.c_str(), 200);
			SaveProgress(pathbuffer);
		}
	}
	
	//Render background and text
	graphics->DrawRectangle({ 0.0f, 0.0f, (float)winWidth, (float)winHeight }, SGD::Color(100, 0, 0, 0));
	graphics->DrawString("Press \"Esc\" to exit", { 50, 50 });
	graphics->DrawString("Save Game", { winWidth / 2.0f - 70, winHeight / 8.0f });


	graphics->DrawString("Press \"Enter\" to select.", { winWidth / 2.0f - 120, (winHeight / 8.0f) * 7 });
	graphics->DrawString("\n Use arrows to go to prev / next", { winWidth / 2.0f - 170, (winHeight / 8.0f) * 7 });

	graphics->DrawTextureSection(next, { 650, 450 }, { 20, 10, 100, 115 });
	graphics->DrawTextureSection(previous, { 50, 450 }, { 20, 10, 100, 115 });

	//prev
	if (selectedSave - 1 >= 0 && selectedSave - 1 < (int)saveFiles.size())
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle(
		{ winWidth / 50.0f, (winHeight / 6.0f) * 2, (winWidth / 10.0f) * 3.5f, (winHeight / 6.0f) * 4 },
		SGD::Color::White,
		SGD::Color::Black
		);

		SGD::GraphicsManager::GetInstance()->DrawTexture(
			GetCharClassPortrait(saveCharClass[selectedSave - 1]),
			{ winWidth / 50.0f + 20, (winHeight / 6.0f) * 2.43f }
		);

		SGD::GraphicsManager::GetInstance()->DrawString(
			saveFiles[selectedSave - 1].c_str(),
			{ winWidth / 50.0f + 20, (winHeight / 6.0f) * 2.15f },
			SGD::Color::Black
			);

		SGD::GraphicsManager::GetInstance()->DrawString(
			saveCurrObjective[selectedSave - 1].c_str(),
			{ winWidth / 50.0f + 20, (winHeight / 6.0f) * 3.65f },
			SGD::Color::Black
			);
	}

	//next
	if (selectedSave + 1 >= 0 && selectedSave + 1 < (int)saveFiles.size())
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle(
		{ (winWidth / 10.0f) * 7.5f, (winHeight / 6.0f) * 2, winWidth - (winWidth / 50.0f), (winHeight / 6.0f) * 4 },
		SGD::Color::White,
		SGD::Color::Black
		);

		SGD::GraphicsManager::GetInstance()->DrawTexture(
			GetCharClassPortrait(saveCharClass[selectedSave + 1]),
			{ (winWidth / 10.0f) * 7.0f, (winHeight / 6.0f) * 2.43f }
		);

		SGD::GraphicsManager::GetInstance()->DrawString(
			saveFiles[selectedSave + 1].c_str(),
			{ (winWidth / 10.0f) * 7.0f, (winHeight / 6.0f) * 2.15f },
			SGD::Color::Black
			);

		SGD::GraphicsManager::GetInstance()->DrawString(
			saveCurrObjective[selectedSave + 1].c_str(),
			{ (winWidth / 10.0f) * 7.0f, (winHeight / 6.0f) * 3.65f },
			SGD::Color::Black
			);
	}

	//curr
	if (selectedSave >= 0 && selectedSave < (int)saveFiles.size())
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle(
		{ winWidth / 4.0f, winHeight / 4.0f, (winWidth / 4.0f) * 3, (winHeight / 4.0f) * 3 },
		SGD::Color::White,
		SGD::Color::Blue
		);

		SGD::GraphicsManager::GetInstance()->DrawTexture(
			GetCharClassPortrait(saveCharClass[selectedSave]),
			{ winWidth / 4.0f + 20, (winHeight / 6.0f) * 2.43f }
		);

		//File Name
		SGD::GraphicsManager::GetInstance()->DrawString(
			saveFiles[selectedSave].c_str(),
			{ winWidth / 4.0f + 20, (winHeight / 6.0f) * 1.65f },
			SGD::Color::Black
			);

		//Current objective
		SGD::GraphicsManager::GetInstance()->DrawString(
			saveCurrObjective[selectedSave].c_str(),
			{ winWidth / 4.0f + 20, (winHeight / 6.0f) * 4.15f },
			SGD::Color::Black
			);

		//Last modified
		SGD::GraphicsManager::GetInstance()->DrawString(
			saveTime[selectedSave].c_str(),
			{ winWidth / 4.0f + 20, (winHeight / 6.0f) * 2.0f },
			SGD::Color::Black
			);

		//Level
		SGD::GraphicsManager::GetInstance()->DrawString(
			saveLevel[selectedSave].c_str(),
			{ winWidth / 4.0f + 20, (winHeight / 6.0f) * 3.85f },
			SGD::Color::Black
			);

		//Difficulty
		SGD::GraphicsManager::GetInstance()->DrawString(
			saveDifficulty[selectedSave].c_str(),
			{ winWidth / 4.0f + 300, (winHeight / 6.0f) * 1.65f },
			SGD::Color::Black
			);
	}

	if (pausedSaveNew)
	{
		graphics->DrawRectangle({ winWidth / 4.0f + 125, (winHeight / 6.0f) * 1.65f, (winWidth / 6.0f) * 4 + 50, (winHeight / 6.0f) * 1.65f + 25 }, Color::White, Color::Black);
		graphics->DrawString(pausedSaveInput.str().c_str(), { winWidth / 4.0f + 125, (winHeight / 6.0f) * 1.65f }, Color::Black);

		char letterIn[2] = { '\0', '\0' };
		letterIn[0] = alphaEntrySelected + 65;

		if (ARCADEBUILD)
		{
			if (alphaentry)
			{
				graphics->DrawRectangle(
				{ Game::GetInstance()->GetScreenWidth() / 2.0f + 10, Game::GetInstance()->GetScreenHeight() / 4.0f + 60, Game::GetInstance()->GetScreenWidth() / 2.0f + 43, Game::GetInstance()->GetScreenHeight() / 4.0f + 107 },
				Color::White,
				SGD::Color(int(Game::GetInstance()->opacity * 255), 0, 0, 0));
			}
			else
			{
				graphics->DrawRectangle(
				{ Game::GetInstance()->GetScreenWidth() / 2.0f + 10, Game::GetInstance()->GetScreenHeight() / 4.0f + 120, Game::GetInstance()->GetScreenWidth() / 2.0f + 43, Game::GetInstance()->GetScreenHeight() / 4.0f + 145 },
				Color::White,
				SGD::Color(int(Game::GetInstance()->opacity * 255), 0, 0, 0));
			}

			Game::GetInstance()->GetFontTitle()->Draw(letterIn, Game::GetInstance()->GetScreenWidth() / 2 + 15, Game::GetInstance()->GetScreenHeight() / 4 + 50, 1.0f, SGD::Color::Black);
			Game::GetInstance()->GetFont()->Draw("Done", Game::GetInstance()->GetScreenWidth() / 2 + 12, Game::GetInstance()->GetScreenHeight() / 4 + 120, 1.0f, SGD::Color::Black);
		}
	}
}

void GameplayState::RenderStatsScreen()
{
	if (!renderPassiveInfo)
	{
		Game::GetInstance()->GetFont()->Draw("Player 1:", 30, 30, 1.0f, { 255, 255, 255 });
		SGD::GraphicsManager::GetInstance()->DrawLine({ 200, 0 }, { 200, 565 }, { 255, 255, 255 }, 2);
		Game::GetInstance()->GetFont()->Draw("CPU 1:", 230, 30, 1.0f, { 255, 255, 255 });
		SGD::GraphicsManager::GetInstance()->DrawLine({ 400, 0 }, { 400, 565 }, { 255, 255, 255 }, 2);
		Game::GetInstance()->GetFont()->Draw("CPU 2:", 430, 30, 1.0f, { 255, 255, 255 });
		SGD::GraphicsManager::GetInstance()->DrawLine({ 600, 0 }, { 600, 565 }, { 255, 255, 255 }, 2);
		Game::GetInstance()->GetFont()->Draw("CPU 3:", 630, 30, 1.0f, { 255, 255, 255 });
		SGD::GraphicsManager::GetInstance()->DrawLine({ 0, 60 }, { 800, 60 }, { 255, 255, 255 }, 2);
		SGD::GraphicsManager::GetInstance()->DrawLine({ 0, 565 }, { 800, 565 }, { 255, 255, 255 }, 2);
		SGD::GraphicsManager::GetInstance()->DrawLine({ 630, 565 }, { 600, 600 }, { 255, 255, 255 }, 2);
		SGD::GraphicsManager::GetInstance()->DrawTexture(GetCharActive(player->GetCharacter()->GetType()), { 65, 490 }, 0.0f, {}, {}, { .26f, .26f });
		//SGD::GraphicsManager::GetInstance()->DrawRectangle({ 75, 470, 135, 530 }, { 255, 200, 200, 200 }, { 255, 255, 255 }, 0);
		for (unsigned int i = 0; i < player->party.size(); i++)
		{
			SGD::GraphicsManager::GetInstance()->DrawTexture(GetCharActive(player->party[i]->getCharacter()->GetType()), { 265.f + 200 * i, 490.f }, 0.0f, {}, {}, { .26f, .26f });
		}
		//SGD::GraphicsManager::GetInstance()->DrawRectangle({ 275, 470, 335, 530 }, { 255, 200, 200, 200 }, { 255, 255, 255 }, 0);
		//SGD::GraphicsManager::GetInstance()->DrawRectangle({ 475, 470, 535, 530 }, { 255, 200, 200, 200 }, { 255, 255, 255 }, 0);
		//SGD::GraphicsManager::GetInstance()->DrawRectangle({ 675, 470, 735, 530 }, { 255, 200, 200, 200 }, { 255, 255, 255 }, 0);

		//show stats for your character
		stringstream classInfo;

		string characterType;
		if (player->GetCharacter()->GetClass() == ClassType::Gunslinger)
			characterType = "Gunslinger:";
		else if (player->GetCharacter()->GetClass() == ClassType::Sniper)
			characterType = "Sniper:";
		else if (player->GetCharacter()->GetClass() == ClassType::Brawler)
			characterType = "Brawler:";
		else if (player->GetCharacter()->GetClass() == ClassType::Medic)
			characterType = "Medic:";
		else if (player->GetCharacter()->GetClass() == ClassType::Cyborg)
			characterType = "Cyborg:";

		unsigned int lvl = player->GetCharacter()->GetLevel();


		int health = (int)player->GetCharacter()->GetStat(StatType::health);
		int currHealth = (int)player->GetCharacter()->GetCurrHealth();
		float accuracy = player->GetCharacter()->GetStat(StatType::accuracy);
		float defense = player->GetCharacter()->GetStat(StatType::defense);
		float strength = player->GetCharacter()->GetStat(StatType::strength);
		float dexterity = player->GetCharacter()->GetStat(StatType::dexterity);
		char* weaponPrefix = player->GetCharacter()->GetWeapon()->GetPrefixName();
		char* weaponName = player->GetCharacter()->GetWeapon()->GetName();

		float weaponDamage = player->GetCharacter()->GetWeapon()->GetDamage();
		float weaponRateOfFire = player->GetCharacter()->GetWeapon()->GetRateOfFire();

		int reputation = player->GetReputation();
		classInfo << characterType << "\n\nLevel: " << lvl << "\nHealth: " << currHealth << "/" << health << "\nAccuracy: " << accuracy << "\nDefense: " << defense <<

			"\nStrength: " << strength << "\nDexterity: " << dexterity << "\n\nWeapon Info: \n" << weaponPrefix << '\n' << weaponName << "\n\nDamage: " << (int)weaponDamage << "\nRate of Fire: " << weaponRateOfFire <<
			"\nReputation: " << reputation;

		Game::GetInstance()->GetFont()->Draw(classInfo.str().c_str(), 20, 60, 1.0f, { 255, 255, 255 });
		classInfo.clear();

		//do the same for your party
		for (unsigned int i = 0; i < player->party.size(); i++)
		{
			stringstream otherClassInfo;

			string characterType;
			if (player->party[i]->getCharacter()->GetClass() == ClassType::Gunslinger)
				characterType = "Gunslinger:";
			else if (player->party[i]->getCharacter()->GetClass() == ClassType::Sniper)
				characterType = "Sniper:";
			else if (player->party[i]->getCharacter()->GetClass() == ClassType::Brawler)
				characterType = "Brawler:";
			else if (player->party[i]->getCharacter()->GetClass() == ClassType::Medic)
				characterType = "Medic:";
			else if (player->party[i]->getCharacter()->GetClass() == ClassType::Cyborg)
				characterType = "Cyborg:";

			unsigned int lvl = player->party[i]->getCharacter()->GetLevel();


			int health = (int)player->party[i]->character->GetHealth();
			int currHealth = (int)player->party[i]->character->GetCurrHealth();
			float accuracy = player->party[i]->character->GetStat(StatType::accuracy);
			float defense = player->party[i]->character->GetStat(StatType::defense);
			float strength = player->party[i]->character->GetStat(StatType::strength);
			float dexterity = player->party[i]->character->GetStat(StatType::dexterity);
			char* weaponPrefix = player->party[i]->character->GetWeapon()->GetPrefixName();
			char* weaponName = player->party[i]->character->GetWeapon()->GetName();
			float weaponRateOfFire = player->party[i]->character->GetWeapon()->GetRateOfFire();
			float weaponDamage = player->party[i]->character->GetWeapon()->GetDamage();


			otherClassInfo << characterType << "\n\nLevel: " << lvl << "\nHealth: " << currHealth << "/" << health << "\nAccuracy: " << accuracy << "\nDefense: " << defense <<

				"\nStrength: " << strength << "\nDexterity: " << dexterity << "\n\nWeapon: \n" << weaponPrefix << '\n' << weaponName << "\n\nDamage: " << (int)weaponDamage << "\nRate of Fire: " << weaponRateOfFire;


			Game::GetInstance()->GetFont()->Draw(otherClassInfo.str().c_str(), 220 + 200 * i, 60, 1.0f, { 255, 255, 255 });
			otherClassInfo.clear();
		}
		Game::GetInstance()->GetFont()->Draw("*Hover over the icons to see a character's passive ability", 20, 570, 1.0f, { 255, 255, 255 });

		DrawButton({ 650, 570 }, *font, "Back");
	}
	if (renderPassiveInfo)
	{
		//render the string with the passive info infront of a rect
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 50, 0, SCREENWIDTH - 50, SCREENHEIGHT - 50 }, { 220, 0, 0, 0 }, { 100, 200, 50, 10 }, 3);
		Game::GetInstance()->GetFont()->Draw("Passive Ability", 340, 35, 1.f, { 255, 255, 255 });

		Game::GetInstance()->GetFont()->Draw(passiveInfo.c_str(), 65, 105, 1.f, { 255, 255, 255 });
	}
}

//Render a simple screen that says "you lose" and gives you a prompt that allows you to retry from your last save or return to the main menu
void GameplayState::RenderLoseScreen()
{
	if (timer == 0)
	{
		int randSound = rand() % 10;
		if (randSound > 5)
			SGD::AudioManager::GetInstance()->PlayAudio(loss_sound_1);
		else
			SGD::AudioManager::GetInstance()->PlayAudio(loss_sound_2);
	}

	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 0, 0, SCREENWIDTH, SCREENHEIGHT }, { 180, 0, 0, 0 });

	Game::GetInstance()->GetFont()->Draw("You Lose", 350, 50, 3.0f, { 255, 255, 255 });

	if (timer > 3)
	{
		Game::GetInstance()->GetFontTitle()->Draw("I", 340, 240 + 100 * m_nCursor, 0.8f, { (unsigned char)(Game::GetInstance()->opacity * 255), 255, 255, 255 });

		DrawButton({ 350, 250 }, *font, "Load Game");
		DrawButton({ 350, 350 }, *font, "Main Menu");
	}
}

void GameplayState::RenderWinScreen()
{
	if (timer == 0)
	{
		int randSound = rand() % 10;
		if (randSound > 5)
			SGD::AudioManager::GetInstance()->PlayAudio(win_sound_1);
		else
			SGD::AudioManager::GetInstance()->PlayAudio(win_sound_2);
	}
	win = true;
	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 0, 0, SCREENWIDTH, SCREENHEIGHT }, { 180, 0, 0, 0 });

	Game::GetInstance()->GetFont()->Draw("You Win!", 350, 50, 1.f, { 255, 255, 255 });

	if (timer > 3)
		SGD::GraphicsManager::GetInstance()->DrawString("Press any Button to Continue...", { 150, 300 }, { 255, 255, 255 });
}

void GameplayState::ShowCharacterSelectScreen()
{
	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 0, 0, SCREENWIDTH, SCREENHEIGHT }, { 180, 0, 0, 0 });
	//stringstream mouseposx;
	//mouseposx << SGD::InputManager::GetInstance()->GetMousePosition().x << "    " << SGD::InputManager::GetInstance()->GetMousePosition().y;
	//SGD::GraphicsManager::GetInstance()->DrawString(mouseposx.str().c_str(), { 10, 50 }, { 255, 255, 255, 255 });

	if (swapParty)
	{
		Game::GetInstance()->GetFont()->Draw("Press Esc to leave without switching party members", 200, 350, 1.f, { 255, 255, 255 });
		Game::GetInstance()->GetFont()->Draw("Choose which of your party memebers you would like to switch.", 100, 50, 1.f, { 255, 255, 255 });
	}
	else
		Game::GetInstance()->GetFont()->Draw("Your character died.\nChoose which character in your party you want to control.", 200, 50, 1.f, { 255, 255, 255 });

	for (unsigned int i = 0; i < player->party.size(); ++i)
	{
		if (player->party[i]->character->GetClass() == ClassType::Gunslinger)
		{
			SGD::GraphicsManager::GetInstance()->DrawTexture(gunslingerPortrait, { 100.0f + 150 * i, 150 });
		}
		else if (player->party[i]->character->GetClass() == ClassType::Sniper)
		{

			SGD::GraphicsManager::GetInstance()->DrawTexture(sniperPortrait, { 100.0f + 150 * i, 150 });
		}
		else if (player->party[i]->character->GetClass() == ClassType::Brawler)
		{

			SGD::GraphicsManager::GetInstance()->DrawTexture(brawlerPortrait, { 100.0f + 150 * i, 150 });
		}
		else if (player->party[i]->character->GetClass() == ClassType::Medic)
		{

			SGD::GraphicsManager::GetInstance()->DrawTexture(medicPortrait, { 100.0f + 150 * i, 150 });
		}
		else if (player->party[i]->character->GetClass() == ClassType::Cyborg)
		{

			SGD::GraphicsManager::GetInstance()->DrawTexture(cyborgPortrait, { 100.0f + 150 * i, 150 });
		}
	}
}

/**************************************************************/
// Update
//	- update game entities
/*virtual*/ void GameplayState::Update(float elapsedTime)
{
	if (lose || win)
		timer += elapsedTime;
	if (player != nullptr && player->GetCharacter()->GetCurrHealth() <= 0)
	{
		//if there are no remaining party members
		if (player->party.size() == 0)
		{
			m_pEntities->RemoveEntity(player->GetCharacter());
			lose = true;
			return;
		}
		//Give player choice of which party member to switch with if there is more than 1 other in his party
		else if (player->party.size() >= 1)
		{
			paused = true;
			chooseCharacterScreen = true;
			ShowCharacterSelectScreen();
		}
	}

	sinceFailedRecruiting += elapsedTime;
	sinceRecruitment += elapsedTime;

	if (tutorial)
		sinceLastTutorial += elapsedTime;

	if (sinceLastTutorial > 10.0f)
	{
		currentTutorialObjective++;

		if (currentTutorialObjective > signed int(tutorialDialogue.size()))
			tutorial = false;

		sinceLastTutorial = 0.0f;
	}

	if (fadeInCooldown > 0.0f) fadeInCooldown -= elapsedTime;

	//Cooldown finished particle effects
	if (player && player->GetCharacter() && false)
	{
		float currCooldown = player->GetCharacter()->GetCurrCoolDown();
		float lastCooldown = player->GetCharacter()->GetLastCoolDown();

		if (lastCooldown > 0.0f && currCooldown != lastCooldown && currCooldown <= 0.0f)
		{
			//No longer cooling down
			Emitter *newEmitter = CreateEmitter("skillCooldownFinished", false);
			HUDentities.push_back(newEmitter);
		}
	}
	for (unsigned int i = 0; i < HUDentities.size();)
	{
		HUDentities[i]->Update(elapsedTime);

		if (HUDentities[i]->sinceEmitterSpawn > HUDentities[i]->particleLifeTime)
		{
			delete HUDentities[i];
			HUDentities.erase(HUDentities.begin() + i);
			continue;
		}
		i++;
	}

	// update the worlds camera
	world->GetCamera()->Update(elapsedTime);

	if (!inRecruitmentDialogue)
	{
		//if the player is killed


		if (!paused && !pausedSave)
		{
			//Update NPCs
			unsigned int i = 0;

			//for (auto i = weapons.begin(); i != weapons.end(); ++i)
			//{
			//	Weapon *wep = dynamic_cast<Weapon*>((*i));
			//	if (wep && wep->GetType() == Entity::EntityType::ENT_WEAPON)
			//	{
			//		if (wep)
			//			(*i)->Update(elapsedTime);
			//	}
			//}

			for (; i < npcs.size(); i++)
			{
				if (!npcs[i]->getCharacter()->GetIsEnemy()) // Enemies now handled in spawnlist
					npcs[i]->Update(elapsedTime);
			}

			// Update the entities
			m_pEntities->UpdateAll(elapsedTime*timeStep);

			// update the player
			if (player != nullptr)
			{
				player->Update(elapsedTime);
			}

			// Process the events & messages
			m_pEvents->Update();
			m_pMessages->Update();

			// Update objectives
			objectives->Update(elapsedTime);

			// Update each character's dialogue
			for (unsigned int i = 0; i < npcs.size(); i++)
			{
				npcs[i]->GetDialogue()->Update(elapsedTime);
			}


			// Update spawn system
			SpawnList::GetInstance()->Update(elapsedTime);
		}

		// check collisions between characters and characters
		collidingWithAlly = false;
		m_pEntities->CheckCollisions(CHARACTERS, CHARACTERS);
		CheckIfCharacterIsCloseEnoughToAnotherCharacterToBeConsideredACollision();
		// check collisions between characters and pickups or hazards
		m_pEntities->CheckCollisions(CHARACTERS, PICKUPSandHAZARDS);
		// check collisions between buckets characters to bullets
		m_pEntities->CheckBulletCollisions(CHARACTERS, BULLETS);
		// check collisions between buckets skills to characters
		m_pEntities->CheckCollisions(CHARACTERS, SKILLS);
	}
}

void GameplayState::CheckIfCharacterIsCloseEnoughToAnotherCharacterToBeConsideredACollision()
{
	if (player)
	{
		// loop through vector of ALL NPCs
		for (int i = 0; i < (int)npcs.size(); i++)
		{

			// if NPC is not NULL && 
			if (npcs[i]->getCharacter())
			{
				//don't call this if its a friendly, since we can move through friendly characters we would end up calling handle collision twice as much
				//for (unsigned int partyindex = 0; partyindex < player->party.size(); partyindex++)
				//{
				if (npcs[i]->getCharacter()->GetInParty())
					break;
				// check to see if rects are intersecting
				SGD::Rectangle largerCharacterRect = npcs[i]->getCharacter()->GetRect();
				largerCharacterRect.left -= 20;
				largerCharacterRect.top -= 20;
				largerCharacterRect.right += 20;
				largerCharacterRect.bottom += 20;
				if (player->GetCharacter()->GetRect().IsIntersecting(largerCharacterRect))
				{
					// consider it a collision even though they aren't on top of eachother
					player->GetCharacter()->HandleCollision(npcs[i]->getCharacter());
				}
				//}

			}
		}
	}
}

void GameplayState::SetWeaponCompare(Weapon* pWep)
{
	//show comparison of weapon stats
	if (!alreadySetWeaponInfo)
	{
		alreadySetWeaponInfo = true;
		comparingWeapons = true;
		if (pWep->GetGunType() == Weapon::GunType::Shotgun)
			weaponCompare << "Damage: " << (int)pWep->GetCompareDamage() << " x " << ((Shotgun*)(pWep))->GetNumPellets() << "\nRate of Fire: " << pWep->GetRateOfFire();
		else
			weaponCompare << "Damage: " << (int)pWep->GetCompareDamage() << "\nRate of Fire: " << pWep->GetRateOfFire();

		if (player->GetCharacter()->GetWeapon()->GetGunType() == Weapon::GunType::Shotgun && pWep->GetGunType() == Weapon::GunType::Shotgun)
		{
			int flargleschmidtertz = (int)pWep->GetCompareDamage()  * ((Shotgun*)pWep)->GetNumPellets();
			int clertzflaymingo = (int)player->GetCharacter()->GetWeapon()->GetCompareDamage() * ((Shotgun*)player->GetCharacter()->GetWeapon())->GetNumPellets();
			if ((int)pWep->GetCompareDamage()  * ((Shotgun*)pWep)->GetNumPellets() == (int)player->GetCharacter()->GetWeapon()->GetCompareDamage() * ((Shotgun*)player->GetCharacter()->GetWeapon())->GetNumPellets())
				damageCompare = 0;
			else if ((int)pWep->GetCompareDamage()  * ((Shotgun*)pWep)->GetNumPellets() > (int)player->GetCharacter()->GetWeapon()->GetCompareDamage() * ((Shotgun*)player->GetCharacter()->GetWeapon())->GetNumPellets())
				damageCompare = 1;
			else
				damageCompare = -1;
		}
		else if (pWep->GetGunType() == Weapon::GunType::Shotgun)
		{
			if ((int)pWep->GetCompareDamage() * ((Shotgun*)pWep)->GetNumPellets() == (int)player->GetCharacter()->GetWeapon()->GetCompareDamage())
				damageCompare = 0;
			else if ((int)pWep->GetCompareDamage() * ((Shotgun*)pWep)->GetNumPellets() > (int)player->GetCharacter()->GetWeapon()->GetCompareDamage())
				damageCompare = 1;
			else
				damageCompare = -1;
		}
		else if (player->GetCharacter()->GetWeapon()->GetGunType() == Weapon::GunType::Shotgun)
		{
			if ((int)pWep->GetCompareDamage() == (int)player->GetCharacter()->GetWeapon()->GetCompareDamage() * ((Shotgun*)player->GetCharacter()->GetWeapon())->GetNumPellets())
				damageCompare = 0;
			else if ((int)pWep->GetCompareDamage() > (int)player->GetCharacter()->GetWeapon()->GetCompareDamage() * ((Shotgun*)player->GetCharacter()->GetWeapon())->GetNumPellets())
				damageCompare = 1;
			else
				damageCompare = -1;
		}
		else
		{
			if ((int)pWep->GetCompareDamage() == (int)player->GetCharacter()->GetWeapon()->GetCompareDamage())
				damageCompare = 0;
			else if ((int)pWep->GetCompareDamage() > (int)player->GetCharacter()->GetWeapon()->GetCompareDamage())
				damageCompare = 1;
			else
				damageCompare = -1;
		}
		if (pWep->GetRateOfFire() == player->GetCharacter()->GetWeapon()->GetRateOfFire())
			rateOfFireCompare = 0;
		else if (pWep->GetRateOfFire() > player->GetCharacter()->GetWeapon()->GetRateOfFire())
			rateOfFireCompare = 1;
		else
			rateOfFireCompare = -1;
	}
	//font->Draw(weaponCompare.str().c_str(), 595, 320, .6f, { 255, 0, 255, 0 });
}

void GameplayState::RenderWeaponCompare()
{
	if (comparingWeapons && !inRecruitmentDialogue)
	{
		//SGD::GraphicsManager::GetInstance()->DrawRectangle({ 550, 310, 770, 380 }, { 200, 0, 0, 0 }, { 255, 255, 255, 255 }, 2);
		//font->Draw(weaponCompare.str().c_str(), 565, 320, .5f, { 255, 255, 255 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 200, 380, 390, 450 }, { 200, 0, 0, 0 }, { 255, 255, 255, 255 }, 2);
		font->Draw(weaponCompare.str().c_str(), 205, 390, 1.f, { 255, 255, 255 });
		//SGD::GraphicsManager::GetInstance()->DrawString(weaponCompare.str().c_str(), { 565, 320 }, { 255, 255, 255, 255 });
		if (damageCompare == 1)//-350 + 70
			SGD::GraphicsManager::GetInstance()->DrawTexture(upArrow, { 365, 385 }, 0.0f, {}, {}, { .17f, .17f });
		else if (damageCompare == -1)
			SGD::GraphicsManager::GetInstance()->DrawTexture(downArrow, { 365, 385 }, 0.0f, {}, {}, { .17f, .17f });
		else
			Game::GetInstance()->GetFont()->Draw("=", 365, 385, 1.f, { 255, 255, 255 });

		if (rateOfFireCompare == 1)
			SGD::GraphicsManager::GetInstance()->DrawTexture(upArrow, { 365, 420 }, 0.0f, {}, {}, { .17f, .17f });
		else if (rateOfFireCompare == -1)
			SGD::GraphicsManager::GetInstance()->DrawTexture(downArrow, { 365, 420 }, 0.0f, {}, {}, { .17f, .17f });
		else
			Game::GetInstance()->GetFont()->Draw("=", 365, 420, 1.f, { 255, 255, 255 });

		weaponCompare.clear();
		weaponCompare.str("");
		gameplayDialogue->SetStringToDisplay(" ");
	}
	else
	{
		weaponCompare.clear();
		weaponCompare.str("");
	}
	alreadySetWeaponInfo = false;
}

void GameplayState::CheckWeaponCollision()
{
	for (auto i = weapons.begin(); i != weapons.end(); ++i)
	{
		// Local variables help with debugging
		SGD::Rectangle rEntity1 = player->GetRect();
		SGD::Rectangle rEntity2 = (*i)->GetRect();

		// Check for collision between the entities
		if (rEntity1.IsIntersecting(rEntity2) == true)
		{
			// Players character handle collision
			player->GetCharacter()->HandleCollision(*i);
			return;
		}
	}
	if (!collidingWithAlly)
		GameplayState::GetInstance()->comparingWeapons = false;
	else
		gameplayDialogue->SetStringToDisplay(" ");

}
///**************************************************************/

void GameplayState::CheckCursorCollisionWithCharacterPortraits()
{
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();

	//If player hovers over character portraits, descriptions of the character you are over should appear

	if (pInput->GetMousePosition().IsWithinRectangle({ 100, 103, 150, 153 }))
	{
		//SGD::GraphicsManager::GetInstance()->DrawString(player->GetCharacter()->GetActiveDiscription().str().c_str(), { 132, 60 }, { 255, 255, 255 });
		SGD::GraphicsManager::GetInstance()->DrawRectangle({ 130, 58, 700, 150 }, { 185, 0, 0, 0 });
		Game::GetInstance()->GetFont()->Draw(player->GetCharacter()->GetActiveDiscription().str().c_str(), 132, 60, 1.0f, { 255, 255, 255 });
	}
	for (unsigned int i = 0; i < player->party.size(); i++)
	{
		if (OptionsState::GetInstance()->getFullScreen())
		{
			if (pInput->GetMousePosition().IsWithinRectangle({ 100.0f, 195.0f + i * 90, 150, (195.0f + i * 90) + 50 }))
			{
				SGD::GraphicsManager::GetInstance()->DrawRectangle({ 130, 58, 700, 150 }, { 185, 0, 0, 0 });
				//SGD::GraphicsManager::GetInstance()->DrawString(player->party[i]->getCharacter()->GetActiveDiscription().str().c_str(), { 132, 60 }, { 255, 255, 255 });
				Game::GetInstance()->GetFont()->Draw(player->party[i]->getCharacter()->GetActiveDiscription().str().c_str(), 132, 60, 1.0f, { 255, 255, 255 });
			}
		}
		else
		{
			if (pInput->GetMousePosition().IsWithinRectangle({ 100.0f, (210.0f + i * 100), 150, (210.0f + i * 100) + 50 }))
			{
				SGD::GraphicsManager::GetInstance()->DrawRectangle({ 130, 58, 700, 150 }, { 185, 0, 0, 0 });
				//SGD::GraphicsManager::GetInstance()->DrawString(player->party[i]->getCharacter()->GetActiveDiscription().str().c_str(), { 132, 60 }, { 255, 255, 255 });
				Game::GetInstance()->GetFont()->Draw(player->party[i]->getCharacter()->GetActiveDiscription().str().c_str(), 132, 60, 1.0f, { 255, 255, 255 });
			}
		}
	}
}

/**************************************************************/
// SaveProgress
//	- Saves progress into a slot
void GameplayState::SaveProgress(string file)
{
	TiXmlDocument doc;
	TiXmlDeclaration * decl = new TiXmlDeclaration("1.0", "utf-8", "");
	TiXmlElement *xWorld = new TiXmlElement("World");
	TiXmlElement *xTileset = new TiXmlElement("tileset");
	TiXmlElement *xTiles = new TiXmlElement("Tiles");
	TiXmlElement *xLayers = new TiXmlElement("Layers");
	TiXmlElement *xSpawns = new TiXmlElement("Spawns");
	TiXmlElement *xObjectives = new TiXmlElement("Objectives");
	doc.LinkEndChild(decl);
	doc.LinkEndChild(xWorld);
	xWorld->LinkEndChild(xTileset);
	xWorld->LinkEndChild(xTiles);
	xWorld->LinkEndChild(xLayers);
	xWorld->LinkEndChild(xSpawns);
	xWorld->LinkEndChild(xObjectives);

	TileLayer *sampleLayer = world->GetTileLayers()[0];

	//World attributes
	xWorld->SetAttribute("rows", sampleLayer->tiles.size() / sampleLayer->layerColumns);
	xWorld->SetAttribute("columns", sampleLayer->layerColumns);

	if (objectives->GetCurrentObjectiveNumber() < objectives->GetNumberOfObjectives())
		if ((objectives->GetCurrentObjectiveNumber() - 1) == -1)
			xWorld->SetAttribute("currentObjective", "Tutorial");
		else
			xWorld->SetAttribute("currentObjective", objectives->GetObjectivesList()[objectives->GetCurrentObjectiveNumber() - 1]->GetCompareString().c_str());

	xTileset->SetAttribute("numColumns", World::GetInstance()->GetTileLayers()[0]->GetTileColumns());
	xTileset->SetAttribute("numRows", World::GetInstance()->GetTileLayers()[0]->GetTileRows());
	xTileset->SetAttribute("tileHeight", (int)sampleLayer->GetTileSize().height);
	xTileset->SetAttribute("tileWidth", (int)sampleLayer->GetTileSize().width);
	xTileset->SetAttribute("filename", World::GetInstance()->GetTileLayers()[0]->GetFilePath().c_str());

	//Unique Tiles
	for (unsigned int i = 0; i < world->GetTileTypes()->size(); i++)
	{
		Tile *tile = (*world->GetTileTypes())[i];

		TiXmlElement *xTile = new TiXmlElement("Tile");

		xTile->SetAttribute("event", tile->event.c_str());
		xTile->SetAttribute("isPassable", (int)tile->isPassable);
		xTile->SetAttribute("tileNumber", tile->tileNumber);
		xTile->SetAttribute("X", tile->X);
		xTile->SetAttribute("Y", tile->Y);

		xTiles->LinkEndChild(xTile);
	}

	//Layers
	std::vector<TileLayer *> layers = world->GetTileLayers();
	for (unsigned int layerNum = 0; layerNum < layers.size(); layerNum++)
	{
		string layer;

		std::vector<Tile *> tiles = world->GetTileLayers()[layerNum]->tiles;
		for (unsigned int tileNum = 0; tileNum < tiles.size(); tileNum++)
		{
			char num[100];
			_itoa_s(tiles[tileNum]->tileNumber, num, 10);
			layer.append(num);
			layer.append(",");
		}

		TiXmlElement *xLayer = new TiXmlElement("Layer");
		xLayer->LinkEndChild(new TiXmlText(layer.c_str()));
		xLayers->LinkEndChild(xLayer);
	}

	//Player Spawn
	TiXmlElement *xPlayer = new TiXmlElement("Spawn");

	xPlayer->SetAttribute("SpawnType", "Player");
	xPlayer->SetAttribute("X", (int)(player->GetCharacter()->GetPosition().x));
	xPlayer->SetAttribute("Y", (int)(player->GetCharacter()->GetPosition().y));
	xPlayer->SetAttribute("Class", player->GetCharacter()->GetClassString().c_str());

	xPlayer->SetAttribute("Difficulty", Game::GetInstance()->playerDifficulty);

	xPlayer->SetAttribute("Level", player->GetCharacter()->GetLevel());
	xPlayer->SetAttribute("Experience", player->GetCharacter()->GetCurrExp());

	xPlayer->SetAttribute("HP", (int)player->GetCharacter()->GetCurrHealth());
	xPlayer->SetAttribute("Rep", player->GetReputation());

	switch (player->GetCharacter()->GetWeapon()->GetGunType())
	{
	case Weapon::GunType::chainsawLauncher:
		xPlayer->SetAttribute("ChainsawLauncher", "Chainsaw Launcher");
		break;
	case Weapon::GunType::flameThrower:
		xPlayer->SetAttribute("Flamethrower", "Flamethrower");
		break;
	case Weapon::GunType::gatlingGun:
		xPlayer->SetAttribute("GatlingGun", "Gatling Gun");
		break;
	case Weapon::GunType::meleeWeapon:
		xPlayer->SetAttribute("Melee", dynamic_cast<MeleeWeapon*>(player->GetCharacter()->GetWeapon())->GetMeleeWeaponType());
		break;
	case Weapon::GunType::Pistol:
		xPlayer->SetAttribute("Pistol", dynamic_cast<Pistol*>(player->GetCharacter()->GetWeapon())->GetPistolType());
		break;
	case Weapon::GunType::Shotgun:
		xPlayer->SetAttribute("Shotgun", dynamic_cast<Shotgun*>(player->GetCharacter()->GetWeapon())->GetShotgunType());
		break;
	case Weapon::GunType::SniperRifle:
		xPlayer->SetAttribute("SniperRifle", dynamic_cast<SniperRifle*>(player->GetCharacter()->GetWeapon())->GetSniperType());
		break;
	}

	xPlayer->SetAttribute("Quality", (int)player->GetCharacter()->GetWeapon()->GetPrefix());

	xSpawns->LinkEndChild(xPlayer);

	// Make sure all spawns are ALIVE before saving
	SpawnList::GetInstance()->RespawnAll();

	//Spawns

	for (unsigned int i = 0; i < npcs.size(); i++)
	{
		if (!npcs[i]->getCharacter()) continue;

		TiXmlElement *xSpawn = new TiXmlElement("Spawn");

		xSpawn->SetAttribute("SpawnType", "NPC");
		xSpawn->SetAttribute("IsEnemy", (int)npcs[i]->GetIsEnemy());
		xSpawn->SetAttribute("X", (int)(npcs[i]->getCharacter()->GetPosition().x / 64));
		xSpawn->SetAttribute("Y", (int)(npcs[i]->getCharacter()->GetPosition().y / 64));

		if (dynamic_cast<Mutant*>(npcs[i]->getCharacter()))
		{
			switch (dynamic_cast<Mutant*>(npcs[i]->getCharacter())->GetMutantType())
			{
			case MutantType::MUTT:
				xSpawn->SetAttribute("Class", "MUTt");
				break;
			case MutantType::RADIANT:
				xSpawn->SetAttribute("Class", "RadiAnt");
				break;
			case MutantType::RADZILLA:
				xSpawn->SetAttribute("Class", "Radzilla");
				break;
			case MutantType::RAZARD:
				xSpawn->SetAttribute("Class", "Razard");
				break;
			}
		}
		else
			xSpawn->SetAttribute("Class", npcs[i]->getCharacter()->GetClassString().c_str());

		xSpawn->SetAttribute("Level", npcs[i]->getCharacter()->GetLevel());

		if (npcs[i]->getCharacter()->GetEvent() != "")
			xSpawn->SetAttribute("Event", npcs[i]->getCharacter()->GetEvent().c_str());


		// If the player is a member of your party
		if (npcs[i]->anchor == player->GetCharacter())
		{
			xSpawn->SetAttribute("PartyMate", "PartyMate");
			//xSpawn->SetAttribute("WeaponType", (int)npcs[i]->getCharacter()->GetWeapon()->GetGunType());

			switch (npcs[i]->getCharacter()->GetWeapon()->GetGunType())
			{
			case Weapon::GunType::chainsawLauncher:
				xSpawn->SetAttribute("ChainsawLauncher", "Chainsaw Launcher");
				break;
			case Weapon::GunType::flameThrower:
				xSpawn->SetAttribute("Flamethrower", "Flamethrower");
				break;
			case Weapon::GunType::gatlingGun:
				xSpawn->SetAttribute("GatlingGun", "Gatling Gun");
				break;
			case Weapon::GunType::meleeWeapon:
				xSpawn->SetAttribute("Melee", dynamic_cast<MeleeWeapon*>(npcs[i]->getCharacter()->GetWeapon())->GetMeleeWeaponType());
				break;
			case Weapon::GunType::Pistol:
				xSpawn->SetAttribute("Pistol", dynamic_cast<Pistol*>(npcs[i]->getCharacter()->GetWeapon())->GetPistolType());
				break;
			case Weapon::GunType::Shotgun:
				xSpawn->SetAttribute("Shotgun", dynamic_cast<Shotgun*>(npcs[i]->getCharacter()->GetWeapon())->GetShotgunType());
				break;
			case Weapon::GunType::SniperRifle:
				xSpawn->SetAttribute("SniperRifle", dynamic_cast<SniperRifle*>(npcs[i]->getCharacter()->GetWeapon())->GetSniperType());
				break;
			}

			xSpawn->SetAttribute("Quality", (int)npcs[i]->getCharacter()->GetWeapon()->GetPrefix());
		}
		else
		{
			if (npcs[i]->anchor && npcs[i]->anchor->GetType() == Entity::EntityType::ENT_BASE)
			{
				//anchored to pos
				xSpawn->SetAttribute("AnchorPointX", (int)(npcs[i]->anchor->GetPosition().x / 64));
				xSpawn->SetAttribute("AnchorPointY", (int)(npcs[i]->anchor->GetPosition().y / 64));
			}
			else if (npcs[i]->anchor && npcs[i]->anchor->GetType() >= 3 && npcs[i]->anchor->GetType() <= 7)
			{
				//anchored to NPC

				//Loop through npcs looking for match
				int index = 0;
				for (int j = 0; j < (int)npcs.size(); j++)
				{
					if (npcs[j]->getCharacter() == npcs[i]->anchor)
						xSpawn->SetAttribute("AnchorIndex", index);

					index++;
				}
			}
		}
		xSpawns->LinkEndChild(xSpawn);
	}



	// Traps

	for (unsigned int i = 0; i < m_pEntities->GetEntityTable()[PICKUPSandHAZARDS].size(); i++)
	{
		Hazards* currentHazard = dynamic_cast<Hazards*>(m_pEntities->GetEntityTable()[PICKUPSandHAZARDS][i]);

		if (currentHazard)
		{
			TiXmlElement *xSpawn = new TiXmlElement("Spawn");

			xSpawn->SetAttribute("SpawnType", "Trap");
			xSpawn->SetAttribute("X", (int)(currentHazard->GetPosition().x / 64));
			xSpawn->SetAttribute("Y", (int)(currentHazard->GetPosition().y / 64));

			HazardType thisType = currentHazard->GetHazardType();

			switch (thisType)
			{
			case HazardType::BEAR_TRAP:
				xSpawn->SetAttribute("Type", "Bear Trap");
				break;
			case HazardType::PITFALL:
				xSpawn->SetAttribute("Type", "Pitfall");
				break;
			case HazardType::RADIATION_POOL:
				xSpawn->SetAttribute("Type", "Radiation Pool");
				break;
			}

			xSpawns->LinkEndChild(xSpawn);
		}
	}

	// Weapons

	auto weaponIter = weapons.begin();

	for (; weaponIter != weapons.end(); weaponIter++)
	{
		if (!(*weaponIter)->GetNotWorld())
		{
			TiXmlElement *xSpawn = new TiXmlElement("Spawn");

			xSpawn->SetAttribute("SpawnType", "Weapon");
			xSpawn->SetAttribute("X", (int)((*weaponIter)->GetPosition().x / 64));
			xSpawn->SetAttribute("Y", (int)((*weaponIter)->GetPosition().y / 64));

			switch ((*weaponIter)->GetGunType())
			{
			case Weapon::GunType::meleeWeapon:
				xSpawn->SetAttribute("Type", "Melee");
				break;
			case Weapon::GunType::Pistol:
				xSpawn->SetAttribute("Type", "Pistol");
				break;
			case Weapon::GunType::Shotgun:
				xSpawn->SetAttribute("Type", "Shotgun");
				break;
			case Weapon::GunType::SniperRifle:
				xSpawn->SetAttribute("Type", "Sniper Rifle");
				break;
			}

			xSpawn->SetAttribute("Level", (*weaponIter)->GetSpawnLevel());

			xSpawns->LinkEndChild(xSpawn);
		}
	}

	// Objectives

	for (unsigned int i = 0; i < objectives->GetObjectivesList().size(); i++)
	{
		TiXmlElement * xObjective = new TiXmlElement("Objective");

		xObjective->SetAttribute("StringToCompare", objectives->GetObjectivesList()[i]->GetCompareString().c_str());
		xObjective->SetAttribute("StringToDisplay", objectives->GetObjectivesList()[i]->GetObjectiveText().c_str());
		xObjective->SetAttribute("LocationX", objectives->GetObjectivesList()[i]->GetLocX());
		xObjective->SetAttribute("LocationY", objectives->GetObjectivesList()[i]->GetLocY());

		xObjectives->LinkEndChild(xObjective);
	}

	//Save it out
	doc.SaveFile(file.c_str());
	doc.Clear();
}

/**************************************************************/
// LoadEmitters
//	- Loads all files in directory to map
//	- Returns number of files loaded.
int GameplayState::LoadEmitters(std::string dir)
{
	int numLoaded = 0;

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	wchar_t newPath[200];
	mbstowcs_s(nullptr, newPath, dir.c_str(), 200);

	hFind = FindFirstFile(newPath, &FindFileData);
	do {
		// ignore directories
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			wstring name = FindFileData.cFileName;
			if (name[name.find_last_of(L".") + 1] == 'x') //Is it an xml?
			{
				char newName[200];
				wcstombs_s(nullptr, newName, name.c_str(), 200);

				string fullName = "resources/Particles/";
				fullName.append(newName);

				string truncName = newName;
				truncName.erase(truncName.find_first_of("."));

				//Load the emitter
				Emitter *newEmitter = LoadEmitterFile(fullName);
				emitters.emplace(truncName, *newEmitter);

				numLoaded++;
			}
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);

	return numLoaded;
}

/**************************************************************/
// CreateEmitter
//	- Returns an emitter generated from file path
Emitter *GameplayState::LoadEmitterFile(std::string file)
{
	TiXmlDocument doc;
	Emitter *newEmitter = new Emitter();

	bool doc_loaded = doc.LoadFile(file.c_str());

	assert(doc_loaded && "Failed to find world file!");

	TiXmlElement *root = doc.RootElement();
	assert(root && "Failed to find root node of world file");

	double tempA, tempB, tempC, tempD;

	//Start reading
	TiXmlElement *emitter = root->FirstChildElement("Emitter");
	TiXmlElement *particle = root->FirstChildElement("Particle");

	//Get nodes
	TiXmlElement *lifetime = emitter->FirstChildElement();
	TiXmlElement *position = lifetime->NextSiblingElement();
	TiXmlElement *velocity = position->NextSiblingElement();
	TiXmlElement *spawnPosition = velocity->NextSiblingElement();
	TiXmlElement *spawnRate = spawnPosition->NextSiblingElement();
	TiXmlElement *particleImage = spawnRate->NextSiblingElement();

	TiXmlElement *life = particle->FirstChildElement();
	TiXmlElement *particleVelocity = life->NextSiblingElement();
	TiXmlElement *color = particleVelocity->NextSiblingElement();
	TiXmlElement *size = color->NextSiblingElement();
	TiXmlElement *rotation = size->NextSiblingElement();

	//Image
	string particleImagePath = "resources/graphics/";
	particleImagePath.append(particleImage->Attribute("src"));
	newEmitter->SetImage(particleImagePath.c_str());
	particleImage->Attribute("width", &tempA);
	particleImage->Attribute("height", &tempB);
	newEmitter->SetSize({ (float)tempA, (float)tempB });

	//Emitter attributes
	int runningTemp;
	int enumTemp;
	emitter->Attribute("shape", &enumTemp);
	emitter->Attribute("running", &runningTemp);
	newEmitter->running = *reinterpret_cast<bool *>(&runningTemp);
	newEmitter->shape = (Emitter::SHAPE)enumTemp;
	emitter->Attribute("maxParticles", &newEmitter->maxParticles);

	//Position
	lifetime->Attribute("life", &tempA);
	newEmitter->lifeTime = (float)tempA;
	position->Attribute("x", &tempC);
	position->Attribute("y", &tempD);
	newEmitter->SetPosition({ (float)tempC, (float)tempD });

	//Emitter velocity
	velocity->Attribute("x", &tempA);
	velocity->Attribute("y", &tempB);
	newEmitter->SetVelocity({ (float)tempA, (float)tempB });

	//Spawn Data
	double rot;
	spawnPosition->Attribute("radius", &tempA);
	spawnPosition->Attribute("angle", &tempB);
	newEmitter->radius = (float)tempA;
	newEmitter->angle = (float)tempB;
	spawnPosition->Attribute("rotation", &rot);
	newEmitter->SetRotation((float)rot);
	spawnPosition->Attribute("rotationRand", &tempC);
	newEmitter->rotationRand = (float)tempC;

	//Spawn Rate
	spawnRate->Attribute("rate", &tempA);
	spawnRate->Attribute("rateRand", &tempB);
	newEmitter->spawnRate = (float)tempA;
	newEmitter->spawnRateRand = (float)tempB;

	//Lifetime
	life->Attribute("lifetime", &tempA);
	life->Attribute("lifetimeRand", &tempB);
	newEmitter->particleLifeTime = (float)tempA;
	newEmitter->particleLifeTimeRand = (float)tempB;

	//Velocity
	particleVelocity->Attribute("start", &tempA);
	particleVelocity->Attribute("end", &tempB);
	particleVelocity->Attribute("startRand", &tempC);
	particleVelocity->Attribute("endRand", &tempD);
	newEmitter->SetParticleVelocity((float)tempA, (float)tempB, (float)tempC, (float)tempD);

	//Size
	size->Attribute("start", &tempA);
	size->Attribute("end", &tempB);
	size->Attribute("startRand", &tempC);
	size->Attribute("endRand", &tempD);
	newEmitter->SetParticleSize((float)tempA, (float)tempB, (float)tempC, (float)tempD);

	//Color
	SGD::Color tempcolor, tempcolor2;
	TiXmlElement *startColor = color->FirstChildElement();
	TiXmlElement *endColor = startColor->NextSiblingElement();
	startColor->Attribute("a", &tempA);
	startColor->Attribute("r", &tempB);
	startColor->Attribute("g", &tempC);
	startColor->Attribute("b", &tempD);
	tempcolor = { (unsigned char)(tempA * 255), (unsigned char)tempB, (unsigned char)tempC, (unsigned char)tempD };

	endColor->Attribute("a", &tempA);
	endColor->Attribute("r", &tempB);
	endColor->Attribute("g", &tempC);
	endColor->Attribute("b", &tempD);
	tempcolor2 = { (unsigned char)(tempA * 255), (unsigned char)tempB, (unsigned char)tempC, (unsigned char)tempD };

	color->Attribute("startRand", &tempA);
	color->Attribute("endRand", &tempB);
	newEmitter->SetParticleColor(tempcolor, tempcolor2, (float)tempA, (float)tempB);

	//Rotation
	rotation->Attribute("start", &tempA);
	rotation->Attribute("end", &tempB);
	rotation->Attribute("startRand", &tempC);
	rotation->Attribute("endRand", &tempD);
	newEmitter->SetParticleRotation((float)tempA, (float)tempB, (float)tempC, (float)tempD);

	doc.Clear();

	return newEmitter;
}

/**************************************************************/
// MessageProc
//	- process messages queued in the MessageManager
//	- STATIC METHOD
//		- does NOT have invoking object!!!
//		- must use singleton to access members
/*static*/ void GameplayState::MessageProc(const SGD::Message* pMsg)
{
	/* Show warning when a Message ID enumerator is not handled */
#pragma warning( push )
#pragma warning( 1 : 4061 )

	// What type of message?
	switch (pMsg->GetMessageID())
	{
	case MSG_CREATEBULLET:
	{
		CreateBulletMsg* msg = (CreateBulletMsg*)pMsg;
		Bullet* bullet = new Bullet(msg->owner);

		// create a vector to use for bullets velocity
		//SGD::Vector velocity = SGD::Vector(cos(msg->direction), sin(msg->direction)) * msg->speed;
		SGD::Vector velocity = SGD::Vector(0, -1);
		velocity.Rotate(msg->direction);
		velocity *= msg->speed;

		if (velocity.ComputeLength() < (velocity + msg->owner->GetOwner()->plusBulletVector).ComputeLength())
			velocity += msg->owner->GetOwner()->plusBulletVector;

		// set bullets velocity
		bullet->SetVelocity(velocity);
		if (msg->enemy && bullet->GetOwner()->bulType[0] == BulletType::multiplyingShots)
		{
			Character* e = msg->enemy;
			//bullet->SetStartPoint(e->GetPosition() + e->GetSize() + SGD::Size(32, 32));
			bullet->SetStartPoint(e->GetPosition() + e->GetSize());
			bullet->damage = msg->damage*0.5f;
			bullet->alreadyMultiplied = true;
			bullet->m_fRotation = msg->direction;
		}
		else
		{
			// set bullet position
			SGD::Point pt;
			if (msg->owner->GetOwner()->GetClass() != ClassType::Mutant)
			{
				pt = msg->owner->GetWeaponPoint();
				pt.y -= (msg->owner->GetSize().height*0.85f);
				SGD::Vector temp = { pt - msg->owner->GetAnchor() };
				temp.Rotate(msg->owner->GetRotation());
				pt.x = (msg->owner->GetAnchor().x + temp.x);
				pt.y = (msg->owner->GetAnchor().y + temp.y);
			}
			else
			{
				pt = msg->owner->GetWeaponPoint();
				SGD::Vector temp = { pt - msg->owner->GetAnchor() };
				temp.Rotate(msg->owner->GetRotation());
				pt.x = (msg->owner->GetAnchor().x + temp.x);
				pt.y = (msg->owner->GetAnchor().y + temp.y);
			}

			bullet->SetStartPoint(pt);
			// set bullet damage
			bullet->damage = msg->damage;
			// set bullet rotation
			bullet->m_fRotation = msg->direction;
		}
		bullet->DamageModifier();

		// put new bullet into entity manager
		GameplayState::GetInstance()->m_pEntities->AddEntity(bullet, BULLETS);
		bullet->Release();
	}
		break;

	case MSG_CREATELASER:
	{
		CreateLaserBullet* msg = (CreateLaserBullet*)pMsg;
		LaserBullet* bullet = new LaserBullet(msg->owner);


		// create a vector to use for bullets velocity
		//SGD::Vector velocity = SGD::Vector(cos(msg->direction), sin(msg->direction)) * msg->speed;
		SGD::Vector velocity = SGD::Vector(1, 0);
		velocity.Rotate(msg->direction);
		velocity *= msg->speed;

		// set bullets velocity
		bullet->SetVelocity(velocity);
		// set bullet position
		SGD::Point pt = msg->owner->GetPosition();
		pt.x += msg->owner->GetSize().width*0.5f;
		pt.y += msg->owner->GetSize().height;
		bullet->SetPosition(pt);
		// set bullet damage
		bullet->SetDamage(msg->damage);
		// set bullet rotation
		bullet->SetRotation(msg->direction);

		// put new bullet into entity manager
		GameplayState::GetInstance()->m_pEntities->AddEntity(bullet, SKILLS);
	}
		break;

	case MSG_CREATEHEALTHPICKUP:
	{
		CreateHealthPickupMsg* msg = (CreateHealthPickupMsg*)pMsg;

		HealthPickup* pickup = new HealthPickup(msg->GetPosition());
		GameplayState::GetInstance()->m_pEntities->AddEntity(pickup, PICKUPSandHAZARDS);
		pickup->Release();
	}
		break;
	case MSG_CREATENPC:
	{

		CreateNPCMsg* msg = (CreateNPCMsg*)pMsg;

		GameplayState::GetInstance()->m_pEntities->AddEntity(msg->GetNpc()->getCharacter(), CHARACTERS);
		GameplayState::GetInstance()->npcs.push_back(msg->GetNpc());

		break;
	}

	case MSG_CREATEPLAYER:
	{

	}
		break;
	case MSG_CREATEWEAPON:
	{
		CreateWeaponMsg* msg = (CreateWeaponMsg*)pMsg;
		Weapon* newWeapon = nullptr;

		switch (msg->GetWeapon()->GetGunType())
		{
		case Weapon::GunType::meleeWeapon:
		{
			MeleeWeapon::meleeWeaponTypes temp = (MeleeWeapon::meleeWeaponTypes)(msg->GetWeapon()->GetType());
			newWeapon = new MeleeWeapon((temp));
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
			break;
		case Weapon::GunType::Pistol:
		{
			Pistol::PistolType temp = (Pistol::PistolType)(msg->GetWeapon()->GetType());
			newWeapon = new Pistol((temp));
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
			break;
		case Weapon::GunType::Shotgun:
		{
			Shotgun::ShotgunTypes temp = (Shotgun::ShotgunTypes)(msg->GetWeapon()->GetType());
			newWeapon = new Shotgun((temp));
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
			break;
		case Weapon::GunType::SniperRifle:
		{
			SniperRifle::SniperRifleTypes temp = (SniperRifle::SniperRifleTypes)(msg->GetWeapon()->GetType());
			newWeapon = new SniperRifle((temp));
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
			break;
		case Weapon::GunType::flameThrower:
		{
			newWeapon = new Flamethrower();
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
			break;
		case Weapon::GunType::gatlingGun:
		{
			newWeapon = new GatlingGun();
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
			break;
		case Weapon::GunType::chainsawLauncher:
		{
			newWeapon = new BuzzsawLauncher();
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
			break;
		case Weapon::GunType::baseweapon:
		case Weapon::GunType::MutantAtk:
		default:
		{
			Pistol::PistolType temp = (Pistol::PistolType)(msg->GetWeapon()->GetType());
			newWeapon = new Pistol((temp));
			newWeapon->SetPrefix(msg->GetWeapon()->GetPrefix());
		}
		}

		if (newWeapon)
		{
			newWeapon->SetOwner(nullptr);
			newWeapon->SetPosition(msg->GetWeapon()->GetPosition());
		}

		if (newWeapon->GetPrefix() == GunPrefix::rusty)
			newWeapon->SetColor(SGD::Color(149, 69, 29));
		else if (newWeapon->GetPrefix() == GunPrefix::antique)
			newWeapon->SetColor(SGD::Color::Green);
		else if (newWeapon->GetPrefix() == GunPrefix::standard)
			newWeapon->SetColor(SGD::Color::Blue);
		else if (newWeapon->GetPrefix() == GunPrefix::highQuality)
			newWeapon->SetColor(SGD::Color::Purple);
		else if (newWeapon->GetPrefix() == GunPrefix::futuristic)
			newWeapon->SetColor(SGD::Color::Red);

		if (msg->GetWeapon()->GetOwner() != nullptr)
			newWeapon->SetNotWorld(true);

		GameplayState::GetInstance()->weapons.push_back(newWeapon);
		break;
	}

	case MSG_DESTROYENTITY:
	{

		DestroyEntityMsg* cdem = (DestroyEntityMsg*)pMsg;
		Entity* ptr = cdem->GetEntity();
		if (ptr)
			GameplayState::GetInstance()->m_pEntities->RemoveEntity(ptr);

		break;
	}

	case MSG_CREATE_ENEMY:
	{
		CreateEnemyMsg* msg = (CreateEnemyMsg*)pMsg;

		Character* temp = msg->GetNpc()->getCharacter();
		if (temp->GetWeapon() == nullptr && temp->GetClass() != ClassType::Mutant)
			temp->SetWeapon(temp->SetStartingWeapon());
		GameplayState::GetInstance()->m_pEntities->AddEntity(temp, CHARACTERS);
		GameplayState::GetInstance()->npcs.push_back(msg->GetNpc());

		break;
	}
	case MSG_CREATEABILITY:
	{
		CreateAbility* skill = (CreateAbility*)pMsg;
		GameplayState::GetInstance()->m_pEntities->AddEntity(skill->GetAbility(), SKILLS);
	}
		break;
	case MSG_CREATEHITMARKER:
	{
		// cast message
		CreateHitMarkerMsg* msg = (CreateHitMarkerMsg*)pMsg;

		// allocate a new hitmarker
		HitMarker* hitmarker = new HitMarker(msg->GetDamage(), msg->GetPosition(), msg->GetDirection(), msg->GetColor());
		// Send the hit marker into the entity manager
		GetInstance()->m_pEntities->AddEntity(hitmarker, Buckets::CHARACTERS);
		hitmarker->Release();
		break;
	}
	default:
	{
		break;
	}
	}
}

bool GameplayState::GetPaused()
{
	return paused;
}

void GameplayState::SetPaused(bool newPaused)
{
	paused = newPaused;
}