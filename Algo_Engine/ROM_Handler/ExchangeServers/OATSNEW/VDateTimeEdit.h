#pragma once
const UINT WM_DATETIME_EDIT = ::RegisterWindowMessage("WM_DATETIME_EDIT");
#define DTE_DATETIMECHANGED 0x0001

class VDateTimeEdit : public CEdit
{
// Construction
public:
    VDateTimeEdit();

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(VDateTimeEdit)
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
public:
    CString GetMask() const {return m_strMask;}
    void SetMask(CString mask);
    COleDateTime GetDateTime();
    COleDateTime GetDateTime(CString str);
    void SetDateTime(COleDateTime dt);

    virtual ~VDateTimeEdit();

    // Generated message map functions
protected:
    afx_msg void OnContextMenu(CWnd*, CPoint point);

    CString m_strMask;
    //{{AFX_MSG(VDateTimeEdit)
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};


inline bool VAtoi(CString str, int &res)
{
    str.Remove(char(32));
    if (str.IsEmpty()) return false;
    bool error = false;
    for (int i = 0; i < str.GetLength() && !error; i++) {
        int c = ::tolower(str[i]);
        error = !(
            (c >= '0' && c <= '9')
            || ((c == '-' || c == '+') && i == 0)
            );
    }
    if (!error) res = atoi(LPCTSTR(str));
    return !error;
}

template<class TYPE>
void VSetInRange(TYPE &x, TYPE min, TYPE max) 
{
    ASSERT(min<=max);
    if (x < min) x = min; else if (x > max) x = max;
}