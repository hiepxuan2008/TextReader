// TTSLib: The lib of Text to Speech in WinApi C++ Programming
// @Author: Mai Thanh Hiep (1312206)
// @Organization: University of Science
// @Email: hiepxuan2008@gmail.com
// @Facebook: facebook.com/hiepit
// @Built at: 15/11/2015

#pragma once
#include <vector>
#include <atlstr.h> //CString

#include <sapi.h> 
#include <sphelper.h> 
#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>
#include <stdio.h>
#include <unordered_map>
#include <unordered_set>

class TTSLib
{
public:
	TTSLib();
	~TTSLib();
public:
	HRESULT Initialize();
	HRESULT Destroy();
	CString GetLastErrorMessage();
public:
	//Load language indentity
	void LoadLanguageID();
	//Get available languages in system
	std::vector<WCHAR*> GetVoiceLanguages();
	LANGID TTSLib::getLangeIdByLangeName(WCHAR* langeName);
	//Get available voices in system
	std::vector<WCHAR*> GetVoiceNamesByLanguageName(WCHAR* languageName);
	//Set voice by index
	HRESULT SetVoice(int idx);
	//Set voice by name
	HRESULT SetVoice(WCHAR*);

	//Speak a text synchronous
	HRESULT Speak(WCHAR* text);
	//Speak a text asynchronous
	HRESULT SpeakAsync(WCHAR* text);
	//Stop the speaking process
	HRESULT Stop();
	//Check if speaker is speaking
	bool IsSpeaking();

	//Volume from 0 to 100
	HRESULT SetVolume(USHORT pusVolume);
	HRESULT GetVolume(USHORT* pusVolume);
	//Rate from -10 to 10
	HRESULT SetRate(long lrate);
	HRESULT GetRate(long* plRate);

private:
	CComPtr<ISpVoice> m_ispVoice;
	std::vector<CComPtr<ISpObjectToken>> m_ispObjectTokens; //Available object token voices in system
	CString m_lastErrorMessage;
	std::unordered_map<LANGID, WCHAR*> languagesID;
};

