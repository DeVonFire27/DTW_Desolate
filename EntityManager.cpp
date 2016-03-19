/***************************************************************
|	File:		EntityManager.cpp
|	Author:
|	Course:
|	Purpose:	EntityManager class stores & maintains all game entities
***************************************************************/

#include "globals.h"
#include "EntityManager.h"
#include "Agents\IMinimapVisible.h"
#include "Camera.h"
#include "globals.h"
#include "Tile System\World.h"
#include "IEntity.h"
#include <cassert>
#include "Math.h"

EntityManager::EntityManager(void)
{
	passableTileTex = SGD::GraphicsManager::GetInstance()->LoadTexture("resources\\graphics\\passibleTileMinimap.png");
}

EntityManager::~EntityManager(void)
{
	SGD::GraphicsManager::GetInstance()->UnloadTexture(passableTileTex);
}


/**************************************************************/
// AddEntity
//	- store the entity into the specified bucket
//	- the Entity Manager holds a reference to the entity
void EntityManager::AddEntity(IEntity* pEntity, unsigned int bucket)
{
	// Validate the parameter
	assert(pEntity != nullptr && "EntityManager::AddEntity - parameter cannot be null");


	// Expand the table?
	if (bucket >= m_tEntities.size())
		m_tEntities.resize(bucket + 1);


	// Append the entity into the specified vector
	m_tEntities[bucket].push_back(pEntity);

	// Hold a reference to keep the entity in memory
	pEntity->AddRef();
}


/**************************************************************/
// RemoveEntity
//	- remove the entity from the specified bucket
//	- release the reference to the entity
void EntityManager::RemoveEntity(IEntity* pEntity, unsigned int bucket)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::RemoveEntity - cannot remove while iterating");

	// Validate the parameters
	assert(pEntity != nullptr && "EntityManager::RemoveEntity - cannot remove NULL");
	assert(bucket < m_tEntities.size() && "EntityManager::RemoveEntity - invalid bucket");


	// Try to find the entity
	EntityVector& vec = m_tEntities[bucket];
	for (unsigned int i = 0; i < vec.size(); i++)
	{
		if (vec[i] == pEntity)
		{
			// Remove the entity
			vec.erase(vec.begin() + i);
			pEntity->Release();
			break;
		}
	}
}


/**************************************************************/
// RemoveEntity
//	- remove & release the entity from any bucket
void EntityManager::RemoveEntity(IEntity* pEntity)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::RemoveEntity - cannot remove while iterating");

	// Validate the parameters
	assert(pEntity != nullptr && "EntityManager::RemoveEntity - pointer cannot be null");


	// Try to find the entity in any buckect
	for (unsigned int bucket = 0; bucket < m_tEntities.size(); bucket++)
	{
		EntityVector& vec = m_tEntities[bucket];
		for (unsigned int i = 0; i < vec.size(); i++)
		{
			if (vec[i] == pEntity)
			{
				// Remove the entity
				vec.erase(vec.begin() + i);
				pEntity->Release();
				return;
			}
		}
	}
}


/**************************************************************/
// RemoveAll
//	- remove all entities from a specific bucket
void EntityManager::RemoveAll(unsigned int unBucket)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::RemoveAll - cannot remove while iterating");

	// Validate the parameter
	assert(unBucket < m_tEntities.size() && "EntityManager::RemoveAll - invalid bucket");


	// Lock the iterator
	m_bIterating = true;
	{
		// Release the reference to EVERY entity
		EntityVector& vec = m_tEntities[unBucket];
		for (unsigned int i = 0; i < vec.size(); i++)
		{
			vec[i]->Release();
			vec[i] = nullptr;
		}

		vec.clear();
	}
	// Unlock the iterator
	m_bIterating = false;
}



