#pragma once

#include <Windows.h>

class CError
{
public:
	
	static HRESULT DispatchWin32Error(DWORD dwError,
					REFCLSID clsid, TCHAR * szSource,
					DWORD dwHelpContext,
					TCHAR * szHelpFileName);

	static HRESULT DispatchError(HRESULT hError,
					REFCLSID clsid, TCHAR * szSource,
					TCHAR * szDescription,
					DWORD dwHelpContext,
					TCHAR * szHelpFileName);
};
