using System;
using System.Collections.Generic;
using System.Windows.Forms;

using Common;
using RDSEx;
using SerializationEx;
using CSVEx;
using MarketData;

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
				if (int.TryParse(cboMatrixRange.Text, out range))
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
		private Market _deltas = new Market();

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

			CurrentSecInfo = new IMSecurityBase();

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

		private delegate void UpdateTicketByProcessDelegate(bool updateIM, List<ROCOrder> orders, Market deltas);
		public void UpdateTicketByProcess(bool updateIM, List<ROCOrder> orders, Market deltas)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				try
				{
					_updateIM = updateIM;
					_deltas.Merge(deltas);
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
					BeginInvoke(new UpdateTicketByProcessDelegate(UpdateTicketByProcess), updateIM, orders, deltas);
					return;
				}
				try
				{
					if (updateIM)
					{
						UpdateSecurityInfo();
					}
					if (!deltas.Empty && !IsProcessing)
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

					Market deltas = _deltas.Release();
					if (!deltas.Empty && !IsProcessing)
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
					if (SymbolDetailMatrix.TryGetValue(symbolDetail, out MatrixPosition position))
					{
						IMSecurityBase secInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
						if (secInfo != null && secInfo.SecType != "")
						{
							UpdateMatrixIMInfo(position.ColumnIndex, position.RowIndex, secInfo);
							if (!removeList.Contains(symbolDetail))
								removeList.Add(symbolDetail);
						}
					}
				}

				// Clean Up
				if (removeList.Count > 0)
				{
					lock (ImSymbolNeeded)
					{
						foreach (string symbolDetail in removeList)
							ImSymbolNeeded.Remove(symbolDetail);
					}
				}
			}
		}

		private void UpdateIMInfo(string symbolDetail, IMSecurityBase secInfo)
		{
			if (!IsProcessing)
			{
				System.Threading.ThreadPool.QueueUserWorkItem(RefreshFutureMatrix);
			}
			HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecurityTypes.Future:
					break;
				default:
					DDSymbolDetails.DeleteSymbolDetail(CurrentSymbolDetail);
					break;
			}
		}

		private void UpdateMatrixIMInfo(int colIndex, int rowIndex, IMSecurityBase secInfo)
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
						case CSVFieldIDs.SecurityTypes.Equity:
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
				BeginInvoke(new RefreshFutureMatrixDelegate(RefreshFutureMatrix), input);
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
						string symbolDetail = MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].SymbolDetail;
						if (!SymbolDetailMatrix.ContainsKey(symbolDetail))
						{
							SymbolDetailMatrix.Add(symbolDetail, new MatrixPosition(colIndex, rowIndex));
							UpdateSecurityInfo(symbolDetail);
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

		private void ProcessFutureMatrix(IMSecurityBase secInfo)
		{
			try
			{
				if (secInfo.Expiration.HasValue)
				{
					string _baseSymbol = CurrentSymbolDetailBase;
					int range = MatrixRange * MatrixInterval;
					for (int index = 0; index < range; index++)
					{
						MatrixColumns.Add(new FutureMatrixColumn(range - index, MatrixInterval, _baseSymbol, secInfo.Expiration.Value.AddMonths(index)));
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

			if (SymbolDetailMatrix.TryGetValue(symbolDetail, out MatrixPosition position))
			{
				MatrixColumns[position.ColumnIndex].FuturesMatrixObjects[position.RowIndex].SecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
			}

			lock (ImSymbolNeeded)
			{
				ImSymbolNeeded[symbolDetail] = CurrentSecInfo.MDSymbol;
			}

		}

		// Update with Security Info On Play back & onProcess
		private void UpdateMarketDataDeltas(string symbolDetail, Market deltas)
		{
			// Update Level 1
			if (deltas.TryGet(symbolDetail, out Book found))
			{
				UpdateMarketDataDelta(found);
			}

			// Update Matrix
			foreach ((string symbol, Book delta) in deltas)
			{
				if (MDSymbolMatrix.TryGetValue(symbol, out MatrixPosition pos))
				{
					UpdateMarketDataDelta(pos.ColumnIndex, pos.RowIndex, delta); 
				}
			}
		}

		// Update with matching market data Level 1
		private void UpdateMarketDataDelta(Book delta)
		{
			double price;
			long size;
			string text;

			lock (panelTicker)
			{
				panelTicker.SuspendLayout();

				if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
				{
					dspBidPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.BidSize, out size)) {
					dspBidSize.Value = size;
				}

				if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
					dspAskPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.AskSize, out size)) {
					dspAskSize.Value = size;
				}

				if (delta.TryGetNonZero(Book.FieldEnum.TradePrice, out price))
				{
					dspTradedPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.TotalVolume, out size))
				{
					dspVolume.Value = size;
				}

				if (delta.TryGetNonEmpty(Book.FieldEnum.TradeTick, out text))
				{
					switch (text)
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

				if (delta.TryGetField(Book.FieldEnum.TradeVolume, out size))
				{
					dspTradeVolume.Value = size;
				}

				if (delta.TryGetField(Book.FieldEnum.NetChange, out price)) {
					dspNetChange.Value = price;
					dspTradedPrice.ForeColor = dspNetChange.ForeColor;
				}

				if (delta.TryGetField(Book.FieldEnum.PctChange, out price)) {
					dspPctChange.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.HighPrice, out price)) {
					dspHighPrice.Value = price;
				}

				if (delta.TryGetField(Book.FieldEnum.LowPrice, out price)) {
					dspLowPrice.Value = price;
				}

				if (delta.TryGetNonZero(Book.FieldEnum.ClosePrice, out price))
				{
					dspClosePrice.Value = price;
				}
				else
				{
					switch (CurrentSecInfo.SecType)
					{
						case "F":
							if (delta.TryGetNonZero(Book.FieldEnum.SettlePrice, out price))
							{
								dspClosePrice.Value = price;
							}
							else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
								dspClosePrice.Value = price;
							}
							break;
						default:
							if (delta.TryGetNonZero(Book.FieldEnum.AdjPrevClosePrice, out price)) {
								dspClosePrice.Value = price;
							}
							else if (delta.TryGetNonZero(Book.FieldEnum.PrevClosePrice, out price)) {
								dspClosePrice.Value = price;
							}
							break;
					}
				}

				if (delta.TryGetField(Book.FieldEnum.SecurityStatus, out text)) {
					switch (text.ToLower()) {
						case "none":
							break;
						case "normal":
							if (lblSecurityStatus.Visible) {
								lblSecurityStatus.Visible = false;
								lblSecurityStatus.Blink = false;
								lblSecurityStatus.Text = "STA";
							}
							break;
						default:
							lblSecurityStatus.Text = text;
							if (!lblSecurityStatus.Visible) {
								lblSecurityStatus.Visible = true;
								lblSecurityStatus.Blink = true;
							}
							break;
					}
				}

				panelTicker.ResumeLayout();
			}
		}

		private void UpdateMarketDataDelta(int colIndex, int rowIndex, Book delta)
		{
			double price;
			long size;

			lock (MatrixColumns)
			{
				if (delta.TryGetField(Book.FieldEnum.BidPrice, out price))
				{
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].BidPrice = price;
				}

				if (delta.TryGetField(Book.FieldEnum.BidSize, out size))
				{
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].BidSize = size;
				}

				if (delta.TryGetField(Book.FieldEnum.AskPrice, out price)) {
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].AskPrice = price;
				}

				if (delta.TryGetField(Book.FieldEnum.AskSize, out size)) {
					MatrixColumns[colIndex].FuturesMatrixObjects[rowIndex].AskSize = size;
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
			CurrentSecInfo = new IMSecurityBase();

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