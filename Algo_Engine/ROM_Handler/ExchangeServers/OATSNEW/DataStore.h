// DataStore.h: interface for the DataStore class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DATASTORE_H__77C286D9_FF13_47CE_A527_0A7C2134F589__INCLUDED_)
#define AFX_DATASTORE_H__77C286D9_FF13_47CE_A527_0A7C2134F589__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable:4786)
#include <string>
#include <set>

template<typename T>
class DataStore  
{
	DataStore() : m_nRecSize(0), m_nBlockSize(0), m_bInit(false), m_hFile(NULL), m_hMapFile(NULL), m_lpBase(NULL)
	{
	}

public:
	virtual ~DataStore()
	{
		Shutdown();
	}

	static DataStore<T> & getInstance()
	{
		static DataStore<T> store;
		return store;
	}

	bool Grow( int nNewRecSize )
	{
		::UnmapViewOfFile( m_lpBase );
		::CloseHandle( m_hMapFile );
		m_lpBase = ::MapViewOfFileEx(  m_hMapFile, FILE_MAP_WRITE|FILE_MAP_READ,
			0, 0, nNewRecSize*m_nBlockSize, m_lpBase );
		LPBYTE lpObj = (LPBYTE)m_lpBase;
		m_FreeObjs.clear();
		m_UsedObjs.clear();
		for( int i = 0; i < nNewRecSize; i++ )
		{
			T * pT = (T*)lpObj;
			if( i < m_nRecSize )
			{
				if( pT->GetUsed() == false )
					m_FreeObjs.insert( lpObj );
				else
					m_UsedObjs.insert( lpObj );
			}
			else
			{
				memset( pT, 0, sizeof(T) );
				m_FreeObjs.insert( lpObj );
			}
			lpObj += m_nBlockSize;
		}
		m_nRecSize = nNewRecSize;
		m_itUsed = m_UsedObjs.begin();
		return true;
	}

	bool Initialize( LPCSTR pszFileName, int nRecSize, bool bReset = false )
	{
		if( m_bInit )
		{
			Shutdown();
		}
		m_hFile = ::CreateFile( pszFileName, 
			GENERIC_READ|GENERIC_WRITE, 
			FILE_SHARE_READ|FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH|FILE_FLAG_RANDOM_ACCESS,
			NULL );
		if( INVALID_HANDLE_VALUE == m_hFile )
			return false;
		m_nBlockSize = sizeof(T);
		if( m_nRecSize == 0 ) // if not resize 
			m_nRecSize = nRecSize;
		m_hMapFile = ::CreateFileMapping( m_hFile, NULL, PAGE_READWRITE, 0, nRecSize*m_nBlockSize, pszFileName );
		if( m_hMapFile == NULL )
			return false;
		m_lpBase = ::MapViewOfFile( m_hMapFile, FILE_MAP_WRITE|FILE_MAP_READ, 0, 0, nRecSize*m_nBlockSize );
		if( !m_lpBase )
			return false;
		if( bReset )
			memset( m_lpBase, 0, nRecSize*m_nBlockSize );
		LPBYTE lpObj = (LPBYTE)m_lpBase;
		for( int i = 0; i < nRecSize; i++ )
		{
			T * pT = (T*)lpObj;
			if( i < m_nRecSize )
			{
				if( pT->GetUsed() == false )
					m_FreeObjs.insert( lpObj );
				else
					m_UsedObjs.insert( lpObj );
			}
			else
			{
				memset( pT, 0, sizeof(T) );
				m_FreeObjs.insert( lpObj );
			}
			lpObj += m_nBlockSize;
		}
		m_nRecSize = nRecSize;
		m_itUsed = m_UsedObjs.begin();
		m_bInit = true;
		return true;
	}

	void Shutdown()
	{
		if( m_hMapFile )
		{
			::UnmapViewOfFile( m_lpBase );
			::CloseHandle( m_hMapFile );
		}
		if( m_hFile )
			::CloseHandle( m_hFile );
		m_hMapFile = NULL;
		m_hFile = NULL;
		m_bInit = false;
		m_UsedObjs.clear();
		m_FreeObjs.clear();
		m_itUsed = m_UsedObjs.end();
		m_lpBase = NULL;
	}

