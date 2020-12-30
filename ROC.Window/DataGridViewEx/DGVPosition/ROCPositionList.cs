using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;

using BindingListEx;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCPositionList : DGVBase, INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, e);
			}
		}

		#endregion

		public enum RowColorTypes
		{
			None,
			CurrentQty,
			TotalPnL,
		}

		#region - Properties With Events -

		private bool _showColumnMenu = false;
		[Browsable(false)]
		public bool ShowColumnMenu
		{
			get
			{
				return _showColumnMenu;
			}
			set
			{
				_showColumnMenu = value;
				if (_showColumnMenu)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowColumnMenu"));
				}
			}
		}

		private bool _showActionMenu = false;
		[Browsable(false)]
		public bool ShowActionMenu
		{
			get
			{
				return _showActionMenu;
			}
			set
			{
				_showActionMenu = value;
				if (_showActionMenu)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowActionMenu"));
				}
			}
		}

		private bool _showSymbolFilterMenu = false;
		[Browsable(false)]
		public bool ShowSymbolFilterMenu
		{
			get
			{
				return _showSymbolFilterMenu;
			}
			set
			{
				_showSymbolFilterMenu = value;
				if (_showSymbolFilterMenu)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowSymbolFilterMenu"));
				}
			}
		}

		#endregion

		#region - Properties -

		private RowColorTypes _rowColorType = RowColorTypes.None;
		[DefaultValue(typeof(DataGridViewEx.ROCPositionList.RowColorTypes), "None"), Category("Appearance")]
		public RowColorTypes RowColorType
		{
			get
			{
				return _rowColorType;
			}
			set
			{
				_rowColorType = value;
			}
		}

		private string _lastSortColumnName = "";
		[Browsable(false)]
		public string LastSortColumnName
		{
			get
			{
				return _lastSortColumnName;
			}
			set
			{
				_lastSortColumnName = value;
			}
		}

		private SortOrder _lastSortColumnDirection = SortOrder.None;
		[Browsable(false)]
		public SortOrder LastSortColumnDirection
		{
			get
			{
				return _lastSortColumnDirection;
			}
			set
			{
				_lastSortColumnDirection = value;
			}
		}

		private bool _importDone = false;
		[Browsable(false)]
		public bool ImportDone
		{
			set
			{
				if (_importDone != value)
				{
					_importDone = value;
					if (_importDone)
					{
						BuildRowFilter();
					}
				}
			}
		}

		#endregion

		#region - Filter Sec Type -

		private bool _filterOutStock = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutStock
		{
			get
			{
				return _filterOutStock;
			}
			set
			{
				if (_filterOutStock != value)
				{
					_filterOutStock = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutFuture = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutFuture
		{
			get
			{
				return _filterOutFuture;
			}
			set
			{
				if (_filterOutFuture != value)
				{
					_filterOutFuture = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutOption = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutOption
		{
			get
			{
				return _filterOutOption;
			}
			set
			{
				if (_filterOutOption != value)
				{
					_filterOutOption = value;
					BuildRowFilter();
				}
			}
		}

		#endregion

		#region - Filter Symbol -

		private string _filterOutSymbols = "";
		[DefaultValue(typeof(string), ""), Category("Appearance")]
		public string FilterOutSymbols
		{
			get
			{
				return _filterOutSymbols;
			}
			set
			{
				if (_filterOutSymbols != value.ToUpper())
				{
					_filterOutSymbols = value.ToUpper();
					_filterOutSymbols = _filterOutSymbols.Trim();
					BuildRowFilter();
					onPropertyChanged(new PropertyChangedEventArgs("FilterOutSymbols"));
				}
			}
		}

		private string _showBuySymbolDetail = "";
		[DefaultValue(typeof(string), ""), Category("Appearance")]
		public string ShowBuySymbolDetail
		{
			get
			{
				return _showBuySymbolDetail;
			}
			set
			{
				//if (_showBuySymbolDetail != value)
				//{
					_showBuySymbolDetail = value;
					onPropertyChanged(new PropertyChangedEventArgs("ShowBuySymbolDetail"));
				//}
			}
		}

		private string _showSellSymbolDetail = "";
		[DefaultValue(typeof(string), ""), Category("Appearance")]
		public string ShowSellSymbolDetail
		{
			get
			{
				return _showSellSymbolDetail;
			}
			set
			{
				//if (_showSellSymbolDetail != value)
				//{
					_showSellSymbolDetail = value;
					onPropertyChanged(new PropertyChangedEventArgs("ShowSellSymbolDetail"));
				//}
			}
		}

		private string _selectedSymbolDetail = "";
		[DefaultValue(typeof(string), ""), Category("Appearance")]
		public string SelectedSymbolDetail
		{
			get
			{
				return _selectedSymbolDetail;
			}
			set
			{
				_selectedSymbolDetail = value;
			}
		}

		#endregion

		#region - Filter Mode -

		private bool _filterOutTPOS = true;
		[DefaultValue(typeof(bool), "true"), Category("Appearance")]
		public bool FilterOutTPOS
		{
			get
			{
				return _filterOutTPOS;
			}
			set
			{
				if (_filterOutTPOS != value)
				{
					_filterOutTPOS = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutROC = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutROC
		{
			get
			{
				return _filterOutROC;
			}
			set
			{
				if (_filterOutROC != value)
				{
					_filterOutROC = value;
					BuildRowFilter();
				}
			}
		}

		#endregion

		#region - Filter Accounts -

		private string _filterOutAccounts = "";
		[DefaultValue(typeof(string), ""), Category("Appearance")]
		public string FilterOutAccounts
		{
			get
			{
				return _filterOutAccounts;
			}
			set
			{
				if (_filterOutAccounts != value.ToUpper())
				{
					_filterOutAccounts = value.ToUpper();
					_filterOutAccounts = _filterOutAccounts.Trim();
					BuildRowFilter();
				}
			}
		}

		#endregion

		#region - Filter Reset -

		public void FilterReset()
		{
			_filterOutStock = false;
			_filterOutFuture = false;
			_filterOutOption = false;

			_filterOutTPOS = false;
			_filterOutROC = false;

			_filterOutSymbols = "";
			_filterOutAccounts = "";

			BuildRowFilter();
		}

		#endregion

		#region - Local Variable -

		private Color _currentCellColor = Color.Empty;

		private string _currentCellValue = "";

		private bool _isRowSelected = false;

		#endregion

		#region - Data Table -

		private DataTable _rocGridTable;
		[Browsable(false)]
		public DataTable RocGridTable
		{
			get
			{
				SetGridTable(null);
				return _rocGridTable;
			}
			set
			{
				_rocGridTable = value;
			}
		}

		private void SetGridTable(DataTable source)
		{
			if (_rocGridTable == null)
			{
				if (source != null)
				{
					_rocGridTable = source.Clone();

					_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
				}
				else
				{
					_rocGridTable = new DataTable();

					_rocGridTable.Columns.Add(new DataColumn("PositionKey", Type.GetType("System.String")));
					//List<DataColumn> keyColumns = new List<DataColumn>();
					//keyColumns.Add(_rocGridTable.Columns["PositionKey"]);

					_rocGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SymbolDetail", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SymbolDisplay", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("BidPrice", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("BidSize", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("AskPrice", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("AskSize", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("LastTraded", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("NetChange", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("PctChange", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("Volume", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("ClearingAcct", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Trader", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("OpenQty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("OpenAvg", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("BuyQty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("BuyAvg", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("SellQty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("SellAvg", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("CurrentQty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

					_rocGridTable.Columns.Add(new DataColumn("OpenPnL", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("DayPnL", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("DayRealizedPnL", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("TotalPnL", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("MarketValue", Type.GetType("System.Double")));

					_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SecurityStatus", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("Underlying", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("ExpDate", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("StrikePrice", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("CallPut", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("TPOS", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SettlePrice", Type.GetType("System.Double")));

					//_rocGridTable.PrimaryKey = keyColumns.ToArray();

					_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
				}

				_rocGridTable.DefaultView.Sort = "[SymbolDetail] asc";
				LastSort = _rocGridTable.DefaultView.Sort;
			}
		}

		private void RocGridTable_RowChanged(object sender, DataRowChangeEventArgs e)
		{
			switch (e.Action)
			{
				case DataRowAction.Add:
				case DataRowAction.Change:
				case DataRowAction.Commit:
				case DataRowAction.ChangeCurrentAndOriginal:
				case DataRowAction.ChangeOriginal:
				case DataRowAction.Delete:
				case DataRowAction.Nothing:
				case DataRowAction.Rollback:
				default:
					break;
			}
		}

		#endregion

		#region - Aggregation -

		#region - Open Qty -

		private long _openQty = 0;
		[Browsable(false)]
		public long OpenQty
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(OpenQty)", RocGridTable.DefaultView.RowFilter).ToString(), out _openQty))
					{
						return _openQty;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _openQty_STK = 0;
		[Browsable(false)]
		public long OpenQty_STK
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(OpenQty)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _openQty_STK))
					{
						return _openQty_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _openQty_FUT = 0;
		[Browsable(false)]
		public long OpenQty_FUT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(OpenQty)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _openQty_FUT))
					{
						return _openQty_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _openQty_OPT = 0;
		[Browsable(false)]
		public long OpenQty_OPT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(OpenQty)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _openQty_OPT))
					{
						return _openQty_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Buy Qty -

		private long _buyQty = 0;
		[Browsable(false)]
		public long BuyQty
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(BuyQty)", RocGridTable.DefaultView.RowFilter).ToString(), out _buyQty))
					{
						return _buyQty;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _buyQty_STK = 0;
		[Browsable(false)]
		public long BuyQty_STK
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(BuyQty)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _buyQty_STK))
					{
						return _buyQty_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _buyQty_FUT = 0;
		[Browsable(false)]
		public long BuyQty_FUT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(BuyQty)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _buyQty_FUT))
					{
						return _buyQty_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _buyQty_OPT = 0;
		[Browsable(false)]
		public long BuyQty_OPT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(BuyQty)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _buyQty_OPT))
					{
						return _buyQty_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Sell Qty -

		private long _sellQty = 0;
		[Browsable(false)]
		public long SellQty
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(SellQty)", RocGridTable.DefaultView.RowFilter).ToString(), out _sellQty))
					{
						return _sellQty;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _sellQty_STK = 0;
		[Browsable(false)]
		public long SellQty_STK
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(SellQty)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _sellQty_STK))
					{
						return _sellQty_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _sellQty_FUT = 0;
		[Browsable(false)]
		public long SellQty_FUT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(SellQty)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _sellQty_FUT))
					{
						return _sellQty_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _sellQty_OPT = 0;
		[Browsable(false)]
		public long SellQty_OPT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(SellQty)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _sellQty_OPT))
					{
						return _sellQty_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Current Qty -

		private long _currentQty = 0;
		[Browsable(false)]
		public long CurrentQty
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(CurrentQty)", RocGridTable.DefaultView.RowFilter).ToString(), out _currentQty))
					{
						return _currentQty;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _currentQty_STK = 0;
		[Browsable(false)]
		public long CurrentQty_STK
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(CurrentQty)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _currentQty_STK))
					{
						return _currentQty_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _currentQty_FUT = 0;
		[Browsable(false)]
		public long CurrentQty_FUT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(CurrentQty)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _currentQty_FUT))
					{
						return _currentQty_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _currentQty_OPT = 0;
		[Browsable(false)]
		public long CurrentQty_OPT
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(CurrentQty)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _currentQty_OPT))
					{
						return _currentQty_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Open PnL -

		private double _openPnL = 0;
		[Browsable(false)]
		public double OpenPnL
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(OpenPnL)", RocGridTable.DefaultView.RowFilter).ToString(), out _openPnL))
					{
						return _openPnL;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _openPnL_STK = 0;
		[Browsable(false)]
		public double OpenPnL_STK
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(OpenPnL)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _openPnL_STK))
					{
						return _openPnL_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _openPnL_FUT = 0;
		[Browsable(false)]
		public double OpenPnL_FUT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(OpenPnL)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _openPnL_FUT))
					{
						return _openPnL_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _openPnL_OPT = 0;
		[Browsable(false)]
		public double OpenPnL_OPT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(OpenPnL)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _openPnL_OPT))
					{
						return _openPnL_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Day PnL -

		private double _dayPnL = 0;
		[Browsable(false)]
		public double DayPnL
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayPnL)", RocGridTable.DefaultView.RowFilter).ToString(), out _dayPnL))
					{
						return _dayPnL;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _dayPnL_STK = 0;
		[Browsable(false)]
		public double DayPnL_STK
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayPnL)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _dayPnL_STK))
					{
						return _dayPnL_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _dayPnL_FUT = 0;
		[Browsable(false)]
		public double DayPnL_FUT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayPnL)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _dayPnL_FUT))
					{
						return _dayPnL_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _dayPnL_OPT = 0;
		[Browsable(false)]
		public double DayPnL_OPT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayPnL)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _dayPnL_OPT))
					{
						return _dayPnL_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Realized PnL -

		private double _realizedPnL = 0;
		[Browsable(false)]
		public double RealizedPnL
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayRealizedPnL)", RocGridTable.DefaultView.RowFilter).ToString(), out _realizedPnL))
					{
						return _realizedPnL;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _realizedPnL_STK = 0;
		[Browsable(false)]
		public double RealizedPnL_STK
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayRealizedPnL)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _realizedPnL_STK))
					{
						return _realizedPnL_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _realizedPnL_FUT = 0;
		[Browsable(false)]
		public double RealizedPnL_FUT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayRealizedPnL)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _realizedPnL_FUT))
					{
						return _realizedPnL_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _realizedPnL_OPT = 0;
		[Browsable(false)]
		public double RealizedPnL_OPT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(DayRealizedPnL)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _realizedPnL_OPT))
					{
						return _realizedPnL_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Total PnL -

		private double _totalPnL = 0;
		[Browsable(false)]
		public double TotalPnL
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(TotalPnL)", RocGridTable.DefaultView.RowFilter).ToString(), out _totalPnL))
					{
						return _totalPnL;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _totalPnL_STK = 0;
		[Browsable(false)]
		public double TotalPnL_STK
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(TotalPnL)", AggragationFilter(AggragationFilterTypes.STK)).ToString(), out _totalPnL_STK))
					{
						return _totalPnL_STK;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _totalPnL_FUT = 0;
		[Browsable(false)]
		public double TotalPnL_FUT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(TotalPnL)", AggragationFilter(AggragationFilterTypes.FUT)).ToString(), out _totalPnL_FUT))
					{
						return _totalPnL_FUT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private double _totalPnL_OPT = 0;
		[Browsable(false)]
		public double TotalPnL_OPT
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(TotalPnL)", AggragationFilter(AggragationFilterTypes.OPT)).ToString(), out _totalPnL_OPT))
					{
						return _totalPnL_OPT;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#endregion

		#region - Grid -

		public void Initialize(DataTable source)
		{
			SetGridTable(source);

			SetGrid(this);
		}

		private void SetGrid(ROCPositionList grid)
		{
			DGVPositionTextColumn txtCol;
			DGVPositionTextOverlayColumn txtoverlayCol;
			DGVPositionNumberColumn numCol;

			grid.BackgroundColor = Color.Black;
			grid.GridColor = Color.Black;

			grid.ShowCellToolTips = false;
			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.AllowUserToAddRows = false;
			grid.AllowUserToDeleteRows = false;
			grid.AllowUserToOrderColumns = true;

			grid.VirtualMode = true;

			// Position grid specials
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVPositionTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Key";
			txtCol.Name = "PositionKey";
			txtCol.DataPropertyName = "PositionKey";
			grid.Columns.Add(txtCol);

			txtCol = new DGVPositionTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "MD Symbol";
			txtCol.Name = "Symbol";
			txtCol.DataPropertyName = "Symbol";
			grid.Columns.Add(txtCol);

			txtCol = new DGVPositionTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "SecType";
			txtCol.Name = "SecType";
			txtCol.DataPropertyName = "SecType";
			grid.Columns.Add(txtCol);

			txtCol = new DGVPositionTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "SecurityStatus";
			txtCol.Name = "SecurityStatus";
			txtCol.DataPropertyName = "SecurityStatus";
			grid.Columns.Add(txtCol);

			txtoverlayCol = new DGVPositionTextOverlayColumn();
			txtoverlayCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtoverlayCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtoverlayCol.DefaultCellStyle.BackColor = Color.Black;
			txtoverlayCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtoverlayCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtoverlayCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtoverlayCol.ReadOnly = true;
			txtoverlayCol.ValueType = typeof(string);
			txtoverlayCol.HeaderText = "Symbol";
			txtoverlayCol.Name = "SymbolDetail";
			txtoverlayCol.DataPropertyName = "SymbolDetail";
			grid.Columns.Add(txtoverlayCol);

			txtoverlayCol = new DGVPositionTextOverlayColumn();
			txtoverlayCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtoverlayCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtoverlayCol.DefaultCellStyle.BackColor = Color.Black;
			txtoverlayCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtoverlayCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtoverlayCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtoverlayCol.ReadOnly = true;
			txtoverlayCol.Visible = false;
			txtoverlayCol.ValueType = typeof(string);
			txtoverlayCol.HeaderText = "Display";
			txtoverlayCol.Name = "SymbolDisplay";
			txtoverlayCol.DataPropertyName = "SymbolDisplay";
			grid.Columns.Add(txtoverlayCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.NotifyType = NotifyTypes.Border;
			numCol.IsNotified = true;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Bid";
			numCol.Name = "BidPrice";
			numCol.DataPropertyName = "BidPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BidSize";
			numCol.Name = "BidSize";
			numCol.DataPropertyName = "BidSize";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.NotifyType = NotifyTypes.Border;
			numCol.IsNotified = true;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Ask";
			numCol.Name = "AskPrice";
			numCol.DataPropertyName = "AskPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "AskSize";
			numCol.Name = "AskSize";
			numCol.DataPropertyName = "AskSize";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.NotifyType = NotifyTypes.Border;
			numCol.IsNotified = true;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Last";
			numCol.Name = "LastTraded";
			numCol.DataPropertyName = "LastTraded";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.AutoColor = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Net";
			numCol.Name = "NetChange";
			numCol.DataPropertyName = "NetChange";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.AutoColor = true;
			numCol.IsPercent = true;
			numCol.MaxDecimal = 2;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Pct";
			numCol.Name = "PctChange";
			numCol.DataPropertyName = "PctChange";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Volume";
			numCol.Name = "Volume";
			numCol.DataPropertyName = "Volume";
			grid.Columns.Add(numCol);

			txtCol = new DGVPositionTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Acct";
			txtCol.Name = "ClearingAcct";
			txtCol.DataPropertyName = "ClearingAcct";
			grid.Columns.Add(txtCol);

			txtCol = new DGVPositionTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Trader";
			txtCol.Name = "Trader";
			txtCol.DataPropertyName = "Trader";
			grid.Columns.Add(txtCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.AutoColor = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "OQty";
			numCol.Name = "OpenQty";
			numCol.DataPropertyName = "OpenQty";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "OAvg";
			numCol.Name = "OpenAvg";
			numCol.DataPropertyName = "OpenAvg";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "SettlePrice";
			numCol.Name = "SettlePrice";
			numCol.DataPropertyName = "SettlePrice";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BQty";
			numCol.Name = "BuyQty";
			numCol.DataPropertyName = "BuyQty";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 2;
			numCol.IsUnderLine = true;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.AllowResolutionChange = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BAvg";
			numCol.Name = "BuyAvg";
			numCol.DataPropertyName = "BuyAvg";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "SQty";
			numCol.Name = "SellQty";
			numCol.DataPropertyName = "SellQty";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 2;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.IsUnderLine = true;
			numCol.AllowResolutionChange = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "SAvg";
			numCol.Name = "SellAvg";
			numCol.DataPropertyName = "SellAvg";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.AutoColor = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "CQty";
			numCol.Name = "CurrentQty";
			numCol.DataPropertyName = "CurrentQty";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Mult";
			numCol.Name = "ContractSize";
			numCol.DataPropertyName = "ContractSize";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "TickSize";
			numCol.Name = "TickSize";
			numCol.DataPropertyName = "TickSize";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "DisplayFactor";
			numCol.Name = "DisplayFactor";
			numCol.DataPropertyName = "DisplayFactor";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 0;
			numCol.AutoColor = true;
			numCol.IsCurrency = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "OPnL";
			numCol.Name = "OpenPnL";
			numCol.DataPropertyName = "OpenPnL";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 0;
			numCol.AutoColor = true;
			numCol.IsCurrency = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "DPnL";
			numCol.Name = "DayPnL";
			numCol.DataPropertyName = "DayPnL";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 0;
			numCol.AutoColor = true;
			numCol.IsCurrency = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "TPnL";
			numCol.Name = "TotalPnL";
			numCol.DataPropertyName = "TotalPnL";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 0;
			numCol.AutoColor = true;
			numCol.IsCurrency = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Market";
			numCol.Name = "MarketValue";
			numCol.DataPropertyName = "MarketValue";
			grid.Columns.Add(numCol);

			numCol = new DGVPositionNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 0;
			numCol.AutoColor = true;
			numCol.IsCurrency = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "RPnL";
			numCol.Name = "DayRealizedPnL";
			numCol.DataPropertyName = "DayRealizedPnL";
			grid.Columns.Add(numCol);

			foreach (DataGridViewColumn col in Columns)
			{
				col.SortMode = DataGridViewColumnSortMode.NotSortable;
			}

			grid.DataSource = RocGridTable.DefaultView;
		}

		public void UpdateResolution(int maxDecimal)
		{
			int doneCount = 0;
			foreach (DataGridViewColumn col in Columns)
			{
				switch (col.Name)
				{
					case "SellAvg":
					case "BuyAvg":
						((DGVPositionNumberColumn)col).MaxDecimal = maxDecimal;
						doneCount = doneCount + 1;
						break;
					default:
						break;
				}

				if (doneCount == 2)
				{
					break;
				}
			}
		}

		#endregion

		#region - Grid Event -

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			if (e.RowIndex >= 0 &&
				!((DGVBaseTextOverlayCell)this["SymbolDetail", e.RowIndex]).Flaged &&
				this["SecurityStatus", e.RowIndex].Value != null && 
				this["SecurityStatus", e.RowIndex].Value.ToString() != "")
			{
				InvalidateCell(Columns["SymbolDetail"].Index, e.RowIndex);
				InvalidateCell(Columns["SymbolDisplay"].Index, e.RowIndex);
			}
			base.OnRowPrePaint(e);
		}

		protected override void OnRowPostPaint(DataGridViewRowPostPaintEventArgs e)
		{
			switch (RowColorType)
			{
				case RowColorTypes.CurrentQty:
					_currentCellColor = ((DGVBaseNotifiedNumberCell)Rows[e.RowIndex].Cells["CurrentQty"]).CurrentForeColor;
					break;
				case RowColorTypes.TotalPnL:
					_currentCellColor = ((DGVBaseNotifiedNumberCell)Rows[e.RowIndex].Cells["TotalPnL"]).CurrentForeColor;
					break;
				case RowColorTypes.None:
				default:
					_currentCellColor = Color.Empty;
					break;
			}

			if ((e.State & DataGridViewElementStates.Selected) == DataGridViewElementStates.Selected)
			{
				_isRowSelected = true;
			}
			else
			{
				_isRowSelected = false;
			}

			for (int i = 0; i < Columns.Count; i++)
			{
				if (Columns[i].Visible)
				{
					switch (Columns[i].Name)
					{
						case "BidPrice":
						case "AskPrice":
						case "LastTraded":
						case "NetChange":
						case "PctChange":
						case "CurrentQty":
						case "TotalPnL":
							if (((DGVPositionNumberColumn)Columns[i]).NotifyType == NotifyTypes.None)
							{
								if (_isRowSelected & _currentCellColor != Color.Empty)
								{
									if (Rows[e.RowIndex].Cells[i].Style.SelectionForeColor != _currentCellColor)
									{
										Rows[e.RowIndex].Cells[i].Style.SelectionForeColor = _currentCellColor;
									}
								}
								else
								{
									if (Rows[e.RowIndex].Cells[i].Style.ForeColor != _currentCellColor)
									{
										Rows[e.RowIndex].Cells[i].Style.ForeColor = _currentCellColor;
									}
								}
							}
							break;
						default:
							if (_isRowSelected && _currentCellColor != Color.Empty)
							{
								if (Rows[e.RowIndex].Cells[i].Style.SelectionForeColor != _currentCellColor)
								{
									Rows[e.RowIndex].Cells[i].Style.SelectionForeColor = _currentCellColor;
								}
							}
							else
							{
								if (Rows[e.RowIndex].Cells[i].Style.ForeColor != _currentCellColor)
								{
									Rows[e.RowIndex].Cells[i].Style.ForeColor = _currentCellColor;
								}
							}
							break;
					}
				}
			}

			base.OnRowPostPaint(e);
		}

		protected override void OnColumnHeaderMouseClick(DataGridViewCellMouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					ShowColumnMenu = true;
					break;
				default:
					if (_lastSortColumnName != Columns[e.ColumnIndex].Name)
					{
						_lastSortColumnName = Columns[e.ColumnIndex].Name;
						_lastSortColumnDirection = SortOrder.Ascending;
						RocGridTable.DefaultView.Sort = _lastSortColumnName + " ASC";
					}
					else
					{
						if (_lastSortColumnDirection == SortOrder.Ascending)
						{
							_lastSortColumnDirection = SortOrder.Descending;
							RocGridTable.DefaultView.Sort = _lastSortColumnName + " DESC";
						}
						else
						{
							_lastSortColumnDirection = SortOrder.Ascending;
							RocGridTable.DefaultView.Sort = _lastSortColumnName + " ASC";
						}
					}
					break;
			}

			base.OnColumnHeaderMouseClick(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if (e.Y > ColumnHeadersHeight && Rows.Count == 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Right:
						ShowActionMenu = true;
						break;
				}
			}

			base.OnMouseDown(e);
		}

		protected override void OnCellMouseDoubleClick(DataGridViewCellMouseEventArgs e)
		{
			base.OnCellMouseDoubleClick(e);

			//if (e.RowIndex >= 0)
			//{
			//    switch (Columns[e.ColumnIndex].Name)
			//    {
			//        case "SymbolDetail":
			//            _currentCellValue = Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString().Trim();
			//            if (FilterOutSymbols == _currentCellValue)
			//            {
			//                FilterOutSymbols = "";
			//            }
			//            else
			//            {
			//                FilterOutSymbols = Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString().Trim();
			//            }
			//            break;
			//        default:
			//            break;
			//    }
			//}
		}

		protected override void OnCellMouseClick(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Right:
						switch (Columns[e.ColumnIndex].Name)
						{
							case "Symbol":
							case "SymbolDetail":
							case "SymbolDisplay":
								if (Rows[e.RowIndex].Cells["SymbolDetail"].Value != null && Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString() != "")
								{
									SelectedSymbolDetail = Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString();
									ShowSymbolFilterMenu = true;
								}
								else
								{
									SelectedSymbolDetail = "";
								}
								break;
							default:
								ShowActionMenu = true;
								break;
						}
						break;
					default:
						switch (Columns[e.ColumnIndex].Name)
						{
							case "BuyAvg":
								_currentCellValue = Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().Trim();
								if (ShowBuySymbolDetail == _currentCellValue)
								{
									ShowBuySymbolDetail = "";
								}
								else
								{
									ShowBuySymbolDetail = Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().Trim();
								}
								break;
							case "SellAvg":
								_currentCellValue = Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().Trim();
								if (ShowSellSymbolDetail == _currentCellValue)
								{
									ShowSellSymbolDetail = "";
								}
								else
								{
									ShowSellSymbolDetail = Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().Trim();
								}
								break;
						}
						break;
				}
			}

			base.OnCellMouseClick(e);
		}

		protected override void OnDataError(bool displayErrorDialogIfNoHandler, DataGridViewDataErrorEventArgs e)
		{
			//base.OnDataError(displayErrorDialogIfNoHandler, e);
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				DataSource = null;

				RocGridTable.Clear();
				RocGridTable = null;

				Columns.Clear();
			}
			base.Dispose(disposing);
		}

		#endregion

		#region - Row Filter -

		private void BuildRowFilter()
		{
			string filter = "";

			if (FilterOutSymbols != "")
			{
				string[] symbols = FilterOutSymbols.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);

				if (symbols.Length > 1)
				{
					foreach (string symbol in symbols)
					{
						filter = BuildRowFilterString(filter, String.Concat(new object[] { "SymbolDetail Not Like '", symbol, "%'" }));
					}

					filter = String.Concat(new object[] { "(", filter, ")" });
				}
				else
				{
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SymbolDetail Not Like '", symbols[0], "%'" }));
				}
			}

			if (FilterOutAccounts != "")
			{
				string[] accts = FilterOutAccounts.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);

				foreach (string acct in accts)
				{
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "ClearingAcct Not Like '", acct, "%'" }));
				}
			}

			if (FilterOutStock)
			{
				filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.Equity, "'" }));
			}

			if (FilterOutFuture)
			{
				filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.Future, "'" }));
				filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.SingleStockFuture, "'" }));
			}

			if (FilterOutOption)
			{
				filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.Option, "'" }));
				filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.OptionFuture, "'" }));
				filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.OptionIndex, "'" }));
			}

			//if (FilterOutTPOS)
			//{
			//    filter = BuildRowFilterString(filter, String.Concat(new object[] { "TPOS <> 1" }));
			//}

			//if (FilterOutROC)
			//{
			//    filter = BuildRowFilterString(filter, String.Concat(new object[] { "TPOS <> 0" }));
			//}

			try
			{
				lock (RocGridTable)
				{
					RocGridTable.DefaultView.RowFilter = filter;
				}

				RefreshSharedRows = true;
				RefreshAggragation = true;
				ShouldScrollToLastRow = true;
			}
			catch (Exception ex)
			{
				Debug.Assert(false, ex.Message, ex.StackTrace);
			}
		}

		private enum AggragationFilterTypes
		{
			STK,
			FUT,
			OPT,
		}

		private string AggragationFilter(AggragationFilterTypes type)
		{
			string filter = RocGridTable.DefaultView.RowFilter;

			switch (type)
			{
				case AggragationFilterTypes.STK:
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType = '", CSVEx.CSVFieldIDs.SecurityTypes.Equity, "'" }));
					break;
				case AggragationFilterTypes.FUT:
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType = '", CSVEx.CSVFieldIDs.SecurityTypes.Future, "'" }));
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType = '", CSVEx.CSVFieldIDs.SecurityTypes.SingleStockFuture, "'" }), true);
					break;
				case AggragationFilterTypes.OPT:
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType = '", CSVEx.CSVFieldIDs.SecurityTypes.Option, "'" }));
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType = '", CSVEx.CSVFieldIDs.SecurityTypes.OptionFuture, "'" }), true);
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType = '", CSVEx.CSVFieldIDs.SecurityTypes.OptionIndex, "'" }), true);
					break;
			}
			return filter;
		}

		#endregion

		#region - Get All Positions with Non Zero Current Qty -

		public DataRow[] GetCurrentNonZeroPositions()
		{
			lock (RocGridTable)
			{
				return RocGridTable.Select("CurrentQty <> 0");
			}
		}

		#endregion
	}
}
