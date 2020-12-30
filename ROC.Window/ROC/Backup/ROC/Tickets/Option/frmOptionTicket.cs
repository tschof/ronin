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

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmOptionTicket : OptionSupport
	{
		private sealed class SubscriptionQueue
		{
			private string _mdsymbol = "";
			public string MDSymbol
			{
				get
				{
					return _mdsymbol;
				}
			}

			private string _mdsource = "";
			public string MDSource
			{
				get
				{
					return _mdsource;
				}
			}

			private string _secType = "";
			public string SecType
			{
				get
				{
					return _secType;
				}
			}

			private string _currentSymbolDetail = "";
			public string CurrentSymbolDetail
			{
				get
				{
					return _currentSymbolDetail;
				}
			}

			public SubscriptionQueue(string mdsymbol, string mdsource, string secType)
			{
				_mdsymbol = mdsymbol;
				_mdsource = mdsource;
				_secType = secType;
			}

			public SubscriptionQueue(string mdsymbol, string currentSymbolDetail)
			{
				_mdsymbol = mdsymbol;
				_currentSymbolDetail = currentSymbolDetail;
			}
		}

		#region - Properties -

		public bool IsProcessing
		{
			get
			{
				return rocOptionList.Processing;
			}
			set
			{
				rocOptionList.Processing = value;
			}
		}

		private bool gotOptionChain
		{
			get
			{
				if (CurrentSecInfo != null &&
					CurrentSecInfo.OptionChain.Count > 0)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		private DateTime nextMonth
		{
			get
			{
				return DateTime.Now.AddMonths(1);
			}
		}

		public string StrikesToShow
		{
			get
			{
				if (cboStrikesToShow.Text == "")
				{
					return "5";
				}
				return cboStrikesToShow.Text;
			}
			set
			{
				cboStrikesToShow.Text = value;
				if (rocOptionList.StrikeRange != StrikeRange)
				{
					rocOptionList.StrikeRange = StrikeRange;
				}
			}
		}

		private int StrikeRange
		{
			get
			{
				int range = 0;
				if (Int32.TryParse(cboStrikesToShow.Text, out range))
				{
					return range;
				}
				else if (cboStrikesToShow.Text == "ALL")
				{
					// return all
					return 0;
				}
				else
				{
					cboStrikesToShow.Text = "5";
					return 5;
				}
			}
		}

		public string ExpirationsToShow
		{
			get
			{
				if (cboExpirationToShow.Text == "")
				{
					return "1";
				}
				return cboExpirationToShow.Text;
			}
			set
			{
				cboExpirationToShow.Text = value;
				if (rocOptionList.ExpirationRange != ExpirationRange)
				{
					rocOptionList.ExpirationRange = ExpirationRange;
				}
			}
		}

		private int ExpirationRange
		{
			get
			{
				int range = 0;
				if (Int32.TryParse(cboExpirationToShow.Text, out range))
				{
					return range;
				}
				else if (cboExpirationToShow.Text == "ALL")
				{
					// return all
					return 0;
				}
				else
				{
					cboExpirationToShow.Text = "1";
					return 1;
				}
			}
		}

		public bool ShowMonthlyExpirationsOnly
		{
			get
			{
				return chkMonthlyOnly.Checked;
			}
			set
			{
				chkMonthlyOnly.Checked = value;
			}
		}

		private decimal LimitPriceIncrementMultiple
		{
			get
			{
				try
				{
					if (dspLimitPriceIncrement.Value != null && dspLimitPriceIncrement.Value.ToString() != "")
					{
						return Convert.ToDecimal(dspLimitPriceIncrement.Value);
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
				return 1;
			}
		}

		private decimal StopPriceIncrementMultiple
		{
			get
			{
				try
				{
					if (dspStopPriceIncrement.Value != null && dspStopPriceIncrement.Value.ToString() != "")
					{
						return Convert.ToDecimal(dspStopPriceIncrement.Value);
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
				return 1;
			}
		}

		#endregion

		#region - Local Variable -

		private bool _doNotLoadOPRAParticipant = Configuration.User.Default.DoNotLoadOPRAParticipant;

		private string _lastSymbolDetail = "";
		private bool _lastIsFuture = true;

		private Size _lastSize = new Size();
		private bool _widthChanged = false;
		private bool _heightChanged = false;
		private bool _resizeGridToFit = false;

		private bool _isInError = false;
		//List of controls that is InError
		private Dictionary<string, bool> _controlsInError = new Dictionary<string, bool>();

		private bool _isLoadingValue = true;
		private bool _isAtMoney = false;

		private DataView _orderTypesView;
		private DataView _durationsView;
		private DataView _optionView;

		private DataTable _expirationTable;
		private DataTable expirationTable
		{
			get
			{
				if (_expirationTable == null)
				{
					_expirationTable = new DataTable();
					_expirationTable.Columns.Add(new DataColumn("ExpDateTime", Type.GetType("System.DateTime")));
					_expirationTable.Columns.Add(new DataColumn("ExpString", Type.GetType("System.String")));
					_expirationTable.Columns.Add(new DataColumn("ExpDisplay", Type.GetType("System.String")));

					//List<DataColumn> keyColumns = new List<DataColumn>();
					//keyColumns.Add(_expirationTable.Columns["ExpDateTime"]);
					//_expirationTable.PrimaryKey = keyColumns.ToArray();

					_expirationTable.DefaultView.Sort = "ExpDateTime";
				}
				return _expirationTable;
			}
		}
		private DataView _expirationView;
		private List<DateTime> ExpDateKeys = new List<DateTime>();

		private DataTable _strikeTable;
		private DataTable strikeTable
		{
			get
			{
				if (_strikeTable == null)
				{
					_strikeTable = new DataTable();
					_strikeTable.Columns.Add(new DataColumn("StrikePrice", Type.GetType("System.Double")));
					_strikeTable.Columns.Add(new DataColumn("StrikeString", Type.GetType("System.String")));

					//List<DataColumn> keyColumns = new List<DataColumn>();
					//keyColumns.Add(_expirationTable.Columns["StrikePrice"]);
					//_strikeTable.PrimaryKey = keyColumns.ToArray();

					_strikeTable.DefaultView.Sort = "StrikePrice";
				}

				return _strikeTable;
			}
		}
		private DataView _strikeView;
		private List<double> StrikeKeys = new List<double>();

		private NumericUpDown _crrentPriceObj;

		//private bool _orderLoaded = false;
		private bool _ticketClosed = false;

		private Dictionary<string, ROCOrder> _buyOrdersByTag = new Dictionary<string, ROCOrder>();
		private Dictionary<string, ROCOrder> _sellOrdersByTag = new Dictionary<string, ROCOrder>();

		#region - Update NBBO By Cycle -
		//private DateTime _lastNBBORefreshTime = DateTime.Now;
		//private int _lastFirstDisplayedRowIndex = -1;
		//private int _lastRowsToShow = 0;
		//private bool _needsNewNBBO = true;
		#endregion

		private double _stockSmallIncr = 0.01;
		private double _stockLargeIncr = 0.05;

		private bool _showOnlySelectedExchange = false;

		private char[] _trimZero = new char[] { '0' };

		//DataTable _localTable = new DataTable();
		private double _strikeHigh;
		private double _strikeLow;
		private DateTime _expDateHigh;
		private DateTime _expDateLow;

		private bool _resetExpirationOnly = false;

		private bool _updatingUI = false;
		private bool _updateIM = false;
		private Dictionary<string, MDServerToClient> _deltas = new Dictionary<string, MDServerToClient>();

		private Dictionary<string, SubscriptionQueue> _subQueue = new Dictionary<string, SubscriptionQueue>();
		private Dictionary<string, SubscriptionQueue> _subOptionQueue = new Dictionary<string, SubscriptionQueue>();

		//private DateTime _lastValidateTime = DateTime.Now;
		//// Only valide ticket every 10 ms (Max)
		//private int _validateTicketValuesRate = 10;

		private int _idleLimit = 100;

		#endregion

		#region - Constructor -

		public frmOptionTicket()
		{
			InitializeComponent();

			Initialize();
		}

		public frmOptionTicket(string symbolDetail)
		{
			InitializeComponent();

			Initialize();

			CurrentSymbolDetail = symbolDetail;
		}

		public void Initialize()
		{
			DDSymbolDetails = new DropDownSymbolDetails(this);
			DDAccountDropDown = new DropDownTraderAccount(this);

			QuickButtonSupprot = new QuickButtonSupport(this);
			QuickButtonSupprot.QuickButtonClicked += new QuickButtonClickedEventHandler(QuickButtonSupport_QuickButtonClicked);

			CurrentSecInfo = new BaseSecurityInfo();

			_showOnlySelectedExchange = Configuration.User.Default.ShowOnlySelectedExchange;

			_isLoadingValue = true;

			_crrentPriceObj = numLimitPrice;

			cboSymbolDetails.DataSource = CurrentSymbolDetails;
			cboSymbolDetails.DisplayMember = "SymbolDetail";

			_orderTypesView = new DataView(GLOBAL.HOrders.OptionOrderTypes);
			cboOrder.DataSource = _orderTypesView;
			cboOrder.DisplayMember = "Type";
			cboOrder.Text = "LIMIT";

			_durationsView = new DataView(GLOBAL.HOrders.Durations);
			cboDuration.DataSource = _durationsView;
			cboDuration.DisplayMember = "Duration";
			cboDuration.Text = "DAY";

			_optionView = new DataView(GLOBAL.HOrders.OptionTypes);
			cboCallPut.DataSource = _optionView;
			cboCallPut.DisplayMember = "Type";
			cboCallPut.Text = "Put";

			_expirationView = new DataView(expirationTable, "", "ExpDateTime", DataViewRowState.CurrentRows);
			cboExpiration.DataSource = _expirationView;
			cboExpiration.DisplayMember = "ExpDisplay";

			_strikeView = new DataView(strikeTable, "", "StrikePrice", DataViewRowState.CurrentRows);
			cboStrike.DataSource = _strikeView;
			cboStrike.DisplayMember = "StrikeString";

			_lastSymbolDetail = "";
			CurrentSymbolDetail = "";

			cboStrikesToShow.Text = "5";

			Height = MinimumSize.Height;

			if (Configuration.User.Default.AutoFocusOnControls)
			{
				Setup_MouseEnterEvent();

				dspLimitPrice.MouseEnter += new EventHandler(dspLimitPrice_MouseEnter);
				dspLimitPrice.MouseWheel += new MouseEventHandler(dspLimitPrice_MouseEnter);

				dspStopPrice.MouseEnter += new EventHandler(dspStopPrice_MouseEnter);
				dspStopPrice.MouseWheel += new MouseEventHandler(dspStopPrice_MouseEnter);
			}

			panelStopPrice.EnabledChanged += new EventHandler(panelStopPrice_EnabledChanged);

			SetPriceDisplay(PriceType.Limit, true);
			dspLimitPrice.LostFocus += new EventHandler(dspLimitPrice_Leave);

			SetPriceDisplay(PriceType.Stop, true);
			dspStopPrice.LostFocus += new EventHandler(dspStopPrice_Leave);

			SetExpirationDisplay(true);

			UITimerTicked += new EventHandler(Form_UITimerTicked);
			IconClicked += new EventHandler(Form_IconClicked);
			FitClicked += new EventHandler(Form_FitClicked);
			ShowAllClicked += new EventHandler(Form_ShowAllClicked);

			Load += new EventHandler(Form_Load);
			FormClosing += new FormClosingEventHandler(Form_FormClosing);
			SizeChanged += new EventHandler(Form_SizeChanged);
			Activated += new EventHandler(Form_Activated);

			panelManagerOption.SelectedPanel = managedPanelStock;

			rocOptionList.Initialize();
			rocOptionList.ShowOnlySelectedExchange = _showOnlySelectedExchange;

			rocOptionList.PropertyChanged += new PropertyChangedEventHandler(rocOptionList_PropertyChanged);
			rocOptionList.MouseEnter += new EventHandler(rocOptionList_MouseEnter);

			rocOptionList.ProcessingChanged += new EventHandler(rocOptionList_ProcessingChanged);
			rocOptionList.RefreshSharedRowsChanged += new EventHandler(rocOptionList_RefreshSharedRowsChanged);

			rocOptionList.ExchangeColors = new Dictionary<string, Color>(HelperOption.exchangeColors);

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
			if (IsProcessing)
			{
				//if (rocOptionList.Visible)
				//{
				//    rocOptionList.SuspendLayout();
				//    rocOptionList.Visible = false;
				//}
				if (rocOptionList.RocGridTable != null && menuProcessing.Caption != rocOptionList.RocGridTable.Rows.Count.ToString())
				{
					menuProcessing.Caption = rocOptionList.RocGridTable.Rows.Count.ToString();
				}
			}
			else
			{
				if (GLOBAL.UseDelayedUpdate)
				{
					try
					{
						UpdateTicketByProcessByTimer();
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}

				//if (!rocOptionList.Visible)
				//{
				//    rocOptionList.ResumeLayout(true);
				//    rocOptionList.Visible = true;
				//}
				//if (gotOptionChain && GetCurrentMoney() != 0 && !rocOptionList.OptionLoaded)
				if (gotOptionChain && !rocOptionList.OptionLoaded)
				{
					rocOptionList.OptionLoaded = true;
					//if (!GLOBAL.ShownDM)
					//{
					//    //TODO Multi Threaded Loading Cause Index Error and IDE problems
					//    RefreshOptionChain(null);
					//}
					//else
					//{
						RefreshOptionChain(null);
						//System.Threading.ThreadPool.QueueUserWorkItem(RefreshOptionChain);
					//}
				}

				SubscribeByQueue();

				#region - Update NBBO By Cycle -
				//try
				//{
				//    if (!IsFuture && _needsNewNBBO && rocOptionList.FirstDisplayedCell != null && _lastNBBORefreshTime.AddMilliseconds(250) < DateTime.Now)
				//    {
				//        _lastFirstDisplayedRowIndex = rocOptionList.FirstDisplayedCell.RowIndex;
				//        _lastRowsToShow = rocOptionList.RowsToShow;
				//        _lastNBBORefreshTime = DateTime.Now;
				//        _needsNewNBBO = false;

				//        Debug.Print(DateTime.Now.ToString("mm:ss.fff"));

				//        for (int rowIndex = _lastFirstDisplayedRowIndex; rowIndex < _lastFirstDisplayedRowIndex + _lastRowsToShow; rowIndex++)
				//        {
				//            if (rowIndex < rocOptionList.Rows.Count &&
				//                rocOptionList["PutSymbol", rowIndex].Value != null &&
				//                rocOptionList["CallSymbol", rowIndex].Value != null)
				//            {
				//                MakeNBBO(rocOptionList["PutSymbol", rowIndex].Value.ToString(), rocOptionList["CallSymbol", rowIndex].Value.ToString());
				//            }
				//            else
				//            {
				//                break;
				//            }
				//        }
				//    }
				//}
				//catch (Exception ex)
				//{
				//    GLOBAL.HROC.AddToException(ex);
				//}
				#endregion
			}

			if (cboSymbolDetails.BackColor != panelTicker.BackColor)
			{
				cboSymbolDetails.BackColor = panelTicker.BackColor;
			}

			//if (panelCommands.Enabled == _isInError)
			//{
			//    panelCommands.Enabled = !_isInError;
			//}

			panelCommands.Enabled = (CurrentSymbolDetail != "");
			panelCommandsEx.Enabled = (CurrentSymbolDetail != "");

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

			if (_lastIsFuture != IsFuture)
			{
				_lastIsFuture = IsFuture;

				if (_lastSymbolDetail == "")
				{
					//DDAccountDropDown.SelectedExchange = "";
					//DDAccountDropDown.SelectedLocalAccountAcrn = "";
					//DDAccountDropDown.SelectedTraderFor = "";
					//DDAccountDropDown.LoadUserAccounts();

					UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(TicketDefaults.Option, CSVEx.CSVFieldIDs.SecutrityTypes.Option));
				}
				else
				{
					UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Option));
				}
			}

			if (IsFuture)
			{
				panelExchanges.Visible = false;
				//chkMonthlyOnly.Visible = false;
			}
			else
			{
				panelExchanges.Visible = true;
				chkMonthlyOnly.Visible = true;
			}

			CheckMarketData();
		}

		private void CheckMarketData()
		{
			if (Configuration.User.Default.UseMarketData && !GLOBAL.MDSsConnected)
			{
				lblMarketDataError.Visible = true;
				lblMarketDataError.Blink = true;
			}
			else
			{
				lblMarketDataError.Visible = false;
				lblMarketDataError.Blink = false;
			}
		}

		private void Form_IconClicked(object sender, EventArgs e)
		{
			try
			{
				Clone();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_FitClicked(object sender, EventArgs e)
		{
			try
			{
				_lastSize = Size;
				FitWidth(true);
				FitHeight();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_ShowAllClicked(object sender, EventArgs e)
		{
			try
			{
				GLOBAL.HWindows.ShowAll();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_SizeChanged(object sender, EventArgs e)
		{
			if (!_resizeGridToFit && !rocOptionList.Processing && rocOptionList.Rows.Count > 0)
			{
				_widthChanged = (_lastSize.Width != Width);
				_heightChanged = (_lastSize.Height != Height);

				if (_widthChanged || _heightChanged)
				{
					_lastSize = Size;
					if (_widthChanged)
					{
						FitWidth(false);
					}
					if (_heightChanged)
					{
						FitHeight();
					}
				}
			}
		}

		private void FitWidth(bool manual)
		{
			_widthChanged = false;
			_resizeGridToFit = manual;

			//if (_resizeGridToFit)
			//{
			//    Size = new Size(panelStrike.Right + 5, Height);
			//    //Size = MinimumSize;

			//    lock (rocOptionList)
			//    {
			//        SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.Fill);
			//        ResizeToFit(rocOptionList);
			//        SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.None);
			//    }
			//}
			//else
			//{
			//    lock (rocOptionList)
			//    {
			//        SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.Fill);
			//        SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.None);
			//    }
			//}

			try
			{
				if (!IsProcessing)
				{
					if (_resizeGridToFit)
					{
						lock (rocOptionList)
						{
							SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.DisplayedCells);
							ResizeToFit(rocOptionList);
							SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.None);
						}
					}
					else
					{
						SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.Fill);
						SetColumnMode(rocOptionList, DataGridViewAutoSizeColumnMode.None);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			int diff = MinimumSize.Width - Width;

			if (diff > 0)
			{
				int deltaUpper = diff / 9;

				int deltaCommands = diff / 4;

				cmdBuyOpen.Width = cmdBuyOpen.MinimumSize.Width + deltaCommands;
				cmdBuyClose.Width = cmdBuyClose.MinimumSize.Width + deltaCommands;
				cmdSellOpen.Width = cmdSellOpen.MinimumSize.Width + deltaCommands;
				cmdSellClose.Width = cmdSellClose.MinimumSize.Width + deltaCommands;
			}
			else
			{
				cmdBuyOpen.Width = cmdBuyOpen.MinimumSize.Width;
				cmdBuyClose.Width = cmdBuyClose.MinimumSize.Width;
				cmdSellOpen.Width = cmdSellOpen.MinimumSize.Width;
				cmdSellClose.Width = cmdSellClose.MinimumSize.Width;
			}

			panelExchanges.Padding = new Padding((panelExchanges.Width - panelExchangeSelection.Width) / 2, 3, 0, 0);

			_resizeGridToFit = false;
		}

		private void FitHeight()
		{
			_heightChanged = false;
		}

		private void Form_Load(object sender, EventArgs e)
		{
			try
			{
				DDAccountDropDown.LoadUserAccounts();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			StopTimer();

			_ticketClosed = true;

			_orderTypesView = null;
			_durationsView = null;

			FitClicked -= new EventHandler(Form_FitClicked);

			Load -= new EventHandler(Form_Load);
			FormClosing -= new FormClosingEventHandler(Form_FormClosing);
			SizeChanged -= new EventHandler(Form_SizeChanged);

			rocOptionList.Dispose();
			rocOptionList.PropertyChanged -= new PropertyChangedEventHandler(rocOptionList_PropertyChanged);
			rocOptionList.MouseEnter -= new EventHandler(rocOptionList_MouseEnter);

			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		private void Form_Activated(object sender, EventArgs e)
		{
			ActiveControl = cboSymbolDetails;
			cboSymbolDetails.Select();
		}

		#endregion

		#region - Grid Event -

		private delegate void rocOptionList_ProcessingChangedDelegate(object sender, EventArgs e);
		private void rocOptionList_ProcessingChanged(object sender, EventArgs e)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new rocOptionList_ProcessingChangedDelegate(rocOptionList_ProcessingChanged), new object[] { sender, e });
				return;
			}

			try
			{
				if (IsProcessing)
				{
					cboSymbolDetails.Enabled = false;
					cboStrikesToShow.Enabled = false;
					cboExpirationToShow.Enabled = false;
					cboExpiration.Enabled = false;
					cboStrike.Enabled = false;
					cboExpirationToShow.Enabled = false;
					cboStrikesToShow.Enabled = false;
					CaptionBar.Enabled = false;

					rocOptionList.Enabled = false;
					rocOptionList.Visible = false;

					rocOptionList.ScrollBars = ScrollBars.None;
					menuProcessing.Show();

					//GLOBAL.HROC.AddToStatusLogs("IsProcessing rocOptionList Rows = " + rocOptionList.Rows.Count.ToString());
				}
				else
				{
					cboSymbolDetails.Enabled = true;
					cboStrikesToShow.Enabled = true;
					cboExpirationToShow.Enabled = true;
					cboExpiration.Enabled = true;
					cboStrike.Enabled = true;
					cboExpirationToShow.Enabled = true;
					cboStrikesToShow.Enabled = true;
					CaptionBar.Enabled = true;

					// TODO: When Offloading DataTable in Another thread, the DataSource in IDE seem to be needs to
					//		 Rebinded.
					if (rocOptionList.Rows.Count < 2)
					{
						GLOBAL.HROC.AddToStatusLogs("Rebind rocOptionList.DataSource.");
						rocOptionList.DataSource = null;
						rocOptionList.SearchView = null;
						rocOptionList.SearchViewFull = null;
						rocOptionList.StrikeSearchView = null;
						rocOptionList.CallSearchView = null;
						rocOptionList.PutSearchView = null;
						rocOptionList.DataSource = rocOptionList.RocGridTable.DefaultView;
						rocOptionList.Refresh();
					}

					rocOptionList.Enabled = true;
					rocOptionList.Visible = true;

					rocOptionList.ScrollBars = rocOptionList.OriginalScrollBars;
					if (rocOptionList.OptionSymbols.Count > 1)
					{
						rocOptionList.ShowOptionSymbolColumn = true;
					}

					//if (Is64Th)
					//{
					//    rocOptionList.Show64Th = true;
					//}
					menuProcessing.Hide();

					//FitWidth(false);

					// TODO Load on Startup
					lock (GLOBAL.HMarketData.Current)
					{
						//List<string> symbolList = new List<string>();
						//if (GLOBAL.HMarketData.Current.Count > 0)
						//{
						//    string[] optionSymbols = new string[GLOBAL.HMarketData.Current.Count];
						//    GLOBAL.HMarketData.Current.Keys.CopyTo(optionSymbols, 0);
						//    symbolList.AddRange(optionSymbols);
						//    symbolList.Sort();
						//}
						//UpdateMarketDataDeltas(CurrentSymbolDetail, new Dictionary<string, MDServerToClient>(GLOBAL.HMarketData.Current), symbolList);
						// Update Level 1 only
						if (GLOBAL.HMarketData.Current.ContainsKey(CurrentSymbolDetail))
						{
							UpdateMarketDataDelta(GLOBAL.HMarketData.Current[CurrentSymbolDetail]);
						}
					}

					System.Threading.Thread.Sleep(100);

					//GLOBAL.HROC.AddToStatusLogs("rocOptionList Rows = " + rocOptionList.Rows.Count.ToString());
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private delegate void rocOptionList_RefreshSharedRowsChangedDelegate(object sender, EventArgs e);
		private void rocOptionList_RefreshSharedRowsChanged(object sender, EventArgs e)
		{
			if (InvokeRequired)
			{
				BeginInvoke(new rocOptionList_RefreshSharedRowsChangedDelegate(rocOptionList_RefreshSharedRowsChanged), new object[] { sender, e });
				return;
			}

			if (rocOptionList.RefreshSharedRows)
			{
				rocOptionList.RefreshSharedRows = false;

				if (!_resetExpirationOnly)
				{
					_expirationView = new DataView(expirationTable, "", "ExpDateTime", DataViewRowState.CurrentRows);
					cboExpiration.DataSource = _expirationView;
					cboExpiration.DisplayMember = "ExpDisplay";

					//if (!IsFuture && ShowMonthlyExpirationsOnly)
					if (ShowMonthlyExpirationsOnly)
					{
						foreach (DataRow row in _expirationView.Table.Rows)
						{
							DateTime monthlyExpDate = WeekHelper.GetMonthlyExpirationDay((DateTime)row["ExpDateTime"]);
							if (IsFuture)
							{
								if (rocOptionList.Expirations.Contains((DateTime)row["ExpDateTime"]) || WeekHelper.IsThridWeek((DateTime)row["ExpDateTime"]) || WeekHelper.IsEndOfMonth((DateTime)row["ExpDateTime"]))
								{
									// Monthly and Quaterly
									cboExpiration.Text = row["ExpDisplay"].ToString();
									break;
								}
							}
							else
							{
								if (monthlyExpDate == (DateTime)row["ExpDateTime"])
								{
									cboExpiration.Text = row["ExpDisplay"].ToString();
									break;
								}
							}
						}
					}

					_strikeView = new DataView(strikeTable, "", "StrikePrice", DataViewRowState.CurrentRows);
					cboStrike.DataSource = _strikeView;
					cboStrike.DisplayMember = "StrikeString";
				}

				if (StrikeKeys.Contains(rocOptionList.OptionStrike))
				{
					cboStrike.Text = rocOptionList.OptionStrike.ToString(GetStrikFormat(rocOptionList.OptionStrike));
				}
			}
		}

		private void rocOptionList_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "ShowColumnMenu":
					ShowColumnMenu();
					break;
				case "OptionPriceSelected":
					PriceSelected();
					break;
				case "Subscribe":
					SubscribeToVisiable();
					break;
				case "RefreshNBBO":
					RefreshNBBO();
					break;
				case "RelodOptionChain":
					if (!IsProcessing && rocOptionList.OptionLoaded && _resetExpirationOnly)
					{
						//Debug.Print("RelodOptionChain");
						ResetOptionChain();

						rocOptionList.StrikeRange = StrikeRange;
						rocOptionList.ExpirationRange = ExpirationRange;
					}
					break;
			}
		}

		private void PriceSelected()
		{
			switch (rocOptionList.OptionCallPut)
			{
				case CSVFieldIDs.OptionTypes.Call:
					cboCallPut.Text = "Call";
					break;
				case CSVFieldIDs.OptionTypes.Put:
					cboCallPut.Text = "Put";
					break;
			}

			if (StrikeKeys.Contains(rocOptionList.OptionStrike))
			{
				cboStrike.Text = rocOptionList.OptionStrike.ToString(GetStrikFormat(rocOptionList.OptionStrike));
			}

			if (ExpDateKeys.Contains(rocOptionList.OptionExpiration))
			{
				//cboExpiration.Text = rocOptionList.OptionExpiration.ToString("yy MMM dd").ToUpper();
				cboExpiration.Text = ConvertToDisplayDate(rocOptionList.OptionExpiration);
			}

			if (rocOptionList.OptionExchangeName != "")
			{
				foreach (string item in cboExchange.Items)
				{
					if (rocOptionList.OptionExchangeName.Contains(item) || item.Contains(rocOptionList.OptionExchangeName))
					{
						cboExchange.Text = item;
						break;
					}
				}
			}

			_crrentPriceObj.Value = Convert.ToDecimal(rocOptionList.OptionPrice);

			TogglePriceSelection();
		}

		private void SubscribeToVisiable()
		{
			if (IsFuture)
			{
				SubscribeToVisiable(rocOptionList.MDPutSymbol, rocOptionList.MDSource, rocOptionList.SecType);
				SubscribeToVisiable(rocOptionList.MDCallSymbol, rocOptionList.MDSource, rocOptionList.SecType);
			}
			else
			{
				if (_showOnlySelectedExchange)
				{
					if (rocOptionList.MDPutSymbol == rocOptionList.MDPutSymbolOPRA)
					{
						SubscribeToVisiable(rocOptionList.MDPutSymbol, OptionDataSource.OPRANBBO, CSVFieldIDs.SecutrityTypes.Option);
					}
					else
					{
						SubscribeToVisiable(rocOptionList.MDPutSymbolOPRA, OptionDataSource.OPRA, CSVFieldIDs.SecutrityTypes.Option);
					}

					if (rocOptionList.MDCallSymbol == rocOptionList.MDCallSymbolOPRA)
					{
						SubscribeToVisiable(rocOptionList.MDCallSymbol, OptionDataSource.OPRANBBO, CSVFieldIDs.SecutrityTypes.Option);
					}
					else
					{
						SubscribeToVisiable(rocOptionList.MDCallSymbolOPRA, OptionDataSource.OPRA, CSVFieldIDs.SecutrityTypes.Option);
					}
				}
				else
				{
					SubscribeToVisiable(rocOptionList.MDPutSymbol, CurrentSymbolDetail);
					SubscribeToVisiable(rocOptionList.MDCallSymbol, CurrentSymbolDetail);
				}
			}
		}

		private void SubscribeToVisiable(string mdSymbol, string mdSource, string secType)
		{
			lock (_subQueue)
			{
				if (!_subQueue.ContainsKey(mdSymbol))
				{
					_subQueue.Add(mdSymbol, new SubscriptionQueue(mdSymbol, mdSource, secType));
				}
			}

			//if (!MDSymbols.Contains(mdSymbol))
			//{
			//    HelperSubscriber.Subscribe(mdSymbol, mdSource, secType);
			//}
		}

		private void SubscribeToVisiable(string mdSymbol, string currentSymbolDetail)
		{
			lock (_subOptionQueue)
			{
				if (!_subOptionQueue.ContainsKey(mdSymbol))
				{
					_subOptionQueue.Add(mdSymbol, new SubscriptionQueue(mdSymbol, currentSymbolDetail));
				}
			}

			//if (!MDSymbols.Contains(mdSymbol))
			//{
			//    HelperSubscriber.SubscribeOption(mdSymbol, currentSymbolDetail, true);
			//}
		}

		private void SubscribeByQueue()
		{
			lock (_subQueue)
			{
				if (_subQueue.Count > 0)
				{
					foreach (string key in _subQueue.Keys)
					{
						if (!MDSymbols.Contains(key))
						{
							HelperSubscriber.Subscribe(_subQueue[key].MDSymbol, _subQueue[key].MDSource, _subQueue[key].SecType);
						}
					}
				}
				_subQueue.Clear();
			}

			lock (_subOptionQueue)
			{
				if (_subOptionQueue.Count > 0)
				{
					foreach (string key in _subOptionQueue.Keys)
					{
						if (!MDSymbols.Contains(key))
						{
							HelperSubscriber.SubscribeOption(_subOptionQueue[key].MDSymbol, _subOptionQueue[key].CurrentSymbolDetail, true);
						}
					}
				}
				_subOptionQueue.Clear();
			}
		}

		private void rocOptionList_MouseEnter(object sender, EventArgs e)
		{
			if (Configuration.ROC.Default.EnableAutoFocusOptionTicket)
			{
				ActivateGrid(rocOptionList);
			}
		}

		#endregion

		#region - Price Event -

		private void PriceSelection_Click(object sender, EventArgs e)
		{
			try
			{
				if (((Control)sender).Name.ToUpper().Contains("LIMITPRICE"))
				{
					SelectLimitPrice();
				}
				else if (((Control)sender).Name.ToUpper().Contains("STOPPRICE"))
				{
					SelectStopPrice();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Price_MouseDown(object sender, MouseEventArgs e)
		{
			try
			{
				((LabelBase)sender).ForeColor = Color.LimeGreen;
				if (((LabelBase)sender).Value != null && ((LabelBase)sender).Value.ToString() != "")
				{
					_crrentPriceObj.Value = Convert.ToDecimal(((LabelBase)sender).Value);
				}

				TogglePriceSelection();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Price_MouseUp(object sender, MouseEventArgs e)
		{
			try
			{
				((LabelBase)sender).ForeColor = Color.Gainsboro;
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void TogglePriceSelection()
		{
			if (panelLimitPrice.Enabled && panelStopPrice.Enabled)
			{
				if (_crrentPriceObj.Name.ToUpper().Contains("LIMITPRICE"))
				{
					SelectStopPrice();
				}
				else
				{
					SelectLimitPrice();
				}
			}
		}

		private void SelectLimitPrice()
		{
			_crrentPriceObj = numLimitPrice;
			panelLimitPrice.BackColor = Color.CornflowerBlue;
			panelStopPrice.BackColor = Color.Transparent;
		}

		private void SelectStopPrice()
		{
			_crrentPriceObj = numStopPrice;
			panelLimitPrice.BackColor = Color.Transparent;
			panelStopPrice.BackColor = Color.CornflowerBlue;
		}

		#endregion

		#region - Ticket Event -

		// When any of the User selected fields are changed
		private void Ticket_ValueChanged(object sender, EventArgs e)
		{
			try
			{
				//Debug.Print(((Control)sender).Name + " Ticket_ValueChanged().ValidateTicketValues()");
				ValidateTicketValues(((Control)sender).Name);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cboExpiration_MouseClick(object sender, MouseEventArgs e)
		{
			// User had changed to another month that is may not be in the current display range
			_resetExpirationOnly = true;
		}

		private void chkMonthlyOnly_CheckedChanged(object sender, EventArgs e)
		{
			_resetExpirationOnly = false;
			OptionRange_SelectedIndexChanged(sender, e);
		}

		private void cmdCenterOnStrike_Click(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			rocOptionList.CenterOnStrike();
		}

		private void dspLimitPrice_KeyDown(object sender, KeyEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.KeyData)
			{
				case Keys.Enter:
				case Keys.Tab:
					SetPriceDisplayFromUserInput(PriceType.Limit);
					break;
			}
		}

		private void dspLimitPrice_Leave(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			SetPriceDisplayFromUserInput(PriceType.Limit);
		}

		//private void dspLimitPrice_Leave(object sender, MouseEventArgs e)
		//{
		//    GLOBAL.Interrupt = true;
		//    SetPriceDisplayFromUserInput(PriceType.Limit);
		//}

		private void dspLimitPrice_MouseEnter(object sender, EventArgs e)
		{
			Activate();
			ActiveControl = numLimitPrice;
		}

		private void dspStopPrice_KeyDown(object sender, KeyEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.KeyData)
			{
				case Keys.Enter:
				case Keys.Tab:
					SetPriceDisplayFromUserInput(PriceType.Stop);
					break;
			}
		}

		private void dspStopPrice_Leave(object sender, EventArgs e)
		{
			GLOBAL.Interrupt = true;
			SetPriceDisplayFromUserInput(PriceType.Stop);
		}

		//private void dspStopPrice_Leave(object sender, MouseEventArgs e)
		//{
		//    GLOBAL.Interrupt = true;
		//    SetPriceDisplayFromUserInput(PriceType.Stop);
		//}

		private void dspStopPrice_MouseEnter(object sender, EventArgs e)
		{
			Activate();
			ActiveControl = numStopPrice;
		}

		private void panelStopPrice_EnabledChanged(object sender, EventArgs e)
		{
			panelStopPrice.Visible = panelStopPrice.Enabled;
		}

		private void Control_KeyUp(object sender, KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Tab)
			{
				switch (((Control)sender).Name)
				{
					case "cboSymbolDetails":
						ActiveControl = numQuantity;
						numQuantity.Select(0, numQuantity.Text.Length);
						break;
					case "numQuantity":
					case "cboOrder":
						if (panelLimitPrice.Visible)
						{
							ActiveControl = dspLimitPrice;
							dspLimitPrice.Select();
						}
						else if (panelStopPrice.Visible)
						{
							ActiveControl = dspStopPrice;
							dspStopPrice.Select();
						}
						else
						{
							ActiveControl = cboSymbolDetails;
							cboSymbolDetails.Select();
						}
						break;
					case "numLimitPrice":
					case "dspLimitPrice":
						if (panelStopPrice.Visible)
						{
							ActiveControl = dspStopPrice;
							dspStopPrice.Select();
						}
						else
						{
							ActiveControl = cboSymbolDetails;
							cboSymbolDetails.Select();
						}
						break;
					case "numStopPrice":
					case "dspStopPrice":
					case "cboDuration":
					case "cboCallPut":
					case "cboExchange":
					case "cboExpiration":
					case "cboStrike":
						ActiveControl = cboSymbolDetails;
						cboSymbolDetails.Select();
						break;
				}
			}
		}

		private void OptionRange_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (!IsProcessing)
			{
				ResetOptionChain();

				rocOptionList.StrikeRange = StrikeRange;
				rocOptionList.ExpirationRange = ExpirationRange;
			}
		}

		#endregion

		#region - Binding -

		#region - Refresh & First Load -

		private void SubscribeToSymbol(string symbolDetail)
		{
			try
			{
				//_orderLoaded = false;

				UpdateSecurityInfo(symbolDetail);

				// Load Option Chain Data
				//UpdateMarketDataDeltas(symbolDetail, new Dictionary<string, MDServerToClient>(GLOBAL.HMarketData.Current));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#region - Used By Process Thread -

		private delegate void UpdateTicketByProcessDelegate(bool updateIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas, List<string> symbolList);
		public void UpdateTicketByProcess(bool updateIM, List<ROCOrder> orders, Dictionary<string, MDServerToClient> deltas, List<string> symbolList)
		{
			if (GLOBAL.UseDelayedUpdate)
			{
				try
				{
					if (updateIM)
					{
						_updateIM = updateIM;
					}
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
					BeginInvoke(new UpdateTicketByProcessDelegate(UpdateTicketByProcess), new object[] { updateIM, orders, deltas, symbolList });
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
						UpdateMarketDataDeltas(CurrentSecInfo.MDSymbol, deltas, symbolList);
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
						if (!rocOptionList.OptionLoaded)
						{
							if (!IsProcessing)
							{
								UpdateSecurityInfo();
							}
						}
						else
						{
							_updateIM = false;
						}
					}
					if (rocOptionList.OptionLoaded && !IsProcessing)
					{
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
							//List<string> symbolList = new List<string>();
							//symbolList = GetOptionSymbolRange(deltas, symbolList);
							//UpdateMarketDataDeltas(CurrentSecInfo.MDSymbol, deltas, symbolList);
							UpdateMarketDataOptionDeltas(deltas);
						}
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
					CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);
					if (CurrentSecInfo != null && CurrentSecInfo.SecType != "")
					{
						UpdateIMInfo(symbolDetail, CurrentSecInfo);

						if (!gotOptionChain)
						{
							if (!GettingOptionChainSymbols.Contains(symbolDetail))
							{
								GLOBAL.HRDS.GetOptionChain(symbolDetail);
							}
						}
						else
						{
							if (!removeList.Contains(symbolDetail))
							{
								removeList.Add(symbolDetail);
							}

							if (!GettingOptionChainSymbols.Contains(symbolDetail))
							{
								GettingOptionChainSymbols.Add(symbolDetail);
								PlusOneCheck();
								Show64ThCheck();
							}

							UpdateTicket();
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
			HelperSubscriber.Subscribe(secInfo.MDSymbol, secInfo.MDSource, secInfo.SecType);
			switch (secInfo.SecType)
			{
				case CSVFieldIDs.SecutrityTypes.Equity:
				case CSVFieldIDs.SecutrityTypes.Future:
				case CSVFieldIDs.SecutrityTypes.OptionIndex:
				case CSVFieldIDs.SecutrityTypes.OptionFuture:
					// Only subscribe to visiable options
					//if (secInfo.OptionChain.Count > 0)
					//{
					//    foreach (IMOptionInfo opt in secInfo.OptionChain.Values)
					//    {
					//        // Subscribe to the first two month by default;
					//        if (opt.ExpDateDT != null &&
					//            (opt.ExpDateDT.Value.Month == DateTime.Now.Month ||
					//            opt.ExpDateDT.Value.Month == nextMonth.Month))
					//        {
					//            if (IsFuture)
					//            {
					//                HelperSubscriber.Subscribe(opt.MDSymbol, opt.MDSource, opt.SecType);
					//            }
					//            else
					//            {
					//                HelperSubscriber.SubscribeOption(opt.MDSymbol);
					//            }

					//            if (!MDSymbols.Contains(opt.MDSymbol))
					//            {
					//                MDSymbols.Add(opt.MDSymbol);
					//            }
					//        }
					//    }
					//}
					break;
				default:
					DDSymbolDetails.DeleteSymbolDetail(CurrentSymbolDetail);
					break;
			}
		}

		#endregion

		// Update the ticket with all current IM info
		private void UpdateSecurityInfo(string symbolDetail)
		{
			if (!DoNotSend(symbolDetail))
			{
				UpdateOptionTicketWithSecurityInfo(symbolDetail);
				GLOBAL.HRDS.GetSecurityByTicker(symbolDetail);
			}
			else
			{
				lblDoNotTranslate.Dock = DockStyle.Fill;
				lblDoNotTranslate.Visible = true;
				lblDoNotTranslate.Blink = true;
			}
		}

		// Update with Security Info On Play back & onLoad
		private void UpdateOptionTicketWithSecurityInfo(string symbolDetail)
		{
			CurrentSecInfo = GLOBAL.HRDS.GetSecurityInfoBySymbolDetail(symbolDetail);

			lock (ImSymbolNeeded)
			{
				if (!ImSymbolNeeded.ContainsKey(symbolDetail))
				{
					// Should have only the current symbol in the list;
					ImSymbolNeeded.Clear();
					ImSymbolNeeded.Add(symbolDetail, CurrentSecInfo.MDSymbol);
				}
				else
				{
					ImSymbolNeeded[symbolDetail] = CurrentSecInfo.MDSymbol;
				}
			}
		}

		// Update with Security Info On Play back & onProcess
		private void UpdateMarketDataDeltas(string symbolDetail, Dictionary<string, MDServerToClient> deltas, List<string> symbolList)
		{
			// Update Level 1
			if (deltas.ContainsKey(symbolDetail))
			{
				UpdateMarketDataDelta(deltas[symbolDetail]);
			}

			// Update Option Deltas
			if (rocOptionList.OptionLoaded)
			{
				//if (IsFuture)
				//{
					UpdateMarketDataOptionDeltas(deltas);
				//}
				//else
				//{
				//    symbolList = GetOptionSymbolRange(deltas, symbolList);
				//    if (symbolList.Count == 0)
				//    {
				//        UpdateMarketDataOptionDeltas(deltas);
				//    }
				//    else
				//    {
				//        UpdateMarketDataOptionDeltas(deltas, symbolList);
				//    }
				//}
			}
		}

		// Update with matching market data Level 1
		private void UpdateMarketDataDelta(MDServerToClient delta)
		{
			lock (panelTicker)
			{
				panelTicker.SuspendLayout();

				bool newBidPrice = false;
				bool newAskPrice = false;
				bool newTradePrice = false;

				if (delta.BidPrice != null)
				{
					dspBidPrice.Value = (double)delta.BidPrice;
					newBidPrice = true;
				}

				if (delta.BidSize != null)
				{
					dspBidSize.Value = (long)delta.BidSize;
				}

				if (delta.AskPrice != null)
				{
					dspAskPrice.Value = (double)delta.AskPrice;
					newAskPrice = true;
				}

				if (delta.AskSize != null)
				{
					dspAskSize.Value = (long)delta.AskSize;
				}

				if (delta.TradePrice != null && delta.TradePrice != 0)
				{
					dspTradedPrice.Value = (double)delta.TradePrice;
					newTradePrice = true;
				}
				//else
				//{
				//    if (delta.ClosePrice != null && delta.ClosePrice != 0)
				//    {
				//        dspTradedPrice.Value = (double)delta.ClosePrice;
				//        newTradePrice = true;
				//    }
				//    else if (delta.PrevClosePrice != null && delta.PrevClosePrice != 0)
				//    {
				//        dspTradedPrice.Value = (double)delta.PrevClosePrice;
				//        newTradePrice = true;
				//    }
				//}

				if (rocOptionList.Rows.Count > 0 && rocOptionList.OptionLoaded)
				{
					if (newTradePrice && dspTradedPrice.Value != null)
					{
						MarkAtMoney(GetAtMoneyStrike((double)dspTradedPrice.Value));
					}
					else if (newBidPrice && dspBidPrice.Value != null)
					{
						MarkAtMoney(GetAtMoneyStrike((double)dspBidPrice.Value));
					}
					else if (newAskPrice && dspAskPrice.Value != null)
					{
						MarkAtMoney(GetAtMoneyStrike((double)dspAskPrice.Value));
					}
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

		#region - Find At Money Strik -

		private double GetCurrentMoney()
		{
			if (dspTradedPrice.Value != null && (double)dspTradedPrice.Value != 0)
			{
				return Math.Round((double)dspTradedPrice.Value, 7);
			}
			else if (dspBidPrice.Value != null && (double)dspBidPrice.Value != 0)
			{
				return Math.Round((double)dspBidPrice.Value, 7);
			}
			else if (dspAskPrice.Value != null && (double)dspAskPrice.Value != 0)
			{
				return Math.Round((double)dspAskPrice.Value, 7);
			}

			return 0;
		}

		private double GetAtMoneyStrike(double price)
		{
			double tPrice = price;
			if (IsFuture)
			{
				tPrice = Math.Floor(price * rocOptionList.CommonFactor);
			}
			double atMoneyPrice = tPrice;

			bool found = false;
			//int steps = 1;

			double atMoneyPriceLow = 0;
			foreach (double stkPrice in StrikeKeys)
			{
				if (tPrice == stkPrice)
				{
					atMoneyPrice = stkPrice;
					found = true;
					break;
				}
				else if (tPrice < stkPrice)
				{
					break;
				}
				else if (tPrice > stkPrice)
				{
					atMoneyPriceLow = stkPrice;
				}
			}
			if (!found)
			{
				atMoneyPrice = atMoneyPriceLow;
				found = true;
			}

			/*while (!found)
			{

				if (StrikeKeys.Contains(tPrice))
				{
					atMoneyPrice = tPrice;
					found = true;
				}
				else if (StrikeKeys.Contains(tPrice + steps))
				{
					atMoneyPrice = tPrice + steps;
					found = true;
				}
				else if (StrikeKeys.Contains(tPrice - steps))
				{
					atMoneyPrice = tPrice - steps;
					found = true;
				}

				if (!found)
				{
					if (rocOptionList.StrikeRange > 0 || steps > rocOptionList.StrikeRange)
					{
						// Not Found
						atMoneyPrice = 0;
						break;
					}
					steps++;
				}
				else
				{
					break;
				}
			}*/

			return atMoneyPrice;
		}

		private void MarkAtMoney(double atMoney)
		{
			try
			{
				if (rocOptionList.AtMoney != atMoney)
				{
					rocOptionList.AtMoney = atMoney;

					//if (!HasFirstUpdate)
					//{
					//    HasFirstUpdate = true;
					//    numLimitPrice.Value = Math.Round(Convert.ToDecimal(rocOptionList.AtMoney), CurrentSecInfo.DecimalPlaces);
					//}

					int center = 0;
					Dictionary<int, double> _levels = new Dictionary<int, double>();
					lock (StrikeKeys)
					{
						StrikeKeys.Sort();
						center = FindAtMoney();
						if (StrikeKeys.Count > center)
						{
							_levels.Add(1, StrikeKeys[center]);
						}
					}

					if (_levels.Count > 0)
					{
						for (int count = 1; count < rocOptionList.ColorShiftIntervalMax; count++)
						{
							if (center - count >= 0)
							{
								_levels.Add(-(count + 1), StrikeKeys[center - count]);
							}
							if (center + count < StrikeKeys.Count)
							{
								_levels.Add(count + 1, StrikeKeys[center + count]);
							}
						}

						lock (rocOptionList.RocGridTable)
						{
							ResetAtMoney();

							//GLOBAL.HROC.AddToStatusLogs(rocOptionList.SearchView.Table.Rows.Count.ToString());

							DataRowView[] rows = new DataRowView[0];
							foreach (int key in _levels.Keys)
							{
								foreach (DateTime expDate in ExpDateKeys)
								{
									rows = rocOptionList.SearchView.FindRows(new object[] { expDate, _levels[key] });

									foreach (DataRowView row in rows)
									{
										row["MoneyFlag"] = Math.Abs(key);
									}
								}
							}
						}

						if (!_isAtMoney)
						{
							_isAtMoney = true;
							rocOptionList.CenterOnRow(rocOptionList.AtMoney);
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private int FindAtMoney()
		{
			int center = 0;

			if (rocOptionList.AtMoney > 0)
			{
				foreach (double strike in StrikeKeys)
				{
					if (strike == rocOptionList.AtMoney ||
						strike == rocOptionList.AtMoney * 10 ||
						strike == rocOptionList.AtMoney * 100 ||
						strike == rocOptionList.AtMoney * 1000 ||
						strike == rocOptionList.AtMoney * 10000 ||
						strike == rocOptionList.AtMoney * 100000 ||
						strike == rocOptionList.AtMoney * 1000000)
					{
						break;
					}

					center++;
				}
			}

			return center;
		}

		private void ResetAtMoney()
		{
			DataRow[] rows = rocOptionList.RocGridTable.Select("MoneyFlag <> 0");

			if (rows.Length > 0)
			{
				foreach (DataRow row in rows)
				{
					if (row != null)
					{
						row["MoneyFlag"] = 0;
					}
				}
			}
		}

		#endregion

		private List<string> GetOptionSymbolRange(Dictionary<string, MDServerToClient> deltas, List<string> symbolList)
		{
			List<string> result = new List<string>();

			try
			{
				if (symbolList.Count == 0)
				{
					string[] optionSymbols = new string[deltas.Count];
					deltas.Keys.CopyTo(optionSymbols, 0);
					symbolList.AddRange(optionSymbols);
					symbolList.Sort();
				}

				string mdSymbol = "";
				// ICE option symbol filter
				//if (mdSymbol.EndsWith("!"))
				//{
				//    mdSymbol = mdSymbol.TrimEnd(new char[] { '!' });
				//}

				if (rocOptionList.PutKeys.Count > 0)
				{
					mdSymbol = rocOptionList.PutKeys[0];
				}
				else if (rocOptionList.CallKeys.Count > 0)
				{
					mdSymbol = rocOptionList.CallKeys[0];
				}
				else
				{
					return result;
				}

				if (mdSymbol.Contains("_"))
				{
					mdSymbol = mdSymbol.Substring(0, mdSymbol.IndexOf("_")) + "_";
				}
				else if (mdSymbol.Contains(" "))
				{
					mdSymbol = mdSymbol.Substring(0, mdSymbol.IndexOf(" ")) + " ";
				}

				int idleCount = 0;
				bool isInRange = false;
				for (int index = 0; index < symbolList.Count; index++)
				{
					if (idleCount > _idleLimit)
					{
						idleCount = 0;
						System.Threading.Thread.Sleep(1);
						Application.DoEvents();
					}

					if (symbolList[index].Contains(mdSymbol))
					{
						// Flag the range for the option symbol had been reached.
						isInRange = true;

						if (!symbolList[index].Contains("."))
						{
							//if (symbolList[index] != mdSymbol && !result.Contains(symbolList[index]))
							if (!result.Contains(symbolList[index]))
							{
								result.Add(symbolList[index]);
							}
						}
					}
					else
					{
						if (isInRange)
						{
							// We are out of the option symbol range again.
							break;
						}
					}

					idleCount++;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return result;
		}

		// Update with matching market data option chain OPRA and NBBO
		private void UpdateMarketDataOptionDeltas(Dictionary<string, MDServerToClient> deltas)
		{
			DataRowView[] rows = new DataRowView[0];

			lock (rocOptionList.RocGridTable)
			{
				foreach (MDServerToClient delta in deltas.Values)
				{
					if (delta.uParticipentSymbol == null || delta.uParticipentSymbol == "")
					{
						if (!IsFuture)
						{
							// NBBO
							if (rocOptionList.PutKeys.Contains(delta.uSymbol))
							{
								if (rocOptionList.FilterInAll)
								{
									rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, "" });
									UpdateMarketDataOptionDelta(delta, rows, "Put", "", false);
								}

								if (delta.AskPartId != null && delta.AskPartId != "")
								{
									rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, delta.AskPartId });
									UpdateMarketDataOptionDelta(delta, rows, "Put", delta.AskPartId, !rocOptionList.FilterInAll);
								}

								if (delta.BidPartId != null && delta.BidPartId != "")
								{
									rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, delta.BidPartId });
									UpdateMarketDataOptionDelta(delta, rows, "Put", delta.BidPartId, !rocOptionList.FilterInAll);
								}
							}
							else if (rocOptionList.CallKeys.Contains(delta.uSymbol))
							{
								if (rocOptionList.FilterInAll)
								{
									rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, "" });
									UpdateMarketDataOptionDelta(delta, rows, "Call", "", false);
								}

								if (delta.AskPartId != null && delta.AskPartId != "")
								{
									rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, delta.AskPartId });
									UpdateMarketDataOptionDelta(delta, rows, "Call", delta.AskPartId, !rocOptionList.FilterInAll);
								}

								if (delta.BidPartId != null && delta.BidPartId != "")
								{
									rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, delta.BidPartId });
									UpdateMarketDataOptionDelta(delta, rows, "Call", delta.BidPartId, !rocOptionList.FilterInAll);
								}
							}
						}
						else
						{
							// Future
							if (rocOptionList.PutKeys.Contains(delta.uSymbol))
							{
								rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, "" });
								UpdateMarketDataOptionDelta(delta, rows, "Put", "", false);
							}
							else if (rocOptionList.CallKeys.Contains(delta.uSymbol))
							{
								rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, "" });
								UpdateMarketDataOptionDelta(delta, rows, "Call", "", false);
							}
						}
					}
					else
					{
						// Normal
						if (rocOptionList.PutKeys.Contains(delta.uSymbol))
						{
							rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, delta.uParticipentSymbol });
							UpdateMarketDataOptionDelta(delta, rows, "Put", delta.uParticipentSymbol, !rocOptionList.FilterInAll);
						}
						else if (rocOptionList.CallKeys.Contains(delta.uSymbol))
						{
							rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, delta.uParticipentSymbol });
							UpdateMarketDataOptionDelta(delta, rows, "Call", delta.uParticipentSymbol, !rocOptionList.FilterInAll);
						}
					}

					if (GLOBAL.Interrupt && !IsProcessing)
					{
						GLOBAL.Interrupt = false;
						Application.DoEvents();
					}
				}
			}
		}

		// Update with matching market data option chain OPRA and NBBO
		private void UpdateMarketDataOptionDeltas(Dictionary<string, MDServerToClient> deltas, List<string> symbolList)
		{
			DataRowView[] rows = new DataRowView[0];

			lock (rocOptionList.RocGridTable)
			{
				foreach (string symbol in symbolList)
				{
					if (deltas.ContainsKey(symbol))
					{
						MDServerToClient delta = deltas[symbol];
						if (delta.uParticipentSymbol == null || delta.uParticipentSymbol == "")
						{
							if (!IsFuture)
							{
								// NBBO
								if (rocOptionList.PutKeys.Contains(delta.uSymbol))
								{
									if (rocOptionList.FilterInAll)
									{
										rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, "" });
										UpdateMarketDataOptionDelta(delta, rows, "Put", "", false);
									}

									if (delta.AskPartId != null && delta.AskPartId != "")
									{
										rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, delta.AskPartId });
										UpdateMarketDataOptionDelta(delta, rows, "Put", delta.AskPartId, !rocOptionList.FilterInAll);
									}

									if (delta.BidPartId != null && delta.BidPartId != "")
									{
										rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, delta.BidPartId });
										UpdateMarketDataOptionDelta(delta, rows, "Put", delta.BidPartId, !rocOptionList.FilterInAll);
									}
								}
								else if (rocOptionList.CallKeys.Contains(delta.uSymbol))
								{
									if (rocOptionList.FilterInAll)
									{
										rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, "" });
										UpdateMarketDataOptionDelta(delta, rows, "Call", "", false);
									}

									if (delta.AskPartId != null && delta.AskPartId != "")
									{
										rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, delta.AskPartId });
										UpdateMarketDataOptionDelta(delta, rows, "Call", delta.AskPartId, !rocOptionList.FilterInAll);
									}

									if (delta.BidPartId != null && delta.BidPartId != "")
									{
										rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, delta.BidPartId });
										UpdateMarketDataOptionDelta(delta, rows, "Call", delta.BidPartId, !rocOptionList.FilterInAll);
									}
								}
							}
							else
							{
								// Future
								if (rocOptionList.PutKeys.Contains(delta.uSymbol))
								{
									rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, "" });
									UpdateMarketDataOptionDelta(delta, rows, "Put", "", false);
								}
								else if (rocOptionList.CallKeys.Contains(delta.uSymbol))
								{
									rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, "" });
									UpdateMarketDataOptionDelta(delta, rows, "Call", "", false);
								}
							}
						}
						else
						{
							// Normal
							if (rocOptionList.PutKeys.Contains(delta.uSymbol))
							{
								rows = rocOptionList.PutSearchView.FindRows(new object[] { delta.uSymbol, delta.uParticipentSymbol });
								UpdateMarketDataOptionDelta(delta, rows, "Put", delta.uParticipentSymbol, !rocOptionList.FilterInAll);
							}
							else if (rocOptionList.CallKeys.Contains(delta.uSymbol))
							{
								rows = rocOptionList.CallSearchView.FindRows(new object[] { delta.uSymbol, delta.uParticipentSymbol });
								UpdateMarketDataOptionDelta(delta, rows, "Call", delta.uParticipentSymbol, !rocOptionList.FilterInAll);
							}
						}

						if (GLOBAL.Interrupt && !IsProcessing)
						{
							GLOBAL.Interrupt = false;
							Application.DoEvents();
						}
					}
				}
			}
		}

		private void UpdateMarketDataOptionDelta(MDServerToClient delta, DataRowView[] rows, string callPut, string pid, bool makeNBBO)
		{
			foreach (DataRowView row in rows)
			{
				string rowPartID = row[callPut + "PartID"].ToString();

				if (rowPartID == "" || rowPartID == delta.BidPartId)
				{
					if (delta.BidPartId != null && delta.BidPartId != "")
					{
						row[callPut + "BidPartID"] = delta.BidPartId;
					}

					if (delta.BidPrice != null && delta.BidSize != null)
					{
						row[callPut + "BidPrice"] = (double)delta.BidPrice;
						row[callPut + "BidSize"] = (long)delta.BidSize;
					}
				}

				if (rowPartID == "" || rowPartID == delta.AskPartId)
				{
					if (delta.AskPartId != null && delta.AskPartId != "")
					{
						row[callPut + "AskPartID"] = delta.AskPartId;
					}

					if (delta.AskPrice != null && delta.AskSize != null)
					{
						row[callPut + "AskPrice"] = (double)delta.AskPrice;
						row[callPut + "AskSize"] = (long)delta.AskSize;
					}
				}

				if (rowPartID == "" || rowPartID == delta.TradePartId)
				{
					if (delta.TradePartId != null && delta.TradePartId != "")
					{
						row[callPut + "LastPartID"] = delta.TradePartId;
					}

					//bool gotTraded = false;
					if (delta.TradePrice != null && delta.TradePrice != 0)
					{
						row[callPut + "LastTraded"] = (double)delta.TradePrice;
						//gotTraded = true;
					}
					else if (row[callPut + "LastTraded"] == null || row[callPut + "LastTraded"].ToString() == "")
					{
						if (delta.ClosePrice != null && delta.ClosePrice != 0)
						{
							row[callPut + "LastTraded"] = (double)delta.ClosePrice;
							//gotTraded = true;
						}
						else if (delta.PrevClosePrice != null && delta.PrevClosePrice != 0)
						{
							row[callPut + "LastTraded"] = (double)delta.PrevClosePrice;
							//gotTraded = true;
						}
					}

					//else if (gotTraded)
					//{
					//    row[callPut + "LastPartID"] = pid;
					//}
				}

				if (delta.TotalVolume != null)
				{
					row[callPut + "Volume"] = (long)delta.TotalVolume;
				}

				if (delta.PctChange != null)
				{
					row[callPut + "PctChanged"] = (double)delta.PctChange;
				}

				//if (makeNBBO && !_showOnlySelectedExchange)
				//{
				//    if (row[callPut + "Symbol"] != null && row[callPut + "Symbol"].ToString() != "")
				//    {
				//        switch (callPut)
				//        {
				//            case "Call":
				//                MakeNBBO("", (string)row[callPut + "Symbol"]);
				//                break;
				//            case "Put":
				//                MakeNBBO((string)row[callPut + "Symbol"], "");
				//                break;
				//        }
				//    }
				//}
			}
		}

		#region - NBBO Calculation -

		private void RefreshNBBO()
		{
			lock (rocOptionList.RocGridTable)
			{
				DataRow[] rows = rocOptionList.RocGridTable.Select("Subscribed = 1");

				foreach (DataRow row in rows)
				{
					MakeNBBO((string)row["PutSymbol"], (string)row["CallSymbol"]);
				}
			}
		}

		private void MakeNBBO(string putSymbol, string callSymbol)
		{
			if (!IsFuture)
			{
				string selectAsk = "{0}Symbol = '{1}' And {0}PartID <> '' And {0}AskPrice > 0";
				string selectBid = "{0}Symbol = '{1}' And {0}PartID <> '' And {0}BidPrice > 0";
				string sortBid = "{0}{1}Price DESC, {0}{1}Size DESC";
				string sortAsk = "{0}{1}Price, {0}{1}Size DESC";

				if (putSymbol != "")
				{
					UpdateMarketDataOptionDeltaNBBO(
						string.Format(selectBid, "Put", putSymbol),
						string.Format(sortBid, "Put", "Bid"),
						putSymbol, "Put", "Bid");

					UpdateMarketDataOptionDeltaNBBO(
						string.Format(selectAsk, "Put", putSymbol),
						string.Format(sortAsk, "Put", "Ask"),
						putSymbol, "Put", "Ask");
				}

				if (callSymbol != "")
				{
					UpdateMarketDataOptionDeltaNBBO(
						string.Format(selectBid, "Call", callSymbol),
						string.Format(sortBid, "Call", "Bid"),
						callSymbol, "Call", "Bid");

					UpdateMarketDataOptionDeltaNBBO(
						string.Format(selectAsk, "Call", callSymbol),
						string.Format(sortAsk, "Call", "Ask"),
						callSymbol, "Call", "Ask");
				}
			}
		}

		private void UpdateMarketDataOptionDeltaNBBO(string select, string sort, string symbol, string callPut, string bidAsk)
		{
			DataRow[] nbborows = new DataRow[0];
			if (rocOptionList.ExchangeFilter == "")
			{
				nbborows = rocOptionList.RocGridTable.Select(select, sort);
			}
			else
			{
				nbborows = rocOptionList.RocGridTable.Select(rocOptionList.ExchangeFilter + " And " + select, sort);
			}

			DataRowView[] rows = new DataRowView[0];
			switch (callPut)
			{
				case "Put":
					rows = rocOptionList.PutSearchView.FindRows(new object[] { symbol, "" });
					break;
				case "Call":
					rows = rocOptionList.CallSearchView.FindRows(new object[] { symbol, "" });
					break;
			}
			if (rows.Length > 0)
			{
				if (nbborows.Length > 0)
				{
					UpdateMarketDataOptionDeltaNBBO(nbborows[0], rows, callPut, bidAsk);
				}
				else
				{
					UpdateMarketDataOptionDeltaNBBO(null, rows, callPut, bidAsk);
				}
			}
		}

		private void UpdateMarketDataOptionDeltaNBBO(DataRow nbborow, DataRowView[] rows, string callPut, string bidAsk)
		{
			foreach (DataRowView row in rows)
			{
				if (nbborow != null &&
					nbborow[callPut + bidAsk + "PartID"] != null && nbborow[callPut + bidAsk + "PartID"].ToString() != "" &&
					!rocOptionList.ExchangeFilter.Contains("<> '" + (string)nbborow[callPut + bidAsk + "PartID"] + "'"))
				{
					row[callPut + bidAsk + "PartID"] = nbborow[callPut + bidAsk + "PartID"];
					row[callPut + bidAsk + "Price"] = nbborow[callPut + bidAsk + "Price"];
					row[callPut + bidAsk + "Size"] = nbborow[callPut + bidAsk + "Size"];

					row[callPut + "LastTraded"] = nbborow[callPut + "LastTraded"];
					row[callPut + "LastPartID"] = nbborow[callPut + "LastPartID"];
				}
				else
				{
					row[callPut + bidAsk + "PartID"] = DBNull.Value;
					row[callPut + bidAsk + "Price"] = DBNull.Value;
					row[callPut + bidAsk + "Size"] = DBNull.Value;

					row[callPut + "LastTraded"] = DBNull.Value;
					row[callPut + "LastPartID"] = DBNull.Value;
				}
			}
		}

		#endregion

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

							numLimitPrice.DecimalPlaces = 2;
							numStopPrice.DecimalPlaces = 2;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutAskPrice"]).MaxDecimal = 2;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutBidPrice"]).MaxDecimal = 2;
							((DGVOptionListNumberColumn)rocOptionList.Columns["PutLastTraded"]).MaxDecimal = 2;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallAskPrice"]).MaxDecimal = 2;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallBidPrice"]).MaxDecimal = 2;
							((DGVOptionListNumberColumn)rocOptionList.Columns["CallLastTraded"]).MaxDecimal = 2;
							break;
						default:
							dspAskPrice.MaxDecimal = 7;
							dspBidPrice.MaxDecimal = 7;
							dspNetChange.MaxDecimal = 7;
							dspLowPrice.MaxDecimal = 7;
							dspHighPrice.MaxDecimal = 7;
							dspClosePrice.MaxDecimal = 7;
							dspTradedPrice.MaxDecimal = 7;

							numLimitPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
							numStopPrice.DecimalPlaces = CurrentSecInfo.DecimalPlaces;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutAskPrice"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutBidPrice"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							((DGVOptionListNumberColumn)rocOptionList.Columns["PutLastTraded"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallAskPrice"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallBidPrice"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							((DGVOptionListNumberColumn)rocOptionList.Columns["CallLastTraded"]).MaxDecimal = CurrentSecInfo.DecimalPlaces;
							break;
					}

					dspAskPrice.TickSize = CurrentSecInfo.TickSize;
					dspBidPrice.TickSize = CurrentSecInfo.TickSize;
					dspNetChange.TickSize = CurrentSecInfo.TickSize;
					dspLowPrice.TickSize = CurrentSecInfo.TickSize;
					dspHighPrice.TickSize = CurrentSecInfo.TickSize;
					dspClosePrice.TickSize = CurrentSecInfo.TickSize;
					dspTradedPrice.TickSize = CurrentSecInfo.TickSize;

					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec;
					dspLimitPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;

					numStopPrice.Increment = CurrentSecInfo.TickSizeDec;
					dspStopPriceTickSize.TickSize = CurrentSecInfo.TickSize;
					dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;

					if (IsFuture)
					{
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutAskPrice"]).IsNotified = true;
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutBidPrice"]).IsNotified = true;
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallAskPrice"]).IsNotified = true;
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallBidPrice"]).IsNotified = true;
					}
					else
					{
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutAskPrice"]).IsNotified = false;
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["PutBidPrice"]).IsNotified = false;
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallAskPrice"]).IsNotified = false;
						((DGVOptionListNotifiedNumberColumn)rocOptionList.Columns["CallBidPrice"]).IsNotified = false;
					}

					//if (!IsProcessing && gotOptionChain && GetCurrentMoney() != 0 && !rocOptionList.OptionLoaded)
					//{
					//    rocOptionList.OptionLoaded = true;
					//    System.Threading.ThreadPool.QueueUserWorkItem(RefreshOptionChain);
					//}
				}

				UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Option));
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Option));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#region - Option Chain -

		private void RefreshOptionChain(object input)
		{
			UpdateOptionChainStart();

			LoadOptionChain();

			UpdateOptionChainStop();
		}

		private void ResetOptionChain()
		{
			_isAtMoney = false;

			lock (_subQueue)
			{
				_subQueue = new Dictionary<string, SubscriptionQueue>();
			}

			lock (_subOptionQueue)
			{
				_subOptionQueue = new Dictionary<string, SubscriptionQueue>();
			}

			rocOptionList.CommonFactor = 1;
			lock (rocOptionList.Strikes)
			{
				rocOptionList.Strikes.Clear();
			}
			lock (rocOptionList.Expirations)
			{
				rocOptionList.Expirations.Clear();
			}
			rocOptionList.OptionLoaded = false;
			//rocOptionList.StrikeRange = 5;
			//rocOptionList.ExpirationRange = 1;

			rocOptionList.AtMoney = 0;
			rocOptionList.OptionSymbols.Clear();
			rocOptionList.ShowOptionSymbolColumn = false;
			rocOptionList.Show64Th = false;

			lock (rocOptionList.CallKeys)
			{
				rocOptionList.CallKeys.Clear();
			}

			lock (rocOptionList.PutKeys)
			{
				rocOptionList.PutKeys.Clear();
			}

			if (!_resetExpirationOnly)
			{
				rocOptionList.CurrentExpDate = new DateTime();
				rocOptionList.CurrentStrike = 0;

				lock (expirationTable)
				{
					expirationTable.Rows.Clear();
					ExpDateKeys.Clear();
				}

				lock (strikeTable)
				{
					strikeTable.Rows.Clear();
					StrikeKeys.Clear();
				}
			}

			if (rocOptionList.RocGridTable != null)
			{
				lock (rocOptionList.RocGridTable)
				{
					rocOptionList.RocGridTable.Clear();
					rocOptionList.RocGridTable.AcceptChanges();
				}
			}
		}

		private void LoadOptionChain()
		{
			if (gotOptionChain)
			{
				DataTable table = null;
				DataView searchView = null;

				//GLOBAL.HROC.AddToStatusLogs("Loading Option Chain 0");

				lock (rocOptionList.RocGridTable)
				{
					table = rocOptionList.RocGridTable.Copy();
					searchView = new DataView(table);
					searchView.Sort = "Expiration, Strike, OptionSymbol";
				}

				lock (table)
				{
					try
					{
						GetOptionRanges(out _strikeHigh, out _strikeLow, out _expDateHigh, out _expDateLow);

						DateTime expDate;
						double stkPrice;
						DateTime monthlyExpDate;

						List<string> optionSymbols = new List<string>();
						List<string> putKeys = new List<string>();
						List<string> callKeys = new List<string>();

						DataRowView[] rows = new DataRowView[0];

						foreach (IMOptionInfo opt in CurrentSecInfo.OptionChain.Values)
						{
							if (_ticketClosed) break;
							if (opt.ExpDateDT != null && opt.StrikePriceD != null)
							{
								expDate = (DateTime)opt.ExpDateDT;
								stkPrice = (double)opt.StrikePriceD;

								//if (!IsFuture && ShowMonthlyExpirationsOnly)
								if (ShowMonthlyExpirationsOnly)
								{
									monthlyExpDate = WeekHelper.GetMonthlyExpirationDay((DateTime)opt.ExpDateDT);
									if (IsFuture)
									{
										if (rocOptionList.Expirations.Contains((DateTime)opt.ExpDateDT) || WeekHelper.IsThridWeek((DateTime)opt.ExpDateDT) || WeekHelper.IsEndOfMonth((DateTime)opt.ExpDateDT))
										{
											// Monthly and Quaterly
										}
										else
										{
											continue;
										}
									}
									else
									{
										if (monthlyExpDate != (DateTime)opt.ExpDateDT)
										{
											continue;
										}
									}
								}

								if (!_resetExpirationOnly)
								{
									if (!ExpDateKeys.Contains(expDate))
									{
										ExpDateKeys.Add(expDate);
										expirationTable.Rows.Add(new object[] { expDate, opt.ExpDate, ConvertToDisplayDate(expDate) });
									}
								}

								if (stkPrice < _strikeLow || stkPrice > _strikeHigh || expDate < _expDateLow || expDate > _expDateHigh)
								{
									continue;
								}

								//if (!_resetExpirationOnly)
								//{
									if (!StrikeKeys.Contains(stkPrice))
									{
										StrikeKeys.Add(stkPrice);
										strikeTable.Rows.Add(new object[] { stkPrice, (stkPrice).ToString(GetStrikFormat(stkPrice)) });
									}
								//}

								if (!optionSymbols.Contains(opt.OptionSymbol))
								{
									optionSymbols.Add(opt.OptionSymbol);
								}

								rows = new DataRowView[0];
								switch (opt.CallPut)
								{
									case CSVFieldIDs.OptionTypes.Put:
										#region - Put -
										if (!putKeys.Contains(opt.MDSymbol))
										{
											putKeys.Add(opt.MDSymbol);

											//try
											//{
												rows = searchView.FindRows(new object[] { opt.ExpDateDT, opt.StrikePriceD, opt.OptionSymbol });
												if (rows.Length == 0)
												{
													table.Rows.Add(SetOptionRow(opt, table.NewRow(), "Put"));
													if (!IsFuture && !_doNotLoadOPRAParticipant)
													{
														foreach (string key in OptionExchangeCode.IDs.Keys)
														{
															table.Rows.Add(SetOptionRow(opt, table.NewRow(), key, "Put"));
														}
													}
												}
												else
												{
													SetOptionRow(opt, rows, "Put");
												}
											//}
											//catch (Exception ex)
											//{
											//    GLOBAL.HROC.AddToStatusLogs("Put: " + ex.Message);
											//    GLOBAL.HROC.AddToStatusLogs(ex.StackTrace);
											//}
										}
										#endregion
										break;
									case CSVFieldIDs.OptionTypes.Call:
										#region - Call -
										if (!callKeys.Contains(opt.MDSymbol))
										{
											callKeys.Add(opt.MDSymbol);
																						
											//try
											//{
												rows = searchView.FindRows(new object[] { opt.ExpDateDT, opt.StrikePriceD, opt.OptionSymbol });
												if (rows.Length == 0)
												{
													table.Rows.Add(SetOptionRow(opt, table.NewRow(), "Call"));
													if (!IsFuture && !_doNotLoadOPRAParticipant)
													{
														foreach (string key in OptionExchangeCode.IDs.Keys)
														{
															table.Rows.Add(SetOptionRow(opt, table.NewRow(), key, "Call"));
														}
													}
												}
												else
												{
													SetOptionRow(opt, rows, "Call");
												}
											//}
											//catch (Exception ex)
											//{
											//    GLOBAL.HROC.AddToStatusLogs("Call: " + ex.Message);
											//    GLOBAL.HROC.AddToStatusLogs(ex.StackTrace);
											//}
										}
										#endregion
										break;
								}
							}
						}

						rocOptionList.OptionSymbols.Clear();
						rocOptionList.OptionSymbols.AddRange(optionSymbols.ToArray());
						rocOptionList.PutKeys.Clear();
						rocOptionList.PutKeys.AddRange(putKeys.ToArray());
						rocOptionList.CallKeys.Clear();
						rocOptionList.CallKeys.AddRange(callKeys.ToArray());

						lock (rocOptionList.RocGridTable)
						{
							rocOptionList.RocGridTable.BeginLoadData();

							foreach (DataRow row in table.Rows)
							{
								rocOptionList.RocGridTable.ImportRow(row);
							}

							rocOptionList.RocGridTable.EndLoadData();
						}
					}
					catch (Exception ex)
					{
						GLOBAL.HROC.AddToException(ex);
					}
				}
			}
		}

		private void GetOptionRanges(out double strikeHigh, out double strikeLow, out DateTime expDateHigh, out DateTime expDateLow)
		{
			strikeHigh = 10000000;
			strikeLow = 0;
			expDateHigh = DateTime.Now.AddYears(10);
			expDateLow = DateTime.Now;
			try
			{
				lock (rocOptionList.Strikes)
				{
					List<DateTime> _expirations = new List<DateTime>();
					foreach (IMOptionInfo opt in CurrentSecInfo.OptionChain.Values)
					{
						if (opt.StrikePriceD != null)
						{
							if (!rocOptionList.Strikes.Contains((double)opt.StrikePriceD))
							{
								rocOptionList.Strikes.Add((double)opt.StrikePriceD);
							}
						}

						if (opt.ExpDateDT != null)
						{
							if (!rocOptionList.Expirations.Contains((DateTime)opt.ExpDateDT))
							{
								//if (!IsFuture && ShowMonthlyExpirationsOnly)
								if (ShowMonthlyExpirationsOnly)
								{
									DateTime monthlyExpDate = WeekHelper.GetMonthlyExpirationDay((DateTime)opt.ExpDateDT);
									if (IsFuture)
									{
										if (WeekHelper.IsThridWeek((DateTime)opt.ExpDateDT) || WeekHelper.IsEndOfMonth((DateTime)opt.ExpDateDT))
										{
											// Monthly and Quaterly (or Its the only one avaliable)
											rocOptionList.Expirations.Add((DateTime)opt.ExpDateDT);
										}

										if (!_expirations.Contains((DateTime)opt.ExpDateDT))
										{
											_expirations.Add((DateTime)opt.ExpDateDT);
										}
									}
									else
									{
										if (monthlyExpDate == (DateTime)opt.ExpDateDT)
										{
											rocOptionList.Expirations.Add((DateTime)opt.ExpDateDT);
										}
									}
								}
								else
								{
									rocOptionList.Expirations.Add((DateTime)opt.ExpDateDT);
								}
							}
						}
					}

					if (IsFuture)
					{
						// TODO Just in case some future don't expire on the thrid week or at the end of month
						if (rocOptionList.Expirations.Count == 0)
						{
							rocOptionList.Expirations = new List<DateTime>(_expirations);
						}
						_expirations.Clear();
					}

					rocOptionList.Strikes.Sort();
					rocOptionList.Expirations.Sort();
				}

				#region - Get Strike Range -

				if (rocOptionList.StrikeRange == 0)
				{
					// Show All
					strikeHigh = rocOptionList.Strikes[rocOptionList.Strikes.Count - 1];
					strikeLow = rocOptionList.Strikes[0];
				}
				else
				{
					double money = GetCurrentMoney();

					long withoutDecimalMoney = 0;
					int midIndex = 0;

					// Find Common Factor Between Market Data and Strik Price
					rocOptionList.CommonFactor = 1;
					if (money != 0)
					{
						if (IsFuture)
						{
							// Find Common Factor Between Market Data and Strik Price
							rocOptionList.CommonFactor = Math.Round(rocOptionList.Strikes[rocOptionList.Strikes.Count / 2] / money, 0);
							if (rocOptionList.CommonFactor > 900000 && rocOptionList.CommonFactor < 1100000)
							{
								rocOptionList.CommonFactor = 1000000;
							}
							else if (rocOptionList.CommonFactor > 90000 && rocOptionList.CommonFactor < 11000)
							{
								rocOptionList.CommonFactor = 100000;
							}
							else if (rocOptionList.CommonFactor > 9000 && rocOptionList.CommonFactor < 11000)
							{
								rocOptionList.CommonFactor = 10000;
							}
							else if (rocOptionList.CommonFactor > 900 && rocOptionList.CommonFactor < 1100)
							{
								rocOptionList.CommonFactor = 1000;
							}
							else if (rocOptionList.CommonFactor > 90 && rocOptionList.CommonFactor < 110)
							{
								rocOptionList.CommonFactor = 100;
							}
							else if (rocOptionList.CommonFactor > 9 && rocOptionList.CommonFactor < 11)
							{
								rocOptionList.CommonFactor = 10;
							}
							else
							{
								rocOptionList.CommonFactor = 1;
							}
						}
						money = money * rocOptionList.CommonFactor;

						withoutDecimalMoney = Convert.ToInt64(money.ToString("F7").Replace(".", ""));
						long withoutDecimalsp = Convert.ToInt64(rocOptionList.Strikes[0].ToString("F7").Replace(".", ""));
						long smallestDelta = Math.Abs(withoutDecimalMoney - withoutDecimalsp);
						long delta = 0;

						int largerCount = 0;
						for (int index = 1; index < rocOptionList.Strikes.Count; index++)
						{
							withoutDecimalsp = Convert.ToInt64(rocOptionList.Strikes[index].ToString("F7").Replace(".", ""));
							delta = Math.Abs(withoutDecimalMoney - withoutDecimalsp);
							if (delta < smallestDelta)
							{
								midIndex = index;
								smallestDelta = delta;
								largerCount = 0;
							}
							else if (delta > smallestDelta)
							{
								largerCount = largerCount + 1;
								if (largerCount > 5)
								{
									// Already Passed mid point
									break;
								}
							}
						}
					}
					else
					{
						midIndex = rocOptionList.Strikes.Count / 2;
					}

					int highIndex = rocOptionList.Strikes.Count - 1;
					int lowIndex = 0;

					if (rocOptionList.StrikeRange > 0)
					{
						// Get High Strik Limit
						if (midIndex + rocOptionList.StrikeRange < rocOptionList.Strikes.Count)
						{
							highIndex = midIndex + rocOptionList.StrikeRange;
						}

						if (midIndex - rocOptionList.StrikeRange > 0)
						{
							lowIndex = midIndex - rocOptionList.StrikeRange;
						}
					}

					strikeHigh = rocOptionList.Strikes[highIndex];
					strikeLow = rocOptionList.Strikes[lowIndex];
				}

				#endregion

				#region - ExpDate Range -

				if (rocOptionList.ExpirationRange == 0)
				{
					// Show All
					expDateHigh = rocOptionList.Expirations[rocOptionList.Expirations.Count - 1];
					expDateLow = rocOptionList.Expirations[0];
				}
				else
				{
					expDateLow = rocOptionList.CurrentExpDate;
					if (expDateLow <= rocOptionList.Expirations[0])
					{
						expDateLow = rocOptionList.Expirations[0];
					}
					expDateHigh = rocOptionList.Expirations[rocOptionList.Expirations.Count - 1];
					for (int expLowCount = 0; expLowCount < rocOptionList.Expirations.Count; expLowCount++)
					{
						if (rocOptionList.Expirations[expLowCount] >= expDateLow)
						{
							int expHighCount = expLowCount + (rocOptionList.ExpirationRange - 1);
							if (expHighCount < 0)
							{
								expHighCount = 0;
							}
							if (rocOptionList.Expirations.Count > expHighCount)
							{
								expDateHigh = rocOptionList.Expirations[expHighCount];
								break;
							}
						}
					}
				}

				#endregion
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private string GetStrikFormat(double strikePrice)
		{
			string str = strikePrice.ToString("F7");
			string[] strs = str.Split(new char[] { '.' }, StringSplitOptions.RemoveEmptyEntries);

			if (strs.Length == 2)
			{
				return "F" + strs[1].TrimEnd(_trimZero).Length.ToString();
			}
			return "F2";
		}

		private void UpdateOptionChainStart()
		{
			IsProcessing = true;

			//rocOptionList.LastFilter = rocOptionList.RocGridTable.DefaultView.RowFilter;
			//rocOptionList.LastSort = rocOptionList.RocGridTable.DefaultView.Sort;

			//rocOptionList.RocGridTable.BeginLoadData();
			//rocOptionList.RocGridTable.DefaultView.RowFilter = "";
			//rocOptionList.RocGridTable.DefaultView.Sort = "";
		}

		private void UpdateOptionChainStop()
		{
			//rocOptionList.RocGridTable.DefaultView.Sort = rocOptionList.LastSort;
			//rocOptionList.RocGridTable.DefaultView.RowFilter = rocOptionList.LastFilter;
			//rocOptionList.RocGridTable.EndLoadData();

			IsProcessing = false;

			rocOptionList.RefreshSharedRows = true;
			//rocOptionList.RefreshAggragation = true;
			//rocOptionList.ShouldScrollToLastRow = true;
		}

		private DataRow SetOptionRow(IMOptionInfo opt, DataRow row, string callPut)
		{
			try
			{
				if (opt.ExpDateDT != null && opt.StrikePriceD != null)
				{
					row["OptionSymbol"] = opt.OptionSymbol;
					row[callPut + "Symbol"] = opt.MDSymbol;
					row[callPut + "PartID"] = "";
					row["Strike"] = opt.StrikePriceD;
					row["Expiration"] = opt.ExpDateDT;
					row["DisplayExpiration"] = ((DateTime)opt.ExpDateDT).AddMonths(MonthToAdd);

					row["TickSize"] = CurrentSecInfo.TickSize;
					row["ContractSize"] = 100;
					row["DisplayFactor"] = 1;

					row["MDSource"] = opt.MDSource;
					row["SecType"] = opt.SecType;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return row;
		}

		private DataRow SetOptionRow(IMOptionInfo opt, DataRow row, string optKey, string callPut)
		{
			try
			{
				if (opt.ExpDateDT != null && opt.StrikePriceD != null)
				{
					row["OptionSymbol"] = opt.OptionSymbol;
					row[callPut + "Symbol"] = opt.MDSymbol;
					row[callPut + "PartID"] = optKey;
					row["Strike"] = opt.StrikePriceD;
					row["StringStrike"] = opt.StrikePrice.Replace(".", "");
					row["Expiration"] = opt.ExpDateDT;
					row["DisplayExpiration"] = ((DateTime)opt.ExpDateDT).AddMonths(MonthToAdd);

					row["TickSize"] = CurrentSecInfo.TickSize;
					row["ContractSize"] = 100;
					row["DisplayFactor"] = 1;

					row["MDSource"] = opt.MDSource;
					row["SecType"] = opt.SecType;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return row;
		}

		private void SetOptionRow(IMOptionInfo opt, DataRowView[] rows, string callPut)
		{
			try
			{
				foreach (DataRowView row in rows)
				{
					if (_ticketClosed) break;
					row[callPut + "Symbol"] = opt.MDSymbol;
					switch (callPut)
					{
						case "Call":
							if (row["PutPartID"] != null)
							{
								row[callPut + "PartID"] = row["PutPartID"];
							}
							break;
						case "Put":
							if (row["CallPartID"] != null)
							{
								row[callPut + "PartID"] = row["CallPartID"];
							}
							break;
					}
					row["MDSource"] = opt.MDSource;
					row["SecType"] = opt.SecType;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		#endregion

		#endregion

		#region - Order Execution -

		private void QuickButtonSupport_QuickButtonClicked(object sender, QuickButtonClickedEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.Data.secType)
			{
				case CSVFieldIDs.SecutrityTypes.Option:
					numQuantity.Value = e.Data.qty;
					if (!e.Data.loadOnly)
					{
						switch (e.Data.side)
						{
							case QuickButtonSides.BuyOpen:
								cmdBuyOpen_Click(null, EventArgs.Empty);
								break;
							case QuickButtonSides.BuyClose:
								cmdBuyClose_Click(null, EventArgs.Empty);
								break;
							case QuickButtonSides.SellOpen:
								cmdSellOpen_Click(null, EventArgs.Empty);
								break;
							case QuickButtonSides.SellClose:
								cmdSellClose_Click(null, EventArgs.Empty);
								break;
						}
					}
					break;
			}
		}

		private void cmdBuyOpen_Click(object sender, EventArgs e)
		{
			ValidateTicketValues();

			if (!_isInError)
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString(), CSVEx.CSVFieldIDs.OptionTypes.Open);
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Option Buy Open Order", CurrentSymbolDetail));
			}
		}

		private void cmdBuyClose_Click(object sender, EventArgs e)
		{
			ValidateTicketValues();

			if (!_isInError)
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString(), CSVEx.CSVFieldIDs.OptionTypes.Close);
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Option Buy Close Order", CurrentSymbolDetail));
			}
		}

		private void cmdSellOpen_Click(object sender, EventArgs e)
		{
			ValidateTicketValues();

			if (!_isInError)
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString(), CSVEx.CSVFieldIDs.OptionTypes.Close);
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Option Sell Open Order", CurrentSymbolDetail));
			}
		}

		private void cmdSellClose_Click(object sender, EventArgs e)
		{
			ValidateTicketValues();

			if (!_isInError)
			{
				GLOBAL.Interrupt = true;
				BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString(), CSVEx.CSVFieldIDs.OptionTypes.Open);
			}
			else
			{
				GLOBAL.HROC.AddToAlerts(string.Format("Invalid {0} Option Sell Close Order", CurrentSymbolDetail));
			}
		}

		private void BuildOrder(string side, string openClose)
		{
			long orderType = GetOrderTypeCode(cboOrder.Text);
			long duration = GetDurationCode(cboDuration.Text);
			string ocoOrderID = "";

			Activate();

			switch (orderType)
			{
				#region - Market -

				case CSVEx.CSVFieldIDs.OrderTypes.Market:
					BuildOrder(side, openClose,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketAtOpen:
					BuildOrder(side, openClose,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.OPG.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.MarketOnClose:
					BuildOrder(side, openClose,
						orderType.ToString(),
						CSVEx.CSVFieldIDs.TIFCodes.Day.ToString());
					break;

				#endregion

				#region - OCO -

				case CSVEx.CSVFieldIDs.OrderTypes.OCO:

					ocoOrderID = GLOBAL.HROM.RomMessageMaker.GetOrderID(GLOBAL.HROM.UserName);

					// Limit Price - primary side
					BuildOrder(side,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						"",
						ocoOrderID + "M");

					// Stop Price - other side
					if (side == CSVEx.CSVFieldIDs.SideCodes.Buy.ToString())
					{
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Sell.ToString(),
							orderType.ToString(),
							duration.ToString(),
							numStopPrice.Value.ToString(),
							"",
							ocoOrderID + "S");
					}
					else
					{
						BuildOrder(CSVEx.CSVFieldIDs.SideCodes.Buy.ToString(),
							orderType.ToString(),
							duration.ToString(),
							numStopPrice.Value.ToString(),
							"",
							ocoOrderID + "S");
					}
					break;

				#endregion

				#region - STOP -

				case CSVEx.CSVFieldIDs.OrderTypes.Stop:
					BuildOrder(side, openClose,
						orderType.ToString(),
						duration.ToString(),
						"",
						numStopPrice.Value.ToString());
					break;
				case CSVEx.CSVFieldIDs.OrderTypes.StopLimit:
					BuildOrder(side, openClose,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						numStopPrice.Value.ToString());
					break;

				#endregion

				default:
					BuildOrder(side, openClose,
						orderType.ToString(),
						duration.ToString(),
						numLimitPrice.Value.ToString(),
						"");
					break;
			}
		}
		private void BuildOrder(string side, string openClose, string orderType, string duration)
		{
			BuildOrder(side, openClose, orderType, duration, "", "", "");
		}
		private void BuildOrder(string side, string openClose, string orderType, string duration, string limitPrice, string stopPrice)
		{
			BuildOrder(side, openClose, orderType, duration, limitPrice, stopPrice, "");
		}
		private void BuildOrder(string side, string openClose, string orderType, string duration, string limitPrice, string stopPrice, string ocoOrderID)
		{
			RomBasicOrder order = new RomBasicOrder();

			DestinationMap dest = new DestinationMap();
			AccountMap acct = new AccountMap();

			try
			{
				if (DDAccountDropDown.GetCurrentAccount(ref acct) && DDAccountDropDown.GetCurrentExchange(ref dest) && dest.destID != null)
				{
					if (ocoOrderID != "")
					{
						order.orderID = ocoOrderID;
					}

					// First Setup
					if (IsFuture)
					{
						order.multiplier = CurrentSecInfo.ContractSize.ToString();
					}
					else
					{
						order.multiplier = "100";
					}
					if (cboCallPut.Text.ToUpper() == "CALL")
					{
						order.callPut = CSVEx.CSVFieldIDs.OptionTypes.Call;
					}
					else if (cboCallPut.Text.ToUpper() == "PUT")
					{
						order.callPut = CSVEx.CSVFieldIDs.OptionTypes.Put;
					}

					order.expDate = rocOptionList.CurrentExpDate.ToString("yyyyMM");
					order.maturityDay = rocOptionList.CurrentExpDate.ToString("dd");
					order.strike = rocOptionList.CurrentStrike.ToString();
					order.mdSymbol = GetMDSymbolFromCurrentExpDateAndStrik(rocOptionList, order.callPut);

					if (order.mdSymbol.Length > 2)
					{
						if (IsFuture)
						{
							order.symbol = CurrentSymbolDetail;
						}
						else
						{
							order.symbol = GetSymbolFromMDSymbol(order.mdSymbol);
						}
					}
					else
					{
						GLOBAL.HROC.AddToAlerts("Not a valid ExpDate or Strike");
						return;
					}

					//if (!IsFuture)
					//{
						order.secType = CSVEx.CSVFieldIDs.SecutrityTypes.Option;
					//}
					//else
					//{
					//    order.secType = CSVEx.CSVFieldIDs.SecutrityTypes.OptionFuture;
					//}
					order.underlying = CurrentSymbolDetail;
					order.openClose = openClose;

					order.tradeFor = DDAccountDropDown.CurrentTradeFor;
					order.trader = GLOBAL.HROM.UserName;
					order.shares = numQuantity.Value.ToString();
					order.price = limitPrice;
					order.stopPrice = stopPrice;
					order.side = side;
					order.orderType = orderType;
					order.duration = duration;
					order.exchangeID = dest.destID.ToString();
					order.cmtaFirmID = dest.cmtaFirmID;
					order.capacity = acct.capacity;
					order.clearingAcctID = acct.clearingAcID;
					order.clearingFirmID = acct.clearingFirmID;
					order.firmArc = acct.firmAcr;
					order.localAcctAcrn = acct.localAcAcrn;
					order.occAcct = acct.occAcr;
					if (acct.homeExchange != null)
					{
						order.homeExchange = acct.homeExchange;
					}

					GLOBAL.HROM.EnterOrder(order, true);
				}
				else
				{
					ValidateTicketValues();
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private string GetMDSymbolFromCurrentExpDateAndStrik(ROCOptionList grid, string callput)
		{
			try
			{
				DataRow[] rows = grid.RocGridTable.Select("Expiration = '" + grid.CurrentExpDate + "' And Strike = '" + grid.CurrentStrike + "' And CallPartID = '' And PutPartID = ''");
				if (rows.Length > 0)
				{
					foreach (DataRow row in rows)
					{
						switch (callput)
						{
							case CSVEx.CSVFieldIDs.OptionTypes.Call:
								if (grid.OptionCallSymbol == (string)row["CallSymbol"])
								{
									return (string)row["CallSymbol"];
								}
								break;
							case CSVEx.CSVFieldIDs.OptionTypes.Put:
								if (grid.OptionPutSymbol == (string)row["PutSymbol"])
								{
									return (string)row["PutSymbol"];
								}
								break;
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return "";
		}

		private string GetSymbolFromMDSymbol(string mdSymbol)
		{
			if (mdSymbol.Length > 2)
			{
				// New OSI Symbol
				if (mdSymbol.Contains("_"))
				{
					return mdSymbol.Substring(0, mdSymbol.IndexOf("_"));
				}
				else
				{
					return mdSymbol.Substring(0, mdSymbol.Length - 2);
				}
			}

			return "";
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
				_isInError = false;
			}

			if (!_isLoadingValue)
			{
				if (controlName == "" || controlName == cmdTradeFor.Name || controlName == cmdAccount.Name || controlName == cboExchange.Name)
				{
					IsInError(cmdTradeFor.Name, SetState(cmdTradeFor, (cmdTradeFor.Text == "N/A" || cmdTradeFor.Text == "")));
					IsInError(cmdAccount.Name, SetState(cmdAccount, (cmdAccount.Text == "N/A" || cmdAccount.Text == "")));
					IsInError(cboExchange.Name, SetState(lblExchange, (cboExchange.Text == "")));
					SetPanelWidth(ref panelExchange, cboExchange);
				}

				try
				{
					_orderTypesView.RowFilter = TicketSupport.BuildOrderTypeFilter(cboExchange, cboOrder);
					BuildDurationTypeFilter();

					SetTicketStateByOrderType();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}

				if (controlName == "" || controlName == numQuantity.Name)
				{
					IsInError(lblQuantity.Name, SetState(lblQuantity, numQuantity.Value <= 0));
					SetPanelWidth(ref panelShares, numQuantity);
					IsInError(numQuantity.Name, SetState(numQuantity, numQuantity.Value <= 0));
				}

				if (controlName == "" || controlName == cboCallPut.Name)
				{
					SetPanelWidth(ref panelCallPut, cboCallPut);
				}

				if (controlName == "" || controlName == cboDuration.Name)
				{
					SetPanelWidth(ref panelDuration, cboDuration);
				}

				if (panelLimitPrice.Enabled)
				{
					if (controlName == "" || controlName == numLimitPrice.Name)
					{
						IsInError(lblLimitPrice.Name, SetState(lblLimitPrice, numLimitPrice.Value <= 0));
						IsInError(numLimitPrice.Name, SetState(numLimitPrice, numLimitPrice.Value <= 0));
						SetPriceIncrement(numLimitPrice, dspLimitPriceTickSize, LimitPriceIncrementMultiple);

						SetPanelWidth(ref panelLimitPrice, numLimitPrice);
						SetPriceDisplay(PriceType.Limit, false);
					}
				}
				else
				{
					IsInError(lblLimitPrice.Name, false);
					IsInError(numLimitPrice.Name, false);
				}

				if (panelStopPrice.Enabled)
				{
					if (controlName == "" || controlName == numStopPrice.Name)
					{
						IsInError(lblStopPrice.Name, SetState(lblStopPrice, numStopPrice.Value <= 0));
						IsInError(numStopPrice.Name, SetState(numStopPrice, numStopPrice.Value <= 0));
						SetPriceIncrement(numStopPrice, dspStopPriceTickSize, StopPriceIncrementMultiple);

						SetPanelWidth(ref panelStopPrice, numStopPrice);
						SetPriceDisplay(PriceType.Stop, false);
					}
				}
				else
				{
					IsInError(lblStopPrice.Name, false);
					IsInError(numStopPrice.Name, false);
				}

				if (controlName == "" || controlName == cboOrder.Name)
				{
					IsInError(lblOrderType.Name, SetState(lblOrderType, cboOrder.Text == ""));
					SetPanelWidth(ref panelOrderTypes, cboOrder);
				}

				if (controlName == "" || controlName == cboDuration.Name)
				{
					IsInError(lblDuration.Name, SetState(lblDuration, cboDuration.Text == ""));
					SetPanelWidth(ref panelDuration, cboDuration);
				}

				if (controlName == "" || controlName == cboCallPut.Name)
				{
					IsInError(lblCallPut.Name, SetState(lblCallPut, cboCallPut.Text == ""));
					SetPanelWidth(ref panelCallPut, cboCallPut);
				}

				if (controlName == "" || controlName == cboExpiration.Name)
				{
					//Debug.Print("ValidateTicketValue().cboExpiration.Text " + cboExpiration.Text);
					IsInError(lblExpiration.Name, SetState(lblExpiration, cboExpiration.Text == ""));
					SetPanelWidth(ref panelExpiration, cboExpiration);
					SetExpirationDisplay();
				}

				if (controlName == "" || controlName == cboStrike.Name)
				{
					IsInError(lblStrike.Name, SetState(lblStrike, cboStrike.Text == ""));
					SetPanelWidth(ref panelStrike, cboStrike);
				}

				// Auto Resize to fit Strike Panel
				if (panelStrike.Right + 5 > Size.Width)
				{
					Size = new Size(panelStrike.Right + 5, Height);
				}

				if (cboExpiration.Text != "")
				{
					Nullable<DateTime> tDT = ConvertFromDisplayDate(cboExpiration.Text);
					if (tDT != null)
					{
						if (rocOptionList.CurrentExpDate != (DateTime)tDT)
						{
							rocOptionList.CurrentExpDate = (DateTime)tDT;
						}
						//else
						//{
						//    //GLOBAL.HROC.AddToStatusLogs("Duplicated CurrentExpDate");
						//    if (((TimeSpan)DateTime.Now.Subtract(_lastValidateTime)).Milliseconds < _validateTicketValuesRate)
						//    {
						//        Application.DoEvents();
						//    }
						//}
					}
					else
					{
						GLOBAL.HROC.AddToAlerts("Invalid ExpDate");
					}
				}

				if (cboStrike.Text != "")
				{
					double tD;
					if (double.TryParse(cboStrike.Text, out tD))
					{
						if (rocOptionList.CurrentStrike != tD)
						{
							rocOptionList.CurrentStrike = tD;
						}
						//else
						//{
						//    if (((TimeSpan)DateTime.Now.Subtract(_lastValidateTime)).Milliseconds < _validateTicketValuesRate)
						//    {
						//        Application.DoEvents();
						//    }
						//}
					}
				}

				BuildOrderMsg();

				//if (((TimeSpan)DateTime.Now.Subtract(_lastValidateTime)).Milliseconds < _validateTicketValuesRate)
				//{
				//    Application.DoEvents();
				//}
				//_lastValidateTime = DateTime.Now;

			}
		}

		private void SetExpirationDisplay()
		{
			SetExpirationDisplay(false);
		}
		private void SetExpirationDisplay(bool firstTime)
		{
			if (firstTime)
			{
				dspExpiration.Font = cboExpiration.Font;
			}
			dspExpiration.ForeColor = cboExpiration.ForeColor;
			//dspExpiration.Text = ConvertToDisplayDate(cboExpiration.Text);
			dspExpiration.Text = cboExpiration.Text;
		}

		private void SetPriceDisplay(PriceType type, bool firstTime)
		{
			string formatedPrice = "";
			switch (type)
			{
				case PriceType.Limit:
					if (firstTime)
					{
						dspLimitPrice.Font = numLimitPrice.Font;
					}
					if (dspLimitPrice.ForeColor != numLimitPrice.ForeColor)
					{
						dspLimitPrice.ForeColor = numLimitPrice.ForeColor;
					}
					formatedPrice = GetDoubleFormat(Convert.ToDouble(numLimitPrice.Value));
					if (dspLimitPrice.Text != formatedPrice)
					{
						dspLimitPrice.Text = formatedPrice;
					}
					break;
				case PriceType.Stop:
					if (firstTime)
					{
						dspStopPrice.Font = numStopPrice.Font;
					}
					if (dspStopPrice.ForeColor != numStopPrice.ForeColor)
					{
						dspStopPrice.ForeColor = numStopPrice.ForeColor;
					}
					formatedPrice = GetDoubleFormat(Convert.ToDouble(numStopPrice.Value));
					if (dspStopPrice.Text != formatedPrice)
					{
						dspStopPrice.Text = formatedPrice;
					}
					break;
			}
		}
		private void SetPriceDisplayFromUserInput(PriceType type)
		{
			decimal price = 0;

			switch (type)
			{
				case PriceType.Limit:
					price = Convert.ToDecimal(GetPriceFromUserInput(dspLimitPrice.Text));
					if (numLimitPrice.Value != price)
					{
						numLimitPrice.Value = price;
					}
					else
					{
						SetPriceDisplay(PriceType.Limit, false);
					}
					break;
				case PriceType.Stop:
					price = Convert.ToDecimal(GetPriceFromUserInput(dspStopPrice.Text));
					if (numStopPrice.Value != price)
					{
						numStopPrice.Value = price;
					}
					else
					{
						SetPriceDisplay(PriceType.Stop, false);
					}
					break;
			}
		}

		private void SetPriceIncrement(NumericUpDown numUpDown, LabelBase lblTickSize, decimal multiple)
		{
			if (IsStock)
			{
				if (numUpDown.Value > 3M)
				{
					if (numUpDown.Increment != Convert.ToDecimal(_stockLargeIncr) * multiple)
					{
						numUpDown.Increment = Convert.ToDecimal(_stockLargeIncr) * multiple;
					}
					lblTickSize.Value = _stockLargeIncr;
				}
				else
				{
					if (numUpDown.Increment != Convert.ToDecimal(_stockSmallIncr) * multiple)
					{
						numUpDown.Increment = Convert.ToDecimal(_stockSmallIncr) * multiple;
					}
					lblTickSize.Value = _stockSmallIncr;
				}

				lblTickSize.Is64Th = false;
			}
			else if (IsFuture)
			{
				if (Is64Th)
				{
					if (CurrentSecInfo.TickSizeDec == 0.0025m)
					{
						if (numUpDown.Increment != 0.00125m * multiple)
						{
							numUpDown.Increment = 0.00125m * multiple;
						}
						lblTickSize.Value = 0.00125m;
					}
					else
					{
						if (numUpDown.Increment != 0.015625m * multiple)
						{
							numUpDown.Increment = 0.015625m * multiple;
						}
						lblTickSize.Value = 0.015625m;
					}

					lblTickSize.Is64Th = true;
				}
				else
				{
					if (numUpDown.Increment != CurrentSecInfo.TickSizeDec * multiple)
					{
						numUpDown.Increment = CurrentSecInfo.TickSizeDec * multiple;
					}
					lblTickSize.Value = CurrentSecInfo.TickSizeDec;
				}

				lblTickSize.Is64Th = false;
			}
		}

		private void IsInError(string controlName, bool isError)
		{
			if (isError)
			{
				// Add to error list
				if (_controlsInError.ContainsKey(controlName))
				{
					_controlsInError[controlName] = isError;
				}
				else
				{
					_controlsInError.Add(controlName, isError);
				}
			}
			else
			{
				// Remove from error list
				if (_controlsInError.ContainsKey(controlName))
				{
					_controlsInError.Remove(controlName);
				}
			}

			if (_controlsInError.Count > 0)
			{
				_isInError = true;
			}
			else
			{
				_isInError = false;
			}

			//if (!_isInError)
			//{
			//    _isInError = isError;
			//}
		}

		private void SetTicketStateByOrderType()
		{
			switch (cboOrder.Text)
			{
				case "MARKET":
				case "MOC":
					panelLimitPrice.Enabled = false;
					panelStopPrice.Enabled = false;
					cboDuration.Enabled = false;
					cboDuration.Text = "DAY";
					break;
				case "MOO":
					panelLimitPrice.Enabled = false;
					panelStopPrice.Enabled = false;
					cboDuration.Enabled = false;
					cboDuration.Text = "OPG";
					break;
				case "STOPLIMIT":
				case "OCO":
					panelLimitPrice.Enabled = true;
					panelStopPrice.Enabled = true;
					EnableDuration();
					break;
				case "STOP":
					panelLimitPrice.Enabled = false;
					panelStopPrice.Enabled = true;
					EnableDuration();
					PriceSelection_Click(lblStopPrice, EventArgs.Empty);
					break;
				case "LOC":
					panelLimitPrice.Enabled = true;
					panelStopPrice.Enabled = false;
					EnableDuration();
					PriceSelection_Click(lblLimitPrice, EventArgs.Empty);
					break;
				default:
					panelLimitPrice.Enabled = true;
					panelStopPrice.Enabled = false;
					EnableDuration();
					PriceSelection_Click(lblLimitPrice, EventArgs.Empty);
					break;
			}
		}

		private void EnableDuration()
		{
			if (!cboDuration.Enabled)
			{
				cboDuration.Enabled = true;
				cboDuration.Text = "DAY";
			}
		}

		private void BuildDurationTypeFilter()
		{
			string durationFilter = TicketSupport.BuildDurationTypeFilter(cboExchange);
			if (_durationsView.RowFilter != durationFilter)
			{
				_durationsView.RowFilter = durationFilter;
			}

			//string durationFilter = "";

			//if (cboOrder.Text != "MOO")
			//{
			//    durationFilter = "Duration <> 'OPG'";
			//}

			//_durationsView.RowFilter = durationFilter;
			//cboOrder.DataSource = _durationsView;
		}

		private void BuildOrderMsg()
		{
			if (_isInError)
			{
				dspOrderMsg.ForeColor = Color.Red;
			}
			else
			{
				dspOrderMsg.ForeColor = Color.Black;
			}

			if (CurrentSymbolDetail != "")
			{
				if (CurrentSecInfo.LongName == "")
				{
					LongName = string.Format("- {0} -", CurrentSymbolDetail.ToUpper().Trim());
				}
			}
			else
			{
				LongName = "Option Ticket";
			}

			// 0 Quantity
			// 1 Symbol
			// 2 OrderType
			// 3 Limit Price / Stop Price
			// 4 Duration
			// 5 Exchange
			// 6 Stop Price
			// 7 Expiration
			// 8 Strike
			// 9 CallPut
			dspOrderMsg.Text = "";
			dspOrderMsg.SelectionAlignment = HorizontalAlignment.Center;
			dspOrderMsg.SelectionBackColor = dspOrderMsg.BackColor;

			string orderSymbol = cboSymbolDetails.Text;

			if (panelLimitPrice.Enabled && panelStopPrice.Enabled)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {7} {8} {9} {2} {5} - {6} {3} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboExchange.Text,
					GetDisplayLimitPrice(),
					GetDisplayStopPrice(),
					dspExpiration.Text,
					cboStrike.Text,
					cboCallPut.Text);
			}
			else if (panelLimitPrice.Enabled && !panelStopPrice.Enabled)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {7} {8} {9} {2} {5} {3} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboExchange.Text,
					GetDisplayLimitPrice(),
					"",
					dspExpiration.Text,
					cboStrike.Text,
					cboCallPut.Text);
			}
			else if (!panelLimitPrice.Enabled && panelStopPrice.Enabled)
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {7} {8} {9} {2} {5} {3} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboExchange.Text,
					GetDisplayStopPrice(),
					"",
					dspExpiration.Text,
					cboStrike.Text,
					cboCallPut.Text);
			}
			else
			{
				dspOrderMsg.SelectedText = string.Format("{0} {1} {7} {8} {9} {2} {3} {4}",
					numQuantity.Value,
					orderSymbol,
					cboOrder.Text,
					cboDuration.Text,
					cboExchange.Text,
					"",
					"",
					dspExpiration.Text,
					cboStrike.Text,
					cboCallPut.Text);
			}

			if (cboOrder.Text != "LIMIT")
			{
				dspOrderMsg.Select(dspOrderMsg.Find(cboOrder.Text), cboOrder.Text.Length);
				dspOrderMsg.SelectionBackColor = Color.Yellow;
			}

			if (cboDuration.Text != "DAY")
			{
				dspOrderMsg.SelectAll();
				dspOrderMsg.Select(dspOrderMsg.Find(cboDuration.Text), cboDuration.Text.Length);
				dspOrderMsg.SelectionBackColor = Color.Yellow;
			}
		}

		private string GetDisplayLimitPrice()
		{
			if (dspLimitPrice.Text != numLimitPrice.Value.ToString("######0.00#####"))
			{
				return string.Format("{0} ({1})", new object[] { dspLimitPrice.Text, numLimitPrice.Value.ToString("######0.00#####") });
			}
			else
			{
				return numLimitPrice.Value.ToString("######0.00#####");
			}
		}

		private string GetDisplayStopPrice()
		{
			if (dspStopPrice.Text != numStopPrice.Value.ToString("######0.00#####"))
			{
				return string.Format("{0} ({1})", new object[] { dspStopPrice.Text, numStopPrice.Value.ToString("######0.00#####") });
			}
			else
			{
				return numStopPrice.Value.ToString("######0.00#####");
			}
		}

		private void ResetTicketValues()
		{
			_isLoadingValue = true;
			_isAtMoney = false;

			_buyOrdersByTag.Clear();
			_sellOrdersByTag.Clear();

			HasFirstUpdate = false;
			_resetExpirationOnly = false;

			CurrentSecInfo = new BaseSecurityInfo();

			LongName = CurrentSecInfo.LongName;

			GettingOptionChainSymbols.Clear();
			MonthToAdd = 0;
			Is64Th = false;

			dspLimitPriceTickSize.Value = CurrentSecInfo.TickSize;
			dspStopPriceTickSize.Value = CurrentSecInfo.TickSize;

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

			lblDoNotTranslate.Visible = false;
			lblDoNotTranslate.Blink = false;

			numLimitPrice.Value = 0;

			panelExchanges.Visible = true;
			chkMonthlyOnly.Visible = true;

			//cboOrder.Text = "Limit";
			//cboDuration.Text = "DAY";
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(TicketDefaults.Option, CSVEx.CSVFieldIDs.SecutrityTypes.Option));
			QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(TicketDefaults.Option, CSVEx.CSVFieldIDs.SecutrityTypes.Option), false);

			_isLoadingValue = false;

			if (panelManagerOption.SelectedPanel != managedPanelStock)
			{
				panelManagerOption.SelectedPanel = managedPanelStock;
			}

			ResetOptionChain();

			//Debug.Print("ResetTicketValues().ValidateTicketValues()");
			ValidateTicketValues();
		}

		// Used to load symbol defaults from last saved settings, else it loads the Ticket Default Value;
		private void UILoadSymbolDefaults(SymbolSettingData data)
		{
			_isLoadingValue = true;

			bool reloadUserAccounts = false;

			if (data.tradeFor != "")
			{
				reloadUserAccounts = true;
				DDAccountDropDown.SelectedTraderFor = data.tradeFor;
			}

			if (data.account != "")
			{
				reloadUserAccounts = true;
				DDAccountDropDown.SelectedLocalAccountAcrn = data.account;
			}

			if (data.exchange != "")
			{
				reloadUserAccounts = true;
				DDAccountDropDown.SelectedExchange = data.exchange;
			}

			if (reloadUserAccounts)
			{
				DDAccountDropDown.LoadUserAccounts();
			}

			if (data.orderType != "")
			{
				cboOrder.Text = data.orderType;
			}
			else
			{
				cboOrder.Text = "LIMIT";
			}

			if (data.duration != "")
			{
				cboDuration.Text = data.duration;
			}
			else
			{
				cboDuration.Text = "DAY";
			}

			numQuantity.Value = data.qty;
			numQuantity.Increment = data.qtyIncrement;
			dspQuantityIncrement.Value = numQuantity.Increment;

			data.limitPriceIncrement = CurrentSecInfo.TickSizeDec;

			// Override with current traded price
			//numLimitPrice.Value = data.limitPrice;
			numLimitPrice.Increment = data.limitPriceIncrement;
			dspLimitPriceIncrement.Value = data.limitPriceIncrement / CurrentSecInfo.TickSizeDec;

			data.stopPriceIncrement = CurrentSecInfo.TickSizeDec;
			numStopPrice.Value = data.stopPrice;
			numStopPrice.Increment = data.stopPriceIncrement;
			dspStopPriceIncrement.Value = data.stopPriceIncrement / CurrentSecInfo.TickSizeDec;

			_isLoadingValue = false;

			//Debug.Print("UILoadSymbolDefaults().ValidateTicketValues()");
			ValidateTicketValues();
		}

		#endregion

		#region - Export & Import -

		public string ExportOptionGrid()
		{
			ROCOptionListProfile prof = new ROCOptionListProfile(rocOptionList);

			byte[] bytes = new ToBinary().Serialize(prof, SerializationTypes.Normal);
			return System.Convert.ToBase64String(bytes);
		}

		public void ImportOptionGrid(string s)
		{
			if (s != null && s != "")
			{
				byte[] bytes = System.Convert.FromBase64String(s);
				ROCOptionListProfile prof = (ROCOptionListProfile)new ToBinary().Deserialize(bytes, SerializationTypes.Normal);
				if (prof != null)
				{
					prof = new ROCOptionListProfile(rocOptionList, prof);
				}
			}
		}

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

		public void ReloadTicket()
		{
			if (CurrentSymbolDetail != "" && CurrentSecInfo != null)
			{
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVFieldIDs.SecutrityTypes.Option));
			}
			else
			{
				QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(TicketDefaults.Option, CSVFieldIDs.SecutrityTypes.Option));
			}
			//Debug.Print("ReloadTicket().ValidateTicketValues()");
			ValidateTicketValues();
		}

		public void Clone()
		{
			lock (GLOBAL.HWindows.OptionTicketWindows)
			{
				frmOptionTicket w = new frmOptionTicket();

				w.Width = this.Width;
				w.Height = this.Height;

				w.Clone(new ROCTicketProfile(this), new ROCOptionListProfile(rocOptionList));

				GLOBAL.HWindows.OpenWindow(w, true);
			}
		}
		public void Clone(ROCTicketProfile prof, ROCOptionListProfile gridProf)
		{
			if (prof != null)
			{
				prof = new ROCTicketProfile(this, prof);
			}
			if (gridProf != null)
			{
				gridProf = new ROCOptionListProfile(rocOptionList, gridProf);
			}
		}

		#endregion

		#region - Settings DropDown -

		private void itemSaveAsDefault_Click(object sender, EventArgs e)
		{
			SymbolSettingData data = GLOBAL.HSymbolSettingData.GetSymbolDefaults(CurrentSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Option);

			data.tradeFor = cmdTradeFor.Text;
			data.account = cmdAccount.Text;
			data.exchange = cboExchange.Text;

			data.qty = numQuantity.Value;
			data.qtyIncrement = numQuantity.Increment;

			data.orderType = cboOrder.Text;
			data.limitPrice = numLimitPrice.Value;
			data.limitPriceIncrement = numLimitPrice.Increment;

			data.stopPrice = numStopPrice.Value;
			data.stopPriceIncrement = numStopPrice.Increment;

			data.duration = cboDuration.Text;

			if (cboStrike.Text != "")
			{
				data.strike = Convert.ToDouble(cboStrike.Text);
			}
			data.expiration = cboExpiration.Text;
			data.callput = cboCallPut.Text;

			GLOBAL.HSymbolSettingData.SetSymbolDefaults(data);
		}

		private void itemResetToSystemDefault_Click(object sender, EventArgs e)
		{
			GLOBAL.HSymbolSettingData.RemoveSymbolDefault(CurrentSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Option);
			UILoadSymbolDefaults(GLOBAL.HSymbolSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Option));
			QuickButtonSupprot.UILoadSymbolQuickButtonDefaults(GLOBAL.HQuickButtonSettingData.GetSymbolDefaults(_lastSymbolDetail, CSVEx.CSVFieldIDs.SecutrityTypes.Option));
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
				Control_KeyUp(sender, e);
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

		#region - Qty Increment Menu -

		private menuBaseAction _menuQtyIncrement;
		private PropertyContainer _menuQtyIncrementContainer;
		private void dspQuantityIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuQtyIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("1", "1");
						items.Add("5", "5");
						items.Add("10", "10");
						items.Add("25", "25");
						items.Add("50", "50");
						items.Add("75", "75");
						items.Add("100", "100");

						_menuQtyIncrement = new menuBaseAction(items, true);
						_menuQtyIncrement.PropertyChanged += new PropertyChangedEventHandler(menuQtyIncrement_PropertyChanged);
					}

					if (_menuQtyIncrementContainer == null)
					{
						_menuQtyIncrementContainer = new PropertyContainer(_menuQtyIncrement);
						_menuQtyIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuQtyIncrementContainer_Closed);
					}

					_menuQtyIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuQtyIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuQtyIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numQuantity.Increment = 1;
					break;
				case "5":
					numQuantity.Increment = 5;
					break;
				case "10":
					numQuantity.Increment = 10;
					break;
				case "25":
					numQuantity.Increment = 25;
					break;
				case "50":
					numQuantity.Increment = 50;
					break;
				case "75":
					numQuantity.Increment = 75;
					break;
				case "100":
					numQuantity.Increment = 100;
					break;
			}
			dspQuantityIncrement.Value = numQuantity.Increment;
		}

		private void menuQtyIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuQtyIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuQtyIncrementContainer_Closed);
			_menuQtyIncrementContainer = null;
			_menuQtyIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuQtyIncrement_PropertyChanged);
			_menuQtyIncrement = null;
		}

		#endregion

		#region - Limit Price Increment Menu -

		private menuBaseAction _menuLimitPriceIncrement;
		private PropertyContainer _menuLimitPriceIncrementContainer;
		private void dspLimitPriceIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuLimitPriceIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("1", "1");
						items.Add("5", "5");
						items.Add("10", "10");
						items.Add("25", "25");
						items.Add("50", "50");
						items.Add("75", "75");
						items.Add("100", "100");

						_menuLimitPriceIncrement = new menuBaseAction(items, true);
						_menuLimitPriceIncrement.PropertyChanged += new PropertyChangedEventHandler(menuLimitPriceIncrement_PropertyChanged);
					}

					if (_menuLimitPriceIncrementContainer == null)
					{
						_menuLimitPriceIncrementContainer = new PropertyContainer(_menuLimitPriceIncrement);
						_menuLimitPriceIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuLimitPriceIncrementContainer_Closed);
					}

					_menuLimitPriceIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuLimitPriceIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuLimitPriceIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 1;
					break;
				case "5":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 5;
					break;
				case "10":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 10;
					break;
				case "25":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 25;
					break;
				case "50":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 50;
					break;
				case "75":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 75;
					break;
				case "100":
					numLimitPrice.Increment = CurrentSecInfo.TickSizeDec * 100;
					break;
			}
			dspLimitPriceIncrement.Value = numLimitPrice.Increment / CurrentSecInfo.TickSizeDec;
		}

		private void menuLimitPriceIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuLimitPriceIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuLimitPriceIncrementContainer_Closed);
			_menuLimitPriceIncrementContainer = null;
			_menuLimitPriceIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuLimitPriceIncrement_PropertyChanged);
			_menuLimitPriceIncrement = null;
		}

		#endregion

		#region - Stop Price Increment Menu -

		private menuBaseAction _menuStopPriceIncrement;
		private PropertyContainer _menuStopPriceIncrementContainer;
		private void dspStopPriceIncrement_MouseClick(object sender, MouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					if (_menuStopPriceIncrement == null)
					{
						Dictionary<string, string> items = new Dictionary<string, string>();
						items.Add("1", "1");
						items.Add("5", "5");
						items.Add("10", "10");
						items.Add("25", "25");
						items.Add("50", "50");
						items.Add("75", "75");
						items.Add("100", "100");

						_menuStopPriceIncrement = new menuBaseAction(items, true);
						_menuStopPriceIncrement.PropertyChanged += new PropertyChangedEventHandler(menuStopPriceIncrement_PropertyChanged);
					}

					if (_menuStopPriceIncrementContainer == null)
					{
						_menuStopPriceIncrementContainer = new PropertyContainer(_menuStopPriceIncrement);
						_menuStopPriceIncrementContainer.Closed += new ToolStripDropDownClosedEventHandler(menuStopPriceIncrementContainer_Closed);
					}

					_menuStopPriceIncrementContainer.Show((LabelBase)sender);
					break;
			}
		}

		private void menuStopPriceIncrement_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			_menuStopPriceIncrementContainer.Close();
			switch (e.PropertyName)
			{
				case "1":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 1;
					break;
				case "5":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 5;
					break;
				case "10":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 10;
					break;
				case "25":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 25;
					break;
				case "50":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 50;
					break;
				case "75":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 75;
					break;
				case "100":
					numStopPrice.Increment = CurrentSecInfo.TickSizeDec * 100;
					break;
			}
			dspStopPriceIncrement.Value = numStopPrice.Increment / CurrentSecInfo.TickSizeDec;
		}

		private void menuStopPriceIncrementContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuStopPriceIncrementContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuStopPriceIncrementContainer_Closed);
			_menuStopPriceIncrementContainer = null;
			_menuStopPriceIncrement.PropertyChanged -= new PropertyChangedEventHandler(menuStopPriceIncrement_PropertyChanged);
			_menuStopPriceIncrement = null;
		}

		#endregion

		#region - Columns Menu -

		private menuBaseFilter _menuColumns;
		private PropertyContainer _menuColumnsContainer;
		private void ShowColumnMenu()
		{
			if (_menuColumns == null)
			{
				Dictionary<string, FilterItem> items = new Dictionary<string, FilterItem>();

				items.Add("Put Symbol", new FilterItem("Put Symbol", !rocOptionList.Columns["PutSymbol"].Visible));
				items.Add("Put PartID", new FilterItem("Put PartID", !rocOptionList.Columns["PutPartID"].Visible));
				//items.Add("Put OpenQty", new FilterItem("Put OpenQty", !rocOptionList.Columns["PutOpenQty"].Visible));
				items.Add("Put AskPartID", new FilterItem("Put AskPartID", !rocOptionList.Columns["PutAskPartID"].Visible));
				items.Add("Put AskPrice", new FilterItem("Put AskPrice", !rocOptionList.Columns["PutAskPrice"].Visible));
				items.Add("Put AskSize", new FilterItem("Put AskSize", !rocOptionList.Columns["PutAskSize"].Visible));
				items.Add("Put BidPrice", new FilterItem("Put BidPrice", !rocOptionList.Columns["PutBidPrice"].Visible));
				items.Add("Put BidSize", new FilterItem("Put BidSize", !rocOptionList.Columns["PutBidSize"].Visible));
				items.Add("Put BidPartID", new FilterItem("Put BidPartID", !rocOptionList.Columns["PutBidPartID"].Visible));
				items.Add("Put Volume", new FilterItem("Put Volume", !rocOptionList.Columns["PutVolume"].Visible));
				items.Add("Put PctChanged", new FilterItem("Put PctChanged", !rocOptionList.Columns["PutPctChanged"].Visible));
				items.Add("Put LastPartID", new FilterItem("Put LastPartID", !rocOptionList.Columns["PutLastPartID"].Visible));
				items.Add("Put LastTraded", new FilterItem("Put LastTraded", !rocOptionList.Columns["PutLastTraded"].Visible));

				items.Add("S1", new FilterItem("--", false));
				items.Add("Call LastTraded", new FilterItem("Call LastTraded", !rocOptionList.Columns["CallLastTraded"].Visible));
				items.Add("Call LastPartID", new FilterItem("Call LastPartID", !rocOptionList.Columns["CallLastPartID"].Visible));
				items.Add("Call PctChanged", new FilterItem("Call PctChanged", !rocOptionList.Columns["CallPctChanged"].Visible));
				items.Add("Call Volume", new FilterItem("Call Volume", !rocOptionList.Columns["CallVolume"].Visible));
				items.Add("Call BidPartID", new FilterItem("Call BidPartID", !rocOptionList.Columns["CallBidPartID"].Visible));
				items.Add("Call BidSize", new FilterItem("Call BidSize", !rocOptionList.Columns["CallBidSize"].Visible));
				items.Add("Call BidPrice", new FilterItem("Call BidPrice", !rocOptionList.Columns["CallBidPrice"].Visible));
				items.Add("Call AskSize", new FilterItem("Call AskSize", !rocOptionList.Columns["CallAskSize"].Visible));
				items.Add("Call AskPrice", new FilterItem("Call AskPrice", !rocOptionList.Columns["CallAskPrice"].Visible));
				items.Add("Call AskPartID", new FilterItem("Call AskPartID", !rocOptionList.Columns["CallAskPartID"].Visible));
				//items.Add("Call OpenQty", new FilterItem("Call OpenQty", !rocOptionList.Columns["CallOpenQty"].Visible));
				items.Add("Call PartID", new FilterItem("Call PartID", !rocOptionList.Columns["CallPartID"].Visible));
				items.Add("Call Symbol", new FilterItem("Call Symbol", !rocOptionList.Columns["CallSymbol"].Visible));

				//items.Add("S2", new FilterItem("--", false));
				//items.Add("ContractSize", new FilterItem("ContractSize", !rocOptionList.Columns["ContractSize"].Visible));
				//items.Add("TickSize", new FilterItem("TickSize", !rocOptionList.Columns["TickSize"].Visible));
				//items.Add("DisplayFactor", new FilterItem("DisplayFactor", !rocOptionList.Columns["DisplayFactor"].Visible));

				//items.Add("S3", new FilterItem("--", false));
				//items.Add("Strike", new FilterItem("Strike", !rocOptionList.Columns["Strike"].Visible));
				//items.Add("Expiration", new FilterItem("Expiration", !rocOptionList.Columns["Expiration"].Visible));

				//items.Add("S4", new FilterItem("--", false));
				//items.Add("SecType", new FilterItem("SecType", !rocOptionList.Columns["SecType"].Visible));
				//items.Add("MDSource", new FilterItem("MDSource", !rocOptionList.Columns["MDSource"].Visible));
				//items.Add("Subscribed", new FilterItem("Subscribed", !rocOptionList.Columns["Subscribed"].Visible));
				//items.Add("MoneyFlag", new FilterItem("MoneyFlag", !rocOptionList.Columns["MoneyFlag"].Visible));

				_menuColumns = new menuBaseFilter(items, true);
				_menuColumns.PropertyChanged += new PropertyChangedEventHandler(menuColumns_PropertyChanged);
			}

			if (_menuColumnsContainer == null)
			{
				_menuColumnsContainer = new PropertyContainer(_menuColumns);
				_menuColumnsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuColumnsContainer_Closed);
			}

			_menuColumnsContainer.Show(Cursor.Position);
		}

		private void menuColumns_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			switch (e.PropertyName)
			{
				case "Put Symbol":
					rocOptionList.Columns["PutSymbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put PartID":
					rocOptionList.Columns["PutPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Put OpenQty":
					rocOptionList.Columns["PutOpenQty"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Put LastTraded":
					rocOptionList.Columns["PutLastTraded"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put LastPartID":
					rocOptionList.Columns["PutLastPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put PctChanged":
					rocOptionList.Columns["PutPctChanged"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put Volume":
					rocOptionList.Columns["PutVolume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put BidPartID":
					rocOptionList.Columns["PutBidPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put BidSize":
					rocOptionList.Columns["PutBidSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put BidPrice":
					rocOptionList.Columns["PutBidPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put AskSize":
					rocOptionList.Columns["PutAskSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put AskPrice":
					rocOptionList.Columns["PutAskPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Put AskPartID":
					rocOptionList.Columns["PutAskPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Call LastTraded":
					rocOptionList.Columns["CallLastTraded"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call LastPartID":
					rocOptionList.Columns["CallLastPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call PctChanged":
					rocOptionList.Columns["CallPctChanged"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call Volume":
					rocOptionList.Columns["CallVolume"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call BidPartID":
					rocOptionList.Columns["CallBidPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call BidSize":
					rocOptionList.Columns["CallBidSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call BidPrice":
					rocOptionList.Columns["CallBidPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call AskSize":
					rocOptionList.Columns["CallAskSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call AskPrice":
					rocOptionList.Columns["CallAskPrice"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call AskPartID":
					rocOptionList.Columns["CallAskPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Call PartID":
					rocOptionList.Columns["CallPartID"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Call Symbol":
					rocOptionList.Columns["CallSymbol"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "ContractSize":
					rocOptionList.Columns["ContractSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "TickSize":
					rocOptionList.Columns["TickSize"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DisplayFactor":
					rocOptionList.Columns["DisplayFactor"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "Strike":
					rocOptionList.Columns["Strike"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Expiration":
					rocOptionList.Columns["Expiration"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "DisplayExpiration":
					rocOptionList.Columns["DisplayExpiration"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "SecType":
					rocOptionList.Columns["SecType"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;

				case "MDSource":
					rocOptionList.Columns["MDSource"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "Subscribed":
					rocOptionList.Columns["Subscribed"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
				case "MoneyFlag":
					rocOptionList.Columns["MoneyFlag"].Visible = !_menuColumns.Items[e.PropertyName].Hide;
					break;
			}
		}

		private void menuColumnsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuColumnsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuColumnsContainer_Closed);
			_menuColumnsContainer = null;
			_menuColumns.PropertyChanged -= new PropertyChangedEventHandler(menuColumns_PropertyChanged);
			_menuColumns = null;
		}

		#endregion

		#region - Option Exchange Filters -

		private void ExchangeFilter_Click(object sender, EventArgs e)
		{
			if (!_doNotLoadOPRAParticipant)
			{
				if (_showOnlySelectedExchange)
				{
					ShowOnly((Label)sender);
				}
				else
				{
					FilterOnly((Label)sender);
				}
			}
		}

		private void FilterOnly(Label lbl)
		{
			switch (lbl.Name)
			{
				case "lblPHLX":
					rocOptionList.FilterOutPHLX = !rocOptionList.FilterOutPHLX;
					break;
				case "lblBOX":
					rocOptionList.FilterOutBOX = !rocOptionList.FilterOutBOX;
					break;
				case "lblISE":
					rocOptionList.FilterOutISE = !rocOptionList.FilterOutISE;
					break;
				case "lblCBOE":
					rocOptionList.FilterOutCBOE = !rocOptionList.FilterOutCBOE;
					break;
				case "lblAMEX":
					rocOptionList.FilterOutAMEX = !rocOptionList.FilterOutAMEX;
					break;
				case "lblPCX":
					rocOptionList.FilterOutPCX = !rocOptionList.FilterOutPCX;
					break;
				case "lblNSDQ":
					rocOptionList.FilterOutNSDQ = !rocOptionList.FilterOutNSDQ;
					break;
				case "lblBATS":
					rocOptionList.FilterOutBATS = !rocOptionList.FilterOutBATS;
					break;
				case "lblC2":
					rocOptionList.FilterOutC2 = !rocOptionList.FilterOutC2;
					break;
			}

			if (rocOptionList.FilterOutAll)
			{
				rocOptionList.FilterOutAll = false;
			}

			if (!rocOptionList.FilterOutPHLX)
			{
				lblPHLX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.PHLX];
			}
			else
			{
				lblPHLX.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutBOX)
			{
				lblBOX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.BOX];
			}
			else
			{
				lblBOX.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutISE)
			{
				lblISE.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.ISE];
			}
			else
			{
				lblISE.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutCBOE)
			{
				lblCBOE.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.CBOE];
			}
			else
			{
				lblCBOE.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutAMEX)
			{
				lblAMEX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.AMEX];
			}
			else
			{
				lblAMEX.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutPCX)
			{
				lblPCX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.PCX];
			}
			else
			{
				lblPCX.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutNSDQ)
			{
				lblNSDQ.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.NSDQ];
			}
			else
			{
				lblNSDQ.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutBATS)
			{
				lblBATS.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.BATS];
			}
			else
			{
				lblBATS.ForeColor = Color.DimGray;
			}

			if (!rocOptionList.FilterOutC2)
			{
				lblC2.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.C2];
			}
			else
			{
				lblC2.ForeColor = Color.DimGray;
			}
		}

		private void ShowOnly(Label lbl)
		{
			switch (lbl.Name)
			{
				case "lblPHLX":
					if (rocOptionList.FilterOutPHLX == true)
					{
						// Show All
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						// Show Only
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutPHLX = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblBOX":
					if (rocOptionList.FilterOutBOX == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutBOX = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblISE":
					if (rocOptionList.FilterOutISE == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutISE = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblCBOE":
					if (rocOptionList.FilterOutCBOE == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutCBOE = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblAMEX":
					if (rocOptionList.FilterOutAMEX == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutAMEX = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblPCX":
					if (rocOptionList.FilterOutPCX == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutPCX = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblNSDQ":
					if (rocOptionList.FilterOutNSDQ == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutNSDQ = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblBATS":
					if (rocOptionList.FilterOutBATS == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutBATS = true;
						ShowOnlyLabelReset(false);
					}
					break;
				case "lblC2":
					if (rocOptionList.FilterOutC2 == true)
					{
						rocOptionList.FilterOutAll = false;
						ShowOnlyLabelReset(true);
					}
					else
					{
						rocOptionList.FilterOutAllWithoutFilter = false;
						rocOptionList.FilterOutC2 = true;
						ShowOnlyLabelReset(false);
					}
					break;
			}

			if (rocOptionList.FilterOutPHLX)
			{
				lblPHLX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.PHLX];
			}
			else if (rocOptionList.FilterOutBOX)
			{
				lblBOX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.BOX];
			}
			else if (rocOptionList.FilterOutISE)
			{
				lblISE.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.ISE];
			}
			else if (rocOptionList.FilterOutCBOE)
			{
				lblCBOE.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.CBOE];
			}
			else if (rocOptionList.FilterOutAMEX)
			{
				lblAMEX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.AMEX];
			}
			else if (rocOptionList.FilterOutPCX)
			{
				lblPCX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.PCX];
			}
			else if (rocOptionList.FilterOutNSDQ)
			{
				lblNSDQ.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.NSDQ];
			}
			else if (rocOptionList.FilterOutBATS)
			{
				lblBATS.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.BATS];
			}
			else if (rocOptionList.FilterOutC2)
			{
				lblC2.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.C2];
			}
		}

		private void ShowOnlyLabelReset(bool showAll)
		{
			if (showAll)
			{
				lblPHLX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.PHLX];
				lblBOX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.BOX];
				lblISE.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.ISE];
				lblC2.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.C2];
				lblCBOE.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.CBOE];
				lblAMEX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.AMEX];
				lblPCX.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.PCX];
				lblNSDQ.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.NSDQ];
				lblBATS.ForeColor = rocOptionList.ExchangeColors[OptionExchangeCode.BATS];
			}
			else
			{
				lblPHLX.ForeColor = Color.DimGray;
				lblBOX.ForeColor = Color.DimGray;
				lblISE.ForeColor = Color.DimGray;
				lblC2.ForeColor = Color.DimGray;
				lblCBOE.ForeColor = Color.DimGray;
				lblAMEX.ForeColor = Color.DimGray;
				lblPCX.ForeColor = Color.DimGray;
				lblNSDQ.ForeColor = Color.DimGray;
				lblBATS.ForeColor = Color.DimGray;
			}
		}

		#endregion
	}
}