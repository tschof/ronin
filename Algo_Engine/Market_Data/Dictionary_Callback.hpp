#ifndef __DICTIONARY_HPP__
#define __DICTIONARY_HPP__

#pragma warning (push)
#pragma warning (disable:4100)
#pragma warning (disable:4512)
#include <mama/mamacpp.h>
using namespace Wombat;
#pragma warning (pop)

#include <iostream>
using namespace std;

class Dictionary_Callback : public MamaDictionaryCallback
{
public:
  Dictionary_Callback (mamaBridge bridge)
    : bridge_ (bridge),
      dict_fetch_successful_ (false)
  {
  }

  virtual void onTimeout ()
  {
    cerr << "Getting dictionary timed out (60s?)" << endl;
    Mama::stop (bridge_);
  }

  virtual void onError (const char *errmsg)
  {
    cerr << "Error fetching dictionary: " << errmsg << endl;
    Mama::stop (bridge_);
  }

  virtual void onComplete ()
  {
    cout << "Success!  Got the dictionary!" << endl;
    dict_fetch_successful_ = true;
    Mama::stop (bridge_);
  }

  bool fetchSuccessful () const
  {
    return dict_fetch_successful_;
  }

private:
  mamaBridge bridge_;
  bool dict_fetch_successful_;
};


#endif