using System;

namespace LBMEx
{
	public delegate void MarketDataChangedEventHandler(object sender, LBMDataChangedEventArgs e);

	public class LBMData
	{
		public event MarketDataChangedEventHandler MarketDataChanged;
		protected virtual void OnMarketDataChanged(LBMDataChangedEventArgs e)
		{
			if (MarketDataChanged != null)
			{
				MarketDataChanged(this, e);
			}
		}

		private LBM _lbm;

		#region - Constructor -

		public LBMData()
		{
			Initialize("lbm", 3, 30000, false, "", "ronin", "WOMBAT", 500, 500, 0, true, 100, true); 
		}
		public LBMData(string middleware, int maxRetries, uint idelTimeBetweenRetries, bool enableMamaLog, string mamaLogFilePath, string transportName, string dictionarySourceSymbolNamespace, int throttleDefault, int throttleInitial, int throttleRecap, bool useThreadedDecode, int changeNotificationRate, bool useChangeNotificationRate)
		{
			Initialize(middleware, maxRetries, idelTimeBetweenRetries, enableMamaLog, mamaLogFilePath, transportName, dictionarySourceSymbolNamespace, throttleDefault, throttleInitial, throttleRecap, useThreadedDecode, changeNotificationRate, useChangeNotificationRate);
		}

		#endregion

		private void Initialize(string middleware, int maxRetries, uint idelTimeBetweenRetries, bool enableMamaLog, string mamaLogFilePath, string transportName, string dictionarySourceSymbolNamespace, int throttleDefault, int throttleInitial, int throttleRecap, bool useThreadedDecode, int changeNotificationRate, bool useChangeNotificationRate)
		{
			if (_lbm == null)
			{
				_lbm = new LBM(middleware, maxRetries, idelTimeBetweenRetries, enableMamaLog, mamaLogFilePath, transportName, dictionarySourceSymbolNamespace, throttleDefault, throttleInitial, throttleRecap, useThreadedDecode, changeNotificationRate, useChangeNotificationRate);
				_lbm.LBMChanged += new LBMChangedEventHandler(LBMChanged);
			}
		}

		private void LBMChanged(object sender, LBMDataChangedEventArgs e)
		{
			// Event ByPass
			OnMarketDataChanged(e);
		}

		#region - Start -

		public void Start()
		{
			_lbm.Start(null);
		}

		#endregion

		#region - Subscribe -

		public void Subscribe(string symbol, string source)
		{
			_lbm.Subscribe(symbol, source);
		}

		#endregion

		#region - Stop -

		public void Stop()
		{
			if (_lbm != null)
			{
				_lbm.Dispose();
			}
		}

		#endregion

		#region - Force GC -

		public void ForceGC()
		{
			if (_lbm != null)
			{
				_lbm.ForceGC();
			}
		}

		#endregion
	}
}
