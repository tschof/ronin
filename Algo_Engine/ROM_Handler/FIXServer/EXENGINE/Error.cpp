#include "stdafx.h"
#include "Error.h"
#include <crtdbg.h>
#include <atlbase.h>

HRESULT CError::DispatchWin32Error(DWORD dwError,
								   REFCLSID clsid,
								   TCHAR * szSource,
								   DWORD dwHelpContext,
								   TCHAR * szHelpFileName)
{
	return DispatchError(HRESULT_FROM_WIN32(dwError),
			clsid, szSource, NULL, dwHelpContext,
			szHelpFileName);
}

HRESULT CError::DispatchError(HRESULT hError,
							  REFCLSID clsid,
							  TCHAR * szSource,
							  TCHAR * szDescription,
							  DWORD dwHelpContext,
							  TCHAR * szHelpFileName)
{
	USES_CONVERSION;

	LPOLESTR wszError = NULL;
	if(szDescription != NULL)
	{
		wszError = T2OLE(szDescription);
	}
	else
	{
		if(HRESULT_FACILITY(hError) == FACILITY_WIN32)
		{
			LPTSTR szError = NULL;
			if(!::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_ALLOCATE_BUFFER, NULL,
				HRESULT_CODE(hError),
				MAKELANGID(LANG_USER_DEFAULT, SUBLANG_DEFAULT),
				(LPTSTR)&szError, 0, NULL))
				return HRESULT_FROM_WIN32(GetLastError());

			if(szError != NULL)
			{
				wszError = T2OLE(szError);
				LocalFree(szError);
			}
		}
	}

	LPOLESTR wszSource = NULL;
	if(szSource != NULL)
		wszSource = T2OLE(szSource);

	LPOLESTR wszHelpFile = NULL;
	if(szHelpFileName != NULL)
		wszHelpFile = T2OLE(szHelpFileName);

	ICreateErrorInfo *pCreateErrorInfo = NULL;
    HRESULT hSuccess = CreateErrorInfo(&pCreateErrorInfo);
	ATLASSERT(SUCCEEDED(hSuccess));

	pCreateErrorInfo->SetGUID(clsid);
	if(wszError != NULL)
		pCreateErrorInfo->SetDescription(wszError);
	if(wszSource != NULL)
		pCreateErrorInfo->SetSource(wszSource);
	if(wszHelpFile != NULL)
		pCreateErrorInfo->SetHelpFile(wszHelpFile);
	pCreateErrorInfo->SetHelpContext(dwHelpContext);

	IErrorInfo *pErrorInfo = NULL;
	hSuccess = pCreateErrorInfo->QueryInterface(IID_IErrorInfo,
				(LPVOID *)&pErrorInfo);
	if(FAILED(hSuccess))
	{
		pCreateErrorInfo->Release();
		return hSuccess;
	}

	hSuccess = SetErrorInfo(0, pErrorInfo);
	if(FAILED(hSuccess))
	{
		pCreateErrorInfo->Release();
		pErrorInfo->Release();
		return hSuccess;
	}

	pCreateErrorInfo->Release();
	pErrorInfo->Release();
	
	return hError;
}
