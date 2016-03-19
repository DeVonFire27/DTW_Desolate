/***************************************************************
|	File:		HitMarker.cpp
|	Author:		Michael Mozdzierz
|	Date:		05/20/2014
|
***************************************************************/

#include "globals.h"
#include "HitMarker.h"
#include "../Math.h"

HitMarker::HitMarker(int damage, SGD::Point& position, float direction, SGD::Color color)
{
	// store damage
	this->damage = damage;
	// store position
	this->m_ptPosition = position;

	// get a random direction
	int rand_dir = rand() % 20 - 10;
	// set velocity
	m_vtVelocity = SGD::Vector(cos(direction + Math::to_radians(float(rand_dir))), sin(direction + Math::to_radians(float(rand_dir)))) * 100.f;

	// set color to red
	this->color = color;
}

/*virtual*/ HitMarker::~HitMarker(void)
{
}

/*virtual*/ void HitMarker::Update(float dt) /*override*/
{
	// decrament transparancy
	alpha -= 0.3f * dt;
	
	// update blending color
	color.alpha = char(255.f * alpha);

	// if the alpha is less than zero...
	if (alpha <= 0.f)
	{
		// dispatch a message to destroy this instance
		DestroyEntityMsg* msg = new DestroyEntityMsg(this);
		SGD::MessageManager::GetInstance()->QueueMessage(msg);
	}

	// call inherited function
	Entity::Update(dt);
}

#include "../../SGD Wrappers/SGD_GraphicsManager.h"
#include <string>
/*virtual*/ void HitMarker::Render(void) /*override*/
{
	SGD::GraphicsManager::GetInstance()->DrawString((std::string("-") + std::to_string(damage)).c_str(), m_ptPosition, color);
}