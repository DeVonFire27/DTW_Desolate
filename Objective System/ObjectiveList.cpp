#include "globals.h"
#include "ObjectiveList.h"
#include "Objective.h"

#include "../GameplayState.h"
#include "../CreditsState.h"

#include "../Game.h"
#include "../EventProc/EventProc.h"
#include "../BitmapFont.h"

#include "../Dialogue System/Dialogue.h"

ObjectiveList* ObjectiveList::GetInstance()
{
	static ObjectiveList s_Instance;
	return &s_Instance;
}

// Returns the current objective's text
string ObjectiveList::GetCurrentObjectiveText()
{
	return objectiveList[currObjective]->GetObjectiveText();
}

void ObjectiveList::Render()
{
	SGD::GraphicsManager* gManager = SGD::GraphicsManager::GetInstance();

	// Topbar code
	gManager->DrawRectangle({ 0.0f, 30.0f, (float)Game::GetInstance()->GetScreenWidth(), 55.0f }, SGD::Color(100, 0, 0, 0));
	if (currObjective < (int)objectiveList.size())
	{
		Game::GetInstance()->GetFont()->Draw(objectiveList[currObjective]->GetObjectiveText().c_str(), (int)SCREENWIDTH / 2 - (objectiveList[currObjective]->GetObjectiveText().length() * 8 / 2), 30, 1.0f, SGD::Color());
	}

	// Rendering objective complete!
	if (sinceNewObjective < 3.0f && !GameplayState::GetInstance()->GetWin())
	{
		gManager->DrawRectangle(SGD::Rectangle(SCREENWIDTH / 2 + 115, 40, SCREENWIDTH - 20, 150), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);


		//Game::GetInstance()->GetFont()->Draw("New Objective", WINDOW_WIDTH / 2 - 100, WINDOW_HEIGHT / 2 - 35, 1.0f, SGD::Color(255, 255, 255));
		//gManager->DrawString("New Objective!", SGD::Point(SCREENWIDTH / 2 - ((14 * 12) / 2), SCREENHEIGHT / 2 - (12 / 2)));
		Game::GetInstance()->GetFont()->Draw("Objective Complete!\n   Rep + 500", (int)SCREENWIDTH / 2 + 135, 60
			, 1.0f, SGD::Color());
		//string s = "Objective Complete !\n   Rep + 500";
		//GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(s);
		//GameplayState::GetInstance()->gameplayDialogue->DrawString();
	}

	if (sinceNewObjective > 3.0f)
		GameplayState::GetInstance()->gameplayDialogue->StopDrawing();
}

void ObjectiveList::Update(float dt)
{
	sinceNewObjective += dt;
}

void ObjectiveList::AddObjective(Objective* newObjective)
{
	// Adding the objective to the objectives vector
	objectiveList.push_back(newObjective);

	// Subscribing the player for the new objective
	string objectiveString = newObjective->GetCompareString();
	char * cstr = new char[objectiveString.length() + 1];
	strcpy_s(cstr, objectiveString.length() + 1, objectiveString.c_str());
	EventProc::GetInstance()->Subscribe(GameplayState::GetInstance()->player, cstr);

	delete [] cstr;
}

void ObjectiveList::SetCurrentObjective(string toSet)
{
	if (toSet == "")
	{
		currObjective = 0;
		return;
	}

	for (unsigned int i = 0; i < objectiveList.size(); i++)
	{
		if (toSet == objectiveList[i]->GetCompareString())
		{
			currObjective = i + 1;
		}
	}
}

bool ObjectiveList::NextObjective(string completed)
{
	if (unsigned int(currObjective) < objectiveList.size() && GameplayState::GetInstance()->player)
	{
		if (objectiveList[currObjective]->GetCompareString() == completed)
		{
			SGD::AudioManager::GetInstance()->PlayAudio(GameplayState::GetInstance()->success);
			// If we just spoke to the mayor of jamestown, loop through all the tiles and delete the ones that are marked as "Janes Compound Wall", allowing for passing
			if(completed == "Talked to the mayor of Jamestown")
			{
				int tileRows = World::GetInstance()->GetTileLayers()[1]->layerRows;
				for (int r = 0; r < World::GetInstance()->GetTileLayers()[1]->layerRows; r++)
				{
					int tileColumns = World::GetInstance()->GetTileLayers()[1]->layerColumns;
					for (int c = 0; c < World::GetInstance()->GetTileLayers()[1]->layerColumns; c++)
					{
						TileLayer * thisTileLayer = World::GetInstance()->GetTileLayers()[1];
						if(World::GetInstance()->GetTileLayers()[1]->tiles[r * tileColumns + c]->event == "Janes Compound Wall")
						{
							World::GetInstance()->GetTileLayers()[1]->tiles[r * tileColumns + c]->tileNumber = -1;
							World::GetInstance()->GetTileLayers()[1]->tiles[r * tileColumns + c]->X = -1;
							World::GetInstance()->GetTileLayers()[1]->tiles[r * tileColumns + c]->Y = -1;
							World::GetInstance()->GetTileLayers()[1]->tiles[r * tileColumns + c]->isPassable = true;
						}
					}
				}
			}
			currObjective++;
			GameplayState::GetInstance()->player->SetReputation(GameplayState::GetInstance()->player->GetReputation() + 500);
			sinceNewObjective = 0.f;

			return true;
		}
	}
	return false;
}

void ObjectiveList::RemoveAll()
{
	for (unsigned int i = 0; i < objectiveList.size(); i++)
	{
		delete objectiveList[i];
		objectiveList[i] = nullptr;
	}

	objectiveList.clear();
}