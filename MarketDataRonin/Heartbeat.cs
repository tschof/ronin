using System.Threading;

namespace MarketData.Ronin
{
	internal class Heartbeat
	{
		private Timer m_timer = null;
		private MDClientToServer _ping;
		private readonly System.Action<MDClientToServer> _sender;
		private readonly System.Action _onTimeout;
		private readonly int _interval;
		private int _outstanding = 0;

		internal Heartbeat(int interval, System.Action<MDClientToServer> sender, System.Action onTimeout)
		{
			_interval = interval * 1000;
			_sender = sender;
			_onTimeout = onTimeout;
		}

		internal void Start(string username, string password)
		{
			_ping = new MDClientToServer(username, password);
			if (m_timer == null) {
				_outstanding = 0;
				m_timer = new Timer(onHeartbeatTimer, null, _interval, _interval);
			}
		}

		internal void Stop()
		{
			if (m_timer != null) {
				m_timer.Change(Timeout.Infinite, Timeout.Infinite);
				m_timer = null;
			}
		}

		internal void Pong()
		{
			Interlocked.Exchange(ref _outstanding, 0);
		}

		private void onHeartbeatTimer(object state)
		{
			try {
				if (_outstanding > 3) {
					Stop();
					_onTimeout?.Invoke();
				} else {
					_sender?.Invoke(_ping);
					Interlocked.Increment(ref _outstanding);
				}
			} catch {
			}
		}
	}
}
