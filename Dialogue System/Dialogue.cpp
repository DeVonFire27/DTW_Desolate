#include "globals.h"
#include "Dialogue.h"
#include "../GameplayState.h"
#include "../../SGD Wrappers/SGD_GraphicsManager.h"
#include "../../SGD Wrappers/SGD_InputManager.h"
#include "../OptionsState.h"
#include "../BitmapFont.h"
#include <cstring>
#include "../Tile System/World.h"
#include "../Game.h"
#include "../Objective System/ObjectiveList.h"
#include "../Agents/Player.h"
#include "../Objective System/Objective.h"


Dialogue::Dialogue(Character* dialoguesCharacter)
{
	SetCharacter(dialoguesCharacter);
	currSelection = 0;
}
/*virtual*/ Dialogue::~Dialogue(void)
{
	// ensure the character has been deleted
	if (thisCharacter)
		SetCharacter(nullptr);
}

void Dialogue::SetCharacter(Character* character)
{
	if (thisCharacter)
	{
		thisCharacter->Release();
		_ASSERTE(_CrtCheckMemory());
		thisCharacter = nullptr;
	}
	if (character)
	{
		thisCharacter = character;
		thisCharacter->AddRef();
	}
}

void Dialogue::DisplayPopUpDialogue()
{
	SGD::GraphicsManager * gManager = SGD::GraphicsManager::GetInstance();
	Player* player = GameplayState::GetInstance()->player;

	// If this is not an enemy, give it a chance to talk randomly at all times
	if (thisCharacter != GameplayState::GetInstance()->player->GetCharacter())
	{
		if (!thisCharacter->GetIsEnemy())
		{
			// Choosing a random string to display, if they're not in your party
			if (!thisCharacter->GetInParty())
			{
				if (chosenPopupDialogue == "")
					chosenPopupDialogue = popupDialogueOptions[rand() % popupDialogueOptions.size()];
			}

			float stringSize = Game::GetInstance()->GetFont()->MeasureString(chosenPopupDialogue.c_str());

			SGD::Point dialogueLocation = thisCharacter->GetPosition();

			// Offsetting by camera's location
			dialogueLocation.x += World::GetInstance()->GetCamera()->GetPosition().x;
			dialogueLocation.y += World::GetInstance()->GetCamera()->GetPosition().y;

			// Offsetting by half the character's width, to place in the center of the character
			dialogueLocation.x += thisCharacter->GetSize().width / 2;

			// Offsetting by half the string size, times scale
			dialogueLocation.x -= (stringSize * .75f) / 2 - 10;

			// Offsetting the dialogue to above the character
			dialogueLocation.y -= 40;

			// Drawing popup rect large enough to fit the string over this character
			gManager->DrawRectangle(SGD::Rectangle(dialogueLocation, SGD::Size(stringSize, 30.0f)), SGD::Color(120, 0, 0, 0));

			// Drawing a random popup string
			Game::GetInstance()->GetFont()->Draw(
				chosenPopupDialogue.c_str(),
				(int)dialogueLocation.x, (int)dialogueLocation.y,
				1.0f,
				SGD::Color());
		}
		// If this is an enemy, it only talks when it is in attack mode
		else if (thisCharacter->GetCurrHealth() > 0 && FindControllingNPC(thisCharacter)->GetState() == State::ATTACK)
		{
			// Choosing a random string to display
			if (chosenPopupDialogue == "")
				chosenPopupDialogue = popupDialogueOptions[rand() % popupDialogueOptions.size()];

			float stringSize = Game::GetInstance()->GetFont()->MeasureString(chosenPopupDialogue.c_str());

			//int stringSize = (int)Game::GetInstance()->GetFont()->MeasureString(chosenPopupDialogue.c_str());
			SGD::Point dialogueLocation = thisCharacter->GetPosition();

			// Offsetting by camera's location
			dialogueLocation.x += World::GetInstance()->GetCamera()->GetPosition().x;
			dialogueLocation.y += World::GetInstance()->GetCamera()->GetPosition().y;

			// Offsetting by half the character's width, to place in the center of the character
			dialogueLocation.x += thisCharacter->GetSize().width / 2;

			// Offsetting by half the string size, times scale
			dialogueLocation.x -= (stringSize * .75f) / 2;

			// Offsetting the dialogue to above the character
			dialogueLocation.y -= 40;

			// Drawing popup rect large enough to fit the string over this character
			gManager->DrawRectangle(SGD::Rectangle(dialogueLocation, SGD::Size(stringSize, 30.0f)), SGD::Color(120, 0, 0, 0));

			// Drawing a random popup string
			Game::GetInstance()->GetFont()->Draw(
				chosenPopupDialogue.c_str(),
				(int)dialogueLocation.x, (int)dialogueLocation.y,
				1.0f,
				SGD::Color());

			//// Drawing popup rect over this character
			//gManager->DrawRectangle(SGD::Rectangle(
			//	thisCharacter->GetPosition().x - 60 + World::GetInstance()->GetCamera()->GetPosition().x + thisCharacter->GetSize().width / 2,
			//	thisCharacter->GetPosition().y - 40 + World::GetInstance()->GetCamera()->GetPosition().y,
			//	thisCharacter->GetPosition().x + 60 + World::GetInstance()->GetCamera()->GetPosition().x + thisCharacter->GetSize().width / 2,
			//	thisCharacter->GetPosition().y - 10 + World::GetInstance()->GetCamera()->GetPosition().y),
			//	SGD::Color(120, 0, 0, 0));

			//// Choosing a random string to display
			//if (chosenPopupDialogue == "")
			//	chosenPopupDialogue = popupDialogueOptions[rand() % popupDialogueOptions.size()];


			//int stringSize = Game::GetInstance()->GetFont()->MeasureString(chosenPopupDialogue.c_str());

			//// Drawing a random popup string
			//Game::GetInstance()->GetFont()->Draw(
			//	chosenPopupDialogue.c_str(),
			//	thisCharacter->GetPosition().x - ((stringSize / 2) * .75) + World::GetInstance()->GetCamera()->GetPosition().x + thisCharacter->GetSize().width / 2,
			//	thisCharacter->GetPosition().y - 40 + World::GetInstance()->GetCamera()->GetPosition().y,
			//	.75,
			//	SGD::Color());
		}
	}
}

