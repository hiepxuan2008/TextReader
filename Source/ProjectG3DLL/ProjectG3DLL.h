// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PROJECTG3DLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PROJECTG3DLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PROJECTG3DLL_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

// This class is exported from the ProjectG3DLL.dll
class EXPORT CProjectG3DLL {
public:
	CProjectG3DLL(void);
	// TODO: add your methods here.
};

extern EXPORT int nProjectG3DLL;

EXPORT int fnProjectG3DLL(void);
