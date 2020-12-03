using System;
using System.Collections.Generic;
using System.Text;

namespace SRLabEx
{
	public delegate void MarketDataChangedEventHandler(object sender, SRLabChangedEventArgs e);

	public class SRLabWrapper
	{
		#region - Event -

		public event MarketDataChangedEventHandler MarketDataChanged;
		protected virtual void OnMarketDataChanged(SRLabChangedEventArgs e)
		{
			if (MarketDataChanged != null)
			{
				MarketDataChanged(this, e);
			}
		}

		#endregion

		#region - Public Properties -

		public Dictionary<string, SubscriptionFields> Subscriptions
		{
			get
			{
				return _srLab.Subscriptions;
			}
		}

		public List<string> SubscriptionsCreated
		{
			get
			{
				return _srLab.SubscriptionsCreated;
			}
		}

		#endregion

		#region - Local Variable -

		private SRLabBase _srLab;

		#endregion

		#region - Constructor -

		public SRLabWrapper(
			string startupPath,
			string smNames,
			string smType,
			string configlocation,
			string configFile,
			int depth,
			string localIP,
			bool delayMCJoin,
			bool fireLevel1Only,
			bool lastValue,
			string logLevel,
			string maxLogSize,
			string sourceRange,
			string symbolRange,
			string stockSources,
			string futureSources,
			string optionSources,
			bool busyWait,
			int changeNotificationRate,
			int lineTimeHourOffset,
			int lineTimeMinOffset,
			int lineTimeSecOffset,
			bool useChangeNotificationRate,
			bool subscriptionUseWaitForSymbol,
			bool subscribeWithSource)
		{
			Initialize(
				startupPath,
				smNames,
				smType,
				configlocation,
				configFile,
				depth,
				localIP,
				delayMCJoin,
				fireLevel1Only,
				lastValue,
				logLevel,
				maxLogSize,
				sourceRange,
				symbolRange,
				stockSources,
				futureSources,
				optionSources,
				busyWait,
				changeNotificationRate,
				lineTimeHourOffset,
				lineTimeMinOffset,
				lineTimeSecOffset,
				useChangeNotificationRate,
				subscriptionUseWaitForSymbol,
				subscribeWithSource);
		}

		private void Initialize(
			string startupPath,
			string smNames,
			string smType,
			string configlocation,
			string configFile,
			int depth,
			string localIP,
			bool delayMCJoin,
			bool fireLevel1Only,
			bool lastValue,
			string logLevel,
			string maxLogSize,
			string sourceRange,
			string symbolRange,
			string stockSources,
			string futureSources,
			string optionSources,
			bool busyWait,
			int changeNotificationRate,
			int lineTimeHourOffset,
			int lineTimeMinOffset,
			int lineTimeSecOffset,
			bool useChangeNotificationRate,
			bool subscriptionUseWaitForSymbol,
			bool subscribeWithSource)
		{
			if (_srLab == null)
			{
				_srLab = new SRLabBase(
					startupPath,
					smNames,
					smType,
					configlocation,
					configFile,
					depth,
					localIP,
					delayMCJoin,
					fireLevel1Only,
					lastValue,
					logLevel,
					maxLogSize,
					sourceRange,
					symbolRange,
					stockSources,
					futureSources,
					optionSources,
					busyWait,
					changeNotificationRate,
					lineTimeHourOffset,
					lineTimeMinOffset,
					lineTimeSecOffset,
					useChangeNotificationRate,
					subscriptionUseWaitForSymbol,
					subscribeWithSource);

				_srLab.SRLabChanged+= new SRLabChangedEventHandler(SRLabChanged);
			}
		}

		#endregion

		private void SRLabChanged(object sender, SRLabChangedEventArgs e)
		{
			OnMarketDataChanged(e);
		}

		#region - Check Environments -

		public void CheckEnvironments()
		{
			_srLab.CheckEnvironments();
		}

		#endregion

		#region - Start -

		public void Start()
		{
			_srLab.Start();
		}

		#endregion

		#region - Subscribe -

		public void Subscribe(string symbol, string source, bool reload)
		{
			_srLab.Subscribe(symbol, source, reload);
		}

		public void SubscribeBook(string symbol, string source, bool reload)
		{
			_srLab.SubscribeBook(symbol, source, reload);
		}

		public void SubscribeL2(string symbol, string source, bool reload)
		{
			_srLab.SubscribeL2(symbol, source, reload);
		}

		#endregion

		#region - Stop -

		public void Stop()
		{
			if (_srLab != null)
			{
				_srLab.Dispose();
			}
		}

		#endregion

		#region - Force GC -

		public void ForceGC()
		{
			if (_srLab != null)
			{
				_srLab.ForceGC();
			}
		}

		#endregion
	}
}
