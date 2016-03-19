/***********************************************************************\
|																		|
|	File:			WinMain.cpp			  								|
|	Author:			Douglas Monroe										|
|	Last Modified:	2014-02-03											|
|																		|
|	Purpose:		To run the basic window framework for games			|
|																		|
|	?2014 Full Sail, Inc. All rights reserved. The terms "Full Sail", 	|
|	"Full Sail University", and the Full Sail University logo are 	   	|
|	either registered service marks or service marks of Full Sail, Inc.	|
|																		|
\***********************************************************************/

#include "globals.h"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>		// Win32 Application
//#include <vld.h>			// Visual Leak Detector!!!	removed visual leak detector because kept causing LNK error
#include "Game.h"			// Our Game class
#include "../SGD Wrappers/SGD_InputManager.h"
#include <crtdbg.h>
#include "globals.h"
#include "../source/GameplayState.h"
#include "Console.h"

//*********************************************************************//
// Preprocessor Constants
#define WINDOW_CLASS_NAME	((const wchar_t*)L"Desolate")		// window class name
#define WINDOW_TITLE		((const wchar_t*)L"Desolate")		// window title


//*********************************************************************//
// Function Declarations
int WINAPI			WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
bool				IsAlreadyRunning(void);
HWND				MakeWindow(HINSTANCE hInstance);
LRESULT CALLBACK	WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//*********************************************************************//
// WinMain
//	- application entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	// _CrtSetBreakAlloc(-1);
	ShowCursor(false);
	// Don't let more than one instance of the application exist
	if (IsAlreadyRunning() == true)
		return 1;

	// Create the window
	HWND hWnd = MakeWindow(hInstance);
	if (hWnd == 0)
	{
		MessageBoxW(HWND_DESKTOP, L"ERROR: Failed to create the Main Window.", WINDOW_TITLE, MB_OK | MB_ICONERROR);
		return -2;
	}

	HICON hIcon = (HICON)LoadImage(NULL, L"resources/graphics/desolateIcon.ico", IMAGE_ICON, 48, 48, LR_LOADFROMFILE);
	SendMessage(hWnd, WM_SETICON, ICON_SMALL, LPARAM(hIcon));


	// Display the window
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);


	/////////////////////////////////////////////////////////////////////
	// Initialize game

	// Access the singleton
	Game* pGame = Game::GetInstance();

	// Initialize
	//need something to trigger a seperate build like a check for an arcade machine gamepad or something
	if (pGame->Initialize(WINDOW_WIDTH, WINDOW_HEIGHT) == false)
		return -3;

	// Run the message loop
	MSG msg = {};
	while (true)
	{
		if (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE) == TRUE)
		{
			// Quit the application?
			if (msg.message == WM_QUIT)
				break;

			// Send the message to the window proc
			DispatchMessageW(&msg);
		}
		else
		{
			/////////////////////////////////////////////////////////////
			// Run game
			int result = pGame->Main();
			if (result != 0)
				PostQuitMessage(result);
		}
	}


	/////////////////////////////////////////////////////////////////////
	// Terminate game

	pGame->Terminate();
	pGame = nullptr;
	Game::DeleteInstance();



	// Unregister the window class
	UnregisterClassW(WINDOW_CLASS_NAME, hInstance);

	// Return message's Quit code to the OS
	return (int)(msg.wParam);
}


//*********************************************************************//
// IsAlreadyRunning
//	- check if another instance of this application is running
//	- set focus to the other instance if it exists
bool IsAlreadyRunning(void)
{
	// Is there another window with the same class name & title?
	HWND hOtherWnd = FindWindowW(WINDOW_CLASS_NAME, WINDOW_TITLE);
	if (hOtherWnd != 0)
	{
		// Is it minimized?
		if (IsIconic(hOtherWnd) == TRUE)
			ShowWindow(hOtherWnd, SW_RESTORE);

		// Bring it to the foreground
		SetForegroundWindow(hOtherWnd);
		return true;
	}

	// No other instances are running
	return false;
}


