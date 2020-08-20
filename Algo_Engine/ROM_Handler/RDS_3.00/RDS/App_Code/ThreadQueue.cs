using System;
using System.Threading;
using System.Collections;

public class ThreadQueue
{
	private Queue _workQ = new Queue();
	private Thread _t;
	private bool _done = false;

	// This constructor initializes the queue to use a
	// normal priority foreground thread.
	//
	public ThreadQueue()
		: this(ThreadPriority.Normal, false, "DPCQueue")
	{
	}

	// This constructor allows the user to dictate the priority,
	// background nature, and name.  The thread won't be started
	// until the Start method is called.
	//
	public ThreadQueue(ThreadPriority threadPriority, bool useBackgroundThread, string threadName)
	{
		_t = new Thread(new ThreadStart(ThreadProc));
		_t.IsBackground = useBackgroundThread;
		_t.Priority = threadPriority;
		_t.Name = threadName;
	}

	public ApartmentState ApartmentState
	{
		get { return (_t.GetApartmentState()); }

		set
		{
			// This property must be set prior to starting things up.
			//
			if (_t.ThreadState != ThreadState.Unstarted)
			{
				throw new ApplicationException("ApartmentState must be set before calling Start");
			}

			_t.SetApartmentState(value);
		}
	}

	public void Start()
	{
		_t.Start();
	}

	// Signal the thread to stop, and wait for that
	// to happen.
	//
	public void Stop()
	{
		Stop(Timeout.Infinite);
	}

	// Signal the thread to stop, and wait a specified
	// amount of time for that to happen.
	//
	public bool Stop(int timeoutMilliseconds)
	{
		lock (this)
		{
			_done = true;
			Monitor.Pulse(this);
		}

		return _t.Join(timeoutMilliseconds);
	}

	// WorkProc defines the signature of the methods that
	// can be called by the DPC queue.
	//
	public delegate void WorkProc(object procState);

	// Enqueue a request to the DPC queue.  The specified
	// procState object will be passed as-is to the target
	// method.
	//
	public void Enqueue(WorkProc proc, object procState)
	{
		lock (this)
		{
			_workQ.Enqueue(new WorkItem(proc, procState));
			Monitor.Pulse(this);
		}
	}

	// Alternative way to enqueue a request to the DPC queue
	// that's modeled on Control.Invoke/BeginInvoke.
	//
	public void Enqueue(Delegate target, object[] args)
	{
		lock (this)
		{
			_workQ.Enqueue(new WorkItem(target, args));
			Monitor.Pulse(this);
		}
	}

	// WorkItem represents an item on the queue.
	//
	class WorkItem
	{
		public WorkItem(WorkProc proc, object procState)
		{
			this._proc = proc;
			this._procState = procState;
			this._procArgs = null;
		}

		public WorkItem(Delegate proc, object[] args)
		{
			this._proc = proc;
			this._procState = null;
			this._procArgs = args;
		}

		public readonly Delegate _proc;
		public readonly object _procState;
		public readonly object[] _procArgs;
	}

	private void ThreadProc()
	{
		// Until we're told to stop...
		//
		while (!_done)
		{
			WorkItem request = null;

			lock (this)
			{
				// Wait for a work request to be posted to the
				// queue...
				//
				while (!_done && (_workQ.Count == 0))
				{
					Monitor.Wait(this);
				}

				// If we exited the above loop because the queue
				// now has an entry, grab that entry off the queue.
				//
				if (!_done)
				{
					request = (WorkItem)_workQ.Dequeue();
				}
			}

			// No longer holding the lock.

			// If we exited the above loop because we have
			// work to do (and not because we were signaled to
			// stop), make the callback.
			//
			if (!_done && (request != null))
			{
				try
				{
					WorkProc targetProc = request._proc as WorkProc;

					if (targetProc != null)
					{
						// Early bound invocation.
						targetProc(request._procState);
					}
					else
					{
						// Late bound invocation.
						request._proc.DynamicInvoke(request._procArgs);
					}
				}
				catch
				{
				}
			}
		}
	}
}
