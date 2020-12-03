// SRLabWrapper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SRLabWrapper.h"

#include <md-core/mdapplication.h>
#include <md-framework/config/configexception.h>
#include <md-framework/resource/resourceexception.h>
#include <md-core/MDSessionMgr.h>
#include <md-core/MDPriceLevel.h>

#include <iomanip>

#using <mscorlib.dll>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

md_core::MDApplication *_mdApplication = 0;
md_framework::ConfigRP _configRP;
md_framework::ResourceManager *_resourceManager;
md_framework::Resource *_resource;

extern "C" MARKETDATA_API int Initialize(char* configFileLocation, char* configFile, char)
{
	int result = -1;

	_mdApplication = md_core::MDApplication::instance();
	_configRP = _mdApplication->load(configFileLocation, configFile, __MD_VERSION__);
	if (_configRP)
	{
		resourceManager = md_core::MDApplication::instance()->getResourceManager();
		if (_resourceManager)
		{
			_resource = const_cast<md_framework::Resource*>(_resourceManager->getResource(
		}
	}

	return result;
}