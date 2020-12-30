using System;
using System.Collections.Generic;

using CSVEx;
using RDSEx;
using DateTimeEx;
using MarketData;

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
		private List<Market> _pendingMarketDeltas = new List<Market>();
		
		// Consolidation
		private Market _mergedMarketDelta = new Market();

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

					if (!_mergedMarketDelta.Empty || _loadIMMarketData)
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

					Market mergedCopy = null;

					#region - Process IM -

					// Process IM
					if (_loadIMMarketData)
					{
						try
						{
							// Update Tickets
							GLOBAL.HProcess.ProcessAutoSpreadTicketWindows(_loadIMMarketData, mergedCopy);
							GLOBAL.HProcess.ProcessQuickTicketWindows(_loadIMMarketData, mergedCopy);
							GLOBAL.HProcess.ProcessBatchMarketTicketWindows(_loadIMMarketData, mergedCopy);
							GLOBAL.HProcess.ProcessFutureTicketWindows(_loadIMMarketData, mergedCopy);
							GLOBAL.HProcess.ProcessOptionTicketWindows(_loadIMMarketData, mergedCopy);
							GLOBAL.HProcess.ProcessStockTicketWindows(_loadIMMarketData, mergedCopy);
							GLOBAL.HProcess.ProcessFutureMatrixTicketWindow(_loadIMMarketData, mergedCopy);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}

						//ProcessTposExecutions(_newTPOSExecutions);
						GLOBAL.HProcess.ProcessOrderWindows(_loadIMMarketData, mergedCopy);
						GLOBAL.HProcess.ProcessTradeWindows(_loadIMMarketData);
						GLOBAL.HProcess.ProcessPositionWindows(_loadIMMarketData, mergedCopy);

						// Update Market Datas
						GLOBAL.HProcess.ProcessWatchListWindows(_loadIMMarketData, mergedCopy);
						GLOBAL.HProcess.ProcessPlotListWindows(_loadIMMarketData, mergedCopy);

						_loadIMMarketData = false;
					}

					#endregion

					lock (_mergedMarketDelta)
					{
						if (!_mergedMarketDelta.Empty) {
							mergedCopy = Market.Replace(_mergedMarketDelta);
							//System.Threading.Thread.Sleep(1);
						}
					}

					if (mergedCopy != null)
					{
						Market tempMarketdataDelta = new Market();
						int count = 0;

						foreach ((string symbol, Book delta) in mergedCopy) {
							if (count > _newMarketDataProcessLimit)
							{
								// Process Data
								if (!tempMarketdataDelta.Empty)
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
								if (!tempMarketdataDelta.TryGet(symbol, out Book _))
									tempMarketdataDelta.Update(symbol, delta);
							}

							count++;
						}

						if (!tempMarketdataDelta.Empty)
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
						mergedCopy.Clear();
					}

					#endregion
				}
				else
				{
					#region - No Limit Processing -

					try
					{
						// Update Tickets
						GLOBAL.HProcess.ProcessAutoSpreadTicketWindows(_loadIMMarketData, _mergedMarketDelta);
						GLOBAL.HProcess.ProcessQuickTicketWindows(_loadIMMarketData, _mergedMarketDelta);
						GLOBAL.HProcess.ProcessBatchMarketTicketWindows(_loadIMMarketData, _mergedMarketDelta);
						GLOBAL.HProcess.ProcessFutureTicketWindows(_loadIMMarketData, _mergedMarketDelta);
						GLOBAL.HProcess.ProcessOptionTicketWindows(_loadIMMarketData, _mergedMarketDelta);
						GLOBAL.HProcess.ProcessStockTicketWindows(_loadIMMarketData, _mergedMarketDelta);
						GLOBAL.HProcess.ProcessFutureMatrixTicketWindow(_loadIMMarketData, _mergedMarketDelta);
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}

					//ProcessTposExecutions(_newTPOSExecutions);
					GLOBAL.HProcess.ProcessOrderWindows(_loadIMMarketData, _mergedMarketDelta);
					GLOBAL.HProcess.ProcessTradeWindows(_loadIMMarketData);
					GLOBAL.HProcess.ProcessPositionWindows(_loadIMMarketData, _mergedMarketDelta);

					// Update Market Datas
					GLOBAL.HProcess.ProcessWatchListWindows(_loadIMMarketData, _mergedMarketDelta);
					GLOBAL.HProcess.ProcessPlotListWindows(_loadIMMarketData, _mergedMarketDelta);

					_loadIMMarketData = false;

					#endregion
				}

				_pendingMarketDeltas.Clear();
				_mergedMarketDelta.Clear();
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
			_pendingMarketDeltas = new List<Market>();
			_mergedMarketDelta = new Market();

			// Process Orders First
			if (_newCSVs.Length == 0)
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					Market deltas = mds.GetMarketDeltas();
					if (deltas != null)
					{
						// Make a copy of all the latest delta from different servers
						_pendingMarketDeltas.Add(deltas);
					}
				}

				// Consolidate MarketDataDelta into one
				DateTime updateTime = _dtHP.Now;
				foreach (var marketDelta in _pendingMarketDeltas)
				{
					foreach ((string symbol, Book delta) in marketDelta) {
						if (delta != null)
							delta.SetField(Book.FieldEnum.uClientRecivedTime, updateTime);
					}
					_mergedMarketDelta.Merge(marketDelta);
				}
				_pendingMarketDeltas.Clear();

				// DEBUG
				//if (_newMarketDataDelta.Count > 100)
				//{
				//    GLOBAL.HROC.AddToStatusLogs("_newMarketDataDelta count = " + _newMarketDataDelta.Count.ToString());
				//}

				GLOBAL.HMarketData.Current.Merge(_mergedMarketDelta);

				if (_currentReplayList.Count > 0)
				{
					lock (_currentReplayList)
					{
						string[] replaySymbols = _currentReplayList.ToArray();
						foreach (string symbol in replaySymbols)
						{
							if (GLOBAL.HMarketData.Current.TryGet(symbol, out Book book)) {
								_mergedMarketDelta.Update(symbol, book);
								// remove from replay list
								_currentReplayList.Remove(symbol);
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
							if (GLOBAL.HROM.OrginalCSVs.TryGetValue(csv.Tag, out CSV found))
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

								if (csv.CMTAAccount == null || csv.CMTAAccount != found.CMTAAccount)
								{
									csv.CMTAAccount = found.CMTAAccount;
								}
								if (csv.ClearingAcct == null || csv.ClearingAcct != found.ClearingAcct)
								{
									csv.ClearingAcct = found.ClearingAcct;
								}
								if (csv.TradeFor == null || csv.TradeFor != found.TradeFor)
								{
									csv.TradeFor = found.TradeFor;
								}
								if (csv.Trader == null || csv.Trader != found.Trader)
								{
									csv.Trader = found.Trader;
								}
								if (csv.LocalAcct == null || csv.LocalAcct != found.LocalAcct)
								{
									csv.LocalAcct = found.LocalAcct;
								}
								if (csv.SecType == null || csv.SecType != found.SecType)
								{
									csv.SecType = found.SecType;
								}
								if (csv.SecurityDefinition == null || csv.SecurityDefinition != found.SecurityDefinition)
								{
									csv.SecurityDefinition = found.SecurityDefinition;
								}
								if (found.CplxOrderType != CSVFieldIDs.CplxOrderTypes.Container)
								{
									if (csv.Side != found.Side)
									{
										csv.Side = found.Side;
									}
								}
								if (found.MaturityDay != null && csv.MaturityDay != found.MaturityDay)
								{
									csv.MaturityDay = found.MaturityDay;
								}
								if (csv.TIF == -1)
								{
									csv.TIF = found.TIF;
								}

								#region - Imbalanced Only -

								if (found.DisplayInstruction != null && found.DisplayInstruction != "")
								{
									if (csv.DisplayInstruction != found.DisplayInstruction)
									{
										csv.DisplayInstruction = found.DisplayInstruction;
									}
									switch (csv.DisplayInstruction)
									{
										case "I":
											if (found.Instructions.Contains("-O"))
											{
												csv.Type = CSVFieldIDs.OrderTypes.IMBOO;
											}
											else if (found.Instructions.Contains("-C"))
											{
												csv.Type = CSVFieldIDs.OrderTypes.IMBOC;
											}
											else if (found.Instructions.Contains("-I"))
											{
												csv.Type = CSVFieldIDs.OrderTypes.IMBOI;
											}
											break;
									}
								}

								#endregion

								#region - VWAP Order Type -

								if (found.AlgoType != 0)
								{
									if (csv.AlgoType != found.AlgoType)
									{
										csv.AlgoType = found.AlgoType;
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

								if (found.EndTime != null && csv.EndTime != found.EndTime)
								{
									csv.EndTime = found.EndTime;
								}

								#region - Status -

								switch (csv.Status)
								{
									case CSVEx.CSVFieldIDs.StatusCodes.Filled:
									case CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled:
										if (csv.Price == 0 && found.Price != 0)
										{
											csv.Price = found.Price;
										}
										else if (csv.Price != found.Price)
										{
											csv.OriginalPrice = found.Price;
										}
										break;
									case CSVEx.CSVFieldIDs.StatusCodes.ReplaceRejected:
										if (csv.Price != found.Price)
										{
											csv.Price = found.Price;
										}
										break;
									case CSVEx.CSVFieldIDs.StatusCodes.Replaced:
										csv.OriginalPrice = found.Price;
										found.Price = csv.Price;
										break;
									case CSVEx.CSVFieldIDs.StatusCodes.Rejected:
										if (csv.Shares == 0 && csv.Shares != found.Shares)
										{
											csv.Shares = found.Shares;
										}
										//if (csv.TIF == 0 && csv.TIF != found.TIF)
										//{
										//    csv.TIF = found.TIF;
										//}
										if (csv.Price == 0 && found.Price != 0)
										{
											csv.Price = found.Price;
										}
										else if (csv.Price != found.Price)
										{
											csv.OriginalPrice = found.Price;
										}
										break;
									default:
										if (csv.Price == 0 && found.Price != 0)
										{
											csv.Price = found.Price;
										}
										else if (csv.Price != found.Price)
										{
											csv.OriginalPrice = found.Price;
										}
										break;
								}

								#endregion

								#region - Option Back Fill -

								if (csv.Underlying != found.Underlying)
								{
									csv.Underlying = found.Underlying;
								}
								if (csv.StrikePrice != found.StrikePrice)
								{
									csv.StrikePrice = found.StrikePrice;
								}
								if (csv.ExpDate != found.ExpDate)
								{
									csv.ExpDate = found.ExpDate;
								}
								if (csv.CallPut != found.CallPut)
								{
									csv.CallPut = found.CallPut;
								}

								#endregion

								if (csv.ParentTag != found.ParentTag)
								{
									csv.ParentTag = found.ParentTag;
								}
								if (csv.ClientEcho != found.ClientEcho)
								{
									csv.ClientEcho = found.ClientEcho;
								}

								#endregion

								#region - OCO Processing -

								if (csv.Status == CSVEx.CSVFieldIDs.StatusCodes.Filled ||
									csv.Status == CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled ||
									csv.Status == CSVEx.CSVFieldIDs.StatusCodes.Rejected)
								{
									_tStopTag = csv.Tag.Substring(csv.Tag.Length - 1, 1);
									string baseSuffix = null;

									switch (_tStopTag) {
										case "M": baseSuffix = "S"; break;
										case "S": baseSuffix = "M"; break;
									}

									if (baseSuffix != null) {
										_tStopBaseTag = csv.Tag.Substring(0, csv.Tag.Length - 1) + baseSuffix;
										if (GLOBAL.HROM.OrginalCSVs.TryGetValue(_tStopBaseTag, out CSV original))
											GLOBAL.HROM.CancelSingleOrder(original);
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
					order.Set(OrderFieldIDs.ROC.callPut, csv.CallPut);
				}
				if (csv.Contains(CSVFieldIDs.ClearingAcct))
				{
					order.Set(OrderFieldIDs.ROC.clearingAccount, csv.ClearingAcct);
				}
				if (csv.Contains(CSVFieldIDs.ClearingID))
				{
					order.Set(OrderFieldIDs.ROC.clearingID, csv.ClearingID);
				}
				if (csv.Contains(CSVFieldIDs.ExpDate))
				{
					order.Set(OrderFieldIDs.ROC.expDate, csv.ExpDate);
				}
				if (csv.Contains(CSVFieldIDs.MaturityDay))
				{
					order.Set(OrderFieldIDs.ROC.maturityDay, csv.MaturityDay);
				}
				if (csv.Contains(CSVFieldIDs.Firm))
				{
					order.Set(OrderFieldIDs.ROC.firm, csv.Firm);
				}
				if (csv.Contains(CSVFieldIDs.Instructions))
				{
					order.Set(OrderFieldIDs.ROC.instructions, csv.Instructions);
				}
				if (csv.Contains(CSVFieldIDs.ExecutionInstruction))
				{
					order.Set(OrderFieldIDs.ROC.execInstruction, csv.ExecutionInstruction);
				}
				if (csv.Contains(CSVFieldIDs.LocalAcct))
				{
					order.Set(OrderFieldIDs.ROC.localAcct, csv.LocalAcct);
				}
				if (csv.Contains(CSVFieldIDs.OmTag))
				{
					order.Set(OrderFieldIDs.ROC.omTag, csv.OmTag);
				}
				if (csv.Contains(CSVFieldIDs.OpenClose))
				{
					order.Set(OrderFieldIDs.ROC.openClose, csv.OpenClose);
				}
				if (csv.Contains(CSVFieldIDs.Owner))
				{
					order.Set(OrderFieldIDs.ROC.owner, csv.Owner);
				}
				if (csv.Contains(CSVFieldIDs.SecType))
				{
					order.Set(OrderFieldIDs.ROC.secType, csv.SecType);
				}
				if (csv.Contains(CSVFieldIDs.Symbol))
				{
					order.Set(OrderFieldIDs.ROC.symbol, csv.Symbol);
				}
				if (csv.Contains(CSVFieldIDs.Tag))
				{
					order.Set(OrderFieldIDs.ROC.tag, csv.Tag);
				}
				if (csv.Contains(CSVFieldIDs.Text))
				{
					order.Set(OrderFieldIDs.ROC.text, csv.Text);
				}
				if (csv.Contains(CSVFieldIDs.TradeFor))
				{
					order.Set(OrderFieldIDs.ROC.tradeFor, csv.TradeFor);
				}
				if (csv.Contains(CSVFieldIDs.Trader))
				{
					order.Set(OrderFieldIDs.ROC.trader, csv.Trader);
				}
				if (csv.Contains(CSVFieldIDs.Underlying))
				{
					order.Set(OrderFieldIDs.ROC.underlying, csv.Underlying);
				}
				if (csv.Contains(CSVFieldIDs.SecurityDefinition))
				{
					order.Set(OrderFieldIDs.ROC.securityDefinition, csv.SecurityDefinition);
				}
				if (csv.Contains(CSVFieldIDs.ParentTag))
				{
					order.Set(OrderFieldIDs.ROC.parentTag, csv.ParentTag);
				}
				if (csv.Contains(CSVFieldIDs.ClientEcho))
				{
					order.Set(OrderFieldIDs.ROC.clientEcho, csv.ClientEcho);
				}
				if (csv.Contains(CSVFieldIDs.ProgramTrade))
				{
					order.Set(OrderFieldIDs.ROC.programTrade, csv.ProgramTrade);
				}
				if (csv.Contains(CSVFieldIDs.CMTAAccount))
				{
					order.Set(OrderFieldIDs.ROC.cmtaFirmID, csv.CMTAAccount);
				}

				#endregion

				#region - Double -

				if (csv.Contains(CSVFieldIDs.AveragePrice))
				{
					order.Set(OrderFieldIDs.ROC.avgPrice, csv.AveragePrice);
				}
				if (csv.Contains(CSVFieldIDs.Price))
				{
					order.Set(OrderFieldIDs.ROC.price, csv.Price);
				}
				if (csv.Contains(CSVFieldIDs.OriginalPrice))
				{
					order.Set(OrderFieldIDs.ROC.originalPrice, csv.OriginalPrice);
				}
				if (csv.Contains(CSVFieldIDs.ExecPrice))
				{
					order.Set(OrderFieldIDs.ROC.execPrice, csv.ExecPrice);
				}
				if (csv.Contains(CSVFieldIDs.StopPrice))
				{
					order.Set(OrderFieldIDs.ROC.stopPrice, csv.StopPrice);
				}
				if (csv.Contains(CSVFieldIDs.PegOffsetPrice))
				{
					order.Set(OrderFieldIDs.ROC.pegPrice, csv.PegOffsetPrice);
				}
				if (csv.Contains(CSVFieldIDs.StrikePrice))
				{
					order.Set(OrderFieldIDs.ROC.strikePrice, csv.StrikePrice);
				}
				if (csv.Contains(CSVFieldIDs.ExecutionTime))
				{
					order.Set(OrderFieldIDs.ROC.omTime, csv.ExecutionTime.ToOADate());
				}
				else
				{
					order.Set(OrderFieldIDs.ROC.omTime, csv.OmTime.ToOADate());
				}
				if (csv.Contains(CSVFieldIDs.EndTime))
				{
					order.Set(OrderFieldIDs.ROC.endTime, csv.EndTime.ToOADate());
				}
				if (csv.Contains(CSVFieldIDs.OrderExpirationDateTime))
				{
					order.Set(OrderFieldIDs.ROC.orderExpiresDate, csv.OrderExpirationDateTime.ToOADate());
				}

				#endregion

				#region - Long -

				if (csv.Contains(CSVFieldIDs.CplxOrderType))
				{
					order.Set(OrderFieldIDs.ROC.cplxOrderType, csv.CplxOrderType);
				}
				if (csv.Contains(CSVFieldIDs.CumShares))
				{
					order.Set(OrderFieldIDs.ROC.cumQty, csv.CumShares);
				}
				if (csv.Contains(CSVFieldIDs.ExchangeID))
				{
					order.Set(OrderFieldIDs.ROC.destID, csv.ExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.AlgoExchangeID))
				{
					order.Set(OrderFieldIDs.ROC.algoDestID, csv.AlgoExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.Floor))
				{
					order.Set(OrderFieldIDs.ROC.maxFloor, csv.Floor);
				}
				if (csv.Contains(CSVFieldIDs.LeaveShares))
				{
					order.Set(OrderFieldIDs.ROC.leavesQty, csv.LeaveShares);
				}
				if (csv.Contains(CSVFieldIDs.OriginalShares))
				{
					order.Set(OrderFieldIDs.ROC.originalShares, csv.OriginalShares);
				}
				if (csv.Contains(CSVFieldIDs.Shares))
				{
					order.Set(OrderFieldIDs.ROC.qty, csv.Shares);
				}
				if (csv.Contains(CSVFieldIDs.Side))
				{
					order.Set(OrderFieldIDs.ROC.side, csv.Side);
				}
				if (csv.Contains(CSVFieldIDs.Status))
				{
					order.Set(OrderFieldIDs.ROC.status, csv.Status);
				}
				if (csv.Contains(CSVFieldIDs.TIF))
				{
					order.Set(OrderFieldIDs.ROC.tif, csv.TIF);
				}
				if (csv.Contains(CSVFieldIDs.Type))
				{
					order.Set(OrderFieldIDs.ROC.orderType, csv.Type);
				}
				if (csv.Contains(CSVFieldIDs.AlgoType))
				{
					order.Set(OrderFieldIDs.ROC.algoType, csv.AlgoType);
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
					traded.Set(TradedFieldIDs.ROC.omTag, csv.OmTag);
				}
				if (csv.Contains(CSVFieldIDs.OmExecTag))
				{
					traded.Set(TradedFieldIDs.ROC.omExecTag, csv.OmExecTag);
				}
				if (csv.Contains(CSVFieldIDs.ClearingAcct))
				{
					traded.Set(TradedFieldIDs.ROC.account, csv.ClearingAcct);
				}
				if (csv.Contains(CSVFieldIDs.Trader))
				{
					traded.Set(TradedFieldIDs.ROC.trader, csv.Trader);
				}
				if (csv.Contains(CSVFieldIDs.LocalAcct))
				{
					traded.Set(TradedFieldIDs.ROC.localAcct, csv.LocalAcct);
				}
				if (csv.Contains(CSVFieldIDs.Symbol))
				{
					traded.Set(TradedFieldIDs.ROC.symbol, csv.Symbol);
				}
				if (csv.Contains(CSVFieldIDs.ExpDate))
				{
					traded.Set(TradedFieldIDs.ROC.expDate, csv.ExpDate);
				}
				if (csv.Contains(CSVFieldIDs.MaturityDay))
				{
					traded.Set(TradedFieldIDs.ROC.maturityDay, csv.MaturityDay);
				}
				if (csv.Contains(CSVFieldIDs.CallPut))
				{
					traded.Set(TradedFieldIDs.ROC.callPut, csv.CallPut);
				}
				if (csv.Contains(CSVFieldIDs.Underlying))
				{
					traded.Set(TradedFieldIDs.ROC.underlying, csv.Underlying);
				}
				if (csv.Contains(CSVFieldIDs.SecType))
				{
					traded.Set(TradedFieldIDs.ROC.secType, csv.SecType);
				}
				if (csv.Contains(CSVFieldIDs.CplxOrderType))
				{
					traded.Set(TradedFieldIDs.ROC.cplxOrderType, csv.CplxOrderType);
				}
				if (csv.Contains(CSVFieldIDs.ProgramTrade))
				{
					traded.Set(TradedFieldIDs.ROC.programTrade, csv.ProgramTrade);
				}

				#endregion

				#region - Double -

				if (csv.Contains(CSVFieldIDs.ExecPrice))
				{
					traded.Set(TradedFieldIDs.ROC.execPrice, csv.ExecPrice);
				}
				if (csv.Contains(CSVFieldIDs.StrikePrice))
				{
					traded.Set(TradedFieldIDs.ROC.strikePrice, csv.StrikePrice);
				}
				if (csv.Contains(CSVFieldIDs.ExecutionTime))
				{
					traded.Set(TradedFieldIDs.ROC.execTime, csv.ExecutionTime.ToOADate());
				}

				#endregion

				#region - Long -

				if (csv.Contains(CSVFieldIDs.LastShares))
				{
					traded.Set(TradedFieldIDs.ROC.execQty, csv.LastShares);
				}
				if (csv.Contains(CSVFieldIDs.ExchangeID))
				{
					traded.Set(TradedFieldIDs.ROC.destID, csv.ExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.AlgoExchangeID))
				{
					traded.Set(TradedFieldIDs.ROC.algoDestID, csv.AlgoExchangeID);
				}
				if (csv.Contains(CSVFieldIDs.Side))
				{
					traded.Set(TradedFieldIDs.ROC.side, csv.Side);
				}
				if (csv.Contains(CSVFieldIDs.CplxOrderType))
				{
					traded.Set(TradedFieldIDs.ROC.cplxOrderType, csv.CplxOrderType);
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

		public void ProcessWatchListWindows(bool loadIM, Market deltas)
		{
			if (loadIM || !deltas.Empty)
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

					if (GLOBAL.HWindows.WatchListWindows.TryGetValue(key, out var form) && !form.IsProcessing)
					{
						try
						{
							form.UpdateWatchListByProcess(loadIM, deltas);
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

		#region - Process PlotList Windows -

		public void ProcessPlotListWindows(bool loadIM, Market deltas)
		{
			if (loadIM || !deltas.Empty)
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

					if (GLOBAL.HWindows.PlotListWindows.TryGetValue(key, out var form) && !form.IsProcessing)
					{
						try
						{
							form.UpdatePlotListByProcess(loadIM, deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}
			}
		}

		#endregion

		#region - Process Stock Tickets Windows -

		public void ProcessStockTicketWindows(bool loadIM, Market deltas)
		{
			ProcessStockTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessStockTicketWindows(List<ROCOrder> orders)
		{
			ProcessStockTicketWindows(false, orders, new Market());
		} 
		public void ProcessStockTicketWindows(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.StockTicketWindows.TryGetValue(key, out var form))
					{
						try
						{
							form.UpdateTicketByProcess(loadIM, orders, deltas);
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

		public void ProcessOptionTicketWindows(bool loadIM, Market deltas)
		{
			ProcessOptionTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessOptionTicketWindows(List<ROCOrder> orders)
		{
			ProcessOptionTicketWindows(false, orders, new Market());
		} 
		public void ProcessOptionTicketWindows(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
			{
				List<string> symbolList = new List<string>();
				//if (!deltas.Empty)
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

					if (GLOBAL.HWindows.OptionTicketWindows.TryGetValue(key, out var form) && !form.IsProcessing)
					{
						try
						{
							form.UpdateTicketByProcess(loadIM, orders, deltas, symbolList);
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

		public void ProcessFutureTicketWindows(bool loadIM, Market deltas)
		{
			ProcessFutureTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessFutureTicketWindows(List<ROCOrder> orders)
		{
			ProcessFutureTicketWindows(false, orders, new Market());
		} 
		public void ProcessFutureTicketWindows(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.FutureTicketWindows.TryGetValue(key, out var form))
					{
						try
						{
							form.UpdateTicketByProcess(loadIM, orders, deltas);
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

		public void ProcessQuickTicketWindows(bool loadIM, Market deltas)
		{
			ProcessQuickTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessQuickTicketWindows(List<ROCOrder> orders)
		{
			ProcessQuickTicketWindows(false, orders, new Market());
		}
		public void ProcessQuickTicketWindows(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.QuickTicketWindows.TryGetValue(key, out var form))
					{
						try
						{
							form.UpdateTicketByProcess(loadIM, orders.ToArray(), deltas);
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

		public void ProcessAutoSpreadTicketWindows(bool loadIM, Market deltas)
		{
			ProcessAutoSpreadTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessAutoSpreadTicketWindows(List<ROCOrder> orders)
		{
			ProcessAutoSpreadTicketWindows(false, orders, new Market());
		}
		public void ProcessAutoSpreadTicketWindows(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.AutoSpreadTicketWindows.TryGetValue(key, out var form))
					{
						try
						{
							form.UpdateTicketByProcess(loadIM, orders.ToArray(), deltas);
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

		public void ProcessFutureMatrixTicketWindow(bool loadIM, Market deltas)
		{
			ProcessFutureMatrixTicketWindow(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessFutureMatrixTicketWindow(List<ROCOrder> orders)
		{
			ProcessFutureMatrixTicketWindow(false, orders, new Market());
		}
		public void ProcessFutureMatrixTicketWindow(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.FutureMatrixTicketWindows.TryGetValue(key, out var form))
					{
						try
						{
							form.UpdateTicketByProcess(loadIM, orders, deltas);
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

					if (GLOBAL.HWindows.BatchTicketWindows.TryGetValue(key, out var form) && !form.IsProcessing)
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
		}

		#endregion

		#region - Process Market Batch Windows -

		public void ProcessBatchMarketTicketWindows(List<ROCOrder> orders)
		{
			ProcessBatchMarketTicketWindows(false, orders, new Market());
		}
		public void ProcessBatchMarketTicketWindows(bool loadIM, Market deltas)
		{
			ProcessBatchMarketTicketWindows(loadIM, new List<ROCOrder>(), deltas);
		}		
		public void ProcessBatchMarketTicketWindows(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.BatchMarketTicketWindows.TryGetValue(key, out var form) && !form.IsProcessing)
					{
						try
						{
							form.UpdateBatchMarketByProcess(loadIM, orders.ToArray(), deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}

				keys = null;
			}

		}

		#endregion

		#region - Process Order Windows -

		public void ProcessOrderWindows(bool loadIM, Market deltas)
		{
			ProcessOrderWindows(loadIM, new List<ROCOrder>(), deltas);
		}
		public void ProcessOrderWindows(List<ROCOrder> orders)
		{
			ProcessOrderWindows(false, orders, new Market());
		}
		public void ProcessOrderWindows(bool loadIM, List<ROCOrder> orders, Market deltas)
		{
			if (loadIM || orders.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.OrderWindows.TryGetValue(key, out var form) && !form.IsProcessing)
					{
						try
						{
							form.AddUpdateOrdersByProcess(loadIM, orders.ToArray(), deltas);
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}
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

					if (GLOBAL.HWindows.TradeWindows.TryGetValue(key, out var form) && !form.IsProcessing)
					{
						try
						{
							form.AddUpdateTradesByProcess(loadIM, rocExecutions.ToArray(), tposExecutions.ToArray());
						}
						catch (Exception ex)
						{
							GLOBAL.HROC.AddToException(ex);
						}
					}
				}

				keys = null;
			}
		}

		#endregion

		#region - Process Position Windows -

		public void ProcessPositionWindows(bool loadIM, Market deltas)
		{
			ProcessPositionWindows(loadIM, new List<ROCExecution>(), new List<TPOSExecution>(), deltas);
		}
		public void ProcessPositionWindows(List<ROCExecution> rocExecutions, List<TPOSExecution> tposExecutions)
		{
			ProcessPositionWindows(false, rocExecutions, tposExecutions, new Market());
		}
		public void ProcessPositionWindows(bool loadIM, List<ROCExecution> rocExecutions, List<TPOSExecution> tposExecutions, Market deltas)
		{
			if (loadIM || rocExecutions.Count > 0 || tposExecutions.Count > 0 || !deltas.Empty)
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

					if (GLOBAL.HWindows.PositionWindows.TryGetValue(key, out var form) && !form.IsProcessing)
					{
						try
						{
							form.AddUpdatePositionsByProcess(loadIM, newROCPositions, newTPOSPositions, deltas);
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
