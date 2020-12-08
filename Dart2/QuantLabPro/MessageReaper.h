#pragma once
#include "Order.h"
class MessageReaper
{
public:
	MessageReaper() {}
	void reapPointer(Message* mess);
};

inline void MessageReaper::reapPointer(Message* mess)
{
	if (mess != nullptr) {
		delete mess;
		mess = nullptr;
	}
}