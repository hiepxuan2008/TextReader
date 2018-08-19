// TTSLib: The lib of Text to Speech in WinApi C++ Programming
// @Author: Mai Thanh Hiep (1312206)
// @Organization: University of Science
// @Email: hiepxuan2008@gmail.com
// @Facebook: facebook.com/hiepit
// @Built at: 15/11/2015

#include "stdafx.h"
#include "TTSLib.h"
#include <WinNT.h>
#define MAX_LOADSTRING 100

TTSLib::TTSLib()
{
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
#include <mlang.h>
void TTSLib::LoadLanguageID()
{
	std::vector<WCHAR*> names;
	std::vector<WORD> ids;
	HRESULT hr = CoInitialize(NULL);
	if (SUCCEEDED(hr))
	{
		IMultiLanguage * pml;
		hr = CoCreateInstance(CLSID_CMultiLanguage, NULL, CLSCTX_ALL, IID_IMultiLanguage, (void**)&pml);
		if (SUCCEEDED(hr))
		{
			RFC1766INFO info;
			ULONG num;
			IEnumRfc1766 *rfc1766;
			hr = pml->EnumRfc1766(&rfc1766);
			if (SUCCEEDED(hr))
			{
				
				while (S_OK == rfc1766->Next(1, &info, &num))
				{
					WORD wLangID = LANGIDFROMLCID(info.lcid);
					//You can also get the locale name such as 'English' or 'Chinese' at info.wszLocaleName[32]
					WCHAR* name = info.wszLocaleName;
					WCHAR* nameCopy = new TCHAR[wcslen(name) + 1];
					wcscpy(nameCopy, name);

					languagesID[wLangID] = nameCopy;
				}
			}
		}
		pml->Release();
	}
	CoUninitialize();
}

LANGID TTSLib::getLangeIdByLangeName(WCHAR* langeName) {
	for (auto lang : languagesID) {
		if (wcscmp(langeName, lang.second) == 0) {
			return lang.first;
		}
	}
	return -1;
}

//Get available voices in system
std::vector<WCHAR*> TTSLib::GetVoiceNamesByLanguageName(WCHAR* languageName)
{
	std::vector<WCHAR*> voiceNames;
	WCHAR *description;
	LANGID langeId;
	LANGID _langeId = getLangeIdByLangeName(languageName);

	for (size_t i = 0; i < m_ispObjectTokens.size(); ++i)
	{
		SpGetLanguageFromToken(m_ispObjectTokens[i], &langeId);
		SpGetDescription(m_ispObjectTokens[i], &description);
		if (_langeId == -1 || langeId == _langeId) {
			SpGetDescription(m_ispObjectTokens[i], &description);
			voiceNames.push_back(description);
		}
	}

	return voiceNames;
}

std::vector<WCHAR*> TTSLib::GetVoiceLanguages()
{
	std::vector<WCHAR*> voiceLanguages;
	LANGID id;
	std::unordered_set<LANGID> checkDup;

	for (size_t i = 0; i < m_ispObjectTokens.size(); ++i)
	{
		SpGetLanguageFromToken(m_ispObjectTokens[i], &id);
		if (checkDup.find(id) == checkDup.end())
		{
			checkDup.insert(id);
			voiceLanguages.push_back(languagesID[id]);
		}
	}
	return voiceLanguages;
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