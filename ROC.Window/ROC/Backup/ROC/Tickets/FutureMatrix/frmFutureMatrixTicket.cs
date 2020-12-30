using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Diagnostics;

using FormEx;
using RDSEx;
using DataGridViewEx;
using SerializationEx;
using ContextMenuEx;
using LabelEx;
using MarketDataEx;
using ROMEx;
using CSVEx;
using ButtonEx;
using DateTimeEx;
using System.Drawing.Drawing2D;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmFutureMatrixTicket : FutureMatrixSupport
	{
		#region - Properties -

		private bool _isProcessing = false;
		public bool IsProcessing
		{
			get
			{
				return _isProcessing;
			}
			set
			{
				_isProcessing = value;
			}
		}

		// Number of Colums
		public int MatrixRange
		{
			get
			{
				int range = 0;
				if (Int32.TryParse(cboMatrixRange.Text, out range))
				{
					return range;
				}
				else
				{
					cboMatrixRange.Text = "9";
					return 9;
				}
			}
			set
			{
				if (cboMatrixRange.Items.Contains(value.ToString()))
				{
					cboMatrixRange.Text = value.ToString();
				}
				else
				{
					cboMatrixRange.Text = "9";
				}
			}
		}

		// Number of Month to skip between Colums
		public int MatrixInterval
		{
			get
			{
				if (cboMatrixInterval.Text == "Quarterly")
				{
					return 3;
				}
				return 1;
			}
			set
			{
				switch (value)
				{
					case 3:
						cboMatrixInterval.Text = "Quarterly";
						break;
					case 1:
					default:
						cboMatrixInterval.Text = "Monthly";
						break;
				}
			}
		}

		private string CurrentSymbolDetailBase
		{
			get
			{
				if (IsFutureBySymbol(CurrentSymbolDetail))
				{
					return CurrentSymbolDetail.Substring(0, CurrentSymbolDetail.Length - 2);
				}
				return "";
			}
		}

		#endregion

		#region - Local Variable -

		//private Rectangle _defaultRegion;
		//private GraphicsPath _path = null;

		private string _lastSymbolDetail = "";

		private bool _isLoadingValue = true;

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private Dictionary<string, MDServerToClient> _deltas = new Dictionary<string, MDServerToClient>();

		// Queue the symbols untile the matrix is loaded
		private List<string> _symbolDetailToGet = new List<string>();

		#endregion

		#region - Constructor -

		public frmFutureMatrixTicket()
		{
			InitializeComponent();

			Initialize();
		}

		public frmFutureMatrixTicket(string symbolDetail)
		{
			InitializeComponent();

			Initialize();
		}

		public void Initialize()
		{
			DDSymbolDetails = new DropDownSymbolDetails(this);

			CurrentSecInfo = new BaseSecurityInfo();

			_isLoadingValue = true;

			cboSymbolDetails.DataSource = CurrentSymbolDetails;
			cboSymbolDetails.DisplayMember = "SymbolDetail";

			_lastSymbolDetail = "";
			CurrentSymbolDetail = "";

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			IconClicked += new EventHandler(Form_IconClicked);
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);
			SizeChanged += new EventHandler(Form_SizeChanged);
			Activated += new EventHandler(Form_Activated);

			_isLoadingValue = false;

			cboSymbolDetails.LostFocus += new EventHandler(cboSymbolDetails_LostFocus);
			cboSymbolDetails.SelectedValueChanged += new EventHandler(cboSymbolDetails_LostFocus);

			StartTimer();

			ValidateTicketValues();
		}

		#endregion

		#region - Form Event -

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			try
			{
				if (IsProcessing)
				{
					//if (menuProcessing.Caption != panelGrid.Controls.Count.ToString())
					//{
					//    menuProcessing.Caption = panelGrid.Controls.Count.ToString();
					//}
					if (panelGrid.Visible)
					{
						panelGrid.Visible = false;
					}
				}
				else
				{
					//DrawTransparentArea();

					// Process Queued Symboles after UI had been loaded
					if (_symbolDetailToGet.Count > 0)
					{
						foreach (string symbolDetail in _symbolDetailToGet)
						{
							UpdateSecurityInfo(symbolDetail);
						}
						_symbolDetailToGet.Clear();
					}

					if (!panelGrid.Visible)
					{
						panelGrid.Visible = true;
					}
				}

				if (_lastSymbolDetail != CurrentSymbolDetail)
				{
					if (DDSymbolDetails.NewSymbolDetail)
					{
						// After user hit Enter
						ResetTicketValues();
						_lastSymbolDetail = CurrentSymbolDetail;

						if (_lastSymbolDetail != "")
						{
							SubscribeToSymbol(_lastSymbolDetail);
						}
						DDSymbolDetails.NewSymbolDetail = false;
					}
					else if (DDSymbolDetails.UndoSymbolDetail)
					{
						// After user hit Esc
						CurrentSymbolDetail = _lastSymbolDetail;
						DDSymbolDetails.UndoSymbolDetail = false;
					}
					else if (CurrentSymbolDetail == "")
					{
						// After user hit Delete or backspaced to clear
						_lastSymbolDetail = CurrentSymbolDetail;
						ResetTicketValues();

						DDSymbolDetails.NewSymbolDetail = false;
					}
				}
				else
				{
					if (DDSymbolDetails.NewSymbolDetail)
					{
						DDSymbolDetails.NewSymbolDetail = false;
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_IconClicked(object sender, EventArgs e)
		{
		}

		private void Form_FitClicked(object sender, EventArgs e)
		{
			if (MatrixColumns != null && MatrixColumns.Count > 0)
			{
				MatrixColumns[0].Fit();

				Width = MatrixColumns[MatrixColumns.Count - 1].Right + 8;
				Height = MatrixColumns[0].MatrixHeight + panelGrid.Top + 12;

				//_path = null;
			}
		}

		private void Form_ShowAllClicked(object sender, EventArgs e)
		{
		}

		private void Form_SizeChanged(object sender, EventArgs e)
		{
		}

		private void Form_Load(object sender, EventArgs e)
		{
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			UITimerTicked -= new EventHandler(Form_UITimerTicked);
			IconClicked -= new EventHandler(Form_IconClicked);
			FitClicked -= new EventHandler(Form_FitClicked);
			ShowAllClicked -= new EventHandler(Form_ShowAllClicked);

			FitClicked -= new EventHandler(Form_FitClicked);
			Load -= new EventHandler(Form_Load);
			FormClosing -= new FormClosingEventHandler(Form_FormClosing);
			SizeChanged -= new EventHandler(Form_SizeChanged);

			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void Form_Activated(object sender, EventArgs e)
		{
		}

		#endregion

		#region - Matrix Event -

		private void cboMatrix_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (!IsProcessing)
			{
				ClearFutureMatrix();
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshFutureMatrix);
			}
		}

		#endregion

		#region - Binding -

		#region - Refresh & First Load -

		private void SubscribeToSymbol(string symbolDetail)
		{
			try
			{
				UpdateSecurityInfo(symbolDetail);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Used By Process Thread -

		private delegate void UpdateTicketByProcessDelegate(bool updateIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas);
		public void UpdateTicketByProcess(bool updateIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				try
				{
					_updateIM = updateIM;
					lock (_deltas)
					{
						foreach (string key in deltas.Keys)
						{
							if (_deltas.ContainsKey(key))
							{
								_deltas[key].Update(deltas[key]);
							}
							else
							{
								_deltas.Add(key, new MDServerToClient(deltas[key]));
							}
						}
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
			else
			{
				if (InvokeRequired)
				{
					BeginInvoke(new UpdateTicketByProcessDelegate(UpdateTicketByProcess), new object[] { updateIM, orders, deltas });
					return;
				}
				try
				{
					if (updateIM)
					{
						UpdateSecurityInfo();
					}
					if (deltas.Count > 0 && !IsProcessing)
					{
						UpdateMarketDataDeltas(CurrentSecInfo.MDSymbol, deltas);
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void UpdateTicketByProcessByTimer()
		{
			try
			{
				if (!_updatingUI)
				{
					_updatingUI = true;
					if (_updateIM)
					{
						_updateIM = false;
						UpdateSecurityInfo();
					}
					Dictionary<string, MDServerToClient> deltas = new Dictionary<string, MDServerToClient>();
					lock (_deltas)
					{
						if (_deltas.Count > 0)
						{
							deltas = new Dictionary<string, MDServerToClient>(_deltas);
							_deltas.Clear();
						}
					}
					if (deltas.Count > 0 && !IsProcessing)
					{
						UpdateMarketDataDeltas(CurrentSecInfo.MDSymbol, deltas);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
			_updatingUI = false;
		}

		private void UpdateSecurityInfo()
		{
			Dictionary<string, string> lcoImSymbolNeeded = new Dictionary<string, string>();
			if (ImSymbolNeeded.Count > 0)
			{
				lock (ImSymbolNeeded)
				{
					lcoImSymbolNeeded = new Dictionary<string, string>(ImSymbolNeeded);
				}

				List<string> removeList = new List<string>();

				foreach (string symbolDetail in lcoImSymbolNeeded.Keys)
				{
					if (symbolDetail == CurrentSymbolDetail)
					{
						CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);

						if (CurrentSecInfo != null && CurrentSecInfo.SecType != "")
						{
							UpdateIMInfo(symbolDetail, CurrentSecInfo);

							if (!removeList.Contains(symbolDetail))
							{
								removeList.Add(symbolDetail);
							}
						}

						UpdateTicket();
					}
					
					// Matrix Symbol
					if (SymbolDetailMatrix.ContainsKey(symbolDetail))
					{
						BaseSecurityInfo secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
						if (secInfo != null && secInfo.SecType != "")
						{
							UpdateMatrixIMInfo(SymbolDetailMatrix[symbolDetail].ColumnIndex, SymbolDetailMatrix[symbolDetail].RowIndex, secInfo);

							if (!removeList.Contains(symbolDetail))
							{
								removeList.Add(symbolDetail);
							}
						}
					}
				}

				// Clean Up
				if (removeList.Count > 0)
				{
					lock (ImSymbolNeeded)
					{
						foreach (string symbolDetail in removeList)
						{
							if (ImSymbolNeeded.ContainsKey(symbolDetail))
							{
								ImSymbolNeeded.Remove(symbolDetail);
							}
						}
					}
				}
			}
		}

		private void UpdateIMInfo(string symbolDetail, BaseSecurityInfo secInfo)
		{
			if (!IsProcessing)
			{
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshFutureMatrix);
			}
			HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecutrityTypes.Future:
					break;
				default:
					DDSymbolDetails.DeleteSymbolDetail(CurrentSymbolDetail);
					break;
			}
		}

		private void UpdateMatrixIMInfo(int colIndex, int rowIndex, BaseSecurityInfo secInfo)
		{
			MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].SecInfo = secInfo;

			if (!MDSymbolMatrix.ContainsKey(secInfo.MDSymbol))
			{
				MDSymbolMatrix.Add(secInfo.MDSymbol, new MatrixPosition(colIndex, rowIndex));
			}

			HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
		}

		private void UpdateTicket()
		{
			try
			{
				if (CurrentSecInfo != null)
				{
					if (CurrentSecInfo.LongName != null && CurrentSecInfo.LongName != "")
					{
						LongName = CurrentSecInfo.LongName;
					}

					switch (CurrentSecInfo.SecType)
					{
						case CSVFieldIDs.SecutrityTypes.Equity:
							dspAskPrice.MaxDecimal = 2;
							dspBidPrice.MaxDecimal = 2;
							dspNetChange.MaxDecimal = 2;
							dspLowPrice.MaxDecimal = 2;
							dspHighPrice.MaxDecimal = 2;
							dspClosePrice.MaxDecimal = 2;
							dspTradedPrice.MaxDecimal = 2;

							break;
						default:
							dspAskPrice.MaxDecimal = 7;
							dspBidPrice.MaxDecimal = 7;
							dspNetChange.MaxDecimal = 7;
							dspLowPrice.MaxDecimal = 7;
							dspHighPrice.MaxDecimal = 7;
							dspClosePrice.MaxDecimal = 7;
							dspTradedPrice.MaxDecimal = 7;

							break;
					}

					dspAskPrice.TickSize = CurrentSecInfo.TickSize;
					dspBidPrice.TickSize = CurrentSecInfo.TickSize;
					dspNetChange.TickSize = CurrentSecInfo.TickSize;
					dspLowPrice.TickSize = CurrentSecInfo.TickSize;
					dspHighPrice.TickSize = CurrentSecInfo.TickSize;
					dspClosePrice.TickSize = CurrentSecInfo.TickSize;
					dspTradedPrice.TickSize = CurrentSecInfo.TickSize;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#region - Process Future Matrix -

		private delegate void RefreshFutureMatrixDelegate(object input);
		private void RefreshFutureMatrix(object input)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new RefreshFutureMatrixDelegate(RefreshFutureMatrix), new object[] { input });
				return;
			}

			if (MatrixColumns == null || MatrixColumns.Count == 0)
			{
				UpdateFutureMatrixStart();

				ClearFutureMatrix();
				ProcessFutureMatrix(CurrentSecInfo);

				UpdateFutureMatrixStop();
			}
		}

		private void UpdateFutureMatrixStart()
		{
			IsProcessing = true;
			//menuProcessing.Show();

			//panelGrid.Visible = false;
			panelGrid.SuspendLayout();
		}

		private void UpdateFutureMatrixStop()
		{
			try
			{
				//menuProcessing.Hide();
				//panelGrid.Visible = true;
				panelGrid.ResumeLayout();

				for (int colIndex = MatrixColumns.Count - 1; colIndex >= 0; colIndex--)
				{
					MatrixColumns[colIndex].Visible = true;

					for (int rowIndex = MatrixColumns[colIndex].FuturesMatrixObjects.Count - 1; rowIndex >= 0; rowIndex--)
					{
						if (!SymbolDetailMatrix.ContainsKey(MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].SymbolDetail))
						{
							SymbolDetailMatrix.Add(MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].SymbolDetail, new MatrixPosition(colIndex, rowIndex));
							UpdateSecurityInfo(MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].SymbolDetail);
						}
					}
				}

				Form_FitClicked(null, EventArgs.Empty);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			IsProcessing = false;
		}

		//private void DrawTransparentArea()
		//{
		//    try
		//    {
		//        if (_path == null && MatrixColumns != null && MatrixColumns.Count > 0)
		//        {
		//            _path = new GraphicsPath();

		//            // Top Line
		//            Point startPt = new Point(0, 0);
		//            Point rightPt = new Point(Width, 0);
		//            _path.AddLine(startPt, rightPt);

		//            Point bottomPt = new Point(0, 0);
		//            int offset = 3;
		//            for (int colIndex = MatrixColumns.Count - 1; colIndex >= 0; colIndex--)
		//            {
		//                MatrixColumns[colIndex].Fit();

		//                // Vertical Line
		//                bottomPt = new Point(rightPt.X, MatrixColumns[colIndex].MatrixHeight + panelGrid.Top + 6);
		//                _path.AddLine(rightPt, bottomPt);

		//                // Horizontal Line
		//                rightPt.X = rightPt.X - MatrixColumns[colIndex].MatrixWidth - offset;
		//                rightPt.Y = bottomPt.Y;

		//                // Offset Point
		//                _path.AddLine(rightPt, bottomPt);

		//                offset = 0;
		//            }

		//            _path.AddLine(bottomPt, new Point(0, bottomPt.Y));
		//            _path.CloseFigure();

		//            _defaultRegion = new Rectangle(0, 0, Width, Height);
		//            Region = new Region(_path);
		//        }
		//    }
		//    catch (Exception ex)
		//    {
		//        GLOBAL.HROC.AddToException(ex);
		//    }
		//}

		private void ClearFutureMatrix()
		{
			//if (_defaultRegion != null)
			//{
			//    if (CurrentSymbolDetail == "")
			//    {
			//        Region = new Region(_defaultRegion);
			//    }
			//    BackColor = Color.FromKnownColor(KnownColor.Control);
			//}
			MDSymbolMatrix.Clear();
			SymbolDetailMatrix.Clear();
			MatrixColumns.Clear();
			panelGrid.Controls.Clear();
		}

		private void ProcessFutureMatrix(BaseSecurityInfo secInfo)
		{
			try
			{
				DateTime tDT;
				if (DateTime.TryParseExact(secInfo.Expiration, GLOBAL.TimeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT))
				{
					string _baseSymbol = CurrentSymbolDetailBase;
					int range = MatrixRange * MatrixInterval;
					for (int index = 0; index < range; index++)
					{
						MatrixColumns.Add(new FutureMatrixColumn(range - index, MatrixInterval, _baseSymbol, tDT.AddMonths(index)));
						index = index + MatrixInterval - 1;
					}

					for (int index = MatrixColumns.Count - 1; index >= 0; index--)
					{
						panelGrid.Controls.Add(MatrixColumns[index]);
						MatrixColumns[index].Dock = DockStyle.Left;

						//System.Threading.Thread.Sleep(1);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion
		
		#endregion

		// Update the ticket with all current IM info
		private void UpdateSecurityInfo(string symbolDetail)
		{
			if (IsProcessing)
			{
				if (!_symbolDetailToGet.Contains(symbolDetail))
				{
					_symbolDetailToGet.Add(symbolDetail);
				}
			}
			else
			{
				UpdateFutureMatrixTicketWithSecurityInfo(symbolDetail);
				GLOBAL.HRDS.GetSecurityByTicker(symbolDetail);
			}
		}

		// Update with Security Info On Play back & onLoad
		private void UpdateFutureMatrixTicketWithSecurityInfo(string symbolDetail)
		{
			if (symbolDetail == CurrentSymbolDetail)
			{
				CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
			}
			if (SymbolDetailMatrix.ContainsKey(symbolDetail))
			{
				MatrixColumns[SymbolDetailMatrix[symbolDetail].ColumnIndex].FuturesMatrixObjects[SymbolDetailMatrix[symbolDetail].RowIndex].SecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
			}

			lock (ImSymbolNeeded)
			{
				if (!ImSymbolNeeded.ContainsKey(symbolDetail))
				{
					// Should have only the current symbol in the list;
					// ImSymbolNeeded.Clear();
					ImSymbolNeeded.Add(symbolDetail, CurrentSecInfo.MDSymbol);
				}
				else
				{
					ImSymbolNeeded[symbolDetail] = CurrentSecInfo.MDSymbol;
				}
			}

		}

		// Update with Security Info On Play back & onProcess
		private void UpdateMarketDataDeltas(string symbolDetail, Dictionary<string, MDServerToClient> deltas)
		{
			// Update Level 1
			if (deltas.ContainsKey(symbolDetail))
			{
				UpdateMarketDataDelta(deltas[symbolDetail]);
			}

			// Update Matrix
			foreach (string mdSymbol in deltas.Keys)
			{
				if (MDSymbolMatrix.ContainsKey(mdSymbol))
				{
					UpdateMarketDataDelta(MDSymbolMatrix[mdSymbol].ColumnIndex, MDSymbolMatrix[mdSymbol].RowIndex, deltas[mdSymbol]); 
				}
			}
		}

		// Update with matching market data Level 1
		private void UpdateMarketDataDelta(MDServerToClient delta)
		{
			lock (panelTicker)
			{
				panelTicker.SuspendLayout();

				if (delta.BidPrice != null)
				{
					dspBidPrice.Value = (double)delta.BidPrice;
				}

				if (delta.BidSize != null)
				{
					dspBidSize.Value = (long)delta.BidSize;
				}

				if (delta.AskPrice != null)
				{
					dspAskPrice.Value = (double)delta.AskPrice;
				}

				if (delta.AskSize != null)
				{
					dspAskSize.Value = (long)delta.AskSize;
				}

				if (delta.TradePrice != null && delta.TradePrice != 0)
				{
					dspTradedPrice.Value = (double)delta.TradePrice;
				}

				if (delta.TotalVolume != null)
				{
					dspVolume.Value = (long)delta.TotalVolume;
				}

				if (delta.TradeTick != null && delta.TradeTick != "")
				{
					switch (delta.TradeTick)
					{
						case "+":
						case "+0":
							picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Up];
							break;
						case "-":
						case "-0":
							picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.Down];
							break;
						default:
							// Leave the last tick state
							//picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.None];
							break;
					}
				}

				if (delta.TradeVolume != null)
				{
					dspTradeVolume.Value = (long)delta.TradeVolume;
				}

				if (delta.NetChange != null)
				{
					dspNetChange.Value = (double)delta.NetChange;
					dspTradedPrice.ForeColor = dspNetChange.ForeColor;
				}

				if (delta.PctChange != null)
				{
					dspPctChange.Value = (double)delta.PctChange;
				}

				if (delta.HighPrice != null)
				{
					dspHighPrice.Value = (double)delta.HighPrice;
				}

				if (delta.LowPrice != null)
				{
					dspLowPrice.Value = (double)delta.LowPrice;
				}

				if (delta.ClosePrice != null && (double)delta.ClosePrice != 0)
				{
					dspClosePrice.Value = (double)delta.ClosePrice;
				}
				else
				{
					switch (CurrentSecInfo.SecType)
					{
						case "F":
							if (delta.SettlePrice != null && (double)delta.SettlePrice != 0)
							{
								dspClosePrice.Value = (double)delta.SettlePrice;
							}
							else if (delta.PrevClosePrice != null && (double)delta.PrevClosePrice != 0)
							{
								dspClosePrice.Value = (double)delta.PrevClosePrice;
							}
							break;
						default:
							if (delta.AdjPrevClosePrice != null && (double)delta.AdjPrevClosePrice != 0)
							{
								dspClosePrice.Value = (double)delta.AdjPrevClosePrice;
							}
							else if (delta.PrevClosePrice != null && (double)delta.PrevClosePrice != 0)
							{
								dspClosePrice.Value = (double)delta.PrevClosePrice;
							}
							break;
					}
				}

				switch (delta.SecurityStatusINT)
				{
					case SecurityStates.None:
						break;
					case SecurityStates.Normal:
						if (lblSecurityStatus.Visible)
						{
							lblSecurityStatus.Visible = false;
							lblSecurityStatus.Blink = false;
							lblSecurityStatus.Text = "STA";
						}
						break;
					default:
						lblSecurityStatus.Text = delta.SecurityStatus;
						if (!lblSecurityStatus.Visible)
						{
							lblSecurityStatus.Visible = true;
							lblSecurityStatus.Blink = true;
						}
						break;
				}

				panelTicker.ResumeLayout();
			}
		}

		private void UpdateMarketDataDelta(int colIndex, int rowIndex, MDServerToClient delta)
		{
			lock (MatrixColumns)
			{
				if (delta.BidPrice != null)
				{
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].BidPrice = (double)delta.BidPrice;
				}

				if (delta.BidSize != null)
				{
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].BidSize = (long)delta.BidSize;
				}

				if (delta.AskPrice != null)
				{
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].AskPrice = (double)delta.AskPrice;
				}

				if (delta.AskSize != null)
				{
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].AskSize = (long)delta.AskSize;
				}
			}
		}

		#endregion

		#region - Ticket Display -

		private void ValidateTicketValues()
		{
			ValidateTicketValues("");
		}
		private void ValidateTicketValues(string controlName)
		{
			if (controlName == "")
			{
			}

			if (!_isLoadingValue)
			{
			}

			BuildOrderMsg();
		}

		private void BuildOrderMsg()
		{
			if (CurrentSymbolDetail != "")
			{
				if (CurrentSecInfo.LongName == "")
				{
					LongName = string.Format("- {0} -", CurrentSymbolDetail.ToUpper().Trim());
				}
			}
			else
			{
				LongName = "Future Matrix Window";
			}
		}

		private void ResetTicketValues()
		{
			_isLoadingValue = true;

			ClearFutureMatrix();
			CurrentSecInfo = new BaseSecurityInfo();

			LongName = CurrentSecInfo.LongName;

			dspBidPrice.Value = 0.0;
			dspBidSize.Value = 0;
			dspAskPrice.Value = 0.0;
			dspAskSize.Value = 0;
			dspNetChange.Value = 0.0;
			dspPctChange.Value = 0.0;
			dspTradedPrice.Value = 0.0;
			dspTradeVolume.Value = 0;
			picTradeTick.Image = imageListTradeTick.Images[BaseTicket.TradeTicks.None];
			dspVolume.Value = 0;
			dspLowPrice.Value = 0.0;
			dspHighPrice.Value = 0.0;
			dspClosePrice.Value = 0.0;

			lblSecurityStatus.Visible = false;
			lblSecurityStatus.Blink = false;
			lblSecurityStatus.Text = "STA";

			lblMarketDataError.Visible = false;
			lblMarketDataError.Blink = false;

			_isLoadingValue = false;

			ValidateTicketValues();
		}

		#endregion

		#region - Export & Import -

		public string ExportTicket()
		{
			ROCTicketProfile prof = new ROCTicketProfile(this);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportTicket(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCTicketProfile prof = (ROCTicketProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					foreach (string symbolDetail in prof.SymbolDetails)
					{
						CurrentSymbolDetails.Rows.Add(symbolDetail);
					}

					prof = new ROCTicketProfile(this, prof);
				}
			}
		}

		#endregion

		#region - SymbolDetails DropDown -

		private void cboSymbolDetails_LostFocus(object sender, EventArgs e)
		{
			try
			{
				DDSymbolDetails.SymbolDetailsKeyUp(new KeyEventArgs(Keys.Enter));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cboSymbolDetails_KeyUp(object sender, KeyEventArgs e)
		{
			try
			{
				DDSymbolDetails.SymbolDetailsKeyUp(e);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cboSymbolDetails_KeyDown(object sender, KeyEventArgs e)
		{
			try
			{
				DDSymbolDetails.SymbolDetailsKeyDown(e);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion
	}
}