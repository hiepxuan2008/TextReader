#include "stdafx.h"
#include "AppController.h"
#include "TextReader.h"
#include "ControlUtils.h"

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
	int languageIdx, voiceIdx, volume, rate;
	loadConfig(languageIdx, voiceIdx, volume, rate);

	//Initialize speaker (Text To Speech)
	speaker.Initialize(); 

	//Load available languages
	std::vector<WCHAR*> languages = speaker.GetVoiceLanguages();
	setComboBoxData(hLanguageComboBox, languages);

	if (languageIdx >= languages.size())
		languageIdx = DEFAULT_LANGUAGE_IDX;
	ComboBox_SetCurSel(hLanguageComboBox, languageIdx);

	//Load available voices to Voice ComboBox
	std::vector<WCHAR*> voiceNames = speaker.GetVoiceNamesByLanguageName(languages[0]);
	setComboBoxData(hVoiceComboBox, voiceNames);

	//Select voice
	if (voiceIdx >= voiceNames.size())
		voiceIdx = DEFAULT_VOICE_IDX;
	ControlUtils::setComboBoxSelecetedIndex(hVoiceComboBox, voiceIdx);
	
	//Set pos volume slide bar
	ControlUtils::setSliderBarSelecetedIndex(hVolSlideBar, volume);

	//Set pos speed slide bar
	ControlUtils::setSliderBarSelecetedIndex(hSpeedSlideBar, rate);

	//Set voice, volume, speed for speaker
	speaker.SetVoice(voiceIdx);
	speaker.SetVolume(volume);
	speaker.SetRate(rate - 10);
}

void AppController::exitApp()
{
	saveConfig(); //Save voice, volume, rate config
	RemoveHook();
	exitTraySystem(); //Remove icon Tray System
}

void AppController::loadConfig(int &languageIdx, int &voiceIdx, int &volume, int &rate)
{
	FILE* pFile = fopen(CONFIG_FILE_NAME, "r");
	if (pFile == NULL) //If the file is not exists!
	{
		//Set the default value
		languageIdx = DEFAULT_LANGUAGE_IDX;
		voiceIdx = DEFAULT_VOICE_IDX;
		rate = DEFAULT_RATE;
		volume = DEFAULT_VOLUME;
	}
	else 
	{
		fscanf(pFile, "%d %d %d %d", &languageIdx, &voiceIdx, &volume, &rate);
		fclose(pFile);
	}
}

void AppController::saveConfig()
{
	int languageIdx = ControlUtils::getComboBoxSelectedIndex(hLanguageComboBox);
	int voiceIdx = ControlUtils::getComboBoxSelectedIndex(hVoiceComboBox);
	int volume = ControlUtils::getSliderBarSelecetedIndex(hVolSlideBar);
	int rate = ControlUtils::getSliderBarSelecetedIndex(hSpeedSlideBar);

	FILE* pFile = fopen(CONFIG_FILE_NAME, "w");
	fprintf(pFile, "%d %d %d %d", languageIdx, voiceIdx, volume, rate);
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

	hInstDLL = LoadLibrary(L"TextReaderLib.dll");
	if (hInstDLL){
		func = (Func)GetProcAddress(hInstDLL, "InstallHook");
		if (func)
			func(hWnd);
		else
			showMessage(L"InstallHook Error", L"Can't find InstallHook function");
	}
	else
	{
		showMessage(L"InstallHook Error", L"Load ProjectG3DLL.dll failed!");
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

void AppController::showMessage(WCHAR* title, WCHAR* message) {
	MessageBox(hWnd, message, title, MB_OK);
}

void AppController::Speak(WCHAR* text)
{
	if (FAILED(speaker.SpeakAsync(text)))
	{
		showMessage(L"Speak Error", L"Speak text failed!");
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
	WCHAR* languageName = ControlUtils::getComboBoxSelectedText(hLanguageComboBox);
	setComboBoxData(hVoiceComboBox, speaker.GetVoiceNamesByLanguageName(languageName));
	ControlUtils::setComboBoxSelecetedIndex(hVoiceComboBox, 0);
	OnChangeComboBoxVoice();
}

void AppController::OnChangeComboBoxVoice()
{
	WCHAR* buffer = ControlUtils::getComboBoxSelectedText(hVoiceComboBox);
	speaker.SetVoice(buffer);
}

void AppController::OnChangeTrackBar(HWND hTrackBar, int pos)
{
	if (hTrackBar == hVolSlideBar)
	{
		speaker.SetVolume(pos);
	}
	else if (hTrackBar == hSpeedSlideBar)
	{
		speaker.SetRate(pos - 10); //-10 cause rate from -10 to 10
	}
}

void AppController::OnChangeCheckBoxStartUp()
{
	static DWORD state;
	WCHAR* path = L"C:\\SimpleTTS\\TextReader.exe";
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