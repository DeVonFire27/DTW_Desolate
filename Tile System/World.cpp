#include "globals.h"
#include "World.h"
#include "Tile.h"
#include "TileLayer.h"
#include "../TinyXML/tinystr.h"
#include "../TinyXML/tinyxml.h"
#include "../GameplayState.h"
#include "../Agents/NPC.h"
#include "../Agents/Radiant.h"
#include "../Weapons/MeleeWeapon.h"
#include "../Weapons/Pistol.h"
#include "../Weapons/Shotgun.h"
#include "../Weapons/SniperRifle.h"
#include "../NewGameState.h"
#include "../EntityManager.h"
#include "../Game.h"
#include "../Particle System/Emitter.h"

#include "../Objective System/Objective.h"
#include "../Objective System/ObjectiveList.h"

#include "../Spawn System/SpawnList.h"
#include "../Spawn System/Spawn.h"

#include "../NewGameState.h"
#include "../Agents/Stat.h"

#include "../Agents/Jane.h"
#include "../Agents/MUTt.h"
#include "../Agents/Razard.h"
#include "../Agents/Radiant.h"
#include "../Agents/Radzilla.h"

#include "../Weapons/SpecialWeapons/BuzzsawLauncher.h"
#include "../Weapons/SpecialWeapons/Flamethrower.h"
#include "../Weapons/SpecialWeapons/GatlingGun.h"

#include "../Hazards/BearTrap.h"
#include "../Hazards/Pitfall.h"
#include "../Hazards/RadiationPool.h"
#include "../Hazards/Hazards.h"

#pragma warning (disable: 4996) //This iusage of strtok is not unsafe.

World* World::GetInstance()
{
	static World s_Instance;
	return &s_Instance;
}

void World::Render()
{
	//setting camera position uses the players position and therefore will break the game if the player does not exist, so this must not be called if you lose the game
	if (GameplayState::GetInstance()->GetLose() == false)
	{
		camera.SetPosition(-(SGD::Point{
			GameplayState::GetInstance()->player->GetPosition().x - (Game::GetInstance()->GetScreenWidth() / 2 - GameplayState::GetInstance()->player->GetCharacter()->GetSize().width),
			GameplayState::GetInstance()->player->GetPosition().y - (Game::GetInstance()->GetScreenHeight() / 2 - GameplayState::GetInstance()->player->GetCharacter()->GetSize().height) }));
	}

	SGD::GraphicsManager::GetInstance()->SetTransform(camera.GetMatrix());

	for (unsigned int i = 0; i < 2; i++)
		world[i]->Render();
}

void World::LoadWorld()
{
	LoadWorld("world.xml");
}

