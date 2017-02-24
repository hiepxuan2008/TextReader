#include "stdafx.h"
#include "AppController.h"
#include "ProjectG3.h"
#include <CommCtrl.h>
#include <windowsx.h>
#include <winreg.h>
#define MAX_LOADSTRING 100

AppController::AppController()
{
}


AppController::~AppController()
{
	
}

AppController* AppController::getInstance()
{
	static AppController* _instance = new AppController();
	return _instance;
}

void AppController::Initialize(HWND hWnd, HINSTANCE hInst)
{
	this->hWnd = hWnd;
	this->hInst = hInst;
}

void AppController::initApp()
{
	//Create voice combobox, volume sliderbar, speed sliderbar, etc...
	CreateControl();
	//Install hook
	InstallHook();
	//Setup Tray System
	initTraySystem();

	//Load config from file
	loadConfig();
	//Initialize speaker (Text To Speech)
	speaker.Initialize(); 

	//Load available languages
	std::vector<WCHAR*> languages = speaker.GetLanguages();
	//languages.push_back(L"English");
	setComboBoxData(hLanguageComboBox, languages);
	ComboBox_SetCurSel(hLanguageComboBox, 0);

	//Load available voices to Voice ComboBox
	setComboBoxData(hVoiceComboBox, speaker.GetVoices(languages[0]));

	//Select voice
	if (voiceIdx >= speaker.GetVoiceCount())
		voiceIdx = 0;
	ComboBox_SetCurSel(hVoiceComboBox, voiceIdx);
	//Set pos volume slide bar
	SendMessage(hVolSlideBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)volume);
	//Set pos speed slide bar
	SendMessage(hSpeedSlideBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)rate);

	//Set rate, volume, voice for speaker
	speaker.SetRate(rate - 10);
	speaker.SetVolume(volume);
	speaker.SetVoice(voiceIdx);
}

void AppController::exitApp()
{
	saveConfig(); //Save voice, volume, rate config
	RemoveHook();
	exitTraySystem(); //Remove icon Tray System
}

void AppController::loadConfig()
{
	FILE* pFile = fopen(CONFIG_FILE_NAME, "r");
	if (pFile == NULL) //If the file is not exists!
	{
		voiceIdx = DEFAULT_VOICE_IDX, rate = DEFAULT_RATE, volume = DEFAULT_VOLUME; //Set the default value
		return;
	}

	fscanf(pFile, "%d %d %d", &voiceIdx, &volume, &rate);
	fclose(pFile);
}

void AppController::saveConfig()
{
	FILE* pFile = fopen(CONFIG_FILE_NAME, "w");
	fprintf(pFile, "%d %d %d", voiceIdx, volume, rate);
	fclose(pFile);
}

void AppController::CreateControl()
{
	CreateWindow(L"Static", L"Language:", WS_CHILD | WS_VISIBLE, 20, 20, 75, 20, hWnd, NULL, hInst, NULL);
	hLanguageComboBox = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE, 90, 20, 280, 100, hWnd, (HMENU)IDC_COMBOBOX_LANGUAGE, hInst, NULL);

	CreateWindow(L"Static", L"Voice:", WS_CHILD | WS_VISIBLE, 20, 60, 75, 20, hWnd, NULL, hInst, NULL);
	hVoiceComboBox = CreateWindow(L"COMBOBOX", L"", CBS_DROPDOWNLIST | WS_VSCROLL | WS_HSCROLL | WS_CHILD | WS_VISIBLE, 90, 60, 280, 100, hWnd, (HMENU)IDC_COMBOBOX_VOICE, hInst, NULL);

	CreateWindow(L"Static", L"Volume:", WS_CHILD | WS_VISIBLE, 20, 100, 75, 20, hWnd, NULL, hInst, NULL);
	hVolSlideBar = CreateWindow(TRACKBAR_CLASS, L"", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ | TBS_TOOLTIPS, 90, 95, 280, 30, hWnd, (HMENU)IDC_SLIDEBAR_VOLUME, NULL, NULL);
	SendMessage(hVolSlideBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 100));
	SendMessage(hVolSlideBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)100);
	CreateWindow(L"Static", L"  0                             50                           100", WS_CHILD | WS_VISIBLE, 90, 130, 280, 20, hWnd, NULL, hInst, NULL);

	CreateWindow(L"Static", L"Speed:", WS_CHILD | WS_VISIBLE, 20, 160, 75, 20, hWnd, NULL, hInst, NULL);
	hSpeedSlideBar = CreateWindow(TRACKBAR_CLASS, L"", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ | TBS_TOOLTIPS, 90, 155, 280, 30, hWnd, (HMENU)IDC_SLIDEBAR_SPEED, NULL, NULL);
	SendMessage(hSpeedSlideBar, TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(0, 20));
	SendMessage(hSpeedSlideBar, TBM_SETPOS, (WPARAM)TRUE, (LPARAM)10);
	CreateWindow(L"Static", L"  0                             10                            20", WS_CHILD | WS_VISIBLE, 90, 190, 280, 20, hWnd, NULL, hInst, NULL);

	hCheckBoxStartUp = CreateWindow(L"Button", L"Start with Windows", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 230, 215, 280, 20, hWnd, (HMENU)IDC_CHECKBOX_STARTUP, hInst, NULL);
}

