/***************************************************************
|	File:		IGameState.h
|	Author:		
|	Course:		
|	Purpose:	IGameState class declares the interface that all
|				child states must implement
***************************************************************/

#ifndef IGAMESTATE_H
#define IGAMESTATE_H
#include "../SGD Wrappers/SGD_Geometry.h"
#include "../SGD Wrappers/SGD_GraphicsManager.h"
#include "../SGD Wrappers/SGD_Handle.h"

class BitmapFont;

using namespace std;

//namespace SGD
//{
//	class Point;
//}
/**************************************************************/
// IGameState class
//	- abstract base class!
//	- declares interface for game state
class IGameState
{
public:
	/**********************************************************/
	// Virtual Destructor
	virtual ~IGameState( void )	= default;


	/**********************************************************/
	// IGameState Interface:
	virtual void	Enter( void )	= 0;	// load resources
	virtual void	Exit ( void )	= 0;	// unload resources

	virtual bool	Input( void )	= 0;	// handle user input
	virtual void	Update( float elapsedTime )	= 0;	// update entites
	virtual void	Render( void )	= 0;	// render entities / menu
	void DrawButton(SGD::Point position, BitmapFont& font, const char * text, SGD::Color backColor = { 255, 25, 10 });
	SGD::HTexture bulletButton;
};

#endif //IGAMESTATE_H
