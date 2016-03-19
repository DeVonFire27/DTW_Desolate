/***************************************************************
|	File:		MainMenuState.h
|	Author:		Ethan Mills
|	Course:		SGP
|	Purpose:	MainMenuState class handles the main menu
***************************************************************/

#ifndef MAINMENUSTATE_H
#define MAINMENUSTATE_H


#include "IGameState.h"
#include "..\SGD Wrappers\SGD_GraphicsManager.h"
#include "Animation System\AnimationTimeStamp.h"
#include "Particle System/Emitter.h"

class BitmapFont;

/**************************************************************/
// MainMenuState class
//	- handles the main menu
//	- SINGLETON (statically allocated, not dynamic)
class MainMenuState : public IGameState
{
public:
	/**********************************************************/
	// Singleton Accessor
	static MainMenuState* GetInstance( void );

	
	/**********************************************************/
	// IGameState Interface:
	virtual void	Enter( void )				override;	// load resources
	virtual void	Exit ( void )				override;	// unload resources

	virtual bool	Input( void )				override;	// handle user input
	virtual void	Update( float elapsedTime )	override;	// update entites
	virtual void	Render( void )				override;	// render entities / menu
	void setPlayMenu(bool p) { playMenu = p; }

private:
	/**********************************************************/
	// SINGLETON!
	MainMenuState( void )			= default;
	virtual ~MainMenuState( void )	= default;

	MainMenuState( const MainMenuState& )				= delete;	
	MainMenuState& operator= ( const MainMenuState& )	= delete;


	/**********************************************************/
	// Cursor Index
	int		m_nCursor = 0;
	bool playMenu = false;	//bool that is set to true once you press the play button, it will then ask you whether you want to start a new game or load an old save
	SGD::Rectangle playRect;
	SGD::Rectangle instructRect;
	SGD::Rectangle optionRect;
	SGD::Rectangle creditsRect;
	SGD::Rectangle exitRect;
};

#endif //MAINMENUSTATE_H
