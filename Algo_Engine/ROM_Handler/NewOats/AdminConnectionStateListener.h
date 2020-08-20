#ifndef _ADMINCONNECTIONSTATELISTENER_H__
#define _ADMINCONNECTIONSTATELISTENER_H__

#include "Socket.h"

using namespace RASL;

template<class T>
class CAdminConnectionStateListener : public CSocketStateListener
{
public:
	CAdminConnectionStateListener(T* t, void(T::*_notify)(int)) : m_T(t), notify(_notify)
	{
	}

	CAdminConnectionStateListener() {}

	virtual void SocketStateUpdate(const int lastError)
	{
		(m_T->*notify)(lastError);
	}

private:
	T* m_T;
	void (T::*notify)(int error);
};

#endif //_ADMINCONNECTIONSTATELISTENER_H__
