// ProjectG3.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ProjectG3.h"
#include <CommCtrl.h>
#include <Windowsx.h>
#include "AppController.h"

//
#include <io.h> 
#include <fcntl.h>
#include <iostream>

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void initConsole();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	//initConsole(); //Console for debugging

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_PROJECTG3, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PROJECTG3));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_PROJECTG3));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_PROJECTG3);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 300, 100, 400, 300, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_CREATE:
		AppController::getInstance()->Initialize(hWnd, hInst); //Store hWnd, hInst to process
		AppController::getInstance()->initApp(); //Init App
		break;
	case MYWM_SYSTEMT_RAY:
		switch (LOWORD(lParam))
		{
		case WM_LBUTTONDOWN:
			ShowWindow(hWnd, SW_SHOW);
			SetForegroundWindow(hWnd);
			break;
		default:
			break;
		}
		break;
	case WM_SYSCOMMAND:
		switch (wParam)
		{
		case SC_MINIMIZE:
			ShowWindow(hWnd, SW_HIDE);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			break;
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDC_COMBOBOX_VOICE: //Combobox notification
			if (wmEvent == CBN_SELCHANGE)
			{
				AppController::getInstance()->OnChangeComboBoxVoice();
			}
			break;
		case IDC_COMBOBOX_LANGUAGE:
			if (wmEvent == CBN_SELCHANGE)
			{
				AppController::getInstance()->OnChangeComboBoxLanguage();
			}
			break;
		case IDC_CHECKBOX_STARTUP:
			AppController::getInstance()->OnChangeCheckBoxStartUp();
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}

		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;

	//https://msdn.microsoft.com/en-us/library/windows/desktop/bb760149(v=vs.85).aspx#tkb_notifications
	case WM_HSCROLL: //Trackbar changing notifications
	{
					   int iPos = 0;
					   HWND hTrackBar = (HWND)lParam;
					   if (LOWORD(wParam) == TB_ENDTRACK)
					   {
						   iPos = SendMessage(hTrackBar, TBM_GETPOS, 0, 0);
						   AppController::getInstance()->OnChangeTrackBar(hTrackBar, iPos);
						   std::cout << iPos << std::endl;
					   }
	}
		break;
	case WM_DESTROY:
		AppController::getInstance()->exitApp();
		PostQuitMessage(0);
		break;
	case MYWM_SPEAK: //Event speak a text called from dll hook
	{
						 WCHAR* text = (WCHAR*)lParam;
						 std::cout << "MYWM_SPEAK:" << std::endl;
						 std::wcout << text << std::endl;
						 AppController::getInstance()->Speak(text);
						 break;
	}
	case MYWM_STOP: //Stop speaking
	{
					  AppController::getInstance()->Stop();
					  break;
	}
	case MYWM_ISPEAKING: //Check if speaker is speaking
	{
						   return AppController::getInstance()->IsSpeaking();
	}
	case WM_CTLCOLORSTATIC:
	{
							  HDC hdcStatic;
							  hdcStatic = (HDC)wParam;
							  SetTextColor(hdcStatic, RGB(0, 0, 0));
							  SetBkColor(hdcStatic, RGB(255, 255, 255));
							  return (INT_PTR)CreateSolidBrush(RGB(255, 255, 255));
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void initConsole()
{
	AllocConsole();

	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long)handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;

	std::cout << "Console for debugging" << std::endl;
}