// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PROJECTG3DLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PROJECTG3DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TEXTREADERLIB_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

// This class is exported from the TextReaderLib.dll
class EXPORT TextReaderLib {
public:
	TextReaderLib(void);
	// TODO: add your methods here.
};

extern EXPORT int nTextReaderLib;

EXPORT int fnTextReaderLib(void);