void Dialogue::DisplayRecruitmentDialogue()
{
	SGD::GraphicsManager * gManager = SGD::GraphicsManager::GetInstance();
	if (GameplayState::GetInstance()->tutorialChosen)
	{
		Player* player = GameplayState::GetInstance()->player;

		if (!GameplayState::GetInstance()->tutorial)
		{
			// Handle all dialogue events (objectives) here
			if (thisCharacter->GetEvent() == "Talked to the mayor of Jamestown"/* && ObjectiveList::GetInstance()->NextObjective(thisCharacter->GetEvent())*/)
			{
				// Drawing conversation rect
				gManager->DrawRectangle(SGD::Rectangle(200, SCREENHEIGHT - 150, SCREENWIDTH - 225, SCREENHEIGHT - 50), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);

				// Drawing the character's portrait
				gManager->DrawTexture(thisCharacter->GetDialogueIcon(), { 210, SCREENHEIGHT - 140 }, {}, {}, {}, { 0.5f, 0.5f });

				// Drawing dialogue
				stringstream recruitment;
				if (ObjectiveList::GetInstance()->GetCurrentObjectiveNumber() == 8)
					recruitment << "We opened the way to Jane's compound!\nGo and save us from her tyranny!";
				else
					recruitment << "Hello! I am the mayor of Jamestown!";

				Game::GetInstance()->GetFont()->Draw(recruitment.str().c_str(), 300, (int)(SCREENHEIGHT - 140), 1.0f, SGD::Color(255, 255, 255));

			}
			// If no event is attached, display recruitment
			else
			{
				// Drawing conversation rect
				gManager->DrawRectangle(SGD::Rectangle(200.0f, SCREENHEIGHT - 150.0f, SCREENWIDTH - 225.0f, SCREENHEIGHT - 50.0f), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);

				// Drawing the character's portrait
				gManager->DrawTexture(thisCharacter->GetDialogueIcon(), { 210, SCREENHEIGHT - 140 }, {}, {}, {}, { 0.5f, 0.5f });

				// Drawing the recruitment dialogue itself
				stringstream recruitment;
				recruitment << "Would you like to recruit \nthis level " << thisCharacter->GetLevel() << " " << thisCharacter->GetClassString() << "?";
				//GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(recruitment.str());
				//gManager->DrawString(recruitment.str().c_str(), { 300, SCREENHEIGHT - 140 }, SGD::Color(255, 255, 255));

				Game::GetInstance()->GetFont()->Draw(recruitment.str().c_str(), 300, (int)SCREENHEIGHT - 140, 1.0f, SGD::Color(255, 255, 255));
				//gManager->DrawString(recruitment.str().c_str(), { 300, SCREENHEIGHT - 140 }, SGD::Color(255, 255, 255));

				// Drawing options
				Game::GetInstance()->GetFont()->Draw("Yes", 325, (int)SCREENHEIGHT - 75, 1.0f, SGD::Color(255, 255, 255));
				//gManager->DrawString("Yes", { 325, SCREENHEIGHT - 75 }, SGD::Color(255, 255, 255));
				Game::GetInstance()->GetFont()->Draw("No", 425, (int)SCREENHEIGHT - 75, 1.0f, SGD::Color(255, 255, 255));

				// Drawing current selection
				//gManager->DrawString("->", { 300.0f + (currSelection * 100), SCREENHEIGHT - 75 }, SGD::Color(255, 255, 255));
				Game::GetInstance()->GetFontTitle()->Draw("I", 320 + (currSelection * 100), (int)SCREENHEIGHT - 78, 0.5f, SGD::Color(int(Game::GetInstance()->opacity * 255), 255, 255, 255));
			}
		}
	}
	else
	{
		// Drawing conversation rect
		gManager->DrawRectangle(SGD::Rectangle(200.0f, SCREENHEIGHT - 150.0f, SCREENWIDTH - 225.0f, SCREENHEIGHT - 50.0f), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);

		// Drawing the recruitment dialogue itself
		stringstream recruitment;
		recruitment << "Would you like to play the \ntutorial?";
		//GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(recruitment.str());
		//gManager->DrawString(recruitment.str().c_str(), { 300, SCREENHEIGHT - 140 }, SGD::Color(255, 255, 255));

		Game::GetInstance()->GetFont()->Draw(recruitment.str().c_str(), 210, (int)SCREENHEIGHT - 140, 1.0f, SGD::Color(255, 255, 255));
		//gManager->DrawString(recruitment.str().c_str(), { 300, SCREENHEIGHT - 140 }, SGD::Color(255, 255, 255));

		// Drawing options
		Game::GetInstance()->GetFont()->Draw("Yes", 325, (int)SCREENHEIGHT - 75, 1.0f, SGD::Color(255, 255, 255));
		//gManager->DrawString("Yes", { 325, SCREENHEIGHT - 75 }, SGD::Color(255, 255, 255));
		Game::GetInstance()->GetFont()->Draw("No", 425, (int)SCREENHEIGHT - 75, 1.0f, SGD::Color(255, 255, 255));

		// Drawing current selection
		Game::GetInstance()->GetFontTitle()->Draw("I", 320 + (currSelection * 100), (int)SCREENHEIGHT - 78, 0.5f, SGD::Color(int(Game::GetInstance()->opacity * 255), 255, 255, 255));
	}
}

