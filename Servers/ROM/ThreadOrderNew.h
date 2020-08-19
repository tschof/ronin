#ifndef _THREADORDERNEW_H__
#define _THREADORDERNEW_H__

#include "pthread.h"
#include "RomOrderNew.h"
#include "Csv.h"
#include "ClientProfileManagerNew.h"


class CThreadOrderNew
{
public:
  CThreadOrderNew(CRomOrderNew* pOrder = NULL);
  ~CThreadOrderNew();

  void SetOrder(CRomOrderNew* pOrder);
  void GetOrder(CRomOrderNew* pOrder);
  //void GetOrderWithRomExTag(CRomOrderNew* pOrder, std::string& romExTag);
  void GetCsv(CCsv& csv);
  void GetNextRomExecTag(std::string& romExecTag);
  //bool UpdateReplacingFields(CCsv& msg, tKeyedBalanceUpdate& update, std::string& err);
  void ResetReplacingFields();
  void SetActClientTag(const char* pTag);
  void SetStatus(short status);
  void SetUsed(bool value);
  void SetCancelPending(bool value);
  void SetReplacePending(bool value);
  CRomOrderNew* Reset();
  bool ReplacePending();
  short GetCplxType();
  //long UpdateFromChildFill(long last, tBalanceUpdate& BUpdate, CRomOrderNew* pOrder);
  void SetShareChange(long shareChange, std::string& romTag);
  void GetClientTag(char* tag, int len);
  void GetClientTag(std::string& clientTag);
  void GetOwner(char* owner, int len);
  void GetDest(char* dest, int len);
  void SetRatio(long value);
  short GetStatus();
  bool TurnOnCancelPending(const std::string& newTag,
                           bool ownerCancel,
                           CCsv* pCancel);
  bool UpdateParent();

  void SendOatsMsg(CCsv* pCsv);
  void sendOatsMsg(CCsv* pCsv);
  void ProcessUpdate(CCsv* pCsv);

  bool ProcessCancel(CCsv* pCancel, const std::string& actTag, bool ownercancel, CClientConnection* pCon = NULL);
  bool ProcessReplace(CCsv* pReplace, CClientConnection* pCon = NULL);

  //void DoOats(CCsv& msg);
  bool Dead();
  long Ratio();
  bool DeadSpread();
  short TIF();
  void GetRomTag(std::string& romTag);
  bool Filter(const CCsv& filter);

  bool GetPosUpdate(tPosUpdate& update);
  short GetTif();

private:


  CRomOrderNew* m_pOrder;
  pthread_rwlock_t m_lock;

};

#endif //_THREADORDERNEW_H__
