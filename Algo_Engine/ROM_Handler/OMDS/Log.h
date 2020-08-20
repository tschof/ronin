// Log.h: interface for the CLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOG_H__2A192798_E249_46A3_B856_29D650A4815B__INCLUDED_)
#define AFX_LOG_H__2A192798_E249_46A3_B856_29D650A4815B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OMDSCommon.h"

#define LOG_ERROR			0x0001
#define LOG_WARNING			0x0002
#define	LOG_MESSAGE			0x0010
#define	LOG_DEBUG			0x0020
#define	LOG_BUNCH_O_CRAP	0x0050

class CLog
{
public:
	CLog();
	virtual ~CLog();

	std::string	getFile();
	std::string	getObjectName();
	std::string	getDirectory();
	int			getLevel();
	int			getMaxLogFiles();
	void		deleteLogs();
	void		write( const std::string & object, int line, int logLevel, const std::string & message);
	void		write(int level, const std::string & message);
	void		write( const std::string & message);
	void		open();
	void		setMaxLogs(int i);
	void		setLogSize(int size);
	void		setLevel(int logLevel);
	void		setFile( const std::string & fileName);
	void		setObjectName( const std::string & name);
	void		setDirectory( const std::string & directory);
	void		setEventLogLevel(int level);
private:
	void			writeEventLog(int level, const std::string & message);
	std::string		m_objectName;
};


#endif // !defined(AFX_LOG_H__2A192798_E249_46A3_B856_29D650A4815B__INCLUDED_)
