/***************************************************************
|	File:		Console.cpp
|	Author:		Michael Mozdzierz
|	Date:		05/23/2014
|
***************************************************************/

#pragma once

#include "globals.h"
#include "Console.h"
#include "Math.h"
#include "EventProc\EventProc.h"

#define CHAR_HEIGHT 20


Console::Console(void)
{
	// ensure there is at least one line in buffer info
	buffer_info.push_back({ "", SGD::Color::White});
}

Console::~Console(void)
{
}

vector<string> ParseCommand(string& command, const char* delimiter)
{
	// store a container to hold all pieces we find in this string
	vector<string> total;
	// store the current piece of the command we're woring on
	string current;
	// find the length of the delimiter
	int num_delims = strlen(delimiter);
	// iterate over the command string
	for (unsigned int i = 0; i < command.size(); i++)
	{
		// store a bool to indicate if this is a delimiter character and should not be added to 'current'
		bool skip = false;
		// iterate over the delimiters
		for (int ii = 0; ii < num_delims; ii++)
		{
			// if this character is a delimeter
			if (delimiter[ii] == command[i])
			{
				// check to see if the current piece is not empty
				if (current.size() > 0)
				{
					total.push_back(current);
					current.clear();
				}
				// signal we found a delimiter character
				skip = true;
				break;
			}
		}

		if (!skip)
			// add the current character to current if it's not a delimiter character
			current.push_back(command[i]);
		else
			// reset skip for next characters in the command
			skip = true;
	}

	return total;
}

void Console::Update(void)
{
	// keep track of the current line (first string in 'buffer_info')
	current_line = &buffer_info[0].line;
	// store a pointer to input manager
	SGD::InputManager* pInput = SGD::InputManager::GetInstance();
	// store a value for the previous char
	static wchar_t char_prev = L'\0';

	// store a value for the current char
	wchar_t c = pInput->GetAnyCharDown();
	// if there is a character sitting in the input buffer, and it's not being held down...
	if (c && c != char_prev)
	{
		/*===================================================<AUTHORS NOTE>===================================================================*/
		/*                                                                                                                                    */
		/*       size checks occour inside 'if' checks to prevent falling to the final 'else' case, causeing new lines						  */
		/*       and backspace characters to be inserted into the new line																	  */
		/*                                                                                                                                    */
		/*====================================================================================================================================*/
		// ignore tilde characters
		if (c == '`' || c == '~')
			return;
		// if this character is a carrage return character...
		if (c == L'\r' )
		{
			// if the current line is empty..
			if (current_line->size() == 0)
			{
				// update previous character
				char_prev = c;
				// break out of this function
				return;
			}
			// allocate delimiters for string splitting
			char delim[] = { '(', ')', ',' };
			// split the current line into peices (commmand name and its parameters)
			vector<string> command = ParseCommand(*current_line, delim);
			// ensure we have a vaild command
			if (command.size() == 0)
			{
				WriteLine("Failed to parse command", SGD::Color::Red);
				char_prev = c;
				return;
			}
			// store the name of this command
			string command_name = command[0];
			// remove the command name from the data
			command.erase(command.begin());
			// allocate the params for this command (can't be stack allocated, as this memory will need to
			// be used later, but will be destroyed if left in stack memory)
			vector<string>* prams = new vector<string>;
			(*prams) = command;
			// dispatch an event, signaling a console command
			EventProc::GetInstance()->Dispatch(command_name.c_str(), (void*)prams, true);
			// end this line
			buffer_info.insert(buffer_info.begin(), { "", SGD::Color::White});

		}
		// if this is a backspace character...
		else if (c == L'\b')
		{
			// if the current line is empty...
			if (current_line->size() == 0)
			{
				// update previous character
				char_prev = c;
				// break out of this function
				return;
			}
			// delete the last character on the current line
			current_line->pop_back();
		}
		else
			// otherwise, just add this character to the current line
			current_line->push_back(char(c));		
	}
	// update the previous character
	char_prev = c;
}

void Console::Draw(void)
{
	// render the background of the console
	SGD::GraphicsManager::GetInstance()->DrawRectangle({ 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }, { 128, 0, 0, 0 });
	// store a value for the current y position, starting at the bottom of the screen
	int y = WINDOW_HEIGHT - CHAR_HEIGHT;

	// for every line in the buffer
	for (unsigned int i = 0; i < buffer_info.size(); i++)
	{
		// draw this line to the screen
		SGD::GraphicsManager::GetInstance()->DrawString(buffer_info[i].line.c_str(), { 0.f, (float)y }, buffer_info[i].color);
		// decrament 'y' by the height of a single character
		y -= CHAR_HEIGHT;
	}
}

void Console::WriteLine(const char* line, SGD::Color color)
{
	line_info l;
	l.line = line;
	l.color = color;

	buffer_info.insert(buffer_info.begin(), l);
	buffer_info.insert(buffer_info.begin(), { "", SGD::Color::White });
}

/*static*/ Console* Console::GetInstance(void)
{
	static Console singleton;

	return &singleton;
}