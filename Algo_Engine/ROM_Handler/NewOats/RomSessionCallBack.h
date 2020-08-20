#ifndef _ROMSESSIONCALLBACK_H__
#define _ROMSESSIONCALLBACK_H__


#pragma once

#include "OatsServer.h"

template<class T, class L>
class CRomSessionCallBack : public CCallback
{
public:
	CRomSessionCallBack(T* pT, L* pL) : m_pT(pT), m_pL(pL)
	{
	}

	~CRomSessionCallBack(void) {}

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
		DWORD currOff = m_pT->ProcessMessage(l_message, m_pL);
		return currOff;

		/*while(endOff != std::string::npos) 
		{           
			DWORD len = endOff - currOff;
			std::string l_sub = l_message.substr(currOff, len);
			m_pT->ProcessMessage(l_sub);
			currOff = endOff + 1;
			endOff = l_message.find(MESSAGE_END, currOff);
		}

		return currOff;*/
	}

};

#endif //_ROMSESSIONCALLBACK_H__
