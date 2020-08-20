#ifndef _ADMINSESSIONCALLBACK_H__
#define _ADMINSESSIONCALLBACK_H__

#pragma once

#include "OatsServer.h"

template<class T, class L>
class CAdminSessionCallBack : public CCallback
{
public:
	CAdminSessionCallBack(T* pT, L* pL) : m_pT(pT), m_pL(pL)
	{
	}

	~CAdminSessionCallBack(void) {}

	inline DWORD Notify(const std::string& buffer, DWORD len)
	{
		std::string local = buffer;
		DWORD used = ProcessIncoming(local);
		return used;
	}

private:
	T* m_pT;
	L* m_pL;

	inline DWORD ProcessIncoming(std::string& l_message) 
	{
		DWORD currOff = 0;
		DWORD endOff = l_message.find(MESSAGE_END);
		
		while(endOff != std::string::npos) 
		{           
			DWORD len = endOff - currOff;
			std::string l_sub = l_message.substr(currOff, len);
			m_pT->ProcessMessage(l_sub, m_pL);
			currOff = endOff + 1;
			endOff = l_message.find(MESSAGE_END, currOff);
		}

		return currOff;
	}

};
#endif //_ADMINSESSIONCALLBACK_H__
