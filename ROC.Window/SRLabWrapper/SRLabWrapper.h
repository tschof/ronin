#include "stdafx.h"
#include "MutiTypedMap.h"

#ifdef SRLABWRAPPER_EXPORTS
#define MARKETDATA_API __declspec(dllexport)
#else
#define MARKETDATA_API __declspec(dllimport)
#endif

extern MARKETDATA_API int returnCode;

extern "C"
{
	MARKETDATA_API int Initialize(char* configFileLocation, char* configFile);
	MARKETDATA_API int Start();
	MARKETDATA_API int Stop();

	MARKETDATA_API int Subscribe(const char *symbol, const char *source, bool reload);
	MARKETDATA_API int SubscribeL2(const char *symbol, const char *source, bool reload);
	MARKETDATA_API int SubscribeBook(const char *symbol, const char *source, bool reload);

	MARKETDATA_API int Unsubscribe(const char *symbol);

	// Callbacks
	typedef void (__stdcall *OnChange ) (ManagedTypedMap);
	MARKETDATA_API void ReturnDelta(OnChange out);
}