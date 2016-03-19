#include <vector>
#include <string.h>
using namespace std;

class Character;

class Dialogue
{
	vector <string> popupDialogueOptions;		// Dialogue chosen from when the character randomly displays the popup dialogue above their head
	string chosenPopupDialogue;					// The actual string going to be drawn
	Character * thisCharacter = nullptr;					// The character the dialogue is attached to

	string stringToDisplay;

	bool popupDone;
	bool inParty = false;

	int currSelection;

	float sincePopup = 5.0f;

	Dialogue();

public:
	enum DialogueResult { YES, NO, NONE };

	Dialogue(Character* dialoguesCharacter);
	virtual ~Dialogue(void);

	void DrawString();
	void SetStringToDisplay(string);
	string GetStringToDisplay() { return stringToDisplay; }
	void StopDrawing();
	
	void SetCharacter(Character* character);

	void SetWeaponSwapDialogue(Weapon* weapon, Weapon* pWep);

	void DisplayPopUpDialogue();
	void AddPopUpDialogue(string newDialogue) { popupDialogueOptions.push_back(newDialogue); }

	void DisplayRecruitmentDialogue();
	void DisplayLackingRepDialogue();
	void DisplayRecruitmentSuccessDialogue();

	DialogueResult Input();
	void Update(float dt);
	void Render();
};