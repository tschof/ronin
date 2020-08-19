#include "ThreadOrderNew.h"
#include "OatsMsg.h"


CThreadOrderNew::CThreadOrderNew(CRomOrderNew* pOrder)
{
  pthread_rwlock_init(&m_lock, NULL);
  m_pOrder = pOrder;
}

CThreadOrderNew::~CThreadOrderNew(void)
{
  pthread_rwlock_destroy(&m_lock);
}

void CThreadOrderNew::SetOrder(CRomOrderNew* pOrder)
{
  //memcpy(m_pOrder, pOrder, sizeof(CRomOrder));
  m_pOrder = pOrder;
}

void CThreadOrderNew::SetUsed(bool value)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->SetUsed(value);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::SetCancelPending(bool value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_pOrder->SetCancelPending(value);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::SetReplacePending(bool value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_pOrder->SetReplacePending(value);
    pthread_rwlock_unlock(&m_lock);
}

CRomOrderNew* CThreadOrderNew::Reset()
{
  pthread_rwlock_wrlock(&m_lock);
  //memset(m_pOrder, 0, sizeof(CRomOrder));
  CRomOrderNew* rc = m_pOrder;
  m_pOrder = NULL;
  pthread_rwlock_unlock(&m_lock);
  return rc;
}

void CThreadOrderNew::SetActClientTag(const char* pTag)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->SetActClientTag(pTag);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::SetStatus(short status)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->SetStatus(status);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::sendOatsMsg(CCsv* pCsv)
{
    m_pOrder->QueueOatsMsg(pCsv);
}

void CThreadOrderNew::SendOatsMsg(CCsv* pCsv)
{
    pthread_rwlock_wrlock(&m_lock);
    sendOatsMsg(pCsv);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::GetNextRomExecTag(std::string& romExecTag)
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->NextRomExecTag(romExecTag);
  pthread_rwlock_unlock(&m_lock);
}

bool CThreadOrderNew::Dead()
{
     pthread_rwlock_rdlock(&m_lock);
     bool rc = CRomOrderNew::IsDead(m_pOrder);
     pthread_rwlock_unlock(&m_lock);
     return rc;
}
bool CThreadOrderNew::UpdateParent()
{
     pthread_rwlock_rdlock(&m_lock);
     bool rc = m_pOrder->UpdateParent();
     pthread_rwlock_unlock(&m_lock);
     return rc;
}
/*
void CThreadOrderNew::DoOats(CCsv& msg)
{
    //when ordermaster sends cancelall to "all in one"
    //ROM can receive "out" before it receives cancelall from oats.
    //In that case romcancelpending needs to be made up for
    //Oats purpose.
    int newstatus = ROMVALUE::STATUS_NEW;
    msg.GetAt(ROMFIELDS::CSV_STATUS, newstatus);
    COatsMsg* pOatsMsg = NULL;

    if (ROMVALUE::STATUS_CANCELED == newstatus &&
        m_pOrder->CancelPending() == false)
    {
        std::string romcancel;
        if (msg.GetAt(ROMFIELDS::CSV_ROMCANCEL, romcancel) == true &&
            romcancel.compare("1") == 0)
        {
            //make up romcancelpending

            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
            sendOatsMsg(msg);
            msg.SetAt(ROMFIELDS::CSV_STATUS, newstatus);
        }
    }

    if (ROMVALUE::STATUS_FILLED == newstatus)
    {
        if (m_pOrder->CancelPending() == true)
        {
            std::string text = "too later to cancel)";
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
            msg.SetAt(ROMFIELDS::CSV_TEXT, text);
            sendOatsMsg(msg);
            msg.SetAt(ROMFIELDS::CSV_STATUS, newstatus);
        }

        if (m_pOrder->ReplacePending() == true)
        {
            std::string text = "too later to cancel)";
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
            msg.SetAt(ROMFIELDS::CSV_TEXT, text);
            sendOatsMsg(msg);
            msg.SetAt(ROMFIELDS::CSV_STATUS, newstatus);
        }
    }
    else if (ROMVALUE::STATUS_CANCELED == newstatus )
    {
        if (m_pOrder->ReplacePending() == true)
        {
            std::string text = "too later to cancel)";
            msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
            msg.SetAt(ROMFIELDS::CSV_TEXT, text);
            sendOatsMsg(msg);
            msg.SetAt(ROMFIELDS::CSV_STATUS, newstatus);
        }
    }

    int cplxtype = ROMVALUE::CPLXTYPE_SINGLE;
    msg.GetAt(ROMFIELDS::CSV_CPLXORDER_TYPE, cplxtype);

    if (cplxtype == ROMVALUE::CPLXTYPE_SPREAD)
    {
        if (ROMVALUE::STATUS_FILLED != newstatus &&
            ROMVALUE::STATUS_PARTIALLYFILLED != newstatus &&
            ROMVALUE::STATUS_PENDINGCANCEL != newstatus)
        {
            sendOatsMsg(msg);
        }
    }
    else
    {
        sendOatsMsg(msg);
    }
}
*/

/*
void CThreadOrderNew::GetOrderWithRomExTag(CRomOrderNew* pOrder, std::string& romExTag)
{
    pthread_rwlock_wrlock(&m_lock);
    memcpy(pOrder, m_pOrder, sizeof(CRomOrder));
    m_pOrder->NextRomExecTag(romExTag);
    pthread_rwlock_unlock(&m_lock);
}*/

bool CThreadOrderNew::TurnOnCancelPending(const std::string& newTag,
                                       bool ownerCancel,
                                       CCsv* pCancel)
{
    bool rc = true;

    pthread_rwlock_wrlock(&m_lock);

    std::string errStr;
    m_pOrder->GetCancelRequiredFields(*pCancel);

    std::string sStatus;
    pCancel->GetAt(ROMFIELDS::CSV_STATUS, sStatus);
    pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);

    if (true == ownerCancel &&
        m_pOrder->CancelPending() == true)
    {
        errStr.assign("Previous cancel is still pending in ROM");
        rc = false;
    }
    else
    {
        if (m_pOrder->Leave() == 0)
        {
            errStr = "There are not leaves to cancel";
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
        }
    }

    if (false == rc)
    {
        pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
        pCancel->SetAt(ROMFIELDS::CSV_TEXT, errStr);
        sendOatsMsg(pCancel);
    }
    else
    {
        pCancel->SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMCANCELPENDING);
        sendOatsMsg(pCancel);
        pCancel->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
        pCancel->SetAt(ROMFIELDS::CSV_STATUS, sStatus);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

void CThreadOrderNew::GetOrder(CRomOrderNew* pOrder)
{
  pthread_rwlock_rdlock(&m_lock);
  memcpy(pOrder, m_pOrder, sizeof(CRomOrderNew));
  pOrder->SetCoveredSpread(NULL);
  pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::GetCsv(CCsv& csv)
{
  pthread_rwlock_rdlock(&m_lock);
  m_pOrder->GetCsv(csv);
  pthread_rwlock_unlock(&m_lock);
}

short CThreadOrderNew::GetCplxType()
{
  pthread_rwlock_rdlock(&m_lock);
  short rc = m_pOrder->CplxType();
  pthread_rwlock_unlock(&m_lock);
  return rc;
}

/*bool CThreadOrderNew::UpdateReplacingFields(CCsv& msg, tKeyedBalanceUpdate& update, std::string& err)
{
    pthread_rwlock_wrlock(&m_lock);

    bool rc = true;

    char excIns;

    if (msg.GetAt(ROMFIELDS::CSV_EXEC_INSTRUCTION, excIns) &&
        excIns == ROMVALUE::ISO)
    {
        err = "ISO order is not allowed by ROM system";
        rc = false;
    }
    else if (m_pOrder->ReplacePending() == true)
    {
        err = "Previous Replace is still pending in ROM";
        rc = false;
    }

    if (rc)
    {
        m_pOrder->UpdateReplacingFields(msg, update);

        if (m_pOrder->ShareChange() + m_pOrder->Leave() <= 0)
        {
            m_pOrder->ResetReplacingFields();
            char text[100];
            sprintf(text, "New Order Share has to be larger than current Cum(%ld)", m_pOrder->Cum());
            err = text;
            rc = false;
        }
        else
        {
            if (m_pOrder->CoveredSpread())
            {
                rc = CClientProfileManagerNew::getInstance().AllowedToReplaceCoveredSpreadOrder(update, m_pOrder->CoveredSpread(), err);
            }
            else
            {
                rc = CClientProfileManagerNew::getInstance().AllowedToReplaceOrder(update, err);
            }
        }

        if (false == rc)
        {
            m_pOrder->ResetReplacingFields();
        }
        else
        {
            m_pOrder->GetReplaceRequiredFields(msg);
        }
    }

    msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);

    if (rc)
    {
        msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ROMREPLACEPENDING);
    }
    else
    {
        msg.SetAt(ROMFIELDS::CSV_TEXT, err);
        msg.SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
    }

    sendOatsMsg(&msg);

    if (rc)
    {
        msg.SetAt(ROMFIELDS::CSV_STATUS, "");
        msg.SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_REPLACE);
    }

    pthread_rwlock_unlock(&m_lock);

    return rc;
}*/

void CThreadOrderNew::ResetReplacingFields()
{
  pthread_rwlock_wrlock(&m_lock);
  m_pOrder->ResetReplacingFields();
  pthread_rwlock_unlock(&m_lock);
}

bool CThreadOrderNew::ReplacePending()
{
  pthread_rwlock_rdlock(&m_lock);
  bool rc = m_pOrder->ReplacePending();
  pthread_rwlock_unlock(&m_lock);
  return rc;
}

/*
long CThreadOrderNew::UpdateFromChildFill(long last, tBalanceUpdate& BUpdate, CRomOrderNew* pOrder)
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
*/

void CThreadOrderNew::SetShareChange(long shareChange, std::string& romTag)
{
    pthread_rwlock_wrlock(&m_lock);
    romTag = m_pOrder->RomTag();
    m_pOrder->SetShareChange(shareChange);
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::GetClientTag(char* tag, int len)
{
    pthread_rwlock_rdlock(&m_lock);
    CUtility::StrCpy(tag, len, m_pOrder->ClientTag());
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::GetClientTag(std::string& clientTag)
{
    pthread_rwlock_rdlock(&m_lock);
    clientTag = m_pOrder->ClientTag();
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::GetOwner(char* owner, int len)
{
    pthread_rwlock_rdlock(&m_lock);
    CUtility::StrCpy(owner, len, m_pOrder->Owner());
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::GetDest(char* dest, int len)
{
    pthread_rwlock_rdlock(&m_lock);
    CUtility::StrCpy(dest, len, m_pOrder->Dest());
    pthread_rwlock_unlock(&m_lock);
}

void CThreadOrderNew::SetRatio(long value)
{
    pthread_rwlock_wrlock(&m_lock);
    m_pOrder->SetRatio(value);
    pthread_rwlock_unlock(&m_lock);
}

short CThreadOrderNew::GetStatus()
{
    pthread_rwlock_rdlock(&m_lock);
    short rc = m_pOrder->Status();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

long CThreadOrderNew::Ratio()
{
    pthread_rwlock_rdlock(&m_lock);
    long rc = m_pOrder->Ratio();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

void CThreadOrderNew::ProcessUpdate(CCsv* pCsv)
{
    pthread_rwlock_wrlock(&m_lock);

    m_pOrder->ProcessUpdate(pCsv);

    pthread_rwlock_unlock(&m_lock);
}

bool CThreadOrderNew::ProcessCancel(CCsv* pCancel, const std::string& actTag, bool ownercancel, CClientConnection* pCon)
{
    pthread_rwlock_wrlock(&m_lock);

    bool rc = m_pOrder->ProcessCancel(pCancel, actTag, ownercancel, pCon);

    pthread_rwlock_unlock(&m_lock);

    return rc;
}


bool CThreadOrderNew::ProcessReplace(CCsv* pReplace, CClientConnection* pCon)
{
    pthread_rwlock_wrlock(&m_lock);

    bool rc = m_pOrder->ProcessReplace(pReplace, pCon);

    pthread_rwlock_unlock(&m_lock);

    return rc;
}

bool CThreadOrderNew::DeadSpread()
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_pOrder->DeadSpread();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

short CThreadOrderNew::TIF()
{
    pthread_rwlock_rdlock(&m_lock);
    short rc = m_pOrder->TIF();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

void CThreadOrderNew::GetRomTag(std::string& romTag)
{
    pthread_rwlock_rdlock(&m_lock);
    romTag = m_pOrder->RomTag();
    pthread_rwlock_unlock(&m_lock);
}

bool CThreadOrderNew::Filter(const CCsv& filter)
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_pOrder->Filter(filter);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

bool CThreadOrderNew::GetPosUpdate(tPosUpdate& update)
{
    pthread_rwlock_rdlock(&m_lock);
    bool rc = m_pOrder->GetPosUpdate(update);
    pthread_rwlock_unlock(&m_lock);
    return rc;
}

short CThreadOrderNew::GetTif()
{
    short rc = 0;
    pthread_rwlock_rdlock(&m_lock);
    rc = m_pOrder->TIF();
    pthread_rwlock_unlock(&m_lock);
    return rc;
}