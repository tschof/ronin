#include "ThreadOrder.h"
#include "ThreadOrderHandler.h"

CThreadOrder::CThreadOrder(CRomOrder* pOrder)
{
  pthread_rwlock_init(&m_lock, NULL);
  m_pOrder = pOrder;
}

CThreadOrder::~CThreadOrder(void)
{
  pthread_rwlock_destroy(&m_lock);
}

void CThreadOrder::SetOrder(CRomOrder* pOrder)
{
  //memcpy(m_pOrder, pOrder, sizeof(CRomOrder));
  m_pOrder = pOrder;
}

void CThreadOrder::SetUsed(bool value)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->SetUsed(value);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::SetCancelPending(bool value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_pOrder->SetCancelPending(value);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::SetReplacePending(bool value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_pOrder->SetReplacePending(value);
    pthread_rwlock_unlock(&m_lock);
}

CRomOrder* CThreadOrder::Reset()
{
  pthread_rwlock_wrlock(&m_lock);
  //memset(m_pOrder, 0, sizeof(CRomOrder));
  CRomOrder* rc = m_pOrder;
  m_pOrder = NULL;
  pthread_rwlock_unlock(&m_lock);
  return rc;
}

void CThreadOrder::SetActClientTag(const char* pTag)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->SetActClientTag(pTag);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::SetStatus(short status)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->SetStatus(status);
  pthread_rwlock_unlock(&m_lock);
}

/*long CThreadOrder::GetNextRomExecID()
{
  pthread_rwlock_wrlock(&m_lock);
  long rc = m_pOrder->NextRomExecID();
  pthread_rwlock_unlock(&m_lock);
  return rc;
}*/

void CThreadOrder::GetNextRomExecTag(std::string& romExecTag)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->NextRomExecTag(romExecTag);
  pthread_rwlock_unlock(&m_lock);
}


void CThreadOrder::UpdateOrder(CCsv& msg, CRomOrder* pOrder)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->Update(msg, pOrder);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::GetOrderWithRomExTag(CRomOrder* pOrder, std::string& romExTag)
{
    pthread_rwlock_wrlock(&m_lock);
    memcpy(pOrder, m_pOrder, sizeof(CRomOrder));
    m_pOrder->NextRomExecTag(romExTag);
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadOrder::TurnOnCancelPending(const std::string& newTag,
                                       bool ownerCancel,
                                       CRomOrder* pOrder,
                                       CThreadOrderHandler& hd,
                                       CCsv* pCancel,
                                       std::string& errStr)
{
    bool rc = true;

    pthread_rwlock_wrlock(&m_lock);

    if (true == ownerCancel &&
        m_pOrder->CancelPending() == true)
    {
        errStr.assign("Previous cancel is still pending in ROM");
        memcpy(pOrder, m_pOrder, sizeof(CRomOrder));
        rc = false;
    }
    else
    {
        if (m_pOrder->Leave() == 0)
        {
            errStr = "There are not leaves to cancel";
            memcpy(pOrder, m_pOrder, sizeof(CRomOrder));
            rc = false;
        }
        else
        {
            if (newTag.empty() == false)
            {
                m_pOrder->SetActClientTag(newTag.c_str());
            }

            if (ownerCancel)
            {
                m_pOrder->SetCancelPending(true);
            }

            memcpy(pOrder, m_pOrder, sizeof(CRomOrder));

            std::string romExTag;
            m_pOrder->NextRomExecTag(romExTag);
            pCancel->SetAt(ROMFIELDS::CSV_OMEXECUTION_TAG, romExTag);
            hd.DoRomCancelPending(pCancel, pOrder);
        }
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CThreadOrder::GetOrder(CRomOrder* pOrder)
{
  pthread_rwlock_rdlock(&m_lock);
  memcpy(pOrder, m_pOrder, sizeof(CRomOrder));
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::GetCsv(CCsv& csv)
{
  pthread_rwlock_rdlock(&m_lock);
  m_pOrder->GetCsv(csv);
  pthread_rwlock_unlock(&m_lock);
}

short CThreadOrder::GetCplxType()
{
  pthread_rwlock_rdlock(&m_lock);
  short rc = m_pOrder->CplxType();
  pthread_rwlock_unlock(&m_lock);
  return rc;
}

void CThreadOrder::UpdateReplacingFields(CCsv& msg)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->UpdateReplacingFields(msg);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::ResetReplacingFields()
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->ResetReplacingFields();
  pthread_rwlock_unlock(&m_lock);
}

bool CThreadOrder::ReplacePending()
{
  pthread_rwlock_rdlock(&m_lock);
  bool rc = m_pOrder->ReplacePending();
  pthread_rwlock_unlock(&m_lock);
  return rc;
}

long CThreadOrder::UpdateFromChildFill(long last, tBalanceUpdate& BUpdate, CRomOrder* pOrder)
{
    pthread_rwlock_wrlock(&m_lock);
    char secType = ROMVALUE::SECTYPE_EQUITY;
    double dollar = 0;
    long leave = m_pOrder->UpdateFromChildFill(last, secType, dollar);
    BUpdate.shareChange = -last * m_pOrder->Ratio();
    BUpdate.secType = secType;
    BUpdate.dollarChange = dollar;
    if (0 == leave)
    {
        BUpdate.orderChange = -1;
    }
    memcpy(pOrder, m_pOrder, sizeof(CRomOrder));
    pthread_rwlock_unlock(&m_lock);
    return leave;
}


void CThreadOrder::SetShareChange(long shareChange, std::string& romTag)
{
    pthread_rwlock_wrlock(&m_lock);
    romTag = m_pOrder->RomTag();
    m_pOrder->SetShareChange(shareChange);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::GetClientTag(char* tag, int len)
{
    pthread_rwlock_rdlock(&m_lock);
    CUtility::StrCpy(tag, len, m_pOrder->ClientTag());
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::GetOwner(char* owner, int len)
{
    pthread_rwlock_rdlock(&m_lock);
    CUtility::StrCpy(owner, len, m_pOrder->Owner());
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::GetDest(char* dest, int len)
{
    pthread_rwlock_rdlock(&m_lock);
    CUtility::StrCpy(dest, len, m_pOrder->Dest());
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrder::SetRatio(long value)
{
    pthread_rwlock_rdlock(&m_lock);
    m_pOrder->SetRatio(value);
    pthread_rwlock_unlock(&m_lock);
}

short CThreadOrder::GetStatus()
{
    pthread_rwlock_rdlock(&m_lock);
    short rc = m_pOrder->Status();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}
/*
CRomOrder* CThreadOrder::Destroy()
{
  pthread_rwlock_wrlock(&m_lock);

  memset(m_pOrder, 0, sizeof(CRomOrder));

  pthread_rwlock_unlock(&m_lock);

  return m_pOrder;
}
*/
