// Config.h: interface for the CConfig class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIG_H__FF0D5270_F40E_4110_AF88_F5DA6E254724__INCLUDED_)
#define AFX_CONFIG_H__FF0D5270_F40E_4110_AF88_F5DA6E254724__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning( disable : 4786 )

#include <string>
#include <map>
//using namespace std;

#include <atlbase.h>

// #include "OBSClass.h"

#define BASE_REGISTRY "SOFTWARE\\Ragnarok\\OMDS"

class CConfiguration
{
public:
	void setRoot(std::string root);
	CConfiguration();
	virtual ~CConfiguration();

	void		setValue(std::string param, std::string value);
	void		setDWORD(std::string param, DWORD value);
	std::string	getValue(std::string param);
	DWORD		getDWORD(std::string param);

};

#endif // !defined(AFX_CONFIG_H__FF0D5270_F40E_4110_AF88_F5DA6E254724__INCLUDED_)