void World::LoadWorld(string file)
{
	//Clearing out NPCs before loading vector
	GameplayState::GetInstance()->npcs.clear();

	TiXmlDocument doc(file.c_str());

	if (doc.LoadFile())
	{
		TiXmlElement *pRoot = doc.FirstChildElement();
		TiXmlElement *element = pRoot->FirstChildElement();

		int layerColumns;
		pRoot->Attribute("columns", &layerColumns);

		int layerRows;
		pRoot->Attribute("rows", &layerRows);


		string folderpath = "resources/graphics/";
		string filePath = element->Attribute("filename");
		string completePath;
		completePath = folderpath + filePath;

		int tileWidth;// = atoi(element->Attribute("tileWidth"));
		element->Attribute("tileWidth", &tileWidth);

		int tileHeight;// = atoi(element->Attribute("tileHeight"));
		element->Attribute("tileHeight", &tileHeight);

		int tileColumns;// = atoi(element->Attribute("numColumns"));
		element->Attribute("numColumns", &tileColumns);

		int tileRows;// = atoi(element->Attribute("numRows"));
		element->Attribute("numRows", &tileRows);

		// Tiles
		element = element->NextSiblingElement();

		// Tile
		TiXmlElement *tile = element->FirstChildElement();

		while (tile)
		{
			string event = tile->Attribute("event");
			int intisPassable;// = (atoi(tile->Attribute("isPassable")) == 1);
			tile->Attribute("isPassable", &intisPassable);

			bool isPassable = (intisPassable == 1);

			int tileNumber;// = atoi(tile->Attribute("tileNumber"));
			tile->Attribute("tileNumber", &tileNumber);

			int X;// = atoi(tile->Attribute("X"));
			tile->Attribute("X", &X);

			int Y;// = atoi(tile->Attribute("Y"));
			tile->Attribute("Y", &Y);

			Tile *newTile = new Tile(tileNumber, X, Y, isPassable, event);

			// Adding the newly-created tile to a vector of local tileTypes, used to build the map
			tileTypes.push_back(newTile);

			// Stepping to the next tile
			tile = tile->NextSiblingElement();
		}

		// Layers
		element = element->NextSiblingElement();

		// Entering first layer
		TiXmlElement *layer = element->FirstChildElement();

		// Creating a "non-tile" to populate the map locations with -1 indices.
		Tile* nonTile = new Tile(-1, -1, -1, 1, "");
		tileTypes.push_back(nonTile);

		while (layer)
		{
			string layerInfo = layer->GetText();

			// Creating a new Tile Layer
			TileLayer * newLayer = new TileLayer(SGD::Size(float(tileWidth), float(tileHeight)), layerColumns, layerRows, completePath, filePath, tileRows, tileColumns);

			// Storing the layer string into a non-const string
			char* layerInfoNonConst = _strdup(layerInfo.c_str());
			char * thisTile;

			vector<int> tileNumbers;

			// Used to store value of each token split
			thisTile = strtok(layerInfoNonConst, ",\n");

			while (thisTile)
			{
				// Converting char to int and pushing onto the vector
				tileNumbers.push_back(atoi(thisTile));

				// Continues splitting at the last location by passing null
				thisTile = strtok(NULL, ",");
			}

			delete[] layerInfoNonConst;

			// Looping through the vector
			for (unsigned int i = 0; i < tileNumbers.size(); i++)
			{
				// If its a non-tile(-1 index)
				if (tileNumbers[i] < 0)
				{
					newLayer->tiles.push_back(nonTile);
				}
				// If it is a real tile
				else
				{
					// Add the tile
					newLayer->tiles.push_back(tileTypes[tileNumbers[i]]);
				}
			}

			// Adding the layer to the world
			world.push_back(newLayer);

			// Stepping to the next layer
			layer = layer->NextSiblingElement();
		}
		// Spawns
		element = element->NextSiblingElement();

		// Entering first spawn
		TiXmlElement *spawn = element->FirstChildElement();

		// Local NPC spawn vector used for anchoring
		vector<NPC*> NPCVector;
		vector<int> anchorIndices;

		while (spawn)
		{
			string spawnType = spawn->Attribute("SpawnType");

			int X, Y, level;

			if (spawnType == "Player")
			{
				spawn->Attribute("X", &X);
				spawn->Attribute("Y", &Y);

				GameplayState::GetInstance()->player = new Player();
				Weapon *newWeapon = nullptr;
				if (Game::GetInstance()->playerClass == "Gunslinger")
				{
					Gunslinger* g = new Gunslinger();

					GameplayState::GetInstance()->player->SetCharacter(g);
					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = g->SetStartingWeapon();
					g->Release();
				}
				else if (Game::GetInstance()->playerClass == "Sniper")
				{
					Sniper* s = new Sniper();

					GameplayState::GetInstance()->player->SetCharacter(s);

					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = s->SetStartingWeapon();
					s->Release();
				}
				else if (Game::GetInstance()->playerClass == "Brawler")
				{
					Brawler* b = new Brawler();

					GameplayState::GetInstance()->player->SetCharacter(b);
					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = b->SetStartingWeapon();
					b->Release();
				}
				else if (Game::GetInstance()->playerClass == "Medic")
				{
					Medic* m = new Medic();

					GameplayState::GetInstance()->player->SetCharacter(m);
					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = m->SetStartingWeapon();
					m->Release();
				}
				else if (Game::GetInstance()->playerClass == "Cyborg")
				{
					Cyborg* c = new Cyborg();

					GameplayState::GetInstance()->player->SetCharacter(c);
					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = c->SetStartingWeapon();
					c->Release();
				}

				if (newWeapon)
				{
					if (spawn->Attribute("Quality"))
						newWeapon->SetPrefix(GunPrefix(atoi(spawn->Attribute("Quality"))));
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
					GameplayState::GetInstance()->player->GetCharacter()->SetWeapon(newWeapon);
					newWeapon->Release();
				}

				int level;
				if (spawn->Attribute("Level", &level))
					GameplayState::GetInstance()->player->GetCharacter()->SetLevel(level);

				GameplayState::GetInstance()->player->GetCharacter()->LoadStats("resources/xml/CharacterStats.xml");

				int hp;
				if (spawn->Attribute("HP", &hp))
					GameplayState::GetInstance()->player->GetCharacter()->SetCurrHealth((float)hp);

				int exp;
				if (spawn->Attribute("Experience", &exp))
					GameplayState::GetInstance()->player->GetCharacter()->SetExp(exp);

				int intDifficulty;
				if (spawn->Attribute("Difficulty", &intDifficulty))
					NewGameState::GetInstance()->SetDifficulty(difficulty(intDifficulty));

				GameplayState::GetInstance()->player->GetCharacter()->SetPosition(SGD::Point((float)X, (float)Y));

				int rep;
				if (spawn->Attribute("Rep", &rep))
					GameplayState::GetInstance()->player->SetReputation(rep);

				GameplayState::GetInstance()->m_pEntities->AddEntity(GameplayState::GetInstance()->player->GetCharacter(), CHARACTERS);
			}
			else if (spawnType == "NPC")
			{
				spawn->Attribute("X", &X);
				spawn->Attribute("Y", &Y);
				X *= tileWidth;
				Y *= tileHeight;
				spawn->Attribute("Level", &level);
				string job = spawn->Attribute("Class");

				int isEnemy;
				spawn->Attribute("IsEnemy", &isEnemy);

				// Call create NPC message with above info
				Character * thisCharacter;

				Weapon * newWeapon = nullptr;


				if (job == "Brawler")
				{
					thisCharacter = new Brawler();
					thisCharacter->SetLevel(level);

					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));

					thisCharacter->SetIsEnemy(isEnemy == 1);
					thisCharacter->LoadStats("resources/xml/CharacterStats.xml");

					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = thisCharacter->SetStartingWeapon();
				}
				else if (job == "Cyborg")
				{
					thisCharacter = new Cyborg();
					thisCharacter->SetLevel(level);

					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));

					thisCharacter->SetIsEnemy(isEnemy == 1);
					thisCharacter->LoadStats("resources/xml/CharacterStats.xml");

					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = thisCharacter->SetStartingWeapon();
				}
				else if (job == "Gunslinger")
				{
					thisCharacter = new Gunslinger();
					thisCharacter->SetLevel(level);

					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));

					thisCharacter->SetIsEnemy(isEnemy == 1);
					thisCharacter->LoadStats("resources/xml/CharacterStats.xml");

					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = thisCharacter->SetStartingWeapon();
				}
				else if (job == "Medic")
				{
					thisCharacter = new Medic();
					thisCharacter->SetLevel(level);

					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));
					thisCharacter->SetIsEnemy(isEnemy == 1);
					thisCharacter->LoadStats("resources/xml/CharacterStats.xml");

					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = thisCharacter->SetStartingWeapon();
				}
				else if (job == "Sniper")
				{
					thisCharacter = new Sniper();
					thisCharacter->SetLevel(level);

					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));
					thisCharacter->SetIsEnemy(isEnemy == 1);
					thisCharacter->LoadStats("resources/xml/CharacterStats.xml");

					if (spawn->Attribute("ChainsawLauncher"))
						newWeapon = new BuzzsawLauncher();
					else if (spawn->Attribute("Flamethrower"))
						newWeapon = new Flamethrower();
					else if (spawn->Attribute("GatlingGun"))
						newWeapon = new GatlingGun();
					else if (spawn->Attribute("Melee"))
						newWeapon = new MeleeWeapon(MeleeWeapon::meleeWeaponTypes(atoi((spawn->Attribute("Melee")))));
					else if (spawn->Attribute("Pistol"))
						newWeapon = new Pistol(Pistol::PistolType(atoi((spawn->Attribute("Pistol")))));
					else if (spawn->Attribute("Shotgun"))
						newWeapon = new Shotgun(Shotgun::ShotgunTypes(atoi(spawn->Attribute("Shotgun"))));
					else if (spawn->Attribute("SniperRifle"))
						newWeapon = new SniperRifle(SniperRifle::SniperRifleTypes(atoi(spawn->Attribute("SniperRifle"))));
					else
						newWeapon = thisCharacter->SetStartingWeapon();
				}
				else if (job == "MUTt")
				{
					// Spawn a MUTt here
					thisCharacter = new MUTt(level);
					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));
					thisCharacter->SetIsEnemy(true);
				}
				else if (job == "RadiAnt")
				{
					thisCharacter = new Radiant(level);
					thisCharacter->SetLevel(level);

					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));
					thisCharacter->SetIsEnemy(true);
				}
				else if (job == "Razard")
				{
					// Spawn a Razard here
					thisCharacter = new Razard(level);
					thisCharacter->SetPosition(SGD::Point((float)X, (float)Y));
					thisCharacter->SetIsEnemy(true);
				}
				else if (job == "Gatling Gun Guy")
				{
					// Spawn a Gatling Gun Guy here
					thisCharacter = new Gunslinger();
					thisCharacter->SetLevel(level);
					thisCharacter->SetPosition(SGD::Point((float)X, float(Y)));

					newWeapon = new GatlingGun();

					thisCharacter->SetIsEnemy(true);
					thisCharacter->LoadStats("resources/xml/CharacterStats.xml");
				}
				else if (job == "Buzzsaw Berserker")
				{
					// Spawn a Buzzsaw Berserker here
					thisCharacter = new Cyborg();
					thisCharacter->SetLevel(level);
					thisCharacter->SetPosition(SGD::Point((float)X, float(Y)));

					newWeapon = new BuzzsawLauncher();

					thisCharacter->SetIsEnemy(true);
					thisCharacter->LoadStats("resources/xml/CharacterStats.xml");
				}
				else if (job == "Radzilla")
				{
					// Spawn a Radzilla here
					thisCharacter = new Radzilla();
					thisCharacter->SetLevel(level);
					thisCharacter->SetPosition(SGD::Point((float)X, float(Y)));

					newWeapon = new Flamethrower();

					thisCharacter->SetIsEnemy(true);
				}
				else if (job == "Jane")
				{
					// Spawn Jane here
					thisCharacter = new Jane();
					thisCharacter->SetPosition(SGD::Point((float)X, float(Y)));

					thisCharacter->SetIsEnemy(true);
				}

				if (newWeapon)
				{
					if (spawn->Attribute("Quality"))
						newWeapon->SetPrefix(GunPrefix(atoi(spawn->Attribute("Quality"))));

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

					thisCharacter->SetWeapon(newWeapon);
					newWeapon->Release();
				}

				// Setting event
				if (spawn->Attribute("Event"))
					thisCharacter->SetEvent(spawn->Attribute("Event"));
				else
					thisCharacter->SetEvent("");

				NPC * newNPC = new NPC(thisCharacter, GameplayState::GetInstance()->player, &GameplayState::GetInstance()->npcs);
				thisCharacter->Release();

				int diff = Game::GetInstance()->playerDifficulty;

				if (job == "Gatling Gun Guy")
				{
					newNPC->SetBossType(BossType::GATLING_GUN);
					thisCharacter->SetHealth(thisCharacter->GetHealth() + (1250.0f * (diff + 1)));
					thisCharacter->SetCurrHealth(thisCharacter->GetHealth());
				}
				else if (job == "Buzzsaw Berserker")
				{
					newNPC->SetBossType(BossType::BUZZSAW);
					thisCharacter->SetHealth(thisCharacter->GetHealth() + (1750.0f * (diff + 1)));
					thisCharacter->SetCurrHealth(thisCharacter->GetHealth());
				}
				else if (job == "Radzilla")
				{
					newNPC->SetBossType(BossType::RADZILLA);
					thisCharacter->SetHealth(thisCharacter->GetHealth() + (1500.0f * (diff + 1)));
					thisCharacter->SetCurrHealth(thisCharacter->GetHealth());
				}
				else if (job == "Jane")
				{
					newNPC->SetBossType(BossType::JANE);
					thisCharacter->SetHealth(thisCharacter->GetHealth() + (5000.0f * (diff + 1)));
					thisCharacter->SetCurrHealth(thisCharacter->GetHealth());
				}

				// Setting anchor
				int anchorIndex = -1;
				if (spawn->Attribute("AnchorIndex"))
				{
					spawn->Attribute("AnchorIndex", &anchorIndex);
				}
				anchorIndices.push_back(anchorIndex);

				if (spawn->Attribute("AnchorPointX"))
				{
					int anchorPointX = 0;
					int anchorPointY = 0;
					spawn->Attribute("AnchorPointX", &anchorPointX);
					spawn->Attribute("AnchorPointY", &anchorPointY);
					Entity * newAnchor = new Entity();

					newAnchor->SetPosition(SGD::Point((float)(anchorPointX * tileWidth), (float)(anchorPointY * tileHeight)));

					newNPC->SetAnchor(newAnchor);
					newAnchor->Release();
				}

				newNPC->SetRepRequirement(50 + 500 * level);

				// If they're a member of your party, set their anchor to the player, and add them to the party
				if (spawn->Attribute("PartyMate"))
				{
					newNPC->SetAnchor(GameplayState::GetInstance()->player->GetCharacter());
					newNPC->getCharacter()->SetPosition({ GameplayState::GetInstance()->player->GetPosition().x, GameplayState::GetInstance()->player->GetPosition().y + 66 });
					newNPC->getCharacter()->SetInParty(true);
					GameplayState::GetInstance()->player->party.push_back(newNPC);
				}
				NPCVector.push_back(newNPC);
			}
			else if (spawnType == "Weapon")
			{
				spawn->Attribute("X", &X);
				spawn->Attribute("Y", &Y);
				spawn->Attribute("Level", &level);
				string weaponType = spawn->Attribute("Type");

				Weapon * newWeapon = nullptr;

				if (weaponType == "Melee")
				{
					newWeapon = Game::GetInstance()->CreateWeapon(SGD::Point((float)X * tileWidth, (float)Y * tileHeight), Weapon::GunType::meleeWeapon, level);
				}
				else if (weaponType == "Pistol")
				{

					newWeapon = Game::GetInstance()->CreateWeapon(SGD::Point((float)X * tileWidth, (float)Y * tileHeight), Weapon::GunType::Pistol, level);
				}
				else if (weaponType == "Shotgun")
				{

					newWeapon = Game::GetInstance()->CreateWeapon(SGD::Point((float)X * tileWidth, (float)Y * tileHeight), Weapon::GunType::Shotgun, level);
				}
				else if (weaponType == "Sniper")
				{

					newWeapon = Game::GetInstance()->CreateWeapon(SGD::Point((float)X * tileWidth, (float)Y * tileHeight), Weapon::GunType::SniperRifle, level);
				}

				if (newWeapon)
				{
					GameplayState::GetInstance()->weapons.push_back(newWeapon);
					//CreateWeaponMsg * newMsg = new CreateWeaponMsg(newWeapon);
					//SGD::MessageManager::GetInstance()->QueueMessage(newMsg);
				}
			}
			else if (spawnType == "Trap")
			{
				spawn->Attribute("X", &X);
				spawn->Attribute("Y", &Y);
				spawn->Attribute("Level", &level);
				string trapType = spawn->Attribute("Type");
				Hazards* newHazard;

				if (trapType == "Radiation Pool")
				{
					// Spawn Radiation Pool
					newHazard = new RadiationPool();
					newHazard->SetPosition(SGD::Point((float)X * tileWidth, (float)Y * tileHeight));
				}
				else if (trapType == "Bear Trap")
				{
					// Spawn Bear Trap
					newHazard = new BearTrap();
					newHazard->SetPosition(SGD::Point((float)X * tileWidth, (float)Y * tileHeight));
				}
				else if (trapType == "Pitfall")
				{
					// Spawn Pitfall
					newHazard = new Pitfall();
					newHazard->SetPosition(SGD::Point((float)X * tileWidth, (float)Y * tileHeight));
				}

				GameplayState::GetInstance()->m_pEntities->AddEntity(newHazard, PICKUPSandHAZARDS);
				newHazard->Release();
			}

			// Stepping to the next spawn
			spawn = spawn->NextSiblingElement();
		}

		// After all NPC entities have been spawned, set their anchor points and then make a message to create them

		for (int i = 0; i < (int)NPCVector.size(); i++)
		{
			if (anchorIndices[i] > -1)
			{
				// Setting each character's anchor
				NPCVector[i]->SetAnchor(NPCVector[anchorIndices[i]]->getCharacter());
			}

			// Creating the NPC, based upon its isEnemy bool

			if (NPCVector[i]->getCharacter()->GetIsEnemy())
			{
				//CreateEnemyMsg * newMsg = new CreateEnemyMsg(NPCVector[i]);

				//Instead of spawning enemies, add them to the spawnList, so that they can be managed
				//SGD::MessageManager::GetInstance()->QueueMessage(newMsg);

				Spawn* newSpawn = new Spawn(NPCVector[i], (int)NPCVector[i]->getCharacter()->GetPosition().x, (int)NPCVector[i]->getCharacter()->GetPosition().y);

				SpawnList::GetInstance()->AddSpawn(newSpawn);
			}
			else
			{
				CreateNPCMsg * newMsg = new CreateNPCMsg(NPCVector[i]);
				SGD::MessageManager::GetInstance()->QueueMessage(newMsg);
			}
		}

		GameplayState *gState = GameplayState::GetInstance();

		// Moving to objectives
		element = element->NextSiblingElement();

#ifndef IGNORE_OBJECTIVES
		// Stepping into the list
		TiXmlElement *objective = element->FirstChildElement();

		// Adding all objectives
		while (objective)
		{
			gState->objectives->AddObjective(new Objective(objective->Attribute("StringToDisplay"), objective->Attribute("StringToCompare"), atoi(objective->Attribute("LocationX")), atoi(objective->Attribute("LocationY"))));

			// Step to the next objective
			objective = objective->NextSiblingElement();
		}

		// Setting the current active objective
		if (pRoot->Attribute("currentObjective"))
		{
			ObjectiveList::GetInstance()->SetCurrentObjective(pRoot->Attribute("currentObjective"));
		}
		else
			ObjectiveList::GetInstance()->SetCurrentObjective("");
#endif
	}
	camera.GetMatrix();

	Game::GetInstance()->LoadSave = false;
}

void World::Exit()
{
	for (unsigned int i = 0; i < tileTypes.size(); i++)
	{
		delete tileTypes[i];
		tileTypes[i] = nullptr;
	}

	for (unsigned int i = 0; i < world.size(); i++)
	{
		delete world[i];
		world[i] = nullptr;
	}

	world.clear();
	tileTypes.clear();
}