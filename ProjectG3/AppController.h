#pragma once
#include "TTSLib.h" //Lib text to speech win32 api c++ by Hiep IT
#include <shellapi.h> //NOTIFYICONDATA

#define CONFIG_FILE_NAME "config.txt"
#define DEFAULT_VOICE_IDX 0
#define DEFAULT_VOLUME 100
#define DEFAULT_RATE 10

//Singleton Pattern
class AppController
{
private:
	AppController();
public:
	static AppController* getInstance();
	~AppController();
public:
	HWND hLanguageComboBox, hVoiceComboBox, hVolSlideBar, hSpeedSlideBar, hCheckBoxStartUp;
	HKEY hkey = NULL;
	NOTIFYICONDATA nidApp;
	HWND hWnd;
	HINSTANCE hInst, hInstDLL;
	TTSLib speaker;

	int languageIdx, voiceIdx, volume, rate;
public:
	//Initialize first to store hWnd, hInst to process later
	void Initialize(HWND hWnd, HINSTANCE hInst);

	void initApp();
	void exitApp(); //Do something before exiting the app

	//Config
	void loadConfig();
	void saveConfig();

	//Hook
	void InstallHook();
	void RemoveHook();

	//Tray system
	void initTraySystem();
	void exitTraySystem();

	//Other
	void CreateControl();
	void setComboBoxData(HWND hWnd, std::vector<WCHAR*> voices);

public:
	void Speak(WCHAR* text);
	void Stop();
	bool IsSpeaking();
public:
	void OnChangeComboBoxLanguage();
	void OnChangeComboBoxVoice();
	void OnChangeTrackBar(HWND hTrackBar, int pos);
	void OnChangeCheckBoxStartUp();
};

