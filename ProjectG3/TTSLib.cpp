// TTSLib: The lib of Text to Speech in WinApi C++ Programming
// @Author: Mai Thanh Hiep (1312206)
// @Organization: University of Science
// @Email: hiepxuan2008@gmail.com
// @Facebook: facebook.com/hiepit
// @Built at: 15/11/2015

#include "stdafx.h"
#include "TTSLib.h"
#define MAX_LOADSTRING 100

TTSLib::TTSLib()
{
	languagesID.resize(100000);
	LoadLanguageID();
}

TTSLib::~TTSLib()
{
	Destroy();
}

HRESULT TTSLib::Initialize()
{
	HRESULT hr = S_OK;
	CComPtr<IEnumSpObjectTokens>   cpEnum;
	ULONG                          ulCount = 0;

	//Init SAPI
	if (FAILED(::CoInitialize(NULL)))
	{
		m_lastErrorMessage = L"CoInitialize failed!";
		return FALSE;
	}

	// Create the SAPI voice.
	hr = m_ispVoice.CoCreateInstance(CLSID_SpVoice);

	if (FAILED(hr))
	{
		m_lastErrorMessage = L"CoCreateInstance failed!";
		return FALSE;
	}

	// Enumerate the available voices.
	hr = SpEnumTokens(SPCAT_VOICES, NULL, NULL, &cpEnum);

	if (FAILED(hr))
	{
		m_lastErrorMessage = L"SpEnumTokens failed!";
		return FALSE;
	}

	//Get count
	hr = cpEnum->GetCount(&ulCount);
	if (FAILED(hr))
	{
		m_lastErrorMessage = L"GetCount cpEnum failed!";
		return FALSE;
	}

	//Get available object token voices in system
	m_ispObjectTokens.resize(ulCount);
	for (size_t i = 0; i < ulCount; ++i)
	{
		cpEnum->Next(1, &m_ispObjectTokens[i], NULL);
	}

	return TRUE;
}

HRESULT TTSLib::Destroy()
{
	for (size_t i = 0; i < m_ispObjectTokens.size(); ++i)
		m_ispObjectTokens[i].Release();

	::CoUninitialize();
	return S_OK;
}

CString TTSLib::GetLastErrorMessage()
{
	return m_lastErrorMessage;
}

//Load language indentity
void TTSLib::LoadLanguageID()
{
	LPTSTR buff;
	freopen(LANGUAGE_ID_DATA, "rt", stdin);
	int n, x;
	wscanf(L"%d\n", &n);
	for (int i = 0; i < n; i++)
	{
		buff = new WCHAR[MAX_LOADSTRING];
		wscanf(L"%x", &x);
		wscanf(L"%[^\n]\n", buff);
		languagesID[x] = buff;
		//languagesID.push_back(buff);
	}
}

//Get available voices in system
std::vector<WCHAR*> TTSLib::GetVoices(WCHAR* language)
{
	std::vector<WCHAR*> voiceNames;
	voiceNames.resize(m_ispObjectTokens.size());
	WCHAR *description;
	LANGID id;
	int voiceCount = 0;

	for (size_t i = 0; i < m_ispObjectTokens.size(); ++i)
	{
		SpGetDescription(m_ispObjectTokens[i], &description);
		if (language != NULL)
		{
			SpGetLanguageFromToken(m_ispObjectTokens[i], &id);
			if (wcscmp(language, languagesID[id]) == 0)
			{
				voiceNames[voiceCount] = description;
				voiceCount++;
			}
		}
		else
			voiceNames[i] = description;
	}
	
	if (language != NULL)
		voiceNames.resize(voiceCount);

	return voiceNames;
}

std::vector<WCHAR*> TTSLib::GetLanguages()
{
	std::vector<WCHAR*> voiceLanguages;
	std::vector<WCHAR*> temp;// = { NULL };
	temp.resize(100000, NULL);
	voiceLanguages.resize(m_ispObjectTokens.size());
	LANGID id;
	int languageCount = 0;
	for (size_t i = 0; i < m_ispObjectTokens.size(); ++i)
	{
		SpGetLanguageFromToken(m_ispObjectTokens[i], &id);
		if (temp[id] == NULL)
		{
			temp[id] = languagesID[id];
			voiceLanguages[languageCount] = languagesID[id];
			languageCount++;
		}
	}
	voiceLanguages.resize(languageCount);
	return voiceLanguages;
}

//Get count of available voices in system
int TTSLib::GetVoiceCount()
{
	return (int)m_ispObjectTokens.size();
}

//Set voice by index
HRESULT TTSLib::SetVoice(int idx)
{
	if (idx >= 0 && idx < (int)m_ispObjectTokens.size())
	{
		return m_ispVoice->SetVoice(m_ispObjectTokens[idx]);
	}
	return FALSE;
}

//Set voice by index
HRESULT TTSLib::SetVoice(WCHAR* voiceName)
{
	WCHAR *description;

	for (size_t i = 0; i < m_ispObjectTokens.size(); ++i)
	{
		SpGetDescription(m_ispObjectTokens[i], &description);
		if (wcscmp(voiceName, description) == 0)
			return m_ispVoice->SetVoice(m_ispObjectTokens[i]);
	}
	return FALSE;
}

//Speak a text
HRESULT TTSLib::Speak(WCHAR* text)
{
	return m_ispVoice->Speak(text, SPF_DEFAULT, NULL);
}

//Speak a text asynchronous
HRESULT TTSLib::SpeakAsync(WCHAR* text)
{
	return m_ispVoice->Speak(text, SPF_ASYNC, NULL);
}

//Stop the speaking process
HRESULT TTSLib::Stop()
{
	return m_ispVoice->Speak(NULL, SPF_PURGEBEFORESPEAK, 0);
}

//Check if speaker is speaking
bool TTSLib::IsSpeaking()
{
	SPVOICESTATUS stat;
	m_ispVoice->GetStatus(&stat, NULL);
	return stat.dwRunningState == SPRS_IS_SPEAKING;
}

//Volume
HRESULT TTSLib::SetVolume(USHORT usVolume)
{
	if (usVolume >= 0 && usVolume <= 100)
		return m_ispVoice->SetVolume(usVolume);

	return FALSE;
}

HRESULT TTSLib::GetVolume(USHORT *pusVolume)
{
	return m_ispVoice->GetVolume(pusVolume);
}

//Rate
HRESULT TTSLib::SetRate(long lRate)
{
	if (lRate >= -10 && lRate <= 10)
		return m_ispVoice->SetRate(lRate);

	return FALSE;
}

HRESULT TTSLib::GetRate(long *plRate)
{
	return m_ispVoice->GetRate(plRate);
}