/***************************************************************
|	File:		Entity.cpp
|	Author:
|	Course:
|	Purpose:	Entity class stores the shared data members
|				for all child game entities
***************************************************************/

#include "globals.h"
#include "Entity.h"
#include <cassert>


//Entity::~Entity()
//{
//	if(m_hImage != SGD::INVALID_HANDLE)
//		SGD::GraphicsManager::GetInstance()->UnloadTexture(m_hImage);
//}

/**************************************************************/
// Update
//	- move the entity's position by its velocity
/*virtual*/ void Entity::Update(float elapsedTime)
{
	m_ptPosition += m_vtVelocity * elapsedTime;
}


/**************************************************************/
// Render
//	- draw the entity's image at its position
/*virtual*/ void Entity::Render(void)
{
	//If too far then dont render
	if ((m_ptPosition - GameplayState::GetInstance()->player->GetPosition()).ComputeLength() > Game::GetInstance()->GetScreenWidth()) return;

	// Verify the image
	assert(m_hImage != SGD::INVALID_HANDLE && "Entity::Render - image was not set!");

	// HACK: Modify the rotation
	//m_fRotation += 0.01f;

	// Draw the image
	SGD::GraphicsManager::GetInstance()->DrawTexture(
		m_hImage, m_ptPosition,
		m_fRotation, m_szSize / 2, color);
		SGD::GraphicsManager::GetInstance()->DrawTextureSection(m_hImage, m_ptPosition,
		{ 0, 0, GetSize().width, GetSize().height }, m_fRotation, m_szSize / 2, color);

}


/**************************************************************/
// GetRect
//	- calculate the entity's bounding rectangle
/*virtual*/ SGD::Rectangle Entity::GetRect( void ) const
{
	return SGD::Rectangle{ m_ptPosition, m_szSize };
}

/**************************************************************/
// IsACharacter
//	- Determines if this is a character or now
bool Entity::IsACharacter(void)
{
	if (GetType() == Entity::EntityType::ENT_BRAWLER
		|| GetType() == Entity::EntityType::ENT_CYBORG
		|| GetType() == Entity::EntityType::ENT_GUNSLINGER
		|| GetType() == Entity::EntityType::ENT_MEDIC
		|| GetType() == Entity::EntityType::ENT_SNIPER
		)
	{
		return true;
	}

	return false;
}


/**************************************************************/
// HandleCollision
//	- respond to collision between entities
/*virtual*/ void Entity::HandleCollision( const IEntity* pOther )
{
	/* DO NOTHING */
	(void)pOther;		// unused parameter
}


/**************************************************************/
// AddRef
//	- increase the reference count
/*virtual*/ void Entity::AddRef( void )
{
	assert( m_unRefCount != 0xFFFFFFFF && "Entity::AddRef - maximum reference count has been exceeded" );

	++m_unRefCount;
}


/**************************************************************/
// Release
//	- decrease the reference count
//	- self-destruct when the count is 0
/*virtual*/ void Entity::Release( void )
{
	--m_unRefCount;

	if( m_unRefCount == 0 )
		delete this;
}
