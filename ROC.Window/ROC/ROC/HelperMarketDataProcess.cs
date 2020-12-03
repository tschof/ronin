using System;
using System.Collections.Generic;
using System.Text;
using SocketEx;
using MarketDataEx;
using System.Diagnostics;

namespace ROC
{
	public class HelperMarketDataProcess
	{
		private bool _stopping = false;

		private object _syncObj = new object();

		private bool _useUIUpdateRate = true;
		private uint _uiUpdateRate = 100;

		#region - Trade Process Thread -

		private System.Threading.Timer _timerMarketDataProcess;
		private System.Threading.TimerCallback _timerMarketDataProcessDelagate;

		#endregion

		private Dictionary<string, MDServerToClient> _displayedData;
		public Dictionary<string, MDServerToClient> DisplayedData
		{
			get
			{
				if (_displayedData == null)
				{
					_displayedData = new Dictionary<string, MDServerToClient>();
				}
				return _displayedData;
			}
			set
			{
				_displayedData = value;
			}
		}

		private List<string> _replayList;
		public List<string> ReplayList
		{
			get
			{
				if (_replayList == null)
				{
					_replayList = new List<string>();
				}
				return _replayList;
			}
			set
			{
				_replayList = value;
			}
		}

		public void AddToReplayList(string symbol)
		{
			lock (ReplayList)
			{
				if (!ReplayList.Contains(symbol))
				{
					ReplayList.Add(symbol);
				}
			}
		}

		public HelperMarketDataProcess()
		{
			_useUIUpdateRate = Configuration.User.Default.UseUIUpdateRate;
			_uiUpdateRate = (uint)Configuration.User.Default.UIUpdateRate;
		}

		#region - Trade Thread Process -

		public void StartMarketDataProcess()
		{
			if (!_stopping)
			{
				if (_timerMarketDataProcessDelagate == null)
				{
					_timerMarketDataProcessDelagate = new System.Threading.TimerCallback(MarketDataProcess);
				}

				if (_timerMarketDataProcess == null)
				{
					if (Configuration.User.Default.UseUIUpdateRate)
					{
						_timerMarketDataProcess = new System.Threading.Timer(_timerMarketDataProcessDelagate, null, 0, Configuration.User.Default.UIUpdateRate);
					}
					else
					{
						_timerMarketDataProcess = new System.Threading.Timer(_timerMarketDataProcessDelagate, null, 0, 1);
					}
				}
			}
		}

		private void MarketDataProcess(object input)
		{
			if (GLOBAL.HROM.EndOfQueuedMsg)
			{
				// Process all window with Security data
				ProcessSecurityData();

				// Process all windows with market data
				ProcessMarketData();

				// Process all Aggregation
				ProcessAggregation();
			}
		}

		public void StopMarketDataProcess()
		{
			_stopping = true;

			if (_timerMarketDataProcess != null)
			{
				_timerMarketDataProcess.Dispose();
			}
			_timerMarketDataProcess = null;
			_timerMarketDataProcessDelagate = null;
		}

		#endregion

		private void ProcessSecurityData()
		{
			ProcessPositions();
		}
		
		private void ProcessMarketData()
		{
			MDServerToClient[] deltas = new MDServerToClient[0];

			if (GLOBAL.HMDS.LatestData.Count > 0)
			{
				// Make a copy of all the latest data
				Dictionary<string, MDServerToClient> locLatestData = new Dictionary<string, MDServerToClient>();
				lock (GLOBAL.HMDS.LatestData)
				{
					locLatestData = new Dictionary<string, MDServerToClient>(GLOBAL.HMDS.LatestData);
				}

				// Make a copy of replay List
				List<string> locReplayList = new List<string>();
				lock (ReplayList)
				{
					locReplayList.AddRange(ReplayList.ToArray());
					ReplayList.Clear();
				}

				// Make a copy of all the latest delta
				Dictionary<string, MDServerToClient> locLatestDelta = new Dictionary<string, MDServerToClient>();
				lock (GLOBAL.HMDS.LatestDelta)
				{
				    locLatestDelta = new Dictionary<string, MDServerToClient>(GLOBAL.HMDS.LatestDelta);
				    GLOBAL.HMDS.LatestDelta.Clear();
				}

				// Check to see if any symobl needs to be replayed
				foreach (string issuedSymbol in locReplayList)
				{
			        // Replay
					if (locLatestDelta.ContainsKey(issuedSymbol))
					{
						locLatestDelta.Remove(issuedSymbol);
						locLatestDelta.Add(issuedSymbol, locLatestData[issuedSymbol]);
					}
				 }

				if (locLatestDelta.Count > 0)
				{
				    // Convert to Delta arrays
				    deltas = new MDServerToClient[locLatestDelta.Count];
				    locLatestDelta.Values.CopyTo(deltas, 0);
					locLatestDelta.Clear();
				}
			}

			bool displayed = false;
			foreach (MDServerToClient delta in deltas)
			{
				displayed = ProcessPositions(delta);

				if (displayed)
				{
					lock (DisplayedData)
					{
						if (!DisplayedData.ContainsKey(delta.IssueSymbol))
						{
							DisplayedData.Add(delta.IssueSymbol, delta);
						}
					}
				}
			}
		}
		
		private void ProcessAggregation()
		{
			frmPositions[] ws = new frmPositions[0];
			if (ws.Length != GLOBAL.HWindows.PositionWindows.Count)
			{
				lock (GLOBAL.HWindows.PositionWindows)
				{
					ws = new frmPositions[GLOBAL.HWindows.PositionWindows.Count];
					GLOBAL.HWindows.PositionWindows.Values.CopyTo(ws, 0);
				}
			}

			foreach (frmPositions w in ws)
			{
				if (!w.IsProcessing)
				{
					w.UpdateAggragation();
				}
			}
		}

		private void ProcessPositions()
		{
			frmPositions[] ws = new frmPositions[0];
			if (ws.Length != GLOBAL.HWindows.PositionWindows.Count)
			{
				lock (GLOBAL.HWindows.PositionWindows)
				{
					ws = new frmPositions[GLOBAL.HWindows.PositionWindows.Count];
					GLOBAL.HWindows.PositionWindows.Values.CopyTo(ws, 0);
				}
			}

			foreach (frmPositions w in ws)
			{
				if (!w.IsProcessing)
				{
					w.UpdateSecurityInfo();
				}
			}
		}
		private bool ProcessPositions(MDServerToClient delta)
		{
			bool displayed = false;

			frmPositions[] ws = new frmPositions[0];
			if (ws.Length != GLOBAL.HWindows.PositionWindows.Count)
			{
				lock (GLOBAL.HWindows.PositionWindows)
				{
					ws = new frmPositions[GLOBAL.HWindows.PositionWindows.Count];
					GLOBAL.HWindows.PositionWindows.Values.CopyTo(ws, 0);
				}
			}

			foreach (frmPositions w in ws)
			{
				if (_stopping) return displayed;
				if (!w.IsProcessing)
				{
					displayed = w.UpdatePositions(delta);
				}
			}

			return displayed;
		}
	}
}
