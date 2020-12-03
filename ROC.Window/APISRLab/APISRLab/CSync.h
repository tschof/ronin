class CSync
{
public:
  CSync() { InitializeCriticalSection(&m_CriticalSection); }
  ~CSync() { DeleteCriticalSection(&m_CriticalSection); }
  void Acquire() { EnterCriticalSection(&m_CriticalSection); }
  void Release() { LeaveCriticalSection(&m_CriticalSection); }


private:
  CRITICAL_SECTION m_CriticalSection;              // Synchronization object
};

class CLockGuard
{
public:
  CLockGuard(CSync &refSync) : m_refSync(refSync) { Lock(); }
  ~CLockGuard() { Unlock(); }


private:
  CSync &m_refSync;                     // Synchronization object

  CLockGuard(const CLockGuard &refcSource);
  CLockGuard &operator=(const CLockGuard &refcSource);
  void Lock() { m_refSync.Acquire(); }
  void Unlock() { m_refSync.Release(); }
};