using System;
using System.Collections.Generic;

using CSVEx;
using RDSEx;
using System.Diagnostics;
using MarketDataEx;
using System.Windows.Forms;
using DateTimeEx;

namespace ROC
{
	public class HelperProcess
	{
		#region - Local Variable -

		private bool _stopping = false;

		private bool _useUIUpdateRate = true;
		private int _uiUpdateRate = 50;

		private bool _updateBatchTicketStatus = false;

		private string _tStopTag = "";
		private string _tStopBaseTag = "";

		private DateTimeHP _dtHP = new DateTimeHP();

		//private int _newCSVProcessLimit = 100;

		#endregion

		#region - Local Data -

		// New
		private CSV[] _newCSVs = new CSV[0];
		private List<ROCOrder> _newROCOrders = new List<ROCOrder>();
		private List<ROCExecution> _newROCExecutions = new List<ROCExecution>();
		private List<TPOSExecution> _newTPOSExecutions = new List<TPOSExecution>();

		// Sounds
		private bool _playAllStatusSound = false;
		private List<long> _newROCStatusToPlay = new List<long>();

		// All Server Delta List
		private List<Dictionary<string, MDServerToClient>> _newMarketDataDeltaList = new List<Dictionary<string, MDServerToClient>>();
		
		// Consolidation
		private Dictionary<string, MDServerToClient> _newMarketDataDelta = new Dictionary<string, MDServerToClient>();

		// Market Data Process Limit
		private int _newMarketDataProcessLimit = 0;

		private List<string> _currentReplayList = new List<string>();

		private bool _loadIMMarketData = false;

		#endregion

		#region - Market Data Helpers -

		public void AddToReplayList(string symbol)
		{
			lock (_currentReplayList)
			{
				if (!_currentReplayList.Contains(symbol))
				{
					_currentReplayList.Add(symbol);
				}
			}
		}

		#endregion

		#region - Main Market Data Process Thread -

		private bool _threadProcessMarketDataShouldStop = false;
		private bool _threadProcessMarketDataStarted = false;
		private System.Threading.Thread _threadMarketDataProcess;

		#endregion

		#region - Main CSV Process Thread -

		private bool _threadProcessCSVShouldStop = false;
		private bool _threadProcessCSVStarted = false;
		private System.Threading.Thread _threadCSVProcess;

		#endregion

		#region - Main Sound Thread -

		private bool _useSoundTread = false;
		private bool _useSoundAPI = false;
		private bool _threadProcessSoundShouldStop = false;
		private bool _threadProcessSoundStarted = false;
		private System.Threading.Thread _threadSoundProcess;

		#endregion

		#region - Constructor -

		public HelperProcess()
		{
			SetUIUpdateRate();

			//_playAllStatusSound = Configuration.User.Default.PlayAllStatusSound;
			_updateBatchTicketStatus = Configuration.User.Default.UpdateBatchTicketStatus;	
		}

		private void SetUIUpdateRate()
		{
			_useUIUpdateRate = Configuration.User.Default.UseUIUpdateRate;
			_uiUpdateRate = Configuration.User.Default.UIUpdateRate;
			if (_useUIUpdateRate && _uiUpdateRate > 0)
			{
			}
			else
			{
				_uiUpdateRate = 50;
			}
		}

		#endregion

		#region - Process Thread Start Stop -

		public void StartProcess()
		{
			if (_threadCSVProcess == null && !_threadProcessCSVStarted && !_stopping)
			{
				_threadProcessCSVShouldStop = false;

				_threadCSVProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessCSV));
				_threadCSVProcess.Name = "ProcessCSV";
				switch (Configuration.User.Default.CSVThreadPriority)
				{
					case 0:
						_threadCSVProcess.Priority = System.Threading.ThreadPriority.Lowest;
						break;
					case 1:
						_threadCSVProcess.Priority = System.Threading.ThreadPriority.BelowNormal;
						break;
					case 2:
						_threadCSVProcess.Priority = System.Threading.ThreadPriority.Normal;
						break;
					case 3:
						_threadCSVProcess.Priority = System.Threading.ThreadPriority.AboveNormal;
						break;
					case 4:
						_threadCSVProcess.Priority = System.Threading.ThreadPriority.Highest;
						break;
				}
				_threadCSVProcess.Start();

				while (!_threadCSVProcess.IsAlive) ;
				_threadProcessCSVStarted = true;
			}

