/***************************************************************
|	File:		BitmapFont.cpp
|	Author:		
|	Course:		
|	Purpose:	BitmapFont class draws text using an image
|				of fixed-size character glyphs
***************************************************************/

#include "globals.h"
#include "BitmapFont.h"
#include "TinyXML\tinyxml.h"
#include <ctype.h>
#include <cassert>
#include <string>
#include "Dialogue System\Dialogue.h"

void LoadChar(TiXmlElement* element, map<char, CharInfo>& rects);

BitmapFont::BitmapFont(const char* filename)
{
	// allocate an xml document and load-in values from it
	TiXmlDocument doc = TiXmlDocument();
	doc.LoadFile(filename);

	// grab root node
	TiXmlElement* root = (TiXmlElement*)doc.FirstChild();
	root = root->NextSiblingElement();

	// get the 'chars' element out of the file
	TiXmlElement* char_root = root->FirstChildElement("chars");

	// allocate an iterator-pointer for each char defined in this document
	TiXmlElement* char_iter = (TiXmlElement*)char_root->FirstChild();

	// while there is still a charecter left to be read...
	while (char_iter)
	{
		// load-in this char
		LoadChar(char_iter, char_map);

		// go to the next node in the document
		char_iter = (TiXmlElement*)char_iter->NextSibling();
	}

	// find the 'pages' node, the node that stores the filename of this font
	TiXmlElement* pages_node = (TiXmlElement*)root->FirstChildElement("pages")->FirstChild();

	// get the font size from the 'info' node
	TiXmlElement* info = root->FirstChildElement("info");
	info->Attribute("size", &this->size);


	// find the name of the file
	string imageName = string("resources\\graphics\\fonts\\") +  pages_node->Attribute("file");
	
	// load-in image, based off filename
	m_hImage = SGD::GraphicsManager::GetInstance()->LoadTexture(imageName.c_str(), SGD::Color::Black);
}


/**************************************************************/
// Terminate
//	- clean up resources
void BitmapFont::Terminate( void )
{
	// Unload the image
	SGD::GraphicsManager::GetInstance()->UnloadTexture( m_hImage );
}


/**************************************************************/
// Draw
//	- draw the text one character at a time,
//	  using the Cell Algorithm to calculate the source rect
void BitmapFont::Draw( const char* output, int x, int y,
			float scale, SGD::Color color ) const
{
	// Validate the image
	assert( m_hImage != SGD::INVALID_HANDLE 
			&& "BitmapFont::Draw - image was not loaded" );

	// Validate the parameter
	assert( output != nullptr 
			&& "BitmapFont::Draw - string cannot be null" );


	// Store the starting X position for newlines
	int colX = x;

	int xAt = colX;
	int yAt = y;

	// Iterate through the characters in the string
	for( int i = 0; output[ i ]; i++ )
	{
		// Get the current character
		char ch = output[ i ];
		if (ch != 32 && ch != 10)
		{
			// Get the current rectangle
			auto char_info = this->char_map.at(ch);

			// draw the char
			SGD::GraphicsManager::GetInstance()->DrawTextureSection(m_hImage, SGD::Point((float)xAt, (float)yAt + char_info.offset_y * scale), char_info.rect, 0.f, 
			{ char_info.rect.ComputeWidth() / 2.f, char_info.rect.ComputeHeight() / 2.f }, color, { scale, scale });

			// incrament xAt by the width of this char
			xAt += (int)((char_info.rect.right - char_info.rect.left) * scale);
		}
		// handle space
		else if (ch == 32)
		{
			xAt += abs(size / 2);
		}
		// handle new line
		else if (ch == 10)
		{
			xAt = colX;
			yAt += abs(size);
		}
	}
}

