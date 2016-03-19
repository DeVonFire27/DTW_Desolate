#include "globals.h"
#include "LoadGameState.h"
#include "MainMenuState.h"
#include "GameplayState.h"
#include "Game.h"
#include "NewGameState.h"
#include "BitmapFont.h"
#include "TinyXML\tinyxml.h"
#include <sstream>
#include <fstream>

//For checking if a file exists
#include <Shlobj.h>
#include <Shlwapi.h>
#pragma comment( lib, "shlwapi.lib")

LoadGameState* LoadGameState::GetInstance(void)
{
	static LoadGameState s_Instance;
	return &s_Instance;
}

void LoadGameState::Enter(void)
{
	GameplayState::GetInstance()->tutorialChosen = true;
	GameplayState::GetInstance()->tutorial = false;

	//clear vectors
	files.clear();
	currObjective.clear();
	charClass.clear();
	saveLevel.clear();
	saveTime.clear();
	saveDifficulty.clear();

	//load textures
	next = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/next.png");
	previous = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/previous.png");

	gunslinger = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Gunslinger.png");
	sniper = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Sniper.png");
	cyborg = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Cyborg.png");
	medic = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Medic.png");
	brawler = SGD::GraphicsManager::GetInstance()->LoadTexture("resources/graphics/Brawler.png");

	GameplayState::GetInstance()->tutorial = false;

	ifstream ifs("resources/Class Info.txt");
	if (ifs.is_open())
	{
		string charClass, charDescription, charStats, startingWeapon;
		stringstream classInfo;
		for (int i = 0; i < 5; i++)
		{
			getline(ifs, charClass, '\t');
			getline(ifs, charDescription, '\t');
			getline(ifs, charStats, '\t');
			getline(ifs, startingWeapon, '\n');

			classInfo << charClass << '\n' << charDescription << '\n' << charStats << '\n' << startingWeapon << '\n';
			if (i == 0)
				NewGameState::GetInstance()->gunslingerInfo = classInfo.str();
			else if (i == 1)
				NewGameState::GetInstance()->sniperInfo = classInfo.str();
			else if (i == 2)
				NewGameState::GetInstance()->brawlerInfo = classInfo.str();
			else if (i == 3)
				NewGameState::GetInstance()->medicInfo = classInfo.str();
			else if (i == 4)
				NewGameState::GetInstance()->cyborgInfo = classInfo.str();
			classInfo.str("");
		}
	}
	ifs.close();

	//Go through Saves folder
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

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
				newName[name.length()] = '\0';

				string truncName = newName;
				truncName.erase(truncName.find_first_of("."));

				//Load up the current objective
				string newPath = appPath;
				newPath.append(newName);

				TiXmlDocument doc(newPath.c_str());
				if (doc.LoadFile())
				{
					totalSaves++;

					TiXmlElement *xRoot = doc.FirstChildElement();
					currObjective.push_back(xRoot->Attribute("currentObjective"));

					TiXmlElement *xSpawns = xRoot->FirstChildElement("Spawns");
					TiXmlElement *xSpawn = xSpawns->FirstChildElement("Spawn");
					for (; strcmp(xSpawn->Attribute("SpawnType"), "Player") != 0; xSpawn = xSpawn->NextSiblingElement("Spawn")); //iterate xSpawn until Player tag
					charClass.push_back(xSpawn->Attribute("Class"));

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

					//Load the save file
					files.push_back(truncName);

					doc.Clear();
				}
			}
		}
	} while (FindNextFile(hFind, &FindFileData) != 0);
}

void LoadGameState::Exit(void)
{
	files.clear();
	SGD::GraphicsManager::GetInstance()->UnloadTexture(previous);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(next);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(cyborg);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(medic);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(gunslinger);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(sniper);
	SGD::GraphicsManager::GetInstance()->UnloadTexture(brawler);
}

