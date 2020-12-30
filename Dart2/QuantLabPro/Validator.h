#pragma once
#include <string>
#include "Order.h"

class OrdMessageValidator
{
public:
	~OrdMessageValidator() {}
	OrdMessageValidator() {}
	bool validate(OrdMessage* message);
};

inline bool OrdMessageValidator::validate(OrdMessage* message)
{
	bool isValid = false;
	if (message->price > 0 && message->size > 0 && message->orderid.size() > 0) {
		isValid = true;
	}
	return isValid;
}
