#ifndef _MESSAGEBUFFER_H__
#define _MESSAGEBUFFER_H__

#include <windows.h>
#include <list>
#include "RomCsv.h"

//#pragma once

class CStringBuffer
{
public:
	CStringBuffer()
	{
		InitializeCriticalSection(&m_Lock);
	}

	~CStringBuffer()
	{
		DeleteCriticalSection(&m_Lock);
	}

	inline void AddBuffer(const std::string& msgs)
	{
		EnterCriticalSection(&m_Lock);
		m_Buffer += msgs;
		LeaveCriticalSection(&m_Lock);
	}

	inline void RetriveBuffer(std::string& msgs)
	{
		EnterCriticalSection(&m_Lock);
		msgs.clear();
		m_Buffer.swap(msgs);
		LeaveCriticalSection(&m_Lock);
	}


private:
	CRITICAL_SECTION m_Lock;
	std::string m_Buffer;
};


template<class T>
class CMessageBuffer
{
public:
	CMessageBuffer()
	{
		InitializeCriticalSection(&m_Lock);
	}

	~CMessageBuffer()
	{
		DeleteCriticalSection(&m_Lock);
	}
	
	void AddBuffer(T* pMsg)
	{
		EnterCriticalSection(&m_Lock);
		m_Buffer.push_back(pMsg);
		LeaveCriticalSection(&m_Lock);
	}

	void AddBuffer(const std::list<T*>& msg)
	{
		EnterCriticalSection(&m_Lock);
		T* pT = NULL;
		std::list<T*>::const_iterator it = msg.begin();
		while (it != msg.end())
		{
			pT = *it;
			m_Buffer.push_back(pT);
			it++;
		}
		LeaveCriticalSection(&m_Lock);
	}

	void RetriveBuffer(std::list<T*>& msgs)
	{
		EnterCriticalSection(&m_Lock);

		m_Buffer.swap(msgs);
		/*T* pT = NULL;
		while (m_Buffer.empty() == false)
		{
			pT = m_Buffer.front();
			msgs.push_back(pT);
			m_Buffer.pop_front();
		}*/
		LeaveCriticalSection(&m_Lock);
	}

private:
	CRITICAL_SECTION m_Lock;
	std::list<T*> m_Buffer;
	//std::string m_Buffer;
};

#endif //_MESSAGEBUFFER_H__