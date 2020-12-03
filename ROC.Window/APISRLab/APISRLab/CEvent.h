#include <stdio.h>

[event_source(native)]
class CMsgSource 
{
public:
   __event void SRLabOnMessage(const char* msg, SRLabMessageHandler^ map);
};

[event_receiver(native)]
class CMsgReceiver 
{
public:
   virtual void SRLabMessageHandler(const char* msg, SRLabMessageHandler^ map);

   void hookEvent(CMsgSource* pSource) {
      __hook(&CMsgSource::SRLabOnMessage, pSource, &CMsgReceiver::SRLabMessageHandler);
   }

   void unhookEvent(CMsgSource* pSource) {
      __unhook(&CMsgSource::SRLabOnMessage, pSource, &CMsgReceiver::SRLabMessageHandler);
   }
};