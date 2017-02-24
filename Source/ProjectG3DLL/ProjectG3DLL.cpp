// ProjectG3DLL.cpp : Defines the exported functions for the DLL application.
//
// @Author: Nguyen Hoang Phuong 

#include "stdafx.h"
#include <iostream>
#define MYWM_SPEAK 10000
#define MYWM_STOP 10001
#define MYWM_ISPEAKING 10002

HHOOK hHookKey;
HINSTANCE hInst;
TCHAR *str;
HWND hAppWnd;

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam);

EXPORT void InstallHook(HWND hWnd){
	hAppWnd = hWnd;
	hHookKey = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, hInst, 0);
}

void UnInstallHook(){
	UnhookWindowsHookEx(hHookKey);
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode < 0)
		return CallNextHookEx(hHookKey, nCode, wParam, lParam);

	KBDLLHOOKSTRUCT *info = (KBDLLHOOKSTRUCT*)lParam;

	if (wParam == WM_KEYDOWN && GetAsyncKeyState(VK_CONTROL) && info->vkCode == 'E')
	{
		std::cout << "Press Ctrl + E" << std::endl;

		//Check if speaker is speaking
		//If speaking -> stop speak
		if (SendMessage(hAppWnd, MYWM_ISPEAKING, 0, 0))
		{
			SendMessage(hAppWnd, MYWM_STOP, 0, 0);
			return 1;
		}

		//Send Ctrl + C to copy text
		INPUT input;
		input.type = INPUT_KEYBOARD;
		input.ki.wScan = 0;
		input.ki.time = 0;
		input.ki.dwExtraInfo = 0;

		input.ki.wVk = VK_CONTROL;
		input.ki.dwFlags = 0;
		SendInput(1, &input, sizeof(INPUT));

		input.ki.wVk = 'C';
		input.ki.dwFlags = 0;
		SendInput(1, &input, sizeof(INPUT));

		input.ki.wVk = 'C';
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));

		input.ki.wVk = VK_CONTROL;
		input.ki.dwFlags = KEYEVENTF_KEYUP;
		SendInput(1, &input, sizeof(INPUT));

		//Delay 0.2s for copy text
		Sleep(200); 

		//Copy text from clipboard
		OpenClipboard(NULL);
		HGLOBAL hglb = GetClipboardData(CF_UNICODETEXT);
		TCHAR *pchData = (TCHAR*)GlobalLock(hglb);
		CloseClipboard();

		if (pchData){
			str = new TCHAR[wcslen(pchData) + 10];
			wcscpy(str, pchData);
			GlobalUnlock(hglb);
			//Send message Speak Text to mainApp
			SendMessage(hAppWnd, MYWM_SPEAK, 0, (LPARAM)str);

			delete[] str;
		}
		return 1;
	}

	return CallNextHookEx(hHookKey, nCode, wParam, lParam);
}