void BitmapFont::DrawStoredString(int x, int y, float scale)
{	
	// Validate the image
	assert(m_hImage != SGD::INVALID_HANDLE
		&& "BitmapFont::Draw - image was not loaded");

	// Store the starting X position for newlines
	int colX = x;

	int xAt = colX;
	int yAt = y;

	unsigned int wordLength = 0;
	unsigned int numLines = 1;
	unsigned int currLineLength = 0;
	// Iterate through the characters in the string
	for (unsigned int i = 0; i < stored_string.size(); i++, currLineLength++)
	{
		//check each word and see if it would trail off the end of the dialogue box
		char s = stored_string[i].info.c;
		wordLength++;
		if (stored_string[i].info.c == '\n')
		{
			currLineLength = 0;
			wordLength = 0;
		}
		if (stored_string[i].info.c == ' ' || i == stored_string.size() - 1)
		{
			if (currLineLength >= 32)
			{
				stored_string[i - wordLength].info.c = '\n';
				currLineLength = wordLength;
			}
			else
				wordLength = 0;
		}
	}
	for (unsigned int i = 0; i < stored_string.size(); i++)
	{
		// Get the current character
		char ch = stored_string[i].info.c;
		if (ch != 32 && ch != 10 && ch != 13)
		{
			// Get the current rectangle
			auto char_info = this->char_map.at(ch);

			// draw the char
			SGD::GraphicsManager::GetInstance()->DrawTextureSection(m_hImage, SGD::Point((float)xAt, (float)yAt + char_info.offset_y * scale), char_info.rect, 0.f,
			{ char_info.rect.ComputeWidth() / 2.f, char_info.rect.ComputeHeight() / 2.f }, stored_string[i].color, { scale, scale });

			// incrament xAt by the width of this char
			xAt += (int)((char_info.rect.right - char_info.rect.left) * scale);
		}
		// handle space
		else if (ch == 32)
		{
			xAt += abs(size / 2);
		}
		// handle new line
		else if (ch == 10 || ch == 13)
		{
			xAt = colX;
			yAt += abs(size);
		}
	}
	//GameplayState::GetInstance()->gameplayDialogue->SetStringToDisplay(" ");

	stored_string.clear();
}
void LoadChar(TiXmlElement* element, map<char, CharInfo>& rects)
{
	// allocate values for this rect
	int x, y, width, height;

	// pull rect values out of file
	element->Attribute("x", &x);
	element->Attribute("y", &y);
	element->Attribute("width", &width);
	element->Attribute("height", &height);

	// create the rectangle for this char
	SGD::Rectangle r = SGD::Rectangle((float)x, (float)y, (float)x + width, (float)y + height);

	// find which char this rectangle is for
	int char_index;
	element->Attribute("id", &char_index);

	// find the y offset for this charecter
	double offset_y;
	element->Attribute("yoffset", &offset_y);

	// push the char and its info into the map
	rects[char(char_index)] = CharInfo();
	rects[char(char_index)].offset_y = (float)offset_y;
	rects[char(char_index)].rect = r;
	rects[char(char_index)].c = char(char_index);
}

float BitmapFont::MeasureString(const char*const str)
{
	// store a value for the output
	float outval = 0.f;

	// find the length of this string
	int length = strlen(str);
	// iterate over all the charecters in the provided string
	for (int i = 0; i < length; i++)
	{
		// skip this charecter if it is a newline
		if (str[i] == '\n')
			continue;
		// incrament output value by the width of this charecter
		outval += char_map[str[i]].rect.ComputeWidth();
	}

	// return the computed value
	return outval;
}


BitmapFont& BitmapFont::operator<<(SGD::Color color)
{
	if (!GameplayState::GetInstance()->alreadySetWeaponInfo)
	this->color = color;
	return *this;
}

BitmapFont& BitmapFont::operator<<(const char* text)
{	
	if (!GameplayState::GetInstance()->alreadySetWeaponInfo)
	{
		// loop over every char in this string
		for (int i = 0; text[i]; i++)
		{
			// allocate a new char info to add to the stored string
			color_char c;
			c.color = color;
			c.info = char_map[text[i]];

			// push the new char into the vector
			stored_string.push_back(c);
		}
	}
	return *this;
}