/***************************************************************
|	File:		IMinimapVisible.h
|	Author:		Michael Mozdzierz
|	Date:		05/09/2014
|
***************************************************************/

#pragma once

struct IMinimapVisible
{
	virtual ~IMinimapVisible(void) = 0 {}

	virtual void RenderMiniMap(void) = 0 {}
};