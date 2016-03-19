/***************************************************************
|	File:		Console.h
|	Author:		Michael Mozdzierz
|	Date:		05/23/2014
|
***************************************************************/

#pragma once

#include <string>

using namespace std;

class Console
{
	struct line_info
	{
		string line;

		SGD::Color color;
	};

	vector<line_info> buffer_info;

	string* current_line = nullptr;

	/////////////////////////<Private ctor/dtor>///////////////////////////////////////////////////

	Console(void);
	~Console(void);
	
public:
	bool visible = false;

	void Draw(void);
	void Update(void);

	void WriteLine(const char* line, SGD::Color color = SGD::Color::White);

	/////////////////////////<Singleton accessor>///////////////////////////////////////////////////

	static Console* GetInstance(void);
};