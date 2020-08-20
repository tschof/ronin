#ifndef __TRANSPORT_HPP__
#define __TRANSPORT_HPP__

#pragma warning (push)
#pragma warning (disable:4100)
#pragma warning (disable:4512)
#include <mama/mamacpp.h>
using namespace Wombat;
#pragma warning (pop)

#include <iostream>
using namespace std;

#include <windows.h>

class Transport_Callback : public MamaTransportCallback
{
  typedef MamaTransportCallback Parent;

public:

  virtual void onDisconnect (MamaTransport*)
  {
    cout << "Disconnected" << endl;
  }

  virtual void onReconnect (MamaTransport*)
  {
    cout << "Reconnected" << endl;
  }

  virtual void onQuality (MamaTransport *transport,
                          short,
                          const void*)
  {
    cout << "Quality message: "
         << mamaQuality_convertToString (transport->getQuality ())
         << endl;
  }

  virtual void onConnect (MamaTransport *ptransport)
  {
    Parent::onConnect (ptransport);
  }

  virtual void onConnectEx (MamaTransport *ptransport,
                            const void *platform_info)
  {
    Parent::onConnectEx (ptransport,
                         platform_info);
  }
};

#endif