	LPVOID	NewObject()
	{
		tObjects::iterator it = m_FreeObjs.begin();
		if( it == m_FreeObjs.end() )
		{
			return NULL;
		}
		LPVOID lpObj = (*it);
		T * pT = (T *)lpObj;
		pT->SetUsed( true );
		m_UsedObjs.insert( lpObj );
		m_FreeObjs.erase( lpObj );
		return lpObj;
	}

	void FreeObject( LPVOID lpObj )
	{
		tObjects::iterator it = m_UsedObjs.find( lpObj );
		if( it == m_UsedObjs.end() )
			return;
		T * pT = (T*)lpObj;
		pT->SetUsed( false );
		m_UsedObjs.erase( lpObj );
		m_FreeObjs.insert( lpObj );
	}

	T * FirstUsed()
	{
		m_itUsed = m_UsedObjs.begin();
		if( m_itUsed == m_UsedObjs.end() )
			return NULL;
		return (T*)*m_itUsed;
	}

	T * NextUsed()
	{
		if( m_itUsed == m_UsedObjs.end() )
			return NULL;
		m_itUsed++;
		if( m_itUsed == m_UsedObjs.end() )
			return NULL;
		return (T*)*m_itUsed;
	}

private:
	typedef std::set<LPVOID>	tObjects;
	tObjects					m_UsedObjs;
	tObjects					m_FreeObjs;
	tObjects::iterator			m_itUsed;
	HANDLE						m_hFile;
	HANDLE						m_hMapFile;
	LPVOID						m_lpBase;
								
	bool						m_bInit;
	int							m_nRecSize;
	int							m_nBlockSize;

};

template<typename T>
class DataStoreObj
{
public:
	friend class DataStore<T>;

	DataStoreObj() {}
	virtual ~DataStoreObj(){}

	void * operator new( size_t s )
	{
		return DataStore<T>::getInstance().NewObject();
	}

	void operator delete( void * p)
	{
		DataStore<T>::getInstance().FreeObject( (T*)p );
	}

private:
	void SetUsed( bool bUsed ) 
	{ 
		m_bUsed = bUsed; 
	}

	bool GetUsed() 
	{ 
		return m_bUsed; 
	}

	bool	m_bUsed;
};


// Sample Object to be used with DataStore
/*

class Employee : public DataStoreObj<Employee>
{
public:
	Employee()
	{
	}

	virtual ~Employee()
	{
	}
public:
	char		m_szName[10];
	long		m_nID;
	float		m_fSalary;
};

class EmployeeManager
{
public:
	EmployeeManager() : m_nSize(4)
	{
		DataStore<Employee>::getInstance().Initialize( "Employee.map", m_nSize );
		Load();
	}
	virtual ~EmployeeManager()
	{
		DataStore<Employee>::getInstance().Shutdown();
	}

	bool Load()
	{
		Employee * pEmp = DataStore<Employee>::getInstance().FirstUsed();
		while( pEmp )
		{
			Add( pEmp );
			pEmp = DataStore<Employee>::getInstance().NextUsed();
		}

		return true;
	}

	void Reload( int nNewSize )
	{
		DataStore<Employee>::getInstance().Initialize( "Employee.map", nNewSize );
		m_Employees.clear();
		Load();
	}

	void Test()
	{
		Employee * pEmp = new Employee();
		if( !pEmp )
		{
			m_nSize *= 2;
			Reload( m_nSize );
			pEmp = new Employee();
			if( !pEmp )
				return;
		}
		pEmp->m_nID = 100;
		pEmp->m_fSalary = 1001.0f;
		Add( pEmp );
		Del( pEmp->m_nID );
	}

	void Add( Employee * pEmp )
	{
		tEmpMap::iterator it = m_Employees.find( pEmp->m_nID );
		if( it != m_Employees.end() )
			return;
		m_Employees.insert( std::make_pair<long,Employee*>(pEmp->m_nID, pEmp) );
	}

	void Del( long nID )
	{
		tEmpMap::iterator it = m_Employees.find( nID );
		if( it != m_Employees.end() )
		{
			delete (*it).second;
			it = m_Employees.erase( it );
		}
	}

private:
	typedef std::map<long,Employee*>	tEmpMap;
	tEmpMap	m_Employees;
	long	m_nSize;
};
*/

#endif // !defined(AFX_DATASTORE_H__77C286D9_FF13_47CE_A527_0A7C2134F589__INCLUDED_)
