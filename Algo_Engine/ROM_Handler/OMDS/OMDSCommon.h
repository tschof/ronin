
#ifndef OMDSCommonHeaderFile
#define OMDSCommonHeaderFile

#include "Config.h"
#include "Log.h"
#include "Runable.h"
#include "Lock.h"

#pragma warning( disable : 4786 )
#include <set>
#include <map>
#include <list>
#include <string>
#include <exception>
#include <vector>
//using namespace std;
class CMaskNotFoundException : public std::exception
{
public:
	CMaskNotFoundException(){}
	virtual ~CMaskNotFoundException(){}
};

#ifndef IntIntMap
	typedef std::map<int,int>					IntIntMap;
#endif

#ifndef StringStringMap
	typedef std::map<std::string,std::string>	StringStringMap;
#endif

#ifndef IntStringMap
	typedef std::map<int,std::string>			IntStringMap;
#endif

#ifndef IntList
	typedef std::list<int>						IntList;
#endif

#ifndef StringList
	typedef std::list<std::string>				StringList;
#endif

#ifndef IntSet
	typedef std::set<int>						IntSet;
#endif

#ifndef StringSet
	typedef std::set<std::string>				StringSet;
#endif

#define EVERYTHING		0xFFFFFFFF

#endif // OMDSCommonHeaderFile