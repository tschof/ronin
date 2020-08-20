// BrassException.h: interface for the CBrassException class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BRASSEXCEPTION_H__2E52E037_AA59_491D_849C_5C114206101F__INCLUDED_)
#define AFX_BRASSEXCEPTION_H__2E52E037_AA59_491D_849C_5C114206101F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <exception>

class CBrassSessionCancelException : public std::exception  
{
public:
	CBrassSessionCancelException()
	{
	}

	virtual ~CBrassSessionCancelException()
	{
	}
};

class CBrassSessionOrderException : public std::exception  
{
public:
	CBrassSessionOrderException()
	{
	}

	virtual ~CBrassSessionOrderException()
	{
	}
};

class CBrassSessionReplaceException : public std::exception  
{
public:
	CBrassSessionReplaceException()
	{
	}

	virtual ~CBrassSessionReplaceException()
	{
	}
};


#endif // !defined(AFX_BRASSEXCEPTION_H__2E52E037_AA59_491D_849C_5C114206101F__INCLUDED_)