void Dialogue::DisplayLackingRepDialogue()
{
	SGD::GraphicsManager * gManager = SGD::GraphicsManager::GetInstance();
	Player* player = GameplayState::GetInstance()->player;

	// Drawing conversation rect
	gManager->DrawRectangle(SGD::Rectangle(200, SCREENHEIGHT - 150, SCREENWIDTH - 225, SCREENHEIGHT - 50), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);

	// Drawing the character's portrait
	gManager->DrawTexture(thisCharacter->GetDialogueIcon(), { 210, SCREENHEIGHT - 140 }, {}, {}, {}, { 0.5f, 0.5f });

	// Drawing the recruitment dialogue itself
	stringstream recruitment;
	recruitment << "      You lack reputation!" << "\n      Requirement: " << FindControllingNPC(thisCharacter)->GetRepRequirement() << "\n      Your Reputation: " << player->GetReputation();
	Game::GetInstance()->GetFont()->Draw(recruitment.str().c_str(), 200, (int)SCREENHEIGHT - 140, 1.0f, SGD::Color(255, 0, 0));
	//GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(recruitment.str());
}

void Dialogue::DisplayRecruitmentSuccessDialogue()
{
	SGD::GraphicsManager * gManager = SGD::GraphicsManager::GetInstance();
	Player* player = GameplayState::GetInstance()->player;

	// Drawing conversation rect
	//gManager->DrawRectangle(SGD::Rectangle(200, SCREENHEIGHT - 150, SCREENWIDTH - 225, SCREENHEIGHT - 50), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);

	// Drawing the character's portrait

	// Drawing the recruitment dialogue itself
	stringstream recruitment;
	recruitment << "The level " << thisCharacter->GetLevel() << " " << thisCharacter->GetClassString() << " \nhas joined your party!";
	GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(recruitment.str());
	//gManager->DrawString(recruitment.str().c_str(), { 300, SCREENHEIGHT - 140 }, SGD::Color(255, 255, 255));
	gManager->DrawTexture(thisCharacter->GetDialogueIcon(), { 210, SCREENHEIGHT - 140 }, {}, {}, {}, { 0.5f, 0.5f });
}

