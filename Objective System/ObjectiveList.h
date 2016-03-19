#pragma once

#include <vector>
using namespace std;

class Objective;

class ObjectiveList
{
	vector <Objective*> objectiveList;
	int currObjective = 0;

public:
	float sinceNewObjective;

	void RemoveAll();

	static ObjectiveList * GetInstance();
	string GetCurrentObjectiveText();
	int GetCurrentObjectiveNumber() { return currObjective; }
	int GetNumberOfObjectives() { return objectiveList.size(); }
	vector <Objective*>& GetObjectivesList() { return objectiveList; }

	void SetCurrentObjective(string toSet);

	void AddObjective(Objective*);
	bool NextObjective(string);
	
	void Update(float dt);
	void Render();
};