bool LoadGameState::Input(void)
{
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();

	if (Game::GetInstance()->IsMenuBackInput())
	{
		Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
		return true;
	}
	if (Game::GetInstance()->IsMenuConfirmInput())
	{
		if (m_nCursor == 0)
		{
			if (totalSaves == 0) return true;

			Game::GetInstance()->LoadSave = true;
			Game::GetInstance()->loadFile = files[selectedSave];

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

			// Loading the world
			char pathbuffer[100];
			wcstombs_s(nullptr, pathbuffer, path.c_str(), 200);

			TiXmlDocument doc(pathbuffer);

			if (doc.LoadFile())
			{
				TiXmlElement *pRoot = doc.FirstChildElement();
				TiXmlElement *spawns = pRoot->FirstChildElement("Spawns");
				TiXmlElement *spawn = spawns->FirstChildElement("Spawn");
				for (; strcmp(spawn->Attribute("SpawnType"), "Player") != 0; spawn = spawn->NextSiblingElement("Spawn"));

				Game::GetInstance()->playerClass = spawn->Attribute("Class");
				//TODO: Loading difficulty needs to be loading an int rather than a string now
				//Game::GetInstance()->playerDifficulty = spawn->Attribute("Difficulty");
			}

			doc.Clear();
			GameplayState::GetInstance()->newGame = false;
			Game::GetInstance()->ChangeState(GameplayState::GetInstance());
			return true;
		}
		else if (m_nCursor == 1)
		{
			if (totalSaves == 0) return true;

			wchar_t *appDataPath;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &appDataPath);
			wstring path = appDataPath;
			path.append(L"\\Desolate\\");

			char fileToDelete[100];

			wcstombs_s(nullptr, fileToDelete, path.c_str(), 100);

			string thisFileToDelete = fileToDelete;
			thisFileToDelete += files[selectedSave] + ".xml";

			std::vector<string>::iterator iter = files.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			files.erase(iter);

			iter = currObjective.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			currObjective.erase(iter);

			iter = charClass.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			charClass.erase(iter);

			iter = saveLevel.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			saveLevel.erase(iter);

			iter = saveTime.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			saveTime.erase(iter);

			iter = saveDifficulty.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			saveDifficulty.erase(iter);

			totalSaves--;

			selectedSave--;

			if (selectedSave < 0)
				selectedSave = 0;

			remove(thisFileToDelete.c_str());
		}
		else if (m_nCursor == 2)
		{
			Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
			return true;
		}
	}
	//Select button with mouse
	SGD::Point mousePos = pInput->GetMousePosition();
	if (mousePos.x > 250.0f && mousePos.x < 350.0f && mousePos.y > 450 && mousePos.y < 650 && pInput->GetMouseMovement() != SGD::Vector::Zero)
	{
		m_nCursor = (int)((mousePos.y - 450) / 50.0f);
	}

	//clicks
	if (pInput->IsKeyPressed(SGD::Key::LButton) || pInput->IsButtonPressed(0, 1))
	{
		int winWidth = Game::GetInstance()->GetScreenWidth();
		int winHeight = Game::GetInstance()->GetScreenHeight();

		if (mousePos.x > (winWidth / 10.0f) * 7.5f && mousePos.y > (winHeight / 6.0f) * 2 && mousePos.y < (winHeight / 6.0f) * 4)
		{
			//right pane
			if (selectedSave < (int)files.size() - 1) selectedSave++;
		}
		else if (mousePos.x < (winWidth / 10.0f) * 3.5f && mousePos.y >(winHeight / 6.0f) * 2 && mousePos.y < (winHeight / 6.0f) * 4)
		{
			//left pane
			if (selectedSave > 0) selectedSave--;
		}
		// Load
		else if ((mousePos.x > 250.0f && mousePos.x < 350.0f && mousePos.y > 450 && mousePos.y < 500)
			|| (mousePos.x > winWidth / 4.0f && mousePos.y > winHeight / 4.0f && mousePos.x < (winWidth / 4.0f) * 3 && mousePos.y < (winHeight / 4.0f) * 3))
		{
			if (totalSaves == 0) return true;

			Game::GetInstance()->LoadSave = true;
			Game::GetInstance()->loadFile = files[selectedSave];

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

			// Loading the world
			char pathbuffer[100];
			wcstombs_s(nullptr, pathbuffer, path.c_str(), 200);

			TiXmlDocument doc(pathbuffer);

			if (doc.LoadFile())
			{
				TiXmlElement *pRoot = doc.FirstChildElement();
				TiXmlElement *spawns = pRoot->FirstChildElement("Spawns");
				TiXmlElement *spawn = spawns->FirstChildElement("Spawn");
				for (; strcmp(spawn->Attribute("SpawnType"), "Player") != 0; spawn = spawn->NextSiblingElement("Spawn"));

				Game::GetInstance()->playerClass = spawn->Attribute("Class");
				//TODO: Loading difficulty needs to be loading an int rather than a string now
				//Game::GetInstance()->playerDifficulty = spawn->Attribute("Difficulty");
			}

			doc.Clear();
			GameplayState::GetInstance()->newGame = false;

			Game::GetInstance()->ChangeState(GameplayState::GetInstance());
			return true;
		}
		// Delete
		else if (mousePos.x > 250.0f && mousePos.x < 350.0f && mousePos.y > 500 && mousePos.y < 550)
		{
			if (totalSaves == 0) return true;

			wchar_t *appDataPath;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, NULL, NULL, &appDataPath);
			wstring path = appDataPath;
			path.append(L"\\Desolate\\");

			char fileToDelete[100];

			wcstombs_s(nullptr, fileToDelete, path.c_str(), 100);

			string thisFileToDelete = fileToDelete;
			thisFileToDelete += files[selectedSave] + ".xml";

			std::vector<string>::iterator iter = files.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			files.erase(iter);

			iter = currObjective.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			currObjective.erase(iter);

			iter = charClass.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			charClass.erase(iter);

			iter = saveLevel.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			saveLevel.erase(iter);

			iter = saveTime.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			saveTime.erase(iter);

			iter = saveDifficulty.begin();

			for (int i = 0; i < selectedSave; i++)
			{
				iter++;
			}

			saveDifficulty.erase(iter);

			totalSaves--;

			selectedSave--;

			if (selectedSave < 0)
				selectedSave = 0;

			remove(thisFileToDelete.c_str());
		}

		// Back
		else if (mousePos.x > 250.0f && mousePos.x < 350.0f && mousePos.y > 550 && mousePos.y < 600)
		{
			Game::GetInstance()->ChangeState(MainMenuState::GetInstance());
			return true;
		}
	}

	//left and right to change selected save
	if (selectedSave > 0 && (InputManager::GetInstance()->IsKeyPressed(SGD::Key::LButton) || pInput->IsButtonPressed(0, 1)) && InputManager::GetInstance()->GetMousePosition().IsWithinRectangle({ 50, 450, 130, 555 }))
		selectedSave--;
	else if (selectedSave < (int)files.size() - 1 && (InputManager::GetInstance()->IsKeyPressed(SGD::Key::LButton) || pInput->IsButtonPressed(0, 1)) && InputManager::GetInstance()->GetMousePosition().IsWithinRectangle({ 650, 450, 730, 555 }))
		selectedSave++;

	if (Game::GetInstance()->IsMenuDownInput())
	{
		m_nCursor++;
		if (m_nCursor > 2)
			m_nCursor = 0;
	}
	else if (Game::GetInstance()->IsMenuUpInput())
	{
		m_nCursor--;
		if (m_nCursor < 0)
			m_nCursor = 2;
	}
	else if (Game::GetInstance()->IsMenuLeftInput())
	{
		if (selectedSave > 0)
			selectedSave--;
	}
	else if (Game::GetInstance()->IsMenuRightInput())
	{
		if (selectedSave < (int)files.size() - 1)
			selectedSave++;
	}

	return true;
}

