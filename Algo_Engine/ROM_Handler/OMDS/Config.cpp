// CConfiguration.cpp: implementation of the CConfiguration class.
//
//////////////////////////////////////////////////////////////////////
//
//
//	CConfiguration retrieves config information from the registry
//	all entries in the registry are strings
//

#include "stdafx.h"
#include "Config.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
static 	CRegKey		config;
static	bool		created = false;

CConfiguration::CConfiguration()
{
	if (!created)
	{
		config.Create(HKEY_LOCAL_MACHINE,BASE_REGISTRY, REG_NONE, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, NULL);
	};
}

CConfiguration::~CConfiguration()
{

}

void CConfiguration::setValue(std::string param, std::string value)
{
	config.SetValue(value.c_str(),param.c_str());
}

void CConfiguration::setDWORD(std::string param, DWORD value)
{
	config.SetValue(value,param.c_str());
}

std::string CConfiguration::getValue(std::string param)
{
	unsigned long BufferSize = 512;
	char regVal[1024];
	int returnVal;

	returnVal = config.QueryValue(regVal, param.c_str(), &BufferSize);

	if (returnVal != ERROR_SUCCESS) {
		config.SetValue("", param.c_str());
		lstrcpy(regVal, "");
	}

	return((std::string) regVal);
}

DWORD CConfiguration::getDWORD(std::string param)
{
	DWORD regVal;
	if (config.QueryValue(regVal, param.c_str()) != ERROR_SUCCESS) {
		config.SetValue(DWORD(0), param.c_str());
		regVal = 0;
	}
	return regVal;
}

void CConfiguration::setRoot(std::string r)
{
	std::string root = BASE_REGISTRY;

	if (r.size())
		root += "\\" + r;

	config.Detach();
	config.Create(HKEY_LOCAL_MACHINE,root.c_str());
}