void AppController::InstallHook()
{
	typedef void(*Func)(HWND);
	Func func;

	hInstDLL = LoadLibrary(L"ProjectG3DLL.dll");
	if (hInstDLL){
		func = (Func)GetProcAddress(hInstDLL, "InstallHook");
		if (func)
			func(hWnd);
		else
			MessageBox(hWnd, L"Can't find InstallHook function", L"InstallHook Error", MB_OK);
	}
	else
	{
		MessageBox(hWnd, L"Load ProjectG3DLL.dll failed!", L"InstallHook Error", MB_OK);
	}
}

void AppController::RemoveHook()
{
	if (hInstDLL)
		FreeLibrary(hInstDLL);
}

void AppController::initTraySystem()
{
	nidApp.cbSize = sizeof(NOTIFYICONDATA);
	nidApp.hWnd = hWnd;
	nidApp.uID = IDI_SYSTRAY;
	nidApp.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	wcscpy(nidApp.szTip, L"Text Speaker");
	nidApp.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_PROJECTG3));
	nidApp.uCallbackMessage = MYWM_SYSTEMT_RAY;
	Shell_NotifyIcon(NIM_ADD, &nidApp);
}

void AppController::exitTraySystem()
{
	Shell_NotifyIcon(NIM_DELETE, &nidApp);
}

void AppController::setComboBoxData(HWND hWnd, std::vector<WCHAR*> data)
{
	SendMessage(hWnd, CB_RESETCONTENT, 0, 0);
	for (size_t i = 0; i < data.size(); ++i)
		SendMessage(hWnd, CB_ADDSTRING, (WPARAM)MAX_LOADSTRING, (LPARAM)(data[i]));
}

void AppController::Speak(WCHAR* text)
{
	if (FAILED(speaker.SpeakAsync(text)))
	{
		MessageBox(hWnd, L"Speak text failed!", L"Speak Error", MB_OK);
	}
}

void AppController::Stop()
{
	speaker.Stop();
}

bool AppController::IsSpeaking()
{
	return speaker.IsSpeaking();
}

void AppController::OnChangeComboBoxLanguage()
{
	std::vector<WCHAR*> languages = speaker.GetLanguages();
	languageIdx = ComboBox_GetCurSel(hLanguageComboBox);
	setComboBoxData(hVoiceComboBox, speaker.GetVoices(languages[languageIdx]));
	ComboBox_SetCurSel(hVoiceComboBox, 0);
	OnChangeComboBoxVoice();
}

void AppController::OnChangeComboBoxVoice()
{
	WCHAR* buffer = new WCHAR[MAX_LOADSTRING];
	SendMessage(hVoiceComboBox, WM_GETTEXT, (WPARAM)MAX_LOADSTRING, (LPARAM)buffer);
	speaker.SetVoice(buffer);
}

void AppController::OnChangeTrackBar(HWND hTrackBar, int pos)
{
	if (hTrackBar == hVolSlideBar)
	{
		volume = pos;
		speaker.SetVolume(volume);
	}
	else if (hTrackBar == hSpeedSlideBar)
	{
		rate = pos;
		speaker.SetRate(rate - 10); //-10 cause rate from -10 to 10
	}
}

void AppController::OnChangeCheckBoxStartUp()
{
	static DWORD state;
	WCHAR* path = L"D:\\STUDY\\UNIVERSITY\\HK5\\C4WIN\\Group7\\Debug\\ProjectG3.exe";
	BOOL checked = IsDlgButtonChecked(hWnd, IDC_CHECKBOX_STARTUP);
	if (!checked)
	{
		CheckDlgButton(hWnd, IDC_CHECKBOX_STARTUP, BST_CHECKED);
		{
			RegCreateKeyEx(
				HKEY_CURRENT_USER,
				_T("Software\\Microsoft\\Windows\\CurrentVersion\\Run"),
				0,
				NULL,
				REG_OPTION_NON_VOLATILE,    // special options flag
				KEY_QUERY_VALUE | KEY_SET_VALUE,        // desired security access
				NULL,
				&hkey,          // receives opened handle
				&state);
			RegSetValueEx(hkey, L"ProjectG3", 0, REG_SZ, (LPBYTE)path, (_tcsclen(path) + 1)*sizeof(TCHAR));
		}
	}
	else
	{
		CheckDlgButton(hWnd, IDC_CHECKBOX_STARTUP, BST_UNCHECKED);
		RegDeleteValue(hkey, L"ProjectG3");
	}
	
}