			if (_threadMarketDataProcess == null && !_threadProcessMarketDataStarted && !_stopping)
			{
				_threadProcessMarketDataShouldStop = false;

				_threadMarketDataProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessMarketData));
				_threadMarketDataProcess.Name = "ProcessMarketData";
				switch (Configuration.User.Default.MDThreadPriority)
				{
					case 0:
						_threadMarketDataProcess.Priority = System.Threading.ThreadPriority.Lowest;
						break;
					case 1:
						_threadMarketDataProcess.Priority = System.Threading.ThreadPriority.BelowNormal;
						break;
					case 2:
						_threadMarketDataProcess.Priority = System.Threading.ThreadPriority.Normal;
						break;
					case 3:
						_threadMarketDataProcess.Priority = System.Threading.ThreadPriority.AboveNormal;
						break;
					case 4:
						_threadMarketDataProcess.Priority = System.Threading.ThreadPriority.Highest;
						break;
				}
				_threadMarketDataProcess.Start();

				while (!_threadMarketDataProcess.IsAlive) ;
				_threadProcessMarketDataStarted = true;
			}

			if (_useSoundTread)
			{
				if (_threadSoundProcess == null && !_threadProcessSoundStarted && !_stopping)
				{
					_threadProcessSoundShouldStop = false;

					_threadSoundProcess = new System.Threading.Thread(new System.Threading.ThreadStart(ProcessSound));
					_threadSoundProcess.Name = "ProcessSound";
					_threadSoundProcess.Priority = System.Threading.ThreadPriority.Lowest;
					_threadSoundProcess.Start();

					while (!_threadSoundProcess.IsAlive) ;
					_threadProcessSoundStarted = true;
				}
			}
		}

		public void StopProcess()
		{
			_stopping = true;

			_threadProcessMarketDataShouldStop = true;
			if (_threadMarketDataProcess != null)
			{
				int count = 0;
				while (_threadMarketDataProcess.IsAlive)
				{
					count = count + 1;
					if (count > 100)
					{
						_threadMarketDataProcess.Abort();
						break;
					}
				}
			}

			_threadProcessCSVShouldStop = true;
			if (_threadCSVProcess != null)
			{
				int count = 0;
				while (_threadCSVProcess.IsAlive)
				{
					count = count + 1;
					if (count > 100)
					{
						_threadCSVProcess.Abort();
						break;
					}
				}
			}

			_threadProcessSoundShouldStop = true;
			if (_threadSoundProcess != null)
			{
				int count = 0;
				while (_threadSoundProcess.IsAlive)
				{
					count = count + 1;
					if (count > 100)
					{
						_threadSoundProcess.Abort();
						break;
					}
				}
			}
		}

		#endregion

		#region - Market Data Thread -

		private void ProcessMarketData()
		{
			while (!_threadProcessMarketDataShouldStop)
			{
				if (GLOBAL.MainForm != null)
				{
					PreMarketDataProcess();

					if (_newMarketDataDelta.Count > 0 || _loadIMMarketData)
					{
						GLOBAL.MainForm.ProcessingMarketData = true;
						GLOBAL.MainForm.ProcessMarketData();

						while (GLOBAL.MainForm.ProcessingMarketData)
						{
							if (_threadProcessMarketDataShouldStop)
							{
								// Application had been terminated.
								break;
							}

							System.Threading.Thread.Sleep(1);
						}
					}
				}

				if (GLOBAL.ReloadProcessSettings)
				{
					SetUIUpdateRate();
					GLOBAL.ReloadProcessSettings = false;
				}

				System.Threading.Thread.Sleep(_uiUpdateRate);
			}

			_threadProcessMarketDataStarted = false;
		}

		public void ProcessAllMarketData()
		{
			//DateTime hpDTStart = DateTime.Now;
			if (GLOBAL.HROM.EndOfQueuedMsg && GLOBAL.MainForm != null)
			{
				if (_newMarketDataProcessLimit != 0)
				{
					#region - Process Market Data With Limit -

					Dictionary<string, MDServerToClient> locMarketdataDelta = new Dictionary<string,MDServerToClient>();

					#region - Process IM -

					// Process IM
					if (_loadIMMarketData)
					{
						try
						{
							// Update Tickets
							GLOBAL.HProcess.ProcessAutoSpreadTicketWindows(_loadIMMarketData, locMarketdataDelta);
							GLOBAL.HProcess.ProcessQuickTicketWindows(_loadIMMarketData, locMarketdataDelta);
							GLOBAL.HProcess.ProcessBatchMarketTicketWindows(_loadIMMarketData, locMarketdataDelta);
							GLOBAL.HProcess.ProcessFutureTicketWindows(_loadIMMarketData, locMarketdataDelta);
							GLOBAL.HProcess.ProcessOptionTicketWindows(_loadIMMarketData, locMarketdataDelta);
							GLOBAL.HProcess.ProcessStockTicketWindows(_loadIMMarketData, locMarketdataDelta);
							GLOBAL.HProcess.ProcessFutureMatrixTicketWindow(_loadIMMarketData, locMarketdataDelta);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}

						//ProcessTposExecutions(_newTPOSExecutions);
						GLOBAL.HProcess.ProcessOrderWindows(_loadIMMarketData, locMarketdataDelta);
						GLOBAL.HProcess.ProcessTradeWindows(_loadIMMarketData);
						GLOBAL.HProcess.ProcessPositionWindows(_loadIMMarketData, locMarketdataDelta);

						// Update Market Datas
						GLOBAL.HProcess.ProcessWatchListWindows(_loadIMMarketData, locMarketdataDelta);
						GLOBAL.HProcess.ProcessPlotListWindows(_loadIMMarketData, locMarketdataDelta);

						_loadIMMarketData = false;
					}

					#endregion

					lock (_newMarketDataDelta)
					{
						if (_newMarketDataDelta.Count > 0)
						{
							locMarketdataDelta = new Dictionary<string, MDServerToClient>(_newMarketDataDelta);
							_newMarketDataDelta.Clear();
							//System.Threading.Thread.Sleep(1);
						}
					}

					if (locMarketdataDelta.Count > 0)
					{
						Dictionary<string, MDServerToClient> tempMarketdataDelta = new Dictionary<string, MDServerToClient>();
						int count = 0;
						foreach (string key in locMarketdataDelta.Keys)
						{
							if (count > _newMarketDataProcessLimit)
							{
								// Process Data
								if (tempMarketdataDelta.Count > 0)
								{
									#region - Process MarketData To The Limit -

									try
									{
										// Update Tickets
										GLOBAL.HProcess.ProcessAutoSpreadTicketWindows(false, tempMarketdataDelta);
										GLOBAL.HProcess.ProcessQuickTicketWindows(false, tempMarketdataDelta);
										GLOBAL.HProcess.ProcessBatchMarketTicketWindows(false, tempMarketdataDelta);
										GLOBAL.HProcess.ProcessFutureTicketWindows(false, tempMarketdataDelta);
										GLOBAL.HProcess.ProcessOptionTicketWindows(false, tempMarketdataDelta);
										GLOBAL.HProcess.ProcessStockTicketWindows(false, tempMarketdataDelta);
										GLOBAL.HProcess.ProcessFutureMatrixTicketWindow(false, tempMarketdataDelta);
									}
									catch (Exception ex)
									{
										GLOBAL.HROC.AddToException(ex);
									}

									//ProcessTposExecutions(_newTPOSExecutions);
									GLOBAL.HProcess.ProcessOrderWindows(false, tempMarketdataDelta);
									GLOBAL.HProcess.ProcessTradeWindows(false);
									GLOBAL.HProcess.ProcessPositionWindows(false, tempMarketdataDelta);

									// Update Market Datas
									GLOBAL.HProcess.ProcessWatchListWindows(false, tempMarketdataDelta);
									GLOBAL.HProcess.ProcessPlotListWindows(false, tempMarketdataDelta);

									#endregion

									System.Threading.Thread.Sleep(1);
									//Application.DoEvents();
								}

								tempMarketdataDelta.Clear();
								count = 0;
							}
							else
							{
								if (tempMarketdataDelta.ContainsKey(key))
								{
								}
								else
								{
									tempMarketdataDelta.Add(key, locMarketdataDelta[key]);
								}
							}

							count++;
						}

						if (tempMarketdataDelta.Count > 0)
						{
							#region - Process LeftOver -

							try
							{
								// Update Tickets
								GLOBAL.HProcess.ProcessAutoSpreadTicketWindows(false, tempMarketdataDelta);
								GLOBAL.HProcess.ProcessQuickTicketWindows(false, tempMarketdataDelta);
								GLOBAL.HProcess.ProcessBatchMarketTicketWindows(false, tempMarketdataDelta);
								GLOBAL.HProcess.ProcessFutureTicketWindows(false, tempMarketdataDelta);
								GLOBAL.HProcess.ProcessOptionTicketWindows(false, tempMarketdataDelta);
								GLOBAL.HProcess.ProcessStockTicketWindows(false, tempMarketdataDelta);
								GLOBAL.HProcess.ProcessFutureMatrixTicketWindow(false, tempMarketdataDelta);
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}

							//ProcessTposExecutions(_newTPOSExecutions);
							GLOBAL.HProcess.ProcessOrderWindows(false, tempMarketdataDelta);
							GLOBAL.HProcess.ProcessTradeWindows(false);
							GLOBAL.HProcess.ProcessPositionWindows(false, tempMarketdataDelta);

							// Update Market Datas
							GLOBAL.HProcess.ProcessWatchListWindows(false, tempMarketdataDelta);
							GLOBAL.HProcess.ProcessPlotListWindows(false, tempMarketdataDelta);

							#endregion
						}

						tempMarketdataDelta.Clear();
						locMarketdataDelta.Clear();
					}

					#endregion
				}
				else
				{
					#region - No Limit Processing -

					try
					{
						// Update Tickets
						GLOBAL.HProcess.ProcessAutoSpreadTicketWindows(_loadIMMarketData, _newMarketDataDelta);
						GLOBAL.HProcess.ProcessQuickTicketWindows(_loadIMMarketData, _newMarketDataDelta);
						GLOBAL.HProcess.ProcessBatchMarketTicketWindows(_loadIMMarketData, _newMarketDataDelta);
						GLOBAL.HProcess.ProcessFutureTicketWindows(_loadIMMarketData, _newMarketDataDelta);
						GLOBAL.HProcess.ProcessOptionTicketWindows(_loadIMMarketData, _newMarketDataDelta);
						GLOBAL.HProcess.ProcessStockTicketWindows(_loadIMMarketData, _newMarketDataDelta);
						GLOBAL.HProcess.ProcessFutureMatrixTicketWindow(_loadIMMarketData, _newMarketDataDelta);
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}

					//ProcessTposExecutions(_newTPOSExecutions);
					GLOBAL.HProcess.ProcessOrderWindows(_loadIMMarketData, _newMarketDataDelta);
					GLOBAL.HProcess.ProcessTradeWindows(_loadIMMarketData);
					GLOBAL.HProcess.ProcessPositionWindows(_loadIMMarketData, _newMarketDataDelta);

					// Update Market Datas
					GLOBAL.HProcess.ProcessWatchListWindows(_loadIMMarketData, _newMarketDataDelta);
					GLOBAL.HProcess.ProcessPlotListWindows(_loadIMMarketData, _newMarketDataDelta);

					_loadIMMarketData = false;

					#endregion
				}

				_newMarketDataDeltaList.Clear();
				_newMarketDataDelta.Clear();
			}
			//DateTimeHP hpDTEnd = new DateTimeEx.DateTimeHP();
			//TimeSpan ts = hpDTEnd.Now.Subtract(hpDTStart);
			//GLOBAL.HROC.AddToStatusLogs("MKT - " + ts.TotalMilliseconds.ToString("N4"));
		}

		private void PreMarketDataProcess()
		{
			// Check to see if IM needs to be refreshed or not
			lock (GLOBAL.HRDS)
			{
				if (GLOBAL.HRDS.NewSecufityInfo)
				{
					GLOBAL.HRDS.NewSecufityInfo = false;
					_loadIMMarketData = true;
				}
			}

			GetNewMarkData();
		}

		// MarketData
		private void GetNewMarkData()
		{
			_newMarketDataDeltaList = new List<Dictionary<string, MDServerToClient>>();
			_newMarketDataDelta = new Dictionary<string, MDServerToClient>();

			// Process Orders First
			if (_newCSVs.Length == 0)
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					if (mds.LatestDelta.Count > 0)
					{
						// Make a copy of all the latest delta from different servers
						lock (mds.LatestDelta)
						{
							_newMarketDataDeltaList.Add(new Dictionary<string, MDServerToClient>(mds.LatestDelta));
							mds.LatestDelta.Clear();
						}
					}
				}

				// Consolidate MarketDataDelta into one
				foreach (Dictionary<string, MDServerToClient> newMarketDataDelta in _newMarketDataDeltaList)
				{
					foreach (string key in newMarketDataDelta.Keys)
					{
						newMarketDataDelta[key].uClientRecivedTime = _dtHP.Now;
						if (!_newMarketDataDelta.ContainsKey(key))
						{
							_newMarketDataDelta.Add(key, newMarketDataDelta[key]);
						}
						else
						{
							_newMarketDataDelta[key].Update(newMarketDataDelta[key], true);
						}
					}
				}
				_newMarketDataDeltaList.Clear();

				// DEBUG
				//if (_newMarketDataDelta.Count > 100)
				//{
				//    GLOBAL.HROC.AddToStatusLogs("_newMarketDataDelta count = " + _newMarketDataDelta.Count.ToString());
				//}

				foreach (string key in _newMarketDataDelta.Keys)
				{
					if (!GLOBAL.HMarketData.Current.ContainsKey(key))
					{
						GLOBAL.HMarketData.Current.Add(key, _newMarketDataDelta[key]);
					}
					else
					{
						GLOBAL.HMarketData.Current[key].Update(_newMarketDataDelta[key]);
					}
				}

				if (_currentReplayList.Count > 0)
				{
					lock (_currentReplayList)
					{
						string[] replaySymbols = _currentReplayList.ToArray();
						foreach (string replaySymbol in replaySymbols)
						{
							if (GLOBAL.HMarketData.Current.ContainsKey(replaySymbol))
							{
								if (_newMarketDataDelta.ContainsKey(replaySymbol))
								{
									_newMarketDataDelta[replaySymbol].Update(GLOBAL.HMarketData.Current[replaySymbol]);
								}
								else
								{
									_newMarketDataDelta.Add(replaySymbol, GLOBAL.HMarketData.Current[replaySymbol]);
								}
								// remove from replay list
								_currentReplayList.Remove(replaySymbol);
							}
						}
					}
				}
			}
		}

		#endregion

		#region - CSV Thread -

		private void ProcessCSV()
		{
			while (!_threadProcessCSVShouldStop)
			{
				GLOBAL.HROM.AddToCSVs();

				if (GLOBAL.MainForm != null)
				{
					PreCSVProcess();

					if (_newROCOrders.Count > 0 || _newROCExecutions.Count > 0 || _newTPOSExecutions.Count > 0)
					{
						GLOBAL.MainForm.ProcessingCSV = true;						
						GLOBAL.MainForm.ProcessCSV();

						while (GLOBAL.MainForm.ProcessingCSV)
						{
							if (_threadProcessCSVShouldStop)
							{
								// Application had been terminated.
								break;
							}

							System.Threading.Thread.Sleep(1);
						}
					}
				}

				System.Threading.Thread.Sleep(1);
			}

			_threadProcessCSVStarted = false;
		}

		public void ProcessAllCSV()
		{
			//DateTime hpDTStart = DateTime.Now;

			if (GLOBAL.HROM.EndOfQueuedMsg && GLOBAL.MainForm != null)
			{
				try
				{
					// Update Tickets
					GLOBAL.HProcess.ProcessBatchMarketTicketWindows(_newROCOrders);
					GLOBAL.HProcess.ProcessAutoSpreadTicketWindows(_newROCOrders);
					GLOBAL.HProcess.ProcessQuickTicketWindows(_newROCOrders);
					GLOBAL.HProcess.ProcessFutureTicketWindows(_newROCOrders);
					//GLOBAL.HProcess.ProcessOptionTicketWindows(_newROCOrders);
					GLOBAL.HProcess.ProcessStockTicketWindows(_newROCOrders);
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}

				// Update Orders and Trades
				if (_updateBatchTicketStatus)
				{
					GLOBAL.HProcess.ProcessBatchTicketWindows(_newROCOrders);
				}
				GLOBAL.HProcess.ProcessOrderWindows(_newROCOrders);

				ProcessTposExecutions(_newTPOSExecutions);
				GLOBAL.HProcess.ProcessTradeWindows(_newROCExecutions, _newTPOSExecutions);
				GLOBAL.HProcess.ProcessPositionWindows(_newROCExecutions, _newTPOSExecutions);

				_newCSVs = new CSV[0];

				_newROCOrders.Clear();

				_newROCExecutions.Clear();
				_newTPOSExecutions.Clear();

				//_newROCPositions.Clear();
				//_newTPOSPositions.Clear();

				if (!_useSoundTread)
				{
					foreach (long status in _newROCStatusToPlay)
					{
						PlaySoundByStatus(status);
					}
					_newROCStatusToPlay.Clear();
				}
			}
			//DateTimeEx.DateTimeHP hpDTEnd = new DateTimeEx.DateTimeHP();

			//TimeSpan ts = hpDTEnd.Now.Subtract(hpDTStart);
			//GLOBAL.HROC.AddToStatusLogs("CSV - " + ts.TotalMilliseconds.ToString("N4"));
		}

		private void PreCSVProcess()
		{
			GetNewCsvs();
			GetNewRocOrders();
			GetNewRocExecutions();
			GetNewTposExecutions();

			ProcessCSVs(_newCSVs);

			// Check to see if IM needs to be refreshed or not
			//lock (GLOBAL.HRDS)
			//{
			//    if (GLOBAL.HRDS.NewSecufityInfo)
			//    {
			//        GLOBAL.HRDS.NewSecufityInfo = false;
			//        _loadIMCSV = true;
			//    }
			//}
		}

		private void GetNewCsvs()
		{
			_newCSVs = new CSV[0];
			lock (GLOBAL.HROM.NewCriticalRomCSVs)
			{
				if (GLOBAL.HROM.NewCriticalRomCSVs.Count > 0)
				{
					_newCSVs = new CSV[GLOBAL.HROM.NewCriticalRomCSVs.Count];
					GLOBAL.HROM.NewCriticalRomCSVs.CopyTo(_newCSVs, 0);
					GLOBAL.HROM.NewCriticalRomCSVs.Clear();
					return;
				}
			}

			lock (GLOBAL.HROM.NewRomCSVs)
			{
				if (GLOBAL.HROM.NewRomCSVs.Count > 0)
				{
					//if (Configuration.User.Default.DelayUIProcess && GLOBAL.HROM.NewRomCSVs.Count > _newCSVProcessLimit)
					//{
					//    _newCSVs = new CSV[_newCSVProcessLimit];
					//    GLOBAL.HROM.NewRomCSVs.CopyTo(0, _newCSVs, 0, _newCSVProcessLimit);
					//    GLOBAL.HROM.NewRomCSVs.RemoveRange(0, _newCSVProcessLimit);
					//}
					//else
					//{
						_newCSVs = new CSV[GLOBAL.HROM.NewRomCSVs.Count];
						GLOBAL.HROM.NewRomCSVs.CopyTo(_newCSVs, 0);
						GLOBAL.HROM.NewRomCSVs.Clear();
					//}
				}
			}
		}

		private void GetNewRocOrders()
		{
			_newROCOrders = new List<ROCOrder>();
			lock (GLOBAL.HROM.NewOrders)
			{
				if (GLOBAL.HROM.NewOrders.Count > 0)
				{
					ROCOrder[] locArray = new ROCOrder[GLOBAL.HROM.NewOrders.Values.Count];
					GLOBAL.HROM.NewOrders.Values.CopyTo(locArray, 0);
					_newROCOrders.AddRange(locArray);

					GLOBAL.HROM.NewOrders.Clear();
				}
			}
		}

		private void GetNewRocExecutions()
		{
			_newROCExecutions = new List<ROCExecution>();
			lock (GLOBAL.HROM.NewExecutions)
			{
				if (GLOBAL.HROM.NewExecutions.Count > 0)
				{
					ROCExecution[] locArray = new ROCExecution[GLOBAL.HROM.NewExecutions.Values.Count];
					GLOBAL.HROM.NewExecutions.Values.CopyTo(locArray, 0);
					_newROCExecutions.AddRange(locArray);

					GLOBAL.HROM.NewExecutions.Clear();
				}
			}
		}

		private void GetNewTposExecutions()
		{
			_newTPOSExecutions = new List<TPOSExecution>();
			if (GLOBAL.HRDS.NewTposExecutions.Count > 0)
			{
				lock (GLOBAL.HRDS.NewTposExecutions)
				{
					TPOSExecution[] locArray = new TPOSExecution[GLOBAL.HRDS.NewTposExecutions.Values.Count];
					GLOBAL.HRDS.NewTposExecutions.Values.CopyTo(locArray, 0);
					_newTPOSExecutions.AddRange(locArray);

					GLOBAL.HRDS.NewTposExecutions.Clear();
				}
			}
		}

		#region - Processing CSVs -

		private void ProcessCSVs(CSV[] csvs)
		{
			if (csvs.Length > 0)
			{
				foreach (CSV csv in csvs)
				{
					if (_stopping) break;
					switch (csv.Command)
					{
						case CSVFieldIDs.MessageTypes.WorkingOrder:
							CsvToOrder(csv);
							break;
						case CSVFieldIDs.MessageTypes.OrderStatus:
							if (GLOBAL.HROM.OrginalCSVs.ContainsKey(csv.Tag))
							{
								#region - Status Check -

								switch (csv.Status)
								{
									case CSVEx.CSVFieldIDs.StatusCodes.CancelRejected:
										if (GLOBAL.HROM.CanceledOrderIDs.Contains(csv.Tag))
										{
											GLOBAL.HROM.CanceledOrderIDs.Remove(csv.Tag);
										}
										break;
								}

								#endregion

								#region - Status Sounds -

								lock (_newROCStatusToPlay)
								{
									if (_playAllStatusSound || !_newROCStatusToPlay.Contains(csv.Status))
									{
										_newROCStatusToPlay.Add(csv.Status);
									}
								}

								//PlaySoundByStatus(csv.Status);

								#endregion

								#region - Back Fill Order Info -

								if (csv.CMTAAccount == null || csv.CMTAAccount != GLOBAL.HROM.OrginalCSVs[csv.Tag].CMTAAccount)
								{
									csv.CMTAAccount = GLOBAL.HROM.OrginalCSVs[csv.Tag].CMTAAccount;
								}
								if (csv.ClearingAcct == null || csv.ClearingAcct != GLOBAL.HROM.OrginalCSVs[csv.Tag].ClearingAcct)
								{
									csv.ClearingAcct = GLOBAL.HROM.OrginalCSVs[csv.Tag].ClearingAcct;
								}
								if (csv.TradeFor == null || csv.TradeFor != GLOBAL.HROM.OrginalCSVs[csv.Tag].TradeFor)
								{
									csv.TradeFor = GLOBAL.HROM.OrginalCSVs[csv.Tag].TradeFor;
								}
								if (csv.Trader == null || csv.Trader != GLOBAL.HROM.OrginalCSVs[csv.Tag].Trader)
								{
									csv.Trader = GLOBAL.HROM.OrginalCSVs[csv.Tag].Trader;
								}
								if (csv.LocalAcct == null || csv.LocalAcct != GLOBAL.HROM.OrginalCSVs[csv.Tag].LocalAcct)
								{
									csv.LocalAcct = GLOBAL.HROM.OrginalCSVs[csv.Tag].LocalAcct;
								}
								if (csv.SecType == null || csv.SecType != GLOBAL.HROM.OrginalCSVs[csv.Tag].SecType)
								{
									csv.SecType = GLOBAL.HROM.OrginalCSVs[csv.Tag].SecType;
								}
								if (csv.SecurityDefinition == null || csv.SecurityDefinition != GLOBAL.HROM.OrginalCSVs[csv.Tag].SecurityDefinition)
								{
									csv.SecurityDefinition = GLOBAL.HROM.OrginalCSVs[csv.Tag].SecurityDefinition;
								}
								if (GLOBAL.HROM.OrginalCSVs[csv.Tag].CplxOrderType != CSVFieldIDs.CplxOrderTypes.Continer)
								{
									if (csv.Side != GLOBAL.HROM.OrginalCSVs[csv.Tag].Side)
									{
										csv.Side = GLOBAL.HROM.OrginalCSVs[csv.Tag].Side;
									}
								}
								if (GLOBAL.HROM.OrginalCSVs[csv.Tag].MaturityDay != null && csv.MaturityDay != GLOBAL.HROM.OrginalCSVs[csv.Tag].MaturityDay)
								{
									csv.MaturityDay = GLOBAL.HROM.OrginalCSVs[csv.Tag].MaturityDay;
								}
								if (csv.TIF == -1)
								{
									csv.TIF = GLOBAL.HROM.OrginalCSVs[csv.Tag].TIF;
								}

								#region - Imbalanced Only -

								if (GLOBAL.HROM.OrginalCSVs[csv.Tag].DisplayInstruction != null && GLOBAL.HROM.OrginalCSVs[csv.Tag].DisplayInstruction != "")
								{
									if (csv.DisplayInstruction != GLOBAL.HROM.OrginalCSVs[csv.Tag].DisplayInstruction)
									{
										csv.DisplayInstruction = GLOBAL.HROM.OrginalCSVs[csv.Tag].DisplayInstruction;
									}
									switch (csv.DisplayInstruction)
									{
										case "I":
											if (GLOBAL.HROM.OrginalCSVs[csv.Tag].Instructions.Contains("-O"))
											{
												csv.Type = CSVFieldIDs.OrderTypes.IMBOO;
											}
											else if (GLOBAL.HROM.OrginalCSVs[csv.Tag].Instructions.Contains("-C"))
											{
												csv.Type = CSVFieldIDs.OrderTypes.IMBOC;
											}
											else if (GLOBAL.HROM.OrginalCSVs[csv.Tag].Instructions.Contains("-I"))
											{
												csv.Type = CSVFieldIDs.OrderTypes.IMBOI;
											}
											break;
									}
								}

								#endregion

								#region - VWAP Order Type -

								if (GLOBAL.HROM.OrginalCSVs[csv.Tag].AlgoType != 0)
								{
									if (csv.AlgoType != GLOBAL.HROM.OrginalCSVs[csv.Tag].AlgoType)
									{
										csv.AlgoType = GLOBAL.HROM.OrginalCSVs[csv.Tag].AlgoType;
									}
									switch (csv.AlgoType)
									{
										case CSVFieldIDs.AlgoTypes.VWAP:
											switch (csv.Type)
											{
												case CSVFieldIDs.OrderTypes.Market:
													csv.Type = CSVFieldIDs.OrderTypes.VWAPMKT;
													break;
												case CSVFieldIDs.OrderTypes.Limit:
													csv.Type = CSVFieldIDs.OrderTypes.VWAPLIM;
													break;
											}
											break;
										case CSVFieldIDs.AlgoTypes.GVWAP:
											csv.Type = CSVFieldIDs.OrderTypes.GVWAP;
											break;
									}
								}

								#endregion

								if (GLOBAL.HROM.OrginalCSVs[csv.Tag].EndTime != null && csv.EndTime != GLOBAL.HROM.OrginalCSVs[csv.Tag].EndTime)
								{
									csv.EndTime = GLOBAL.HROM.OrginalCSVs[csv.Tag].EndTime;
								}

								#region - Status -

								switch (csv.Status)
								{
									case CSVEx.CSVFieldIDs.StatusCodes.Filled:
									case CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled:
										if (csv.Price == 0 && GLOBAL.HROM.OrginalCSVs[csv.Tag].Price != 0)
										{
											csv.Price = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										}
										else if (csv.Price != GLOBAL.HROM.OrginalCSVs[csv.Tag].Price)
										{
											csv.OriginalPrice = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										}
										break;
									case CSVEx.CSVFieldIDs.StatusCodes.ReplaceRejected:
										if (csv.Price != GLOBAL.HROM.OrginalCSVs[csv.Tag].Price)
										{
											csv.Price = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										}
										break;
									case CSVEx.CSVFieldIDs.StatusCodes.Replaced:
										csv.OriginalPrice = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										GLOBAL.HROM.OrginalCSVs[csv.Tag].Price = csv.Price;
										break;
									case CSVEx.CSVFieldIDs.StatusCodes.Rejected:
										if (csv.Shares == 0 && csv.Shares != GLOBAL.HROM.OrginalCSVs[csv.Tag].Shares)
										{
											csv.Shares = GLOBAL.HROM.OrginalCSVs[csv.Tag].Shares;
										}
										//if (csv.TIF == 0 && csv.TIF != GLOBAL.HROM.OrginalCSVs[csv.Tag].TIF)
										//{
										//    csv.TIF = GLOBAL.HROM.OrginalCSVs[csv.Tag].TIF;
										//}
										if (csv.Price == 0 && GLOBAL.HROM.OrginalCSVs[csv.Tag].Price != 0)
										{
											csv.Price = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										}
										else if (csv.Price != GLOBAL.HROM.OrginalCSVs[csv.Tag].Price)
										{
											csv.OriginalPrice = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										}
										break;
									default:
										if (csv.Price == 0 && GLOBAL.HROM.OrginalCSVs[csv.Tag].Price != 0)
										{
											csv.Price = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										}
										else if (csv.Price != GLOBAL.HROM.OrginalCSVs[csv.Tag].Price)
										{
											csv.OriginalPrice = GLOBAL.HROM.OrginalCSVs[csv.Tag].Price;
										}
										break;
								}

								#endregion

								#region - Option Back Fill -

								if (csv.Underlying != GLOBAL.HROM.OrginalCSVs[csv.Tag].Underlying)
								{
									csv.Underlying = GLOBAL.HROM.OrginalCSVs[csv.Tag].Underlying;
								}
								if (csv.StrikePrice != GLOBAL.HROM.OrginalCSVs[csv.Tag].StrikePrice)
								{
									csv.StrikePrice = GLOBAL.HROM.OrginalCSVs[csv.Tag].StrikePrice;
								}
								if (csv.ExpDate != GLOBAL.HROM.OrginalCSVs[csv.Tag].ExpDate)
								{
									csv.ExpDate = GLOBAL.HROM.OrginalCSVs[csv.Tag].ExpDate;
								}
								if (csv.CallPut != GLOBAL.HROM.OrginalCSVs[csv.Tag].CallPut)
								{
									csv.CallPut = GLOBAL.HROM.OrginalCSVs[csv.Tag].CallPut;
								}

								#endregion

								if (csv.ParentTag != GLOBAL.HROM.OrginalCSVs[csv.Tag].ParentTag)
								{
									csv.ParentTag = GLOBAL.HROM.OrginalCSVs[csv.Tag].ParentTag;
								}
								if (csv.ClientEcho != GLOBAL.HROM.OrginalCSVs[csv.Tag].ClientEcho)
								{
									csv.ClientEcho = GLOBAL.HROM.OrginalCSVs[csv.Tag].ClientEcho;
								}

								#endregion

								#region - OCO Processing -

								if (csv.Status == CSVEx.CSVFieldIDs.StatusCodes.Filled ||
									csv.Status == CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled ||
									csv.Status == CSVEx.CSVFieldIDs.StatusCodes.Rejected)
								{
									_tStopTag = csv.Tag.Substring(csv.Tag.Length - 1, 1);
									switch (_tStopTag)
									{
										case "M":
											_tStopBaseTag = csv.Tag.Substring(0, csv.Tag.Length - 1) + "S";
											if (GLOBAL.HROM.OrginalCSVs.ContainsKey(_tStopBaseTag))
											{
												GLOBAL.HROM.CancelSingleOrder(GLOBAL.HROM.OrginalCSVs[_tStopBaseTag]);
											}
											break;
										case "S":
											_tStopBaseTag = csv.Tag.Substring(0, csv.Tag.Length - 1) + "M";
											if (GLOBAL.HROM.OrginalCSVs.ContainsKey(_tStopBaseTag))
											{
												GLOBAL.HROM.CancelSingleOrder(GLOBAL.HROM.OrginalCSVs[_tStopBaseTag]);
											}
											break;
									}
								}

								#endregion
							}

							// Status message - executions
							CsvToExecution(csv);
							break;
						default:
							break;
					}
				}
			}
		}

		private void PlaySoundByStatus(long input)
		{
			long status = (long)input;

			switch (status)
			{
				case CSVEx.CSVFieldIDs.StatusCodes.Open:
				case CSVEx.CSVFieldIDs.StatusCodes.New:
					if (_useSoundAPI)
					{
						GLOBAL.ROCSounds.APIPlayOpen();
					}
					else
					{
						GLOBAL.ROCSounds.PlayOpen();
					}
					break;
				case CSVEx.CSVFieldIDs.StatusCodes.Replaced:
					if (_useSoundAPI)
					{
						GLOBAL.ROCSounds.APIPlayReplaced();
					}
					else
					{
						GLOBAL.ROCSounds.PlayReplaced();
					}
					break;
				case CSVEx.CSVFieldIDs.StatusCodes.Filled:
					if (_useSoundAPI)
					{
						GLOBAL.ROCSounds.APIPlayFilled();
					}
					else
					{
						GLOBAL.ROCSounds.PlayFilled();
					}
					break;
				case CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled:
					if (_useSoundAPI)
					{
						GLOBAL.ROCSounds.APIPlayPartiallyFilled();
					}
					else
					{
						GLOBAL.ROCSounds.PlayPartiallyFilled();
					}
					break;
				case CSVEx.CSVFieldIDs.StatusCodes.ReplacedAndFilled:
				case CSVEx.CSVFieldIDs.StatusCodes.FilledAndCancelled:
					if (_useSoundAPI)
					{
						GLOBAL.ROCSounds.APIPlayFilledAndCanceled();
					}
					else
					{
						GLOBAL.ROCSounds.PlayFilledAndCanceled();
					}
					break;
				case CSVEx.CSVFieldIDs.StatusCodes.Rejected:
				case CSVEx.CSVFieldIDs.StatusCodes.ReplaceRejected:
				case CSVEx.CSVFieldIDs.StatusCodes.CancelRejected:
					if (_useSoundAPI)
					{
						GLOBAL.ROCSounds.APIPlayRejected();
					}
					else
					{
						GLOBAL.ROCSounds.PlayRejected();
					}
					break;
				case CSVEx.CSVFieldIDs.StatusCodes.Canceled:
					if (_useSoundAPI)
					{
						GLOBAL.ROCSounds.APIPlayCanceled();
					}
					else
					{
						GLOBAL.ROCSounds.PlayCanceled();
					}
					break;
			}
		}

		private void CsvToOrder(CSV csv)
		{
			if (csv.UpdateOrder &&
				csv.Tag != null && 
				csv.Tag != "")
			{
				ROCOrder order = new ROCOrder();

				#region - Update Order Fields -

				#region - String -

				if (csv.Contains(CSVFieldIDs.CallPut))
				{
					order.Update(OrderFieldIDs.ROC.callPut, csv.CallPut);
				}
				if (csv.Contains(CSVFieldIDs.ClearingAcct))
				{
					order.Update(OrderFieldIDs.ROC.clearingAccount, csv.ClearingAcct);
				}
				if (csv.Contains(CSVFieldIDs.ClearingID))
				{
					order.Update(OrderFieldIDs.ROC.clearingID, csv.ClearingID);
				}
				if (csv.Contains(CSVFieldIDs.ExpDate))
				{
					order.Update(OrderFieldIDs.ROC.expDate, csv.ExpDate);
				}
				if (csv.Contains(CSVFieldIDs.MaturityDay))
				{
					order.Update(OrderFieldIDs.ROC.maturityDay, csv.MaturityDay);
				}
				if (csv.Contains(CSVFieldIDs.Firm))
				{
					order.Update(OrderFieldIDs.ROC.firm, csv.Firm);
				}
				if (csv.Contains(CSVFieldIDs.Instructions))
				{
					order.Update(OrderFieldIDs.ROC.instructions, csv.Instructions);
				}
				if (csv.Contains(CSVFieldIDs.ExecutionInstruction))
				{
					order.Update(OrderFieldIDs.ROC.execInstruction, csv.ExecutionInstruction);
				}
				if (csv.Contains(CSVFieldIDs.LocalAcct))
				{
					order.Update(OrderFieldIDs.ROC.localAcct, csv.LocalAcct);
				}
				if (csv.Contains(CSVFieldIDs.OmTag))
				{
					order.Update(OrderFieldIDs.ROC.omTag, csv.OmTag);
				}
				if (csv.Contains(CSVFieldIDs.OpenClose))
				{
					order.Update(OrderFieldIDs.ROC.openClose, csv.OpenClose);
				}
				if (csv.Contains(CSVFieldIDs.Owner))
				{
					order.Update(OrderFieldIDs.ROC.owner, csv.Owner);
				}
				if (csv.Contains(CSVFieldIDs.SecType))
				{
					order.Update(OrderFieldIDs.ROC.secType, csv.SecType);
				}
				if (csv.Contains(CSVFieldIDs.Symbol))
				{
					order.Update(OrderFieldIDs.ROC.symbol, csv.Symbol);
				}
				if (csv.Contains(CSVFieldIDs.Tag))
				{
					order.Update(OrderFieldIDs.ROC.tag, csv.Tag);
				}
				if (csv.Contains(CSVFieldIDs.Text))
				{
					order.Update(OrderFieldIDs.ROC.text, csv.Text);
				}
				if (csv.Contains(CSVFieldIDs.TradeFor))
				{
					order.Update(OrderFieldIDs.ROC.tradeFor, csv.TradeFor);
				}
				if (csv.Contains(CSVFieldIDs.Trader))
				{
					order.Update(OrderFieldIDs.ROC.trader, csv.Trader);
				}
				if (csv.Contains(CSVFieldIDs.Underlying))
				{
					order.Update(OrderFieldIDs.ROC.underlying, csv.Underlying);
				}
				if (csv.Contains(CSVFieldIDs.SecurityDefinition))
				{
					order.Update(OrderFieldIDs.ROC.securityDefinition, csv.SecurityDefinition);
				}
				if (csv.Contains(CSVFieldIDs.ParentTag))
				{
					order.Update(OrderFieldIDs.ROC.parentTag, csv.ParentTag);
				}
				if (csv.Contains(CSVFieldIDs.ClientEcho))
				{
					order.Update(OrderFieldIDs.ROC.clientEcho, csv.ClientEcho);
				}
				if (csv.Contains(CSVFieldIDs.ProgramTrade))
				{
					order.Update(OrderFieldIDs.ROC.programTrade, csv.ProgramTrade);
				}
				if (csv.Contains(CSVFieldIDs.CMTAAccount))
				{
					order.Update(OrderFieldIDs.ROC.cmtaFirmID, csv.CMTAAccount);
				}

				#endregion

				#region - Double -

				if (csv.Contains(CSVFieldIDs.AveragePrice))
				{
					order.Update(OrderFieldIDs.ROC.avgPrice, csv.AveragePrice);
				}
				if (csv.Contains(CSVFieldIDs.Price))
				{
					order.Update(OrderFieldIDs.ROC.price, csv.Price);
				}
				if (csv.Contains(CSVFieldIDs.OriginalPrice))
				{
					order.Update(OrderFieldIDs.ROC.originalPrice, csv.OriginalPrice);
				}
				if (csv.Contains(CSVFieldIDs.ExecPrice))
				{
					order.Update(OrderFieldIDs.ROC.execPrice, csv.ExecPrice);
				}
				if (csv.Contains(CSVFieldIDs.StopPrice))
				{
					order.Update(OrderFieldIDs.ROC.stopPrice, csv.StopPrice);
				}
				if (csv.Contains(CSVFieldIDs.PegOffsetPrice))
				{
					order.Update(OrderFieldIDs.ROC.pegPrice, csv.PegOffsetPrice);
				}
				if (csv.Contains(CSVFieldIDs.StrikePrice))
				{
					order.Update(OrderFieldIDs.ROC.strikePrice, csv.StrikePrice);
				}
				if (csv.Contains(CSVFieldIDs.ExecutionTime))
				{
					order.Update(OrderFieldIDs.ROC.omTime, csv.ExecutionTime.ToOADate());
				}
				else
				{
					order.Update(OrderFieldIDs.ROC.omTime, csv.OmTime.ToOADate());
				}
				if (csv.Contains(CSVFieldIDs.EndTime))
				{
					order.Update(OrderFieldIDs.ROC.endTime, csv.EndTime.ToOADate());
				}
				if (csv.Contains(CSVFieldIDs.OrderExpirationDateTime))
				{
					order.Update(OrderFieldIDs.ROC.orderExpiresDate, csv.OrderExpirationDateTime.ToOADate());
				}

				#endregion

				#region - Long -

				if (csv.Contains(CSVFieldIDs.CplxOrderType))
				{
					order.Update(OrderFieldIDs.ROC.cplxOrderType, csv.CplxOrderType);
				}
				if (csv.Contains(CSVFieldIDs.CumShares))
				{
					order.Update(OrderFieldIDs.ROC.cumQty, csv.CumShares);
				}
				if (csv.Contains(CSVFieldIDs.ExchangeID))
				{
					order.Update(OrderFieldIDs.ROC.destID, csv.ExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.AlgoExchangeID))
				{
					order.Update(OrderFieldIDs.ROC.algoDestID, csv.AlgoExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.Floor))
				{
					order.Update(OrderFieldIDs.ROC.maxFloor, csv.Floor);
				}
				if (csv.Contains(CSVFieldIDs.LeaveShares))
				{
					order.Update(OrderFieldIDs.ROC.leavesQty, csv.LeaveShares);
				}
				if (csv.Contains(CSVFieldIDs.OriginalShares))
				{
					order.Update(OrderFieldIDs.ROC.originalShares, csv.OriginalShares);
				}
				if (csv.Contains(CSVFieldIDs.Shares))
				{
					order.Update(OrderFieldIDs.ROC.qty, csv.Shares);
				}
				if (csv.Contains(CSVFieldIDs.Side))
				{
					order.Update(OrderFieldIDs.ROC.side, csv.Side);
				}
				if (csv.Contains(CSVFieldIDs.Status))
				{
					order.Update(OrderFieldIDs.ROC.status, csv.Status);
				}
				if (csv.Contains(CSVFieldIDs.TIF))
				{
					order.Update(OrderFieldIDs.ROC.tif, csv.TIF);
				}
				if (csv.Contains(CSVFieldIDs.Type))
				{
					order.Update(OrderFieldIDs.ROC.orderType, csv.Type);
				}
				if (csv.Contains(CSVFieldIDs.AlgoType))
				{
					order.Update(OrderFieldIDs.ROC.algoType, csv.AlgoType);
				}

				#endregion

				GLOBAL.HRDS.MakeRocStatus(ref order);

				#endregion

				if (GLOBAL.HROM.EndOfQueuedMsg)
				{
					AddToRomOrders(order, false);

					_newROCOrders.Add(order);
				}
				else
				{
					AddToRomOrders(order, true);
				}
			}
		}

		private void CsvToExecution(CSV csv)
		{
			CsvToOrder(csv);

			if (csv.UpdateTrade && 
				csv.OmExecTag != null && 
				csv.OmExecTag != "" &&
				(csv.Status == CSVEx.CSVFieldIDs.StatusCodes.Filled || csv.Status == CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled))
				//csv.CplxOrderType != CSVEx.CSVFieldIDs.CplxOrderTypes.Continer && (csv.Status == CSVEx.CSVFieldIDs.StatusCodes.Filled || csv.Status == CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled))
			{
				ROCExecution traded = new ROCExecution();

				#region - String -

				if (csv.Contains(CSVFieldIDs.OmTag))
				{
					traded.Update(TradedFieldIDs.ROC.omTag, csv.OmTag);
				}
				if (csv.Contains(CSVFieldIDs.OmExecTag))
				{
					traded.Update(TradedFieldIDs.ROC.omExecTag, csv.OmExecTag);
				}
				if (csv.Contains(CSVFieldIDs.ClearingAcct))
				{
					traded.Update(TradedFieldIDs.ROC.account, csv.ClearingAcct);
				}
				if (csv.Contains(CSVFieldIDs.Trader))
				{
					traded.Update(TradedFieldIDs.ROC.trader, csv.Trader);
				}
				if (csv.Contains(CSVFieldIDs.LocalAcct))
				{
					traded.Update(TradedFieldIDs.ROC.localAcct, csv.LocalAcct);
				}
				if (csv.Contains(CSVFieldIDs.Symbol))
				{
					traded.Update(TradedFieldIDs.ROC.symbol, csv.Symbol);
				}
				if (csv.Contains(CSVFieldIDs.ExpDate))
				{
					traded.Update(TradedFieldIDs.ROC.expDate, csv.ExpDate);
				}
				if (csv.Contains(CSVFieldIDs.MaturityDay))
				{
					traded.Update(TradedFieldIDs.ROC.maturityDay, csv.MaturityDay);
				}
				if (csv.Contains(CSVFieldIDs.CallPut))
				{
					traded.Update(TradedFieldIDs.ROC.callPut, csv.CallPut);
				}
				if (csv.Contains(CSVFieldIDs.Underlying))
				{
					traded.Update(TradedFieldIDs.ROC.underlying, csv.Underlying);
				}
				if (csv.Contains(CSVFieldIDs.SecType))
				{
					traded.Update(TradedFieldIDs.ROC.secType, csv.SecType);
				}
				if (csv.Contains(CSVFieldIDs.CplxOrderType))
				{
					traded.Update(TradedFieldIDs.ROC.cplxOrderType, csv.CplxOrderType);
				}
				if (csv.Contains(CSVFieldIDs.ProgramTrade))
				{
					traded.Update(TradedFieldIDs.ROC.programTrade, csv.ProgramTrade);
				}

				#endregion

				#region - Double -

				if (csv.Contains(CSVFieldIDs.ExecPrice))
				{
					traded.Update(TradedFieldIDs.ROC.execPrice, csv.ExecPrice);
				}
				if (csv.Contains(CSVFieldIDs.StrikePrice))
				{
					traded.Update(TradedFieldIDs.ROC.strikePrice, csv.StrikePrice);
				}
				if (csv.Contains(CSVFieldIDs.ExecutionTime))
				{
					traded.Update(TradedFieldIDs.ROC.execTime, csv.ExecutionTime.ToOADate());
				}

				#endregion

				#region - Long -

				if (csv.Contains(CSVFieldIDs.LastShares))
				{
					traded.Update(TradedFieldIDs.ROC.execQty, csv.LastShares);
				}
				if (csv.Contains(CSVFieldIDs.ExchangeID))
				{
					traded.Update(TradedFieldIDs.ROC.destID, csv.ExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.AlgoExchangeID))
				{
					traded.Update(TradedFieldIDs.ROC.algoDestID, csv.AlgoExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.Side))
				{
					traded.Update(TradedFieldIDs.ROC.side, csv.Side);
				}
				if (csv.Contains(CSVFieldIDs.CplxOrderType))
				{
					traded.Update(TradedFieldIDs.ROC.cplxOrderType, csv.CplxOrderType);
				}

				#endregion

				#region - Not Used -

				#region - Strings -

				//foreach (int key in csv.Strings.Keys)
				//{
				//    switch (key)
				//    {
				//        case CSVFieldIDs.OmTag:
				//            traded.Update(TradedFieldIDs.ROC.omTag, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.OmExecTag:
				//            traded.Update(TradedFieldIDs.ROC.omExecTag, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.ClearingAcct:
				//            traded.Update(TradedFieldIDs.ROC.account, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.Trader:
				//            traded.Update(TradedFieldIDs.ROC.trader, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.LocalAcct:
				//            traded.Update(TradedFieldIDs.ROC.localAcct, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.Symbol:
				//            traded.Update(TradedFieldIDs.ROC.symbol, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.ExpDate:
				//            traded.Update(TradedFieldIDs.ROC.expDate, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.MaturityDay:
				//            traded.Update(TradedFieldIDs.ROC.maturityDay, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.CallPut:
				//            traded.Update(TradedFieldIDs.ROC.callPut, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.Underlying:
				//            traded.Update(TradedFieldIDs.ROC.underlying, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.SecType:
				//            traded.Update(TradedFieldIDs.ROC.secType, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.CplxOrderType:
				//            traded.Update(TradedFieldIDs.ROC.cplxOrderType, csv.Strings[key]);
				//            break;
				//        case CSVFieldIDs.ProgramTrade:
				//            traded.Update(TradedFieldIDs.ROC.programTrade, csv.Strings[key]);
				//            break;
				//    }
				//}

				#endregion

				#region - Doubles -

				//foreach (int key in csv.Doubles.Keys)
				//{
				//    switch (key)
				//    {
				//        case CSVFieldIDs.ExecPrice:
				//            traded.Update(TradedFieldIDs.ROC.execPrice, csv.Doubles[key]);
				//            break;
				//        case CSVFieldIDs.StrikePrice:
				//            traded.Update(TradedFieldIDs.ROC.strikePrice, csv.Doubles[key]);
				//            break;

				//        case CSVFieldIDs.ExecutionTime:
				//            traded.Update(TradedFieldIDs.ROC.execTime, csv.Doubles[key]);
				//            break;
				//    }
				//}

				#endregion

				#region - Longs -

				//foreach (int key in csv.Longs.Keys)
				//{
				//    switch (key)
				//    {
				//        case CSVFieldIDs.LastShares:
				//            traded.Update(TradedFieldIDs.ROC.execQty, csv.Longs[key]);
				//            break;
				//        case CSVFieldIDs.ExchangeID:
				//            traded.Update(TradedFieldIDs.ROC.destID, csv.Longs[key]);
				//            break;
				//        case CSVFieldIDs.AlgoExchangeID:
				//            traded.Update(TradedFieldIDs.ROC.algoDestID, csv.Longs[key]);
				//            break;
				//        case CSVFieldIDs.Side:
				//            traded.Update(TradedFieldIDs.ROC.side, csv.Longs[key]);
				//            break;
				//        case CSVFieldIDs.CplxOrderType:
				//            traded.Update(TradedFieldIDs.ROC.cplxOrderType, csv.Longs[key]);
				//            break;
				//    }
				//}

				#endregion

				#endregion

				if (GLOBAL.HROM.EndOfQueuedMsg)
				{
					// Do this after ther preprocessing is done, so that its in sync to tops trades
					AddToRomExecutions(traded, false);

					// New CSV
					_newROCExecutions.Add(traded);
				}
				else
				{
					AddToRomExecutions(traded, true);
				}
			}
		}

		#endregion

		#region - Processing Executions -

		private void ProcessTposExecutions(List<TPOSExecution> tposExecutions)
		{
			foreach (TPOSExecution tposExecution in tposExecutions)
			{
				AddToTposExecutions(tposExecution);
			}
		}

		#endregion

		#region - Orders and Executions Support -

		private void AddToRomOrders(ROCOrder order, bool addToNew)
		{
			if (order != null) //&& order.OmTag != null && order.OmTag != "")
			{
				lock (GLOBAL.HOrders.Table)
				{
					order = GLOBAL.HOrders.Update(order);
				}

				GLOBAL.HROM.Update(order, addToNew);
			}
		}

		private void AddToRomExecutions(ROCExecution trade, bool addToNew)
		{
			if (trade != null && trade.OmExecTag != null && trade.OmExecTag != "")
			{
				GLOBAL.HROM.Update(trade, addToNew);

				lock (GLOBAL.HExecutions.Table)
				{
					GLOBAL.HExecutions.Update(trade);
					GLOBAL.HPositions.Update(trade);
				}
			}
		}

		private void AddToTposExecutions(TPOSExecution trade)
		{
			if (trade != null && trade.TradeID != null && trade.TradeID != "" && trade.ModReasonID != 3)
			{
				lock (GLOBAL.HExecutions.Table)
				{
					GLOBAL.HExecutions.Update(trade);
					GLOBAL.HPositions.Update(trade);
				}
			}
		}

		#endregion

		#endregion

		#region - Sound Thread -

		private void ProcessSound()
		{
			while (!_threadProcessSoundShouldStop)
			{
				long[] sounds = new long[0];
				lock (_newROCStatusToPlay)
				{
					if (_newROCStatusToPlay.Count > 0)
					{
						sounds = _newROCStatusToPlay.ToArray();
						_newROCStatusToPlay.Clear();
					}
				}

				if (sounds.Length > 0)
				{
					foreach (long status in sounds)
					{
						PlaySoundByStatus(status);
					}
				}

				System.Threading.Thread.Sleep(1);
			}

			_threadProcessSoundStarted = false;
		}

		#endregion

		#region - Process WatchList Windows -

		public void ProcessWatchListWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.WatchListWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.WatchListWindows.Count];
					GLOBAL.HWindows.WatchListWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.WatchListWindows.ContainsKey(key))
					{
						if (!GLOBAL.HWindows.WatchListWindows[key].IsProcessing)
						{
							try
							{
								GLOBAL.HWindows.WatchListWindows[key].UpdateWatchListByProcess(loadIM, deltas);
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		#endregion

		#region - Process PlotList Windows -

		public void ProcessPlotListWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.PlotListWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.PlotListWindows.Count];
					GLOBAL.HWindows.PlotListWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.PlotListWindows.ContainsKey(key))
					{
						if (!GLOBAL.HWindows.PlotListWindows[key].IsProcessing)
						{
							try
							{
								GLOBAL.HWindows.PlotListWindows[key].UpdatePlotListByProcess(loadIM, deltas);
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		#endregion

		#region - Process Stock Tickets Windows -

		public void ProcessStockTicketWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessStockTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessStockTicketWindows(List<ROCOrder> orders)
		{
			ProcessStockTicketWindows(false, orders, new Dictionary<string, MDServerToClient>());
		} 
		public void ProcessStockTicketWindows(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.StockTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.StockTicketWindows.Count];
					GLOBAL.HWindows.StockTicketWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.StockTicketWindows.ContainsKey(key))
					{
						try
						{
							GLOBAL.HWindows.StockTicketWindows[key].UpdateTicketByProcess(loadIM, orders, deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		#endregion

		#region - Process Option Tickets Window -

		public void ProcessOptionTicketWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessOptionTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessOptionTicketWindows(List<ROCOrder> orders)
		{
			ProcessOptionTicketWindows(false, orders, new Dictionary<string, MDServerToClient>());
		} 
		public void ProcessOptionTicketWindows(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				List<string> symbolList = new List<string>();
				//if (deltas.Count > 0)
				//{
				//    string[] optionSymbols = new string[deltas.Count];
				//    deltas.Keys.CopyTo(optionSymbols, 0);
				//    symbolList.AddRange(optionSymbols);
				//    symbolList.Sort();
				//}

				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.OptionTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.OptionTicketWindows.Count];
					GLOBAL.HWindows.OptionTicketWindows.Keys.CopyTo(keys, 0);
				}

				bool isProcessing = false;
				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.OptionTicketWindows.ContainsKey(key))
					{
						try
						{
							if (!GLOBAL.HWindows.OptionTicketWindows[key].IsProcessing)
							{
								GLOBAL.HWindows.OptionTicketWindows[key].UpdateTicketByProcess(loadIM, orders, deltas, symbolList);
							}
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}
				_optionTicketIsBusy = isProcessing;

				keys = new IntPtr[0];
			}
		}

		private bool _optionTicketIsBusy = false;
		public bool OptionTicketIsBusy
		{
			get
			{
				return _optionTicketIsBusy;
			}
		}

		#endregion

		#region - Process Future Tickets Window -

		public void ProcessFutureTicketWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessFutureTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessFutureTicketWindows(List<ROCOrder> orders)
		{
			ProcessFutureTicketWindows(false, orders, new Dictionary<string, MDServerToClient>());
		} 
		public void ProcessFutureTicketWindows(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.FutureTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.FutureTicketWindows.Count];
					GLOBAL.HWindows.FutureTicketWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.FutureTicketWindows.ContainsKey(key))
					{
						try
						{
							GLOBAL.HWindows.FutureTicketWindows[key].UpdateTicketByProcess(loadIM, orders, deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		#endregion

		#region - Process Quick Tickets Windows -

		public void ProcessQuickTicketWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessQuickTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessQuickTicketWindows(List<ROCOrder> orders)
		{
			ProcessQuickTicketWindows(false, orders, new Dictionary<string, MDServerToClient>());
		}
		public void ProcessQuickTicketWindows(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.QuickTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.QuickTicketWindows.Count];
					GLOBAL.HWindows.QuickTicketWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.QuickTicketWindows.ContainsKey(key))
					{
						try
						{
							GLOBAL.HWindows.QuickTicketWindows[key].UpdateTicketByProcess(loadIM, orders.ToArray(), deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		#endregion

		#region - Process Auto Spread Tickets Windows -

		public void ProcessAutoSpreadTicketWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessAutoSpreadTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessAutoSpreadTicketWindows(List<ROCOrder> orders)
		{
			ProcessAutoSpreadTicketWindows(false, orders, new Dictionary<string, MDServerToClient>());
		}
		public void ProcessAutoSpreadTicketWindows(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.AutoSpreadTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.AutoSpreadTicketWindows.Count];
					GLOBAL.HWindows.AutoSpreadTicketWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.AutoSpreadTicketWindows.ContainsKey(key))
					{
						try
						{
							GLOBAL.HWindows.AutoSpreadTicketWindows[key].UpdateTicketByProcess(loadIM, orders.ToArray(), deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		#endregion

		#region - Process Future Matrix Tickets Window -

		public void ProcessFutureMatrixTicketWindow(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessFutureMatrixTicketWindow(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessFutureMatrixTicketWindow(List<ROCOrder> orders)
		{
			ProcessFutureMatrixTicketWindow(false, orders, new Dictionary<string, MDServerToClient>());
		}
		public void ProcessFutureMatrixTicketWindow(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.FutureMatrixTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.FutureMatrixTicketWindows.Count];
					GLOBAL.HWindows.FutureMatrixTicketWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.FutureMatrixTicketWindows.ContainsKey(key))
					{
						try
						{
							GLOBAL.HWindows.FutureMatrixTicketWindows[key].UpdateTicketByProcess(loadIM, orders, deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		#endregion

		#region - Process Batch Windows -

		public void ProcessBatchTicketWindows(List<ROCOrder> orders)
		{
			if (orders.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.BatchTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.BatchTicketWindows.Count];
					GLOBAL.HWindows.BatchTicketWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.BatchTicketWindows.ContainsKey(key))
					{
						if (!GLOBAL.HWindows.BatchTicketWindows[key].IsProcessing)
						{
							try
							{
								GLOBAL.HWindows.BatchTicketWindows[key].LoadRocOrders(orders);
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}
						}
					}
				}

				keys = null;
			}
		}

		#endregion

		#region - Process Market Batch Windows -

		public void ProcessBatchMarketTicketWindows(List<ROCOrder> orders)
		{
			ProcessBatchMarketTicketWindows(false, orders, new Dictionary<string, MDServerToClient>());
		}
		public void ProcessBatchMarketTicketWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessBatchMarketTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}		
		public void ProcessBatchMarketTicketWindows(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.BatchMarketTicketWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.BatchMarketTicketWindows.Count];
					GLOBAL.HWindows.BatchMarketTicketWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.BatchMarketTicketWindows.ContainsKey(key))
					{
						if (!GLOBAL.HWindows.BatchMarketTicketWindows[key].IsProcessing)
						{
							try
							{
								GLOBAL.HWindows.BatchMarketTicketWindows[key].UpdateBatchMarketByProcess(loadIM, orders.ToArray(), deltas);
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}
						}
					}
				}

				keys = null;
			}

		}

		#endregion

		#region - Process Order Windows -

		public void ProcessOrderWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessOrderWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessOrderWindows(List<ROCOrder> orders)
		{
			ProcessOrderWindows(false, orders, new Dictionary<string, MDServerToClient>());
		}
		public void ProcessOrderWindows(bool loadIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || orders.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.OrderWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.OrderWindows.Count];
					GLOBAL.HWindows.OrderWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.OrderWindows.ContainsKey(key))
					{
						if (!GLOBAL.HWindows.OrderWindows[key].IsProcessing)
						{
							try
							{
								GLOBAL.HWindows.OrderWindows[key].AddUpdateOrdersByProcess(loadIM, orders.ToArray(), deltas);
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}
						}
					}
				}

				keys = null;
			}
		}

		#endregion

		#region - Process Trade Windows -

		public void ProcessTradeWindows(bool loadIM)
		{
			ProcessTradeWindows(loadIM, new List<ROCExecution>(), new List<TPOSExecution>());
		}
		public void ProcessTradeWindows(List<ROCExecution> rocExecutions, List<TPOSExecution> tposExecutions)
		{
			ProcessTradeWindows(false, rocExecutions, tposExecutions);
		}
		public void ProcessTradeWindows(bool loadIM, List<ROCExecution> rocExecutions, List<TPOSExecution> tposExecutions)
		{
			if (loadIM || rocExecutions.Count > 0 || tposExecutions.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.TradeWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.TradeWindows.Count];
					GLOBAL.HWindows.TradeWindows.Keys.CopyTo(keys, 0);
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.TradeWindows.ContainsKey(key))
					{
						if (!GLOBAL.HWindows.TradeWindows[key].IsProcessing)
						{
							try
							{
								GLOBAL.HWindows.TradeWindows[key].AddUpdateTradesByProcess(loadIM, rocExecutions.ToArray(), tposExecutions.ToArray());
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}
						}
					}
				}

				keys = null;
			}
		}

		#endregion

		#region - Process Position Windows -

		public void ProcessPositionWindows(bool loadIM, Dictionary<string, MDServerToClient> deltas)
		{
			ProcessPositionWindows(loadIM, new List<ROCExecution>(), new List<TPOSExecution>(), deltas);
		}
		public void ProcessPositionWindows(List<ROCExecution> rocExecutions, List<TPOSExecution> tposExecutions)
		{
			ProcessPositionWindows(false, rocExecutions, tposExecutions, new Dictionary<string, MDServerToClient>());
		}
		public void ProcessPositionWindows(bool loadIM, List<ROCExecution> rocExecutions, List<TPOSExecution> tposExecutions, Dictionary<string, MDServerToClient> deltas)
		{
			if (loadIM || rocExecutions.Count > 0 || tposExecutions.Count > 0 || deltas.Count > 0)
			{
				IntPtr[] keys = new IntPtr[0];
				lock (GLOBAL.HWindows.PositionWindows)
				{
					keys = new IntPtr[GLOBAL.HWindows.PositionWindows.Count];
					GLOBAL.HWindows.PositionWindows.Keys.CopyTo(keys, 0);
				}

				List<TPOSPosition> newROCPositions = new List<TPOSPosition>();
				List<TPOSPosition> newTPOSPositions = new List<TPOSPosition>();

				if (keys.Length > 0)
				{
					foreach (ROCExecution rocExecution in rocExecutions)
					{
						TPOSPosition position = new TPOSPosition();
						ConvertRocExecutionToPosition(rocExecution, ref position);
						newROCPositions.Add(position);
					}

					foreach (TPOSExecution tposExecution in tposExecutions)
					{
						TPOSPosition position = new TPOSPosition();
						ConvertTposExecutionToPosition(tposExecution, ref position);
						newTPOSPositions.Add(position);
					}
				}

				foreach (IntPtr key in keys)
				{
					if (_stopping) return;

					if (GLOBAL.HWindows.PositionWindows.ContainsKey(key))
					{
						if (!GLOBAL.HWindows.PositionWindows[key].IsProcessing)
						{
							try
							{
								GLOBAL.HWindows.PositionWindows[key].AddUpdatePositionsByProcess(loadIM, newROCPositions, newTPOSPositions, deltas);
							}
							catch (Exception ex)
							{
								GLOBAL.HROC.AddToException(ex);
							}
						}
					}
				}

				keys = new IntPtr[0];
			}
		}

		private void ConvertRocExecutionToPosition(ROCExecution rocExecution, ref TPOSPosition position)
		{
			position.Symbol = rocExecution.Symbol;
			position.Underlying = rocExecution.Underlying;
			position.ExpDate = rocExecution.ExpDate;
			position.StrikePrice = rocExecution.StrikePrice;
			position.CallPut = rocExecution.CallPut;
			position.SecType = rocExecution.SecType;
			if (rocExecution.Side == CSVEx.CSVFieldIDs.SideCodes.Buy)
			{
				position.BuyAvg = rocExecution.Price;
				position.BuyQty = rocExecution.Qty;
			}
			else
			{
				position.SellAvg = rocExecution.Price;
				position.SellQty = rocExecution.Qty;
			}
			if (rocExecution.ClearingAcct.Length > 5)
			{
				position.ClearingAcct = rocExecution.ClearingAcct.Substring(0, 5);
			}
			else
			{
				position.ClearingAcct = rocExecution.ClearingAcct;
			}
			position.Trader = rocExecution.LocalAcct;
		}

		private void ConvertTposExecutionToPosition(TPOSExecution tposExecution, ref TPOSPosition position)
		{
			if (tposExecution.ModReasonID != 3)
			{
				position.Symbol = tposExecution.Symbol;
				position.Underlying = tposExecution.Underlying;
				position.ExpDate = tposExecution.ExpDate;
				position.StrikePrice = tposExecution.StrikePrice;
				position.CallPut = tposExecution.CallPut;
				position.SecType = tposExecution.SecType;
				if (tposExecution.Side == CSVEx.CSVFieldIDs.SideCodes.Buy)
				{
					position.BuyAvg = tposExecution.Price;
					position.BuyQty = tposExecution.Qty;
				}
				else
				{
					position.SellAvg = tposExecution.Price;
					position.SellQty = tposExecution.Qty;
				}
				if (tposExecution.ClearingAcct.Length > 5)
				{
					position.ClearingAcct = tposExecution.ClearingAcct.Substring(0, 5);
				}
				else
				{
					position.ClearingAcct = tposExecution.ClearingAcct;
				}
				position.Trader = tposExecution.Trader;
			}
		}

		#endregion
	}
}
