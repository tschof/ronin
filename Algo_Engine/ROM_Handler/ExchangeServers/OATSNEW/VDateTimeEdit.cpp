#include "stdafx.h"
#include "VDateTimeEdit.h"

VDateTimeEdit::VDateTimeEdit()
{
//    m_strMask = _T("DD/MM/YYYY hh:mm:ss");
    m_strMask = _T("hh:mm:ss");
}

VDateTimeEdit::~VDateTimeEdit()
{

}


BEGIN_MESSAGE_MAP(VDateTimeEdit, CEdit)
    ON_WM_CONTEXTMENU()
    //{{AFX_MSG_MAP(VDateTimeEdit)
    ON_WM_CHAR()
    ON_WM_KEYUP()
    ON_WM_KEYDOWN()
    ON_WM_MOUSEMOVE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////
// VDateTimeEdit message handlers

void VDateTimeEdit::OnChar(UINT nChar, UINT /*nRepCnt*/, UINT /*nFlags*/) 
{
    CString old;
    GetWindowText(old);
    int pos;
    GetSel(pos, pos);
	if( pos == 0 )
		return;
	pos--;
    TCHAR c = m_strMask[pos];
    switch (nChar) {
        case VK_BACK: pos--;
            break;
        case VK_UP: pos--;
            break;
        case '0':case '1':case '2':case '3':
        case '4':case '5':case '6':case '7':
        case '8':case '9':
            if (pos < old.GetLength()
                  && pos < m_strMask.GetLength()
                  && (UINT)old[pos] != nChar
                  && (c == 'D' || 
                  c == 'M' || c == 'Y' || c == 'h' 
                  || c == 'm' || c == 's')) {
                CString str = old;
                str.SetAt(pos, (TCHAR)nChar);
                COleDateTime dt = GetDateTime(str);
                if (dt.GetStatus() == COleDateTime::valid) SetDateTime(dt);
            }
            if (++pos < m_strMask.GetLength())
                for ( c = m_strMask[pos]; pos < m_strMask.GetLength() 
                    && c != 'D' && 
                    c != 'M' && c != 'Y' && 
                    c != 'h' && c != 'm' && c != 's';
                  c = m_strMask[pos]) pos++;
            break;
        default:
            if (++pos < m_strMask.GetLength())
                for ( c = m_strMask[pos]; pos < m_strMask.GetLength() 
                      && c != 'D' && c != 'M' 
                      && c != 'Y' && c != 'h' 
                      && c != 'm' && c != 's';
                    c = m_strMask[pos]) pos++;
            break;
    }
    SetSel(pos, pos);
}

void VDateTimeEdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    CEdit::OnKeyUp(nChar, nRepCnt, nFlags);
}

void VDateTimeEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    switch (nChar) {
        case VK_DELETE:case VK_INSERT:
            break;
        case VK_UP: case VK_DOWN:
            {
                CString old;
                GetWindowText(old);
                int pos;
                GetSel(pos, pos);
                if (pos < m_strMask.GetLength()) {
                    TCHAR c = m_strMask[pos];
                    if (c != 'D' && c != 'M' && 
                      c != 'Y' && c != 'h' && 
                      c != 'm' && c != 's')
                        if (--pos >= 0) c = m_strMask[pos];
                    tm t;
                    COleDateTime dt = GetDateTime();
                    t.tm_mday = dt.GetDay();
                    t.tm_mon = dt.GetMonth();
                    t.tm_year = dt.GetYear();
                    t.tm_hour = dt.GetHour();
                    t.tm_min = dt.GetMinute();
                    t.tm_sec = dt.GetSecond();
                    switch (c) {
                        case 'D':
                            t.tm_mday += (nChar!=VK_UP) ? -1 : 1;
                            break;
                        case 'M':
                            t.tm_mon += (nChar!=VK_UP) ? -1 : 1;
                            break;
                        case 'Y':
                            t.tm_year += (nChar!=VK_UP) ? -1 : 1;
                            break;
                        case 'h':
                            t.tm_hour += (nChar!=VK_UP) ? -1 : 1;
                            break;
                        case 'm':
                            t.tm_min += (nChar!=VK_UP) ? -1 : 1;
                            break;
                        case 's':
                            t.tm_sec += (nChar!=VK_UP) ? -1 : 1;
                            break;
                        default    :
                             CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
                            break;
                    }
                    dt.SetDateTime(t.tm_year, t.tm_mon, t.tm_mday, 
                                      t.tm_hour, t.tm_min, t.tm_sec);
                    if (dt.GetStatus() == COleDateTime::valid) 
                        SetDateTime(dt);
                } else CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
            }
            break;
        default:
            CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
            break;
    }
}