//*********************************************************************//
// MakeWindow
//	- registers & creates the application's window
HWND MakeWindow(HINSTANCE hInstance)
{
	// Window class description
	WNDCLASSEXW	winClassEx;

	// Set window class attributes
	winClassEx.cbSize = sizeof(winClassEx);
	winClassEx.style = CS_DBLCLKS | CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	winClassEx.lpfnWndProc = WindowProc;
	winClassEx.cbClsExtra = 0;
	winClassEx.cbWndExtra = 0;
	winClassEx.hInstance = hInstance;
	winClassEx.hIcon = LoadIconW(NULL, (LPWSTR)IDI_APPLICATION);
	winClassEx.hIconSm = LoadIconW(NULL, (LPWSTR)IDI_APPLICATION);
	winClassEx.hCursor = LoadCursorW(NULL, (LPWSTR)IDC_ARROW);
	winClassEx.hbrBackground = (HBRUSH)(1 + COLOR_WINDOW);
	winClassEx.lpszMenuName = NULL;
	winClassEx.lpszClassName = WINDOW_CLASS_NAME;

	// Register the window class
	ATOM atomClass = RegisterClassExW(&winClassEx);
	if (atomClass == 0)
		return 0;


	// Setup window style flags
	DWORD dwWindowStyleFlags = WS_VISIBLE | WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME;

	// Setup the desired client area size
	RECT rWindow;
	rWindow.left = 0;
	rWindow.top = 0;
	rWindow.right = WINDOW_WIDTH;
	rWindow.bottom = WINDOW_HEIGHT;

	// Get the dimensions of a window that will have a client rect that
	// will really be the resolution we're looking for.
	AdjustWindowRectEx(&rWindow, dwWindowStyleFlags, FALSE, WS_EX_APPWINDOW);

	// Calculate the the window's dimensions
	int nWindowWidth = rWindow.right - rWindow.left;
	int nWindowHeight = rWindow.bottom - rWindow.top;


	// Create the window
	HWND hWnd = CreateWindowExW(
		WS_EX_APPWINDOW,										// extended style flags
		WINDOW_CLASS_NAME,										// window class Name
		WINDOW_TITLE,											// title
		dwWindowStyleFlags,										// window style flags
		(GetSystemMetrics(SM_CXSCREEN) - nWindowWidth) / 2,		// position (x, y)
		(GetSystemMetrics(SM_CYSCREEN) - nWindowHeight) / 2,		//	- centered on the desktop
		nWindowWidth,											// width
		nWindowHeight,											// height
		HWND_DESKTOP,											// parent window handle
		NULL,													// menu handle
		hInstance,												// application instance
		NULL													// creation parameters
		);

	// Did that fail?
	if (hWnd == 0)
		UnregisterClassW(WINDOW_CLASS_NAME, hInstance);

	// Return the handle to the new window
	return hWnd;
}


//*********************************************************************//
// WindowProc
//	- window message handler
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// What is the message?
	switch (msg)
	{
	case WM_CREATE:			// Window constructed
		break;

	case WM_KILLFOCUS:
		if (Game::GetInstance()->GetCurrentState() == GameplayState::GetInstance())
			if (!GameplayState::GetInstance()->GetWin() && !GameplayState::GetInstance()->GetLose())
				GameplayState::GetInstance()->SetPaused(true); // Pausing the game if the window loses focus while in the gameplay state
		break;

	case WM_SETFOCUS:
		break;

	case WM_CLOSE:			// Window closed
		DestroyWindow(hWnd);	// completely destroy the window
		break;

	case WM_SIZE:
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;
		Game::GetInstance()->SetScreenWidth(width);
		Game::GetInstance()->SetScreenHeight(height);
		SGD::GraphicsManager::GetInstance()->Resize({ (float)width, (float)height }, !Game::GetInstance()->getFullScreen());
		break;
	}

	case WM_DESTROY:		// Window destroyed
		PostQuitMessage(0);	// completely quit the application
		break;

	case WM_SYSKEYUP:		// System menu (ALT pop-up)
	case WM_SYSCHAR:
		break;

	case WM_ACTIVATE:		// Window activated / deactivated
		if (LOWORD(wParam) != WA_INACTIVE)	//	gaining focus (unpause)
		{
		}
		else									//	losing focus (pause)
		{
		}
		break;

	case WM_PAINT:			// Window needs repainting
		ValidateRect(hWnd, nullptr);	// ignore painting
		break;

	default:				// Any unhandled messages
		return DefWindowProcW(hWnd, msg, wParam, lParam);	// handled by default proc
	}


	// Message has been handled by this proc.
	return 0;
}
