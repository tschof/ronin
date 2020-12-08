#pragma once
#include <vector>
/**
* Ok this class is my attempt at making the design extensible. You could have other types of handlers
the take in data not in csv form and have them derive from this.
*/
using namespace std;
template <typename T >
class DataHandler {
public:
	virtual ~DataHandler() {}
	
	virtual void getData(vector<unique_ptr<T>>&  dStore, size_t maxVectorMem) = 0;
};