/***************************************************************
|	File:		IEntity.h
|	Author:		
|	Course:		
|	Purpose:	IEntity class is the interface for all game entities
***************************************************************/

#ifndef IENTITY_H
#define IENTITY_H


#include "../SGD Wrappers/SGD_Geometry.h"	// Rectangle type


/**************************************************************/
// IEntity class
//	- interface base class:
//		- virtual methods for children classes to override
//		- no data members
class IEntity
{
public:
	/**********************************************************/
	// Destructor MUST be virtual
	IEntity( void )				= default;
	virtual ~IEntity( void )	= default;


	/**********************************************************/
	// Interface:
	//	- pure virtual methods MUST be overridden
	virtual void	Update		( float elapsedTime )	= 0;
	virtual void	Render		( void )				= 0;

	virtual SGD::Rectangle GetRect( void )	const		= 0;
	virtual int		GetType		( void )	const		= 0;
	virtual void	HandleCollision( const IEntity* pOther )	= 0;

	
	/**********************************************************/
	// Reference Counting:
	//	- track how many 'owners' are pointing to this entity
	virtual void	AddRef		( void )				= 0;
	virtual void	Release		( void )				= 0;
};

#endif //IENTITY_H
