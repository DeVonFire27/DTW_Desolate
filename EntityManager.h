/***************************************************************
|	File:		EntityManager.h
|	Author:
|	Course:
|	Purpose:	EntityManager class stores & maintains all game entities
***************************************************************/

#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H


class IEntity;			// uses IEntity*
#include <vector>		// uses std::vector


/**************************************************************/
// EntityManager class
//	- stores references to game entities
//	- updates & renders all game entities
class EntityManager
{
public:
	/**********************************************************/
	// Default constructor & destructor
	EntityManager(void);
	~EntityManager(void);


	/**********************************************************/
	// Entity Storage:
	void	AddEntity(IEntity* pEntity, unsigned int bucket);
	void	RemoveEntity(IEntity* pEntity, unsigned int bucket);
	void	RemoveEntity(IEntity* pEntity);
	void	RemoveAll(unsigned int bucket);
	void	RemoveAll(void);


	/**********************************************************/
	// Entity Upkeep:
	void	UpdateAll(float elapsedTime);
	void	RenderAll(void);

	void	CheckCollisions(unsigned int bucket1, unsigned int bucket2);
	void	CheckBulletCollisions(unsigned int bucket1, unsigned int bucket2);

	void	RenderMinimap(void);



private:
	/**********************************************************/
	// Not a singleton, but still don't want the Trilogy-of-Evil
	EntityManager(const EntityManager&) = delete;
	EntityManager& operator= (const EntityManager&) = delete;


	/**********************************************************/
	// Typedefs will simplify the templates
	typedef std::vector< IEntity* >		EntityVector;
	typedef std::vector< EntityVector >	EntityTable;

	SGD::HTexture passableTileTex = SGD::INVALID_HANDLE;

	/**********************************************************/
	// members:
	EntityTable	m_tEntities;			// vector-of-vector-of-IEntity* (2D table)
	bool		m_bIterating = false;	// read/write lock

public:
	EntityTable GetEntityTable() { return m_tEntities; }
};

#endif //ENTITYMANAGER_H