Dialogue::DialogueResult Dialogue::Input()
{
	SGD::InputManager *pInput = SGD::InputManager::GetInstance();
	static SGD::Vector joystick_prev = pInput->GetLeftJoystick(0);
	if (GameplayState::GetInstance()->tutorialChosen)
	{
		if (!GameplayState::GetInstance()->tutorial)
		{
			if (thisCharacter->GetEvent() == "")
			{
				if (pInput->IsButtonPressed(0, 0) || pInput->IsKeyPressed(SGD::Key::LButton))
				{
					SGD::Point mouseLocation = pInput->GetMousePosition();


					SGD::Rectangle yesRect = SGD::Rectangle(315, SCREENHEIGHT - 80, 360, SCREENHEIGHT - 50);
					SGD::Rectangle noRect = SGD::Rectangle(415, SCREENHEIGHT - 80, 460, SCREENHEIGHT - 50);

					if (yesRect.IsIntersecting(SGD::Rectangle(mouseLocation.x, mouseLocation.y, mouseLocation.x + 1, mouseLocation.y + 1)))
						return Dialogue::YES;

					if (noRect.IsIntersecting(SGD::Rectangle(mouseLocation.x, mouseLocation.y, mouseLocation.x + 1, mouseLocation.y + 1)))
						return Dialogue::NO;
				}


				if ((pInput->GetLeftJoystick(0).x <= -.8f && joystick_prev.x > -.8f) || pInput->IsKeyPressed(SGD::Key::LeftArrow) || pInput->IsKeyPressed(SGD::Key::A))
					currSelection--;

				if (currSelection < 0)
					currSelection = 1;

				if ((pInput->GetLeftJoystick(0).x >= .8f && joystick_prev.x < .8f) || pInput->IsKeyPressed(SGD::Key::RightArrow) || pInput->IsKeyPressed(SGD::Key::D))
					currSelection++;

				if (currSelection > 1)
					currSelection = 0;

				if ((ARCADEBUILD == 1 && pInput->IsKeyPressed(SGD::Key::LButton)) || pInput->IsKeyPressed(SGD::Key::Enter) || pInput->IsKeyPressed(SGD::Key::E) || pInput->IsButtonPressed(0, 0))
				{
					if (currSelection == 0)
						return Dialogue::YES;
					else
						return Dialogue::NO;
				}

				if (pInput->IsKeyPressed(SGD::Key::Escape))
					return Dialogue::NO;
			}
			else
				if ((ARCADEBUILD == 1 && pInput->IsKeyPressed(SGD::Key::LButton)) || pInput->IsAnyKeyPressed())
				{
					// If this character has an objective, advance it
					GameplayState::GetInstance()->objectives->NextObjective(thisCharacter->GetEvent());
					return Dialogue::NO;
				}
		}
		else
			return Dialogue::YES;
	}
	else
	{
		if (pInput->IsKeyPressed(SGD::Key::LButton) || pInput->IsButtonDown(0, 0))
		{
			SGD::Point mouseLocation = pInput->GetMousePosition();


			SGD::Rectangle yesRect = SGD::Rectangle(315, SCREENHEIGHT - 80, 360, SCREENHEIGHT - 50);
			SGD::Rectangle noRect = SGD::Rectangle(415, SCREENHEIGHT - 80, 460, SCREENHEIGHT - 50);

			if (yesRect.IsIntersecting(SGD::Rectangle(mouseLocation.x, mouseLocation.y, mouseLocation.x + 1, mouseLocation.y + 1)))
				return Dialogue::YES;

			if (noRect.IsIntersecting(SGD::Rectangle(mouseLocation.x, mouseLocation.y, mouseLocation.x + 1, mouseLocation.y + 1)))
				return Dialogue::NO;
		}


		if ((pInput->GetLeftJoystick(0).x <= -.8f && joystick_prev.x > -.8f) || pInput->IsKeyPressed(SGD::Key::LeftArrow) || pInput->IsKeyPressed(SGD::Key::A))
			currSelection--;

		if (currSelection < 0)
			currSelection = 1;

		if ((pInput->GetLeftJoystick(0).x >= .8f && joystick_prev.x < .8f) || pInput->IsKeyPressed(SGD::Key::RightArrow) || pInput->IsKeyPressed(SGD::Key::D))
			currSelection++;

		if (currSelection > 1)
			currSelection = 0;

		if ((ARCADEBUILD == 1 && pInput->IsKeyPressed(SGD::Key::LButton)) || (pInput->IsControllerConnected(0) && pInput->IsButtonPressed(0,  0)) || pInput->IsKeyPressed(SGD::Key::Enter) || pInput->IsKeyPressed(SGD::Key::E))
		{
			if (currSelection == 0)
				return Dialogue::YES;
			else
				return Dialogue::NO;
		}

		if (((ARCADEBUILD == 1) && pInput->IsKeyPressed(SGD::Key::RButton)) || pInput->IsKeyPressed(SGD::Key::Escape) || pInput->IsButtonPressed(0, 0))
			return Dialogue::NO;
	}

	joystick_prev = pInput->GetLeftJoystick(0);
	return Dialogue::NONE;
}