/**************************************************************/
// RemoveAll
//	- release each entity in the table
void EntityManager::RemoveAll(void)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::RemoveAll - cannot remove while iterating");

	// Lock the iterator
	m_bIterating = true;
	{
		// Release every entity
		for (unsigned int bucket = 0; bucket < m_tEntities.size(); bucket++)
		{
			EntityVector& vec = m_tEntities[bucket];
			for (unsigned int i = 0; i < vec.size(); i++)
			{
				vec[i]->Release();
				vec[i] = nullptr;
			}
		}
	}
	// Unlock the iterator
	m_bIterating = false;


	// Collapse the table
	m_tEntities.clear();
}


/**************************************************************/
// UpdateAll
//	- update each entity in the table
void EntityManager::UpdateAll(float elapsedTime)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::UpdateAll - cannot update while iterating");

	// Lock the iterator
	m_bIterating = true;
	{
		// Update every entity
		for (unsigned int bucket = 0; bucket < m_tEntities.size(); bucket++)
		{
			EntityVector& vec = m_tEntities[bucket];
			for (unsigned int i = 0; i < vec.size(); i++)
				vec[i]->Update(elapsedTime);
		}
	}
	// Unlock the iterator
	m_bIterating = false;
}


/**************************************************************/
// RenderAll
//	- render each entity in the table
void EntityManager::RenderAll(void)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::RenderAll - cannot render while iterating");

	// Lock the iterator
	m_bIterating = true;
	{
		// Render every entity
		for (unsigned int bucket = 0; bucket < m_tEntities.size(); bucket++)
		{
			EntityVector& vec = m_tEntities[bucket];
			for (unsigned int i = 0; i < vec.size(); i++)
				vec[i]->Render();
		}
	}
	// Unlock the iterator
	m_bIterating = false;
}


/**************************************************************/
// CheckCollisions
//	- check collision between the entities within the two buckets
void EntityManager::CheckCollisions(unsigned int bucket1, unsigned int bucket2)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::CheckCollisions - cannot collide while iterating");

	// Quietly validate the parameters
	if (bucket1 >= m_tEntities.size()
		|| bucket2 >= m_tEntities.size()
		|| m_tEntities[bucket1].size() == 0
		|| m_tEntities[bucket2].size() == 0)
		return;


	// Lock the iterator
	m_bIterating = true;
	{
		// Are they different buckets?
		if (bucket1 != bucket2)
		{
			// Which bucket is smaller?
			//	should be the outer loop for less checks (n0)*(n1+1) + 1
			EntityVector* pVec1 = &m_tEntities[bucket1];
			EntityVector* pVec2 = &m_tEntities[bucket2];

			if (pVec2->size() < pVec1->size())
			{
				EntityVector* pTemp = pVec1;
				pVec1 = pVec2;
				pVec2 = pTemp;
			}

			EntityVector& vec1 = *pVec1;
			EntityVector& vec2 = *pVec2;

			// Iterate through the smaller bucket
			for (unsigned int i = 0; i < vec1.size(); i++)
			{
				// Iterate through the larger bucket
				for (unsigned int j = 0; j < vec2.size(); j++)
				{
					// Ignore self-collision
					if (vec1[i] == vec2[j])
						continue;

					// Local variables help with debugging
					SGD::Rectangle rEntity1 = vec1[i]->GetRect();

					SGD::Rectangle rEntity2 = vec2[j]->GetRect();

					// Check for collision between the entities
					if (rEntity1.IsIntersecting(rEntity2) == true)
					{
						// Both objects handle collision
						vec1[i]->HandleCollision(vec2[j]);

						vec2[j]->HandleCollision(vec1[i]);
					}
				}
			}
		}
		else // bucket1 == bucket2
		{
			EntityVector& vec = m_tEntities[bucket1];

			// Optimized loop to ensure objects do not collide with
			// each other twice
			if (vec.size() != 0)
			{
				for (unsigned int i = 0; i < vec.size() - 1; i++)
				{
					for (unsigned int j = i + 1; j < vec.size() - 1; j++)
					{
						// Ignore self-collision
						if (vec[i] == vec[j])
							continue;

						// Local variables help with debugging
						SGD::Rectangle rEntity1 = vec[i]->GetRect();
						SGD::Rectangle rEntity2 = vec[j]->GetRect();

						// Check for collision between the entities
						if (rEntity1.IsIntersecting(rEntity2) == true)
						{
							// Both objects handle collision
							vec[i]->HandleCollision(vec[j]);
							vec[j]->HandleCollision(vec[i]);
						}
					}
				}
			}
		}
	}
	// Unlock the iterator
	m_bIterating = false;
}

