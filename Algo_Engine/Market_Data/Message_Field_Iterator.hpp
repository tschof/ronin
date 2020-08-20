#ifndef __MESSAGE_FIELD_ITERATOR_HPP__
#define __MESSAGE_FIELD_ITERATOR_HPP__

#pragma warning (push)
#pragma warning (disable:4100)
#pragma warning (disable:4512)
#include <mama/mamacpp.h>
using namespace Wombat;
#pragma warning (pop)

#include <iostream>
using namespace std;

class Message_Field_Iterator : public MamaMsgFieldIterator
{
public:
  virtual void onField (const MamaMsg&,
                        const MamaMsgField &field,
                        void*)
  {
    char buffer [128];
    memset (buffer, 0, sizeof (buffer));
    field.getAsString (buffer, sizeof (buffer));
    cout << field.getName ()
         << '(' << field.getFid ()
         << ':' << field.getTypeName () << ')'
         << " = " << buffer << endl;
  }
};

#endif