void Dialogue::DrawString()
{
	if (stringToDisplay != "" && GameplayState::GetInstance()->objectives->sinceNewObjective > 3.0f)
	{
		SGD::GraphicsManager * gManager = SGD::GraphicsManager::GetInstance();
		Player* player = GameplayState::GetInstance()->player;

		// Drawing conversation rect
		gManager->DrawRectangle(SGD::Rectangle(200, SCREENHEIGHT - 150, SCREENWIDTH - 265, SCREENHEIGHT - 50), SGD::Color(120, 0, 0, 0), SGD::Color(255, 255, 255, 255), 2);



		Game::GetInstance()->GetFont()->Draw(stringToDisplay.c_str(), 210, (int)SCREENHEIGHT - 140, 1.0f, SGD::Color(255, 255, 255));


		// Drawing string
		//360 x 100
		//gManager->DrawString(stringToDisplay.c_str(), { 210, SCREENHEIGHT - 140 }, SGD::Color(255, 255, 255));
	}
}

void Dialogue::SetWeaponSwapDialogue(Weapon* weapon, Weapon* pWep)
{
	//string s = "Press 'E' To Swap " + string(string(weapon->GetPrefixName()) + ' ' + weapon->GetName()) + " with " + string(pWep->GetPrefixName()) + ' ' + pWep->GetName();

	if (weapon->GetPrefix() == GunPrefix::rusty)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << "Press 'E' To Swap " << SGD::Color(149, 69, 29) << weapon->GetPrefixName() << SGD::Color::White << " " << weapon->GetName();
	else if (weapon->GetPrefix() == GunPrefix::antique)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << "Press 'E' To Swap " << SGD::Color::Green << weapon->GetPrefixName() << SGD::Color::White << " " << weapon->GetName();
	else if (weapon->GetPrefix() == GunPrefix::standard)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << "Press 'E' To Swap " << SGD::Color::Blue << weapon->GetPrefixName() << SGD::Color::White << " " << weapon->GetName();
	else if (weapon->GetPrefix() == GunPrefix::highQuality)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << "Press 'E' To Swap " << SGD::Color::Purple << weapon->GetPrefixName() << SGD::Color::White << " " << weapon->GetName();
	else if (weapon->GetPrefix() == GunPrefix::futuristic)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << "Press 'E' To Swap " << SGD::Color::Red << weapon->GetPrefixName() << SGD::Color::White << " " << weapon->GetName();

	if (pWep->GetPrefix() == GunPrefix::rusty)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << " with " << SGD::Color(149, 69, 29) << pWep->GetPrefixName() << SGD::Color::White << " " << pWep->GetName();
	else if (pWep->GetPrefix() == GunPrefix::antique)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << " with " << SGD::Color::Green << pWep->GetPrefixName() << SGD::Color::White << " " << pWep->GetName();
	else if (pWep->GetPrefix() == GunPrefix::standard)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << " with " << SGD::Color::Blue << pWep->GetPrefixName() << SGD::Color::White << " " << pWep->GetName();
	else if (pWep->GetPrefix() == GunPrefix::highQuality)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << " with " << SGD::Color::Purple << pWep->GetPrefixName() << SGD::Color::White << " " << pWep->GetName();
	else if (pWep->GetPrefix() == GunPrefix::futuristic)
		(*Game::GetInstance()->GetFont()) << SGD::Color::White << " with " << SGD::Color::Red << pWep->GetPrefixName() << SGD::Color::White << " " << pWep->GetName();
}

