using System;
using System.Collections.Generic;

namespace LBMEx
{
	public delegate void MarketDataChangedEventHandler(object sender, LBMDataChangedEventArgs e);

	public class LBMWrapper
	{
		public event MarketDataChangedEventHandler MarketDataChanged;
		protected virtual void OnMarketDataChanged(LBMDataChangedEventArgs e)
		{
			if (MarketDataChanged != null)
			{
				MarketDataChanged(this, e);
			}
		}

		private LBMBase _lbmBase;

		public Dictionary<string, SubscriptionFields> Subscriptions
		{
			get
			{
				return _lbmBase.Subscriptions;
			}
		}

		public List<string> MamaSubscriptionsCreated
		{
			get
			{
				return _lbmBase.SubscriptionsCreated;
			}
		}

		#region - Constructor -

		public LBMWrapper(
			string middleware,
			int maxRetries,
			uint idelTimeBetweenRetries,
			bool enableMamaLog,
			string mamaLogFilePath,
			string transportName,
			string dictionarySourceSymbolNamespace,
			int throttleDefault,
			int throttleInitial,
			int throttleRecap,
			int changeNotificationRate,
			bool useChangeNotificationRate,
			bool useWaitForSymbol,
			int mamaMsgQueueLimitPerSymbol,
			string sourceRange,
			string symbolRange,
			int bookDepthLimit,
			string stockSources,
			string futureSources,
			string optionSources,
			bool setRecoverGaps,
			int rawQueueLimit,
			bool logDroppedMessages)
		{
			Initialize(
				middleware,
				maxRetries,
				idelTimeBetweenRetries,
				enableMamaLog,
				mamaLogFilePath,
				transportName,
				dictionarySourceSymbolNamespace,
				throttleDefault,
				throttleInitial,
				throttleRecap,
				changeNotificationRate,
				useChangeNotificationRate,
				useWaitForSymbol,
				mamaMsgQueueLimitPerSymbol,
				sourceRange,
				symbolRange,
				bookDepthLimit,
				stockSources,
				futureSources,
				optionSources,
				setRecoverGaps,
				rawQueueLimit,
				logDroppedMessages);
		}

		#endregion

		private void Initialize(
			string middleware,
			int maxRetries,
			uint idelTimeBetweenRetries,
			bool enableMamaLog,
			string mamaLogFilePath,
			string transportName,
			string dictionarySourceSymbolNamespace,
			int throttleDefault,
			int throttleInitial,
			int throttleRecap,
			int changeNotificationRate,
			bool useChangeNotificationRate,
			bool useWaitForSymbol,
			int mamaMsgQueueLimitPerSymbol,
			string sourceRange,
			string symbolRange,
			int bookDepthLimit,
			string stockSources,
			string futureSources,
			string optionSources,
			bool setRecoverGaps,
			int rawQueueLimit,
			bool logDroppedMessages)
		{
			if (_lbmBase == null)
			{
				if (middleware.ToUpper() == "LBM")
				{
					_lbmBase = new LBM(
						middleware,
						maxRetries,
						idelTimeBetweenRetries,
						enableMamaLog,
						mamaLogFilePath,
						transportName,
						dictionarySourceSymbolNamespace,
						throttleDefault,
						throttleInitial,
						throttleRecap,
						changeNotificationRate,
						useChangeNotificationRate,
						useWaitForSymbol,
						mamaMsgQueueLimitPerSymbol,
						sourceRange,
						symbolRange,
						bookDepthLimit,
						stockSources,
						futureSources,
						optionSources,
						setRecoverGaps,
						rawQueueLimit,
						logDroppedMessages);
					_lbmBase.LBMChanged += new LBMChangedEventHandler(LBMChanged);
				}
				else if (middleware.ToUpper() == "WMW")
				{
					_lbmBase = new LBM(
						middleware,
						maxRetries,
						idelTimeBetweenRetries,
						enableMamaLog,
						mamaLogFilePath,
						transportName,
						dictionarySourceSymbolNamespace,
						throttleDefault,
						throttleInitial,
						throttleRecap,
						changeNotificationRate,
						useChangeNotificationRate,
						useWaitForSymbol,
						mamaMsgQueueLimitPerSymbol,
						sourceRange,
						symbolRange,
						bookDepthLimit,
						stockSources,
						futureSources,
						optionSources,
						false,
						rawQueueLimit,
						logDroppedMessages);
					_lbmBase.LBMChanged += new LBMChangedEventHandler(LBMChanged);
				}
				else
				{
					// Don't do a thing, invalid middleware
				}
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
			_lbmBase.Start();
		}

		#endregion

		#region - Subscribe -

		public void Subscribe(string symbol, string source, bool reload)
		{
			_lbmBase.Subscribe(symbol, source, reload);
		}

		public void SubscribeBook(string symbol, string source, bool reload)
		{
			_lbmBase.SubscribeBook(symbol, source, reload);
		}

		public void SubscribeL2(string symbol, string source, bool reload)
		{
			_lbmBase.SubscribeL2(symbol, source, reload);
		}

		#endregion

		#region - Stop -

		public void Stop()
		{
			if (_lbmBase != null)
			{
				_lbmBase.Dispose();
			}
		}

		#endregion

		#region - Force GC -

		public void ForceGC()
		{
			if (_lbmBase != null)
			{
				_lbmBase.ForceGC();
			}
		}

		#endregion
	}
}
