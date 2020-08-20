#if !defined(CREJECT_H)
#define CREJECT_H


class CReject	: public CMessageBase
{
protected:
	CString		m_Symbol;
	CString		m_Reason;
	CString		m_Debug;
	
public:
	const CString& Reason()				const { return m_Reason;	}
	const CString& Symbol()				const { return m_Symbol;	}
	const CString& Debug()				const { return m_Debug;		}
	
	void	SetReason(const CString& Reason)		{ m_Reason			= Reason; }
	void	SetSymbol(const CString& Symbol)		{ m_Symbol			= Symbol; }

public:
	CReject();
	CReject(const CString& Reason, const COrder& Order);
	CReject(const CString& Reason, const CCancel& Cancel);
	virtual ~CReject();

};


#endif