void Dialogue::SetStringToDisplay(string stringToDisplay)
{
	//check each word and see if it would trail off the end of the dialogue box
	unsigned int wordLength = 0;
	unsigned int numLines = 1;
	unsigned int currLineLength = 0;
	for (unsigned int currLength = 0; currLength < stringToDisplay.length(); currLength++, currLineLength++)
	{
		char s = stringToDisplay[currLength];
		wordLength++;
		if (stringToDisplay[currLength] == '\n')
		{
			currLineLength = 0;
			wordLength = 0;
		}
		if (stringToDisplay[currLength] == ' ' || currLength == stringToDisplay.length() - 1)
		{
			if (currLineLength >= 32)
			{
				stringToDisplay[currLength - wordLength] = '\n';
				currLineLength = wordLength;
			}
			else
				wordLength = 0;
		}
	}
	this->stringToDisplay = stringToDisplay;
}

void Dialogue::StopDrawing()
{
	stringToDisplay = "";
}

void Dialogue::Render()
{
	// If it has been less than 5 seconds since the popup, display it
	if (sincePopup < 5.0f)
		DisplayPopUpDialogue();
}

void Dialogue::Update(float dt)
{
	sincePopup += dt;

	// Reset the chosen popup dialogue
	if (sincePopup > 5.0f)
	{
		popupDone = true;
		chosenPopupDialogue = "";
	}

	// Give them a random chance to say something by resetting the time its been sincePopup
	if (rand() % 1000 == 0 && popupDone == true)
	{
		sincePopup = 0.0f;
		popupDone = false;
	}
}