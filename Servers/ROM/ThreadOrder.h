#ifndef _THREADORDER_H__
#define _THREADORDER_H__

#include "pthread.h"
#include "RomOrder.h"
#include "Csv.h"
#include "ClientProfileManager.h"

class CThreadOrderHandler;


class CThreadOrder
{
public:
  CThreadOrder(CRomOrder* pOrder = NULL);
  ~CThreadOrder();

  void SetOrder(CRomOrder* pOrder);
  void GetOrder(CRomOrder* pOrder);
  void GetOrderWithRomExTag(CRomOrder* pOrder, std::string& romExTag);
  void UpdateOrder(CCsv& msg, CRomOrder* pOrder);
  void GetCsv(CCsv& csv);
  void GetNextRomExecTag(std::string& romExecTag);
  void UpdateReplacingFields(CCsv& msg);
  void ResetReplacingFields();
  void SetActClientTag(const char* pTag);
  void SetStatus(short status);
  void SetUsed(bool value);
  void SetCancelPending(bool value);
  void SetReplacePending(bool value);
  CRomOrder* Reset();
  bool ReplacePending();
  short GetCplxType();
  long UpdateFromChildFill(long last, tBalanceUpdate& BUpdate, CRomOrder* pOrder);
  void SetShareChange(long shareChange, std::string& romTag);
  void GetClientTag(char* tag, int len);
  void GetOwner(char* owner, int len);
  void GetDest(char* dest, int len);
  void SetRatio(long value);
  short GetStatus();
  bool TurnOnCancelPending(const std::string& newTag,
                           bool ownerCancel,
                           CRomOrder* pOrder,
                           CThreadOrderHandler& hd,
                           CCsv* pCancel,
                           std::string& errStr);
  //long GetNextRomExecID();
  //CRomOrder* Destroy();

private:
  CRomOrder* m_pOrder;
  pthread_rwlock_t m_lock;

};

#endif