void EntityManager::CheckBulletCollisions(unsigned int bucket1, unsigned int bucket2)
{
	// Validate the iteration state
	assert(m_bIterating == false && "EntityManager::CheckBulletCollisions - cannot collide while iterating");

	// Quietly validate the parameters
	if (bucket1 >= m_tEntities.size()
		|| bucket2 >= m_tEntities.size()
		|| m_tEntities[bucket1].size() == 0
		|| m_tEntities[bucket2].size() == 0)
		return;


	// Lock the iterator
	m_bIterating = true;
	{
		// Are they different buckets?
		if (bucket1 != bucket2)
		{
			// Which bucket is smaller?
			//	should be the outer loop for less checks (n0)*(n1+1) + 1
			EntityVector* pVec1 = &m_tEntities[bucket1];
			EntityVector* pVec2 = &m_tEntities[bucket2];

			EntityVector& vec1 = *pVec1;
			EntityVector& vec2 = *pVec2;

			// Iterate through the smaller bucket
			for (unsigned int i = 0; i < vec1.size(); i++)
			{
				// Iterate through the larger bucket
				for (unsigned int j = 0; j < vec2.size(); j++)
				{
					// Ignore self-collision
					if (vec1[i] == vec2[j])
						continue;

					// Local variables help with debugging
					SGD::Rectangle rEntity1 = vec1[i]->GetRect();
					SGD::Rectangle rEntity2 = vec2[j]->GetRect();

					// Check for collision between the entities
					if (rEntity1.IsIntersecting(rEntity2) == true)
					{
						// Both objects handle collision
						vec1[i]->HandleCollision(vec2[j]);
						vec2[j]->HandleCollision(vec1[i]);
					}
				}
			}
		}
		else // bucket1 == bucket2
		{
			EntityVector& vec = m_tEntities[bucket1];

			// Optimized loop to ensure objects do not collide with
			// each other twice
			if (vec.size() != 0)
			{
				for (unsigned int i = 0; i < vec.size() - 1; i++)
				{
					for (unsigned int j = i + 1; j < vec.size() - 1; j++)
					{
						// Ignore self-collision
						if (vec[i] == vec[j])
							continue;

						// Local variables help with debugging
						SGD::Rectangle rEntity1 = vec[i]->GetRect();
						SGD::Rectangle rEntity2 = vec[j]->GetRect();

						// Check for collision between the entities
						if (rEntity1.IsIntersecting(rEntity2) == true)
						{
							// Both objects handle collision
							vec[i]->HandleCollision(vec[j]);
							vec[j]->HandleCollision(vec[i]);
						}
					}
				}
			}
		}
	}
	// Unlock the iterator
	m_bIterating = false;
}

