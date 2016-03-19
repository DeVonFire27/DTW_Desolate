/***************************************************************
|	File:		Entity.h
|	Author:		
|	Course:		
|	Purpose:	Entity class stores the shared data members
|				for all child game entities
***************************************************************/

#ifndef ENTITY_H
#define ENTITY_H


#include "IEntity.h"						// IEntity type
#include "../SGD Wrappers/SGD_Handle.h"		// HTexture type
#include "../SGD Wrappers/SGD_Color.h"

using namespace SGD;


/**************************************************************/
// Entity class
//	- parent class of all game entities, stores the shared data members
//	- velocity-based movement
class Entity : public IEntity
{
protected:
	// blend color when drawing
	SGD::Color color = SGD::Color(255, 255, 255);
public:
	/**********************************************************/
	// MUST have a virtual destructor to allow upcasted pointers
	// to trigger children destructors at deallocation
	Entity( void )			{}	// default constructor
	virtual ~Entity( void ) = default;	// VIRTUAL destructor

	SGD::Point origin = SGD::Point(0, 0);

	
	/**********************************************************/
	// Entity Types:
	enum EntityType { ENT_BASE, ENT_WEAPON, ENT_MEDKIT, ENT_GUNSLINGER, ENT_SNIPER, ENT_BRAWLER, ENT_MEDIC, ENT_CYBORG, ENT_EMITTER, ENT_ABILITY, ENT_NPC, ENT_JANE };
	enum AbilityType {NONE_ACT, MEDIC_ACT, GUN_ACT, BRAWL_ACT, SNIPE_ACT, CYBORG_ACT, MORTAR_ACT, RADICORN_ACT};
	
	/**********************************************************/
	// Interface:
	//	- virtual functions for children classes to override
	virtual void	Update			( float elapsedTime )		override;
	virtual void	Render			( void )					override;

	virtual SGD::Rectangle GetRect	( void )	const			override;
	virtual int		GetType			( void )	const			override	{	return ENT_BASE;	}
	virtual int GetAbilityType() const { return NONE_ACT; }
	virtual void	HandleCollision	( const IEntity* pOther )	override;
	

	// Children classes CANNOT override a 'final' method.
	virtual void	AddRef		( void )				final;
	virtual void	Release		( void )				final;

	
	/**********************************************************/
	// Accessors:
	SGD::HTexture		GetImage	( void ) const			{	return m_hImage;		}
	SGD::Point			GetPosition	( void ) const			{	return m_ptPosition;	}
	SGD::Vector			GetVelocity	( void ) const			{	return m_vtVelocity;	}
	SGD::Size			GetSize		( void ) const			{	return m_szSize;		}
	float				GetRotation	( void ) const			{	return m_fRotation;		}
	bool IsACharacter(void);

	// Mutators:
	void				SetImage	( SGD::HTexture	img  )	{	m_hImage		= img;	 }
	void				SetPosition	( SGD::Point	pos  ) 	{	m_ptPosition	= pos;	 }
	void				SetVelocity	( SGD::Vector	vel	 ) 	{	m_vtVelocity	= vel;	 }
	void				SetSize		( SGD::Size		size ) 	{	m_szSize		= size;	 }
	void				SetRotation ( float			ang  )	{	m_fRotation		= ang;	 }
	void				SetColor	( SGD::Color&	value)	{	color			= value; }

protected:
	/**********************************************************/
	// members:
	SGD::HTexture		m_hImage		= SGD::INVALID_HANDLE;	// image handle
	SGD::Point			m_ptPosition	= SGD::Point{0, 0};		// 2D position
	SGD::Vector			m_vtVelocity	= SGD::Vector{0, 0};	// 2D velocity
	SGD::Size			m_szSize		= SGD::Size{0, 0};		// 2D size
	float				m_fRotation		= 0.0f;					// angle in radians

private:
	/**********************************************************/
	// reference count
	unsigned int		m_unRefCount	= 1;	// calling 'new' gives the prime reference
};

#endif //ENTITY_H