void LoadGameState::Update(float elapsedTime)
{

}

void LoadGameState::Render(void)
{
	int winWidth = Game::GetInstance()->GetScreenWidth();
	int winHeight = Game::GetInstance()->GetScreenHeight();

	Game::GetInstance()->GetFontTitle()->Draw("Load Game", Game::GetInstance()->GetScreenWidth() - 250, 50, 1.f, SGD::Color::White);

	//SGD::GraphicsManager::GetInstance()->DrawTextureSection(next, { 650, 450 }, { 20, 10, 100, 115 });
	//SGD::GraphicsManager::GetInstance()->DrawTextureSection(previous, { 50, 450 }, { 20, 10, 100, 115 });

	//prev
	if (selectedSave - 1 >= 0 && selectedSave - 1 < (int)files.size())
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle(
		{
			winWidth / 50.0f, (winHeight / 6.0f) * 2, (winWidth / 10.0f) * 3.5f, (winHeight / 6.0f) * 4 },
			SGD::Color::White,
			SGD::Color::Black
			);

			SGD::GraphicsManager::GetInstance()->DrawTexture(
				GetClassPic(charClass[selectedSave - 1]),
				{ winWidth / 50.0f + 20, (winHeight / 6.0f) * 2.43f }
			);

			SGD::GraphicsManager::GetInstance()->DrawString(
				files[selectedSave - 1].c_str(),
				{ winWidth / 50.0f + 20, (winHeight / 6.0f) * 2.15f },
				SGD::Color::Black
				);

			SGD::GraphicsManager::GetInstance()->DrawString(
				currObjective[selectedSave - 1].c_str(),
				{ winWidth / 50.0f + 20, (winHeight / 6.0f) * 3.65f },
				SGD::Color::Black
				);
	}

	//next
	if (selectedSave + 1 >= 0 && selectedSave + 1 < (int)files.size())
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle(
		{ (winWidth / 10.0f) * 7.5f, (winHeight / 6.0f) * 2, winWidth - (winWidth / 50.0f), (winHeight / 6.0f) * 4 },
		SGD::Color::White,
		SGD::Color::Black
		);

		SGD::GraphicsManager::GetInstance()->DrawTexture(
			GetClassPic(charClass[selectedSave + 1]),
			{ (winWidth / 10.0f) * 7.0f, (winHeight / 6.0f) * 2.43f }
		);

		SGD::GraphicsManager::GetInstance()->DrawString(
			files[selectedSave + 1].c_str(),
			{ (winWidth / 10.0f) * 7.0f, (winHeight / 6.0f) * 2.15f },
			SGD::Color::Black
			);

		SGD::GraphicsManager::GetInstance()->DrawString(
			currObjective[selectedSave + 1].c_str(),
			{ (winWidth / 10.0f) * 7.0f, (winHeight / 6.0f) * 3.65f },
			SGD::Color::Black
			);
	}

	//curr
	if (selectedSave >= 0 && selectedSave < (int)files.size())
	{
		SGD::GraphicsManager::GetInstance()->DrawRectangle(
		{ winWidth / 4.0f, winHeight / 4.0f, (winWidth / 4.0f) * 3, (winHeight / 4.0f) * 3 },
		SGD::Color::White,
		SGD::Color::Blue
		);

		SGD::GraphicsManager::GetInstance()->DrawTexture(
			GetClassPic(charClass[selectedSave]),
			{ winWidth / 4.0f + 20, (winHeight / 6.0f) * 2.43f }
		);

		SGD::GraphicsManager::GetInstance()->DrawString(
			files[selectedSave].c_str(),
			{ winWidth / 4.0f + 20, (winHeight / 6.0f) * 1.65f },
			SGD::Color::Black
			);

		SGD::GraphicsManager::GetInstance()->DrawString(
			currObjective[selectedSave].c_str(),
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

	if (totalSaves == 0)
	{
		DrawButton({ winWidth / 2.75f, winHeight / 4.0f }, *Game::GetInstance()->GetFontButton(), "NO SAVED GAMES");
	}

	//if (m_nCursor == 0)
	//SGD::GraphicsManager::GetInstance()->DrawString("--->", { 210.0f, 470.0f }, { 0, 255, 0 });
	//else if (m_nCursor == 1)
	//SGD::GraphicsManager::GetInstance()->DrawString("--->", { 210.0f, 540.0f }, { 0, 255, 0 });
	Game::GetInstance()->GetFontTitle()->Draw("I", 240, 447 + 50 * m_nCursor, 0.8f, { (unsigned char)(Game::GetInstance()->opacity * 255), 255, 255, 255 });

	DrawButton({ 250, 550 }, *Game::GetInstance()->GetFontButton(), "Back");
	DrawButton({ 250, 500 }, *Game::GetInstance()->GetFontButton(), "Delete");
	DrawButton({ 250, 450 }, *Game::GetInstance()->GetFontButton(), "Load", (selectedSave >= 0 && selectedSave < (int)files.size()) ? SGD::Color(255, 25, 10) : SGD::Color(130, 130, 130));

}

SGD::HTexture LoadGameState::GetClassPic(string charClass)
{
	if (charClass == "Gunslinger") return gunslinger;
	else if (charClass == "Brawler") return brawler;
	else if (charClass == "Medic") return medic;
	else if (charClass == "Cyborg") return cyborg;
	else if (charClass == "Sniper") return sniper;
	else return SGD::INVALID_HANDLE;
}