void EntityManager::RenderMinimap(void)
{
	// store default values for this function
	const SGD::Point scr_offset = SGD::Point(Game::GetInstance()->GetScreenWidth() - 200.0f, Game::GetInstance()->GetScreenHeight() - 160.f);
	const SGD::Size map_size = { 0.14f, 0.14f };

	// get a pointer to the camera
	Camera* camera = World::GetInstance()->GetCamera();

	// create matricies to translate from world space to screen space
	D3DXMATRIX scale;
	D3DXMatrixScaling(&scale, map_size.width, map_size.height, 0.f);
	D3DXMATRIX offset;
	D3DXMatrixTranslation(&offset, scr_offset.x, scr_offset.y, 0.f);
	D3DXMATRIX transform = World::GetInstance()->GetCamera()->GetMatrix() * scale * offset;

	// send the transform to the graphics manager
	SGD::GraphicsManager::GetInstance()->SetTransform(transform);

	// get the clipping rectangle for the mini map
	SGD::Rectangle rect = camera->GetRect();
	rect.Inflate(SGD::Size(200.f, 200.f));

	// draw the background of the mini map
	SGD::GraphicsManager::GetInstance()->DrawRectangle(rect, SGD::Color::Black);

	// store the collision layer
	TileLayer* collisionLayer = World::GetInstance()->GetTileLayers()[1];
	// store the visual layer
	TileLayer* visualLayer = World::GetInstance()->GetTileLayers()[0];

	// store tile width and height values
	float tile_width = collisionLayer->GetTileSize().width;
	float tile_height = collisionLayer->GetTileSize().height;

	// iterate y
	for (unsigned int y = (unsigned int)fabs(rect.top / tile_height); y < rect.bottom / tile_height; y++)
	{
		// iterate x
		for (unsigned int x = (unsigned int)fabs(rect.left / tile_width); x < rect.right / tile_width; x++)
		{

			// store the tile we're on now
			Tile* tile = collisionLayer->GetTileAt(x, y);

			if (!tile || !tile->isPassable)
				continue;

			// find the boundind rect of this tile
			SGD::Rectangle tile_rect = { x * tile_width, y * tile_height, x * tile_width + tile_width, y * tile_height + tile_height };

			// store alpha values for the edges of the screen
			float alpha_left = Math::Ceiling(Math::distance(tile_rect.left, rect.left), 100.f) / 10.f;
			float alpha_right = Math::Ceiling(Math::distance(tile_rect.right, rect.right), 100.f) / 10.f;
			float alpha_top = Math::Ceiling(Math::distance(tile_rect.top, rect.top), 100.f) / 10.f;
			float alpha_down = Math::Ceiling(Math::distance(tile_rect.bottom, rect.bottom), 100.f) / 10.f;
			float alpha_road = 1.f;

			// check to see if this is a road on the visual layer
			if (visualLayer->GetTileAt(x, y)->tileNumber == 6)
				alpha_road = 0.8f;

			// alpha_left = Math::lerp(1.f, 0.f, alpha_left);
			// alpha_right = Math::lerp(1.f, 0.f, alpha_right);
			// alpha_top = Math::lerp(1.f, 0.f, alpha_top);
			// alpha_down = Math::lerp(1.f, 0.f, alpha_down);

			float alpha_total = alpha_left + alpha_right + alpha_top + alpha_down;
			alpha_total /= 40.f;

			alpha_total *= alpha_road;

			SGD::Color blend_color = SGD::Color::White;
			blend_color.alpha = char(255.f * alpha_total);

			// if the tile is passible, and visible on the minimap and not clipping 
			// the clipping rectangle, draw the visible tile
			if (tile->isPassable && tile_rect.IsIntersecting(rect) && !tile_rect.IsClipping(rect))
				SGD::GraphicsManager::GetInstance()->DrawTexture(passableTileTex, SGD::Point(x * collisionLayer->GetTileSize().width, y * collisionLayer->GetTileSize().height), 0.f, { 0.f, 0.f }, blend_color);
		}
	}


	// for all buckets of entities
	for (unsigned int i = 0; i < m_tEntities.size(); i++)
	{
		// get the current bucket we're on
		std::vector<IEntity*>& bucket = m_tEntities[i];
		for (unsigned int ii = 0; ii < bucket.size(); ii++)
		{
			// store what object we're on right now
			IEntity* at = bucket[ii];
			// try to cast the entity to an IMinimapvisible
			IMinimapVisible* ent = dynamic_cast<IMinimapVisible*>(at);

			// if the current entity is intersecting the camera rect
			if (ent && at->GetRect().IsIntersecting(rect) && !at->GetRect().IsClipping(rect))
			{
				// render the entity on the minimap
				ent->RenderMiniMap();
			}
		}
	}

	// reset camera matrix
	D3DXMATRIX identity;
	D3DXMatrixIdentity(&identity);
	SGD::GraphicsManager::GetInstance()->SetTransform(identity);
}
