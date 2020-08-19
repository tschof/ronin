#ifndef __OATSMSG_H__
#define __OATSMSG_H__

//#include "Csv.h"
//#include "CsvStore.h"
#include "Store.h"
#include "RomOrderNew.h"
//#include "GStore.h"
#include "Config.h"

class COatsMsg
{
public:
	COatsMsg(std::string& msg)
	{
		m_msg = msg;
		m_pCsv = NULL;
		m_pOrder = NULL;
	}

    COatsMsg(const char* buf, int len)
    {
        m_msg.append(buf, len);;
		m_pCsv = NULL;
		m_pOrder = NULL;
    }

	COatsMsg(CCsv* pCsv, CRomOrderNew* pOrder, bool newCsv, bool newOrder = true)
	{
            if (newCsv)
            {
                    m_pCsv = CStore<CCsv>::getInstance().Get();
                    m_pCsv->Set(*pCsv);
            }
            else
            {
                    m_pCsv = pCsv;
            }

            m_pOrder = NULL;

            if (CConfig::getInstance().mkOats())
            {
                if (pOrder)
                {
                    if (newOrder)
                    {
                        m_pOrder = CStore<CRomOrderNew>::getInstance().Get();
                        memcpy(m_pOrder, pOrder, sizeof(CRomOrderNew));
                        m_pOrder->SetCoveredSpread(NULL);
                    }
                    else
                    {
                        m_pOrder = pOrder;
                    }
                }
            }
            else //let Oats construct the full message
            {
                if (pOrder)
                {
                    //Oats key order with romTag
                    m_pCsv->SetAt(ROMFIELDS::CSV_STAFFORD_TAG, pOrder->RomTag());
                }
            }

            if (pOrder)
            {
                m_pCsv->SetAt(ROMFIELDS::CSV_CLIENT_ECHO, pOrder->ClientEcho());
            }
	}

	~COatsMsg()
	{
		if (m_pOrder)
		{
			CStore<CRomOrderNew>::getInstance().Return(m_pOrder);
		}

		if (m_pCsv)
		{
			CStore<CCsv>::getInstance().Return(m_pCsv);
		}
	}

	std::string& msg() { return m_msg; }
	CCsv* csv()	{ return m_pCsv; }
	CRomOrderNew* Order() const { return m_pOrder; }
	char cmd()
	{
		if (m_pCsv)
		{
            char cmd;
            m_pCsv->GetAt(ROMFIELDS::CSV_COMMAND, cmd);
			return cmd;
		}
		else
		{
			return m_msg[0];
		}
	}

private:
    std::string m_msg;
	CCsv* m_pCsv;
	CRomOrderNew* m_pOrder;
};

#endif //__OATSMSG_H__
