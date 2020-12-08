#pragma once
#include <iostream>

template <class MessageType>
class MessageFactory
{
public:
	MessageType* create(const std::string& rdata);
};

template<class MessageType>
inline MessageType* MessageFactory<MessageType>::create(const std::string& rdata)
{
	return new MessageType(rdata);
}
