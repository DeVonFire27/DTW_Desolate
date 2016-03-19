/***************************************************************
|	File:		StatusEffect.h
|	Author:		Michael Mozdzierz
|	Date:		04/11/2014
|
***************************************************************/

#pragma once

#include <map>
#include "../Agents/Stat.h"
#include "../EventProc/IObserver.h"

class Character;
class Emitter;

using namespace std;

enum class StatEffectType { NONE, Radiation, Bleeding, Burning, Slow, Stun, Transfusion, Allure };

class StatusEffect : public IObserver
{
protected:
	float duration,
		damage,
		time = 0.f;

	Character* effector = nullptr;
	Emitter* emitter = nullptr;
public:
	StatusEffect(void);
	virtual ~StatusEffect(void);

	virtual StatEffectType GetType(void) const { return StatEffectType::NONE; }

	map<StatType, float> affectors;

	/////////////////////////<Public functions>///////////////////////////////////////////////////

	virtual void Update(float dt);

	virtual bool Complete(void) { return duration < 0.f; }
	bool end = false;

	void SetDuration(const float dur) { duration = dur; }
	float GetDuration() const { return duration; }

	Character *GetEffector() const { return effector; }
	Emitter *GetEmitter() const { return emitter; }

	void SetEffector(Character *);
	void SetEmitter(Emitter *);

	virtual void HandleEvent(string name, void* args = nullptr) override;
};