void VDateTimeEdit::OnMouseMove(UINT nFlags, CPoint point) 
{
    // TODO: Add your message handler code here and/or call default
    
    CEdit::OnMouseMove(nFlags, point);
}

void VDateTimeEdit::PreSubclassWindow() 
{
    SetDateTime(COleDateTime::GetCurrentTime());
    CEdit::PreSubclassWindow();
}

void VDateTimeEdit::SetDateTime(COleDateTime dt)
{
    CString str;
    int i, n = m_strMask.GetLength();
    for (i = 0; i < n; i++) {
        CString s;
        int val;
        TCHAR c = m_strMask[i];
        switch (c) {
            case 'D':case 'M':case 'Y':case 'h':case 'm':case 's':
                for (; i < n; i++) {
                    if (m_strMask[i] == c) s += c;
                    else {i--; break;}
                }
                break;
            default :
                break;
        }
        CString format;
        format.Format("%%0%dd", s.GetLength());
        switch (c) {
            case 'D':
                val = dt.GetDay();
                ::VSetInRange(val, 0, 31);
                s.Format(format, val);
                break;
            case 'M':
                val = dt.GetMonth();
                ::VSetInRange(val, 1, 12);
                s.Format(format, val);
                break;
            case 'Y':
                val = dt.GetYear();
                ::VSetInRange(val, 1900, 9990);
                s.Format(format, val);
                break;
            case 'h':
                val = dt.GetHour();
                ::VSetInRange(val, 0, 23);
                s.Format(format, val);
                break;
            case 'm':
                val = dt.GetMinute();
                ::VSetInRange(val, 0, 59);
                s.Format(format, val);
                break;
            case 's':
                val = dt.GetSecond();
                ::VSetInRange(val, 0, 59);
                s.Format(format, val);
                break;
            default :
                s = c;
                break;
        }
        str += s;
    }
    int pos;
    GetSel(pos, pos);
    SetWindowText(str);
    SetSel(pos, pos);
    CWnd *pOwner = GetParent();
    if (pOwner != NULL) 
      pOwner->PostMessage(WM_DATETIME_EDIT, 
                  (WPARAM)DTE_DATETIMECHANGED, (LPARAM)0);
}

COleDateTime VDateTimeEdit::GetDateTime()
{
    CString str;
    GetWindowText(str);
    return GetDateTime(str);
}

COleDateTime VDateTimeEdit::GetDateTime(CString str)
{
    tm t;
    COleDateTime dt;
    dt.SetStatus(COleDateTime::invalid);
    CTime::GetCurrentTime().GetLocalTm(&t);
    t.tm_mon += 1;
    t.tm_year += 1900;

    int i, n = m_strMask.GetLength();
    for (i = 0; i < n; i++) {
        CString s;
        TCHAR c = m_strMask[i];
        switch (c) {
            case 'D':case 'M':case 'Y':case 'h':case 'm':case 's':
                for ( ; i < n; i++) {
                    if (str.GetLength() <= s.GetLength()) break;
                    if (m_strMask[i] == c) s += str[s.GetLength()];
                    else {i--; break;}
                }
                break;
            default :
                if (str.GetLength() < 0 || str[0] != (s = c)) return dt;
                break;
        }
        str = str.Right(str.GetLength() - s.GetLength());
        int val;
        switch (c) {
            case 'D':
                if (!::VAtoi(s, val)) return dt;
                ::VSetInRange(val, 1, 31);
                t.tm_mday = val;
                break;
            case 'M':
                if (!::VAtoi(s, val)) return dt;
                ::VSetInRange(val, 1, 12);
                t.tm_mon = val;
                break;
            case 'Y':
                if (!::VAtoi(s, val)) return dt;
                ::VSetInRange(val, 1900, 9990);
                t.tm_year = val;
                break;
            case 'h':
                if (!::VAtoi(s, val)) return dt;
                ::VSetInRange(val, 0, 23);
                t.tm_hour = val;
                break;
            case 'm':
                if (!::VAtoi(s, val)) return dt;
                ::VSetInRange(val, 0, 59);
                t.tm_min = val;
                break;
            case 's':
                if (!::VAtoi(s, val)) return dt;
                ::VSetInRange(val, 0, 59);
                t.tm_sec = val;
                break;
            default :
                break;
        }
    }
    dt.SetStatus(COleDateTime::valid);
    dt.SetDateTime(t.tm_year, t.tm_mon, 
            t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec);
    return dt;
}

void VDateTimeEdit::OnContextMenu(CWnd*, CPoint)
{
}

void VDateTimeEdit::SetMask(CString mask) {
    COleDateTime time = GetDateTime();
    m_strMask = mask;
    SetDateTime(time);
}