/***************************************************************
|	File:		BitmapFont.h
|	Author:		
|	Course:		
|	Purpose:	BitmapFont class draws text using an image
|				of fixed-size character glyphs
***************************************************************/

#ifndef BITMAPFONT_H
#define BITMAPFONT_H


#include "../SGD Wrappers/SGD_Handle.h"
#include "../SGD Wrappers/SGD_Color.h"
#include "../SGD Wrappers/SGD_Geometry.h"

#include <map>
#include <vector>

using namespace std;

//Storage for bitmap font charecters
struct CharInfo
{
	SGD::Rectangle rect;

	float offset_y;

	char c;
};

// Storage for colored characters
struct color_char
{
	SGD::Color color;

	CharInfo info;
};

/**************************************************************/
// BitmapFont class
//	- image can be missing characters, as long as there is space reserved
//	- image MUST have a size power-of-2 (e.g. 64, 128, 256, 512)
class BitmapFont
{
	/**********************************************************/
	// image
	SGD::HTexture	m_hImage			= SGD::INVALID_HANDLE;

	// both the max width and height of each charecter in the font
	int size;

	// a lookup for all characters and respective croping and offset data
	map<char, CharInfo> char_map;

	// current stored color for this font
	SGD::Color color = SGD::Color::White;

	// stored string
	vector<color_char> stored_string;

public:
	/**********************************************************/
	// Constructor & Destructor
	BitmapFont( void )	= default;
	BitmapFont(const char* filename);
	~BitmapFont( void )	= default;

	
	/**********************************************************/
	// Initialize & Terminate
	void Initialize( void );	// should have parameters
	void Terminate ( void );

	
	/**********************************************************/
	// Draw
	void Draw( const char* output, int x, int y,
			   float scale, SGD::Color color ) const;

	void DrawStoredString(int x, int y, float scale);

	BitmapFont& operator<<(SGD::Color color);
	BitmapFont& operator<<(const char* text);

	/////////////////////////<Helper functions>///////////////////////////////////////////////////

	// measures the width in pixels of 'str'. Will not account for multiple lines, however
	float MeasureString(const char*const str);
};

#endif //BITMAPFONT_H
