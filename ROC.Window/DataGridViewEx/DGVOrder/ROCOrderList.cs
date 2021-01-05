using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCOrderList : DGVBase, INotifyPropertyChanged
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
			Status,
			Side,
		}

		public class RowSelected
		{
			public string orderID = "";
			public string side = "";
			public string Symbol = "";
			public string mdSymbol = "";
			public string shares = "";
			public string price = "";
			public string exchange = "";
		}

		#region - Properties With Events -

		private bool _showMenu = false;
		[Browsable(false)]
		public bool ShowMenu
		{
			get
			{
				return _showMenu;
			}
			set
			{
				_showMenu = value;
				if (_showMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowMenu"));
				}
			}
		}

		private RowSelected _showMenuSymbol = new RowSelected();
		[Browsable(false)]
		public RowSelected ShowMenuSymbol
		{
			get
			{
				return _showMenuSymbol;
			}
			set
			{
				_showMenuSymbol = value;
			}
		}

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

		#endregion

		#region - Properties -

		private RowColorTypes _rowColorType = RowColorTypes.Status;
		[DefaultValue(typeof(DataGridViewEx.ROCOrderList.RowColorTypes), "Status"), Category("Appearance")]
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

		private Dictionary<string, long> _orderStatuses = new Dictionary<string,long>();
		[Browsable(false)]
		public Dictionary<string, long> OrderStatuses
		{
			get
			{
				return _orderStatuses;
			}
			set
			{
				_orderStatuses = value;
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

		#region - Filter Order Type -

		private bool _filterOutLimit = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutLimit
		{
			get
			{
				return _filterOutLimit;
			}
			set
			{
				if (_filterOutLimit != value)
				{
					_filterOutLimit = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutMarket = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutMarket
		{
			get
			{
				return _filterOutMarket;
			}
			set
			{
				if (_filterOutMarket != value)
				{
					_filterOutMarket = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutDay = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutDay
		{
			get
			{
				return _filterOutDay;
			}
			set
			{
				if (_filterOutDay != value)
				{
					_filterOutDay = value;
					BuildRowFilter();
				}
			}
		}

		#endregion

		#region - Filter Status -

		private bool _filterOutFilled = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutFilled
		{
			get
			{
				return _filterOutFilled;
			}
			set
			{
				if (_filterOutFilled != value)
				{
					_filterOutFilled = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutPartialFilled = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutPartialFilled
		{
			get
			{
				return _filterOutPartialFilled;
			}
			set
			{
				if (_filterOutPartialFilled != value)
				{
					_filterOutPartialFilled = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutReplaced = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutReplaced
		{
			get
			{
				return _filterOutReplaced;
			}
			set
			{
				if (_filterOutReplaced != value)
				{
					_filterOutReplaced = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutOpen = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutOpen
		{
			get
			{
				return _filterOutOpen;
			}
			set
			{
				if (_filterOutOpen != value)
				{
					_filterOutOpen = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutCanceled = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutCanceled
		{
			get
			{
				return _filterOutCanceled;
			}
			set
			{
				if (_filterOutCanceled != value)
				{
					_filterOutCanceled = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutRejected = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutRejected
		{
			get
			{
				return _filterOutRejected;
			}
			set
			{
				if (_filterOutRejected != value)
				{
					_filterOutRejected = value;
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

		private string _showThisTag = "";
		[DefaultValue(typeof(string), ""), Category("Appearance")]
		public string ShowThisTag
		{
			get
			{
				return _showThisTag;
			}
			set
			{
				if (_showThisTag != value)
				{
					_showThisTag = value;
				}
			}
		}

		private string _showThisOMTag = "";
		[DefaultValue(typeof(string), ""), Category("Appearance")]
		public string ShowThisOMTag
		{
			get
			{
				return _showThisOMTag;
			}
			set
			{
				//if (_showThisOMTag != value)
				//{
					_showThisOMTag = value;
					onPropertyChanged(new PropertyChangedEventArgs("ShowThisOMTag"));
				//}
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

			_filterOutFilled = false;
			_filterOutPartialFilled = false;
			_filterOutReplaced = false;
			_filterOutOpen = false;
			_filterOutCanceled = false;
			_filterOutRejected = false;

			_filterOutSymbols = "";
			_filterOutAccounts = "";

			BuildRowFilter();
		}

		#endregion

		#region - Local Variable -

		private Color _currentCellColor = Color.Empty;

		private string _currentCellValue = "";

		private bool _isRowSelected = false;

		private DataRow[] _selectedRows = new DataRow[0];

		private	ToolTip _toolTip = new ToolTip();

		private Point _mousePoint = new Point();

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
				}
				else
				{
					_rocGridTable = new DataTable();

					_rocGridTable.Columns.Add(new DataColumn("Tag", Type.GetType("System.String")));
					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_rocGridTable.Columns["Tag"]);
					_rocGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SymbolDetail", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SymbolDisplay", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Status", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("Side", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("Qty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("Price", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("StopPrice", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("PegPrice", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("LeaveQty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("CumQty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("AvgPrice", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("OrderType", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("TIF", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("AlgoType", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("AlgoEndTime", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("DestID", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("OmTime", Type.GetType("System.DateTime")));
					_rocGridTable.Columns.Add(new DataColumn("ClearingAcct", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

					_rocGridTable.Columns.Add(new DataColumn("OMTag", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Text", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("ParentTag", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Echo", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("CplxOrderType", Type.GetType("System.Int64")));

					_rocGridTable.PrimaryKey = keyColumns.ToArray();

					_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
				}

				_rocGridTable.DefaultView.Sort = "[OmTime] asc";
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

		private string filterBySide(long side)
		{
			if (RocGridTable.DefaultView.RowFilter != "")
			{
				return string.Concat(RocGridTable.DefaultView.RowFilter, " and Side = ", side.ToString());
			}
			else
			{
				return string.Concat("Side = ", side.ToString());
			}
		}

		private long _buySharesCount = 0;
		[Browsable(false)]
		public long BuySharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(Qty)", filterBySide(CSVEx.CSVFieldIDs.SideCodes.Buy)).ToString(), out _buySharesCount))
					{
						return _buySharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _sellSharesCount = 0;
		[Browsable(false)]
		public long SellSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(Qty)", filterBySide(CSVEx.CSVFieldIDs.SideCodes.Sell)).ToString(), out _sellSharesCount))
					{
						return _sellSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _shortSharesCount = 0;
		[Browsable(false)]
		public long ShortSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(Qty)", filterBySide(CSVEx.CSVFieldIDs.SideCodes.Short)).ToString(), out _shortSharesCount))
					{
						return _shortSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private string _openFilter = "";
		private string openFilter
		{
			get
			{
				if (_openFilter == "")
				{
					_openFilter = string.Concat(
						"Status <> ", 
						CSVEx.CSVFieldIDs.StatusCodes.Filled.ToString(), 
						" and Status <> ",
						CSVEx.CSVFieldIDs.StatusCodes.Canceled.ToString(),
						" and Status <> ",
						CSVEx.CSVFieldIDs.StatusCodes.Rejected.ToString());
				}

				if (RocGridTable.DefaultView.RowFilter != "")
				{
					return string.Concat(RocGridTable.DefaultView.RowFilter, " and ", _openFilter);
				}
				else
				{
					return _openFilter;
				}
			}
		}

		private string openFilterBySide(long side)
		{
			return string.Concat(openFilter, " and Side = ", side.ToString());
		}

		private string _filledFilter = "";
		private string filledFilter
		{
			get
			{
				if (_filledFilter == "")
				{
					_filledFilter = string.Concat(
						"(Status = ", 
						CSVEx.CSVFieldIDs.StatusCodes.Filled.ToString(), 
						" or Status = ",
						CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled.ToString(),
						" or Status = ",
						CSVEx.CSVFieldIDs.StatusCodes.FilledAndCancelled.ToString(),
						" or Status = ", 
						CSVEx.CSVFieldIDs.StatusCodes.ReplacedAndFilled.ToString(),
						")");
				}

				if (RocGridTable.DefaultView.RowFilter != "")
				{
					return string.Concat(RocGridTable.DefaultView.RowFilter, " and ", _filledFilter);
				}
				else
				{
					return _filledFilter;
				}
			}
		}

		private string filledFilterBySide(long side)
		{
			return string.Concat(filledFilter, " and Side = ", side.ToString());
		}

		private long _openSharesCount = 0;
		[Browsable(false)]
		public long OpenSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(LeaveQty)", openFilter).ToString(), out _openSharesCount))
					{
						return _openSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _openBuySharesCount = 0;
		[Browsable(false)]
		public long OpenBuySharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(LeaveQty)", openFilterBySide(CSVEx.CSVFieldIDs.SideCodes.Buy)).ToString(), out _openBuySharesCount))
					{
						return _openBuySharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _openSellSharesCount = 0;
		[Browsable(false)]
		public long OpenSellSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(LeaveQty)", openFilterBySide(CSVEx.CSVFieldIDs.SideCodes.Sell)).ToString(), out _openSellSharesCount))
					{
						return _openSellSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _openShortSharesCount = 0;
		[Browsable(false)]
		public long OpenShortSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(LeaveQty)", openFilterBySide(CSVEx.CSVFieldIDs.SideCodes.Short)).ToString(), out _openShortSharesCount))
					{
						return _openShortSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _filledSharesCount = 0;
		[Browsable(false)]
		public long FilledSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(CumQty)", filledFilter).ToString(), out _filledSharesCount))
					{
						return _filledSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _filledBuySharesCount = 0;
		[Browsable(false)]
		public long FilledBuySharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(CumQty)", filledFilterBySide(CSVEx.CSVFieldIDs.SideCodes.Buy)).ToString(), out _filledBuySharesCount))
					{
						return _filledBuySharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _filledSellSharesCount = 0;
		[Browsable(false)]
		public long FilledSellSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(CumQty)", filledFilterBySide(CSVEx.CSVFieldIDs.SideCodes.Sell)).ToString(), out _filledSellSharesCount))
					{
						return _filledSellSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _filledShortSharesCount = 0;
		[Browsable(false)]
		public long FilledShortSharesCount
		{
			get
			{
				try
				{
					if (long.TryParse(RocGridTable.Compute("Sum(CumQty)", filledFilterBySide(CSVEx.CSVFieldIDs.SideCodes.Short)).ToString(), out _filledShortSharesCount))
					{
						return _filledShortSharesCount;
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

		#region - Grid -

		public void Initialize(DataTable source)
		{
			SetGridTable(source);
			
			SetGrid(this);
		}

		private void SetGrid(ROCOrderList grid)
		{
			DGVOrderTextColumn txtCol;
			DGVOrderNumberColumn numCol;
			DGVOrderStatusColumn staCol;
			DGVOrderSideColumn sidCol;
			DGVOrderTypeColumn typCol;
			DGVOrderTIFColumn tifCol;
			DGVOrderAlgoTypeColumn algoCol;
			DGVOrderDestinationColumn desCol;
			DGVOrderTimeColumn timCol;

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

			// Order grid specials
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Order ID";
			txtCol.Name = "OrderID";
			txtCol.DataPropertyName = "Tag";
			grid.Columns.Add(txtCol);

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "BaseSymbol";
			txtCol.Name = "Symbol";
			txtCol.DataPropertyName = "Symbol";
			grid.Columns.Add(txtCol);

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Symbol";
			txtCol.Name = "SymbolDetail";
			txtCol.DataPropertyName = "SymbolDetail";
			grid.Columns.Add(txtCol);

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Display";
			txtCol.Name = "SymbolDisplay";
			txtCol.DataPropertyName = "SymbolDisplay";
			grid.Columns.Add(txtCol);

			staCol = new DGVOrderStatusColumn();
			staCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			staCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			staCol.DefaultCellStyle.BackColor = Color.Black;
			staCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			staCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			staCol.SortMode = DataGridViewColumnSortMode.Automatic;
			staCol.ReadOnly = true;
			staCol.ValueType = typeof(long);
			staCol.DefaultCellStyle.Format = "";
			staCol.HeaderText = "Status";
			staCol.Name = "Status";
			staCol.DataPropertyName = "Status";
			grid.Columns.Add(staCol);

			sidCol = new DGVOrderSideColumn();
			sidCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			sidCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			sidCol.DefaultCellStyle.BackColor = Color.Black;
			sidCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			sidCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			sidCol.SortMode = DataGridViewColumnSortMode.Automatic;
			sidCol.ReadOnly = true;
			sidCol.ValueType = typeof(long);
			sidCol.DefaultCellStyle.Format = "";
			sidCol.HeaderText = "Side";
			sidCol.Name = "Side";
			sidCol.DataPropertyName = "Side";
			grid.Columns.Add(sidCol);

			numCol = new DGVOrderNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Qty";
			numCol.Name = "Qty";
			numCol.DataPropertyName = "Qty";
			grid.Columns.Add(numCol);

			numCol = new DGVOrderNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Leave";
			numCol.Name = "Leave";
			numCol.DataPropertyName = "LeaveQty";
			grid.Columns.Add(numCol);

			numCol = new DGVOrderNumberColumn();
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
			numCol.HeaderText = "Price";
			numCol.Name = "Price";
			numCol.DataPropertyName = "Price";
			grid.Columns.Add(numCol);

			numCol = new DGVOrderNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.IsFraction = true;
			numCol.CheckFor64Th = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "StopPrice";
			numCol.Name = "StopPrice";
			numCol.DataPropertyName = "StopPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVOrderNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PegPrice";
			numCol.Name = "PegPrice";
			numCol.DataPropertyName = "PegPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVOrderNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Filled";
			numCol.Name = "Filled";
			numCol.DataPropertyName = "CumQty";
			grid.Columns.Add(numCol);

			numCol = new DGVOrderNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.MaxDecimal = 2;
			numCol.IsFraction = true;
			numCol.IsUnderLine = true;
			numCol.CheckFor64Th = true;
			numCol.AllowResolutionChange = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "AvgPrice";
			numCol.Name = "AvgPrice";
			numCol.DataPropertyName = "AvgPrice";
			grid.Columns.Add(numCol);

			typCol = new DGVOrderTypeColumn();
			typCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			typCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			typCol.DefaultCellStyle.BackColor = Color.Black;
			typCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			typCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			typCol.SortMode = DataGridViewColumnSortMode.Automatic;
			typCol.ReadOnly = true;
			typCol.ValueType = typeof(long);
			typCol.DefaultCellStyle.Format = "";
			typCol.HeaderText = "Type";
			typCol.Name = "Type";
			typCol.DataPropertyName = "OrderType";
			grid.Columns.Add(typCol);

			algoCol = new DGVOrderAlgoTypeColumn();
			algoCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			algoCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			algoCol.DefaultCellStyle.BackColor = Color.Black;
			algoCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			algoCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			algoCol.SortMode = DataGridViewColumnSortMode.Automatic;
			algoCol.ReadOnly = true;
			algoCol.Visible = false;
			algoCol.ValueType = typeof(long);
			algoCol.DefaultCellStyle.Format = "";
			algoCol.HeaderText = "Algo";
			algoCol.Name = "AlgoType";
			algoCol.DataPropertyName = "AlgoType";
			grid.Columns.Add(algoCol);

			tifCol = new DGVOrderTIFColumn();
			tifCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			tifCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			tifCol.DefaultCellStyle.BackColor = Color.Black;
			tifCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			tifCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			tifCol.SortMode = DataGridViewColumnSortMode.Automatic;
			tifCol.ReadOnly = true;
			tifCol.ValueType = typeof(double);
			tifCol.DefaultCellStyle.Format = "";
			tifCol.HeaderText = "Duration";
			tifCol.Name = "Duration";
			tifCol.DataPropertyName = "TIF";
			grid.Columns.Add(tifCol);

			desCol = new DGVOrderDestinationColumn();
			desCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			desCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			desCol.DefaultCellStyle.BackColor = Color.Black;
			desCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			desCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			desCol.SortMode = DataGridViewColumnSortMode.Automatic;
			desCol.ReadOnly = true;
			desCol.ValueType = typeof(long);
			desCol.DefaultCellStyle.Format = "";
			desCol.HeaderText = "Exchange";
			desCol.Name = "Exchange";
			desCol.DataPropertyName = "DestID";
			grid.Columns.Add(desCol);

			timCol = new DGVOrderTimeColumn();
			timCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			timCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			timCol.DefaultCellStyle.BackColor = Color.Black;
			timCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			timCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			timCol.SortMode = DataGridViewColumnSortMode.Automatic;
			timCol.ReadOnly = true;
			timCol.ValueType = typeof(DateTime);
			timCol.DefaultCellStyle.Format = "";
			timCol.HeaderText = "Time";
			timCol.Name = "Time";
			timCol.DataPropertyName = "OmTime";
			grid.Columns.Add(timCol);

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "AlgoEndTime";
			txtCol.Name = "AlgoEndTime";
			txtCol.DataPropertyName = "AlgoEndTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ShowLength = 5;
			txtCol.ValueType = typeof(string);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Account";
			txtCol.Name = "Account";
			txtCol.DataPropertyName = "ClearingAcct";
			grid.Columns.Add(txtCol);

			numCol = new DGVOrderNumberColumn();
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

			numCol = new DGVOrderNumberColumn();
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

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "SecType";
			txtCol.Name = "SecType";
			txtCol.DataPropertyName = "SecType";
			grid.Columns.Add(txtCol);

			numCol = new DGVOrderNumberColumn();
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

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "PTag";
			txtCol.Name = "ParentTag";
			txtCol.DataPropertyName = "ParentTag";
			grid.Columns.Add(txtCol);

			txtCol = new DGVOrderTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Echo";
			txtCol.Name = "Echo";
			txtCol.DataPropertyName = "Echo";
			grid.Columns.Add(txtCol);

			grid.DataSource = RocGridTable.DefaultView;
		}

		public void UpdateResolution(int maxDecimal)
		{
			int doneCount = 0;
			foreach (DataGridViewColumn col in Columns)
			{
				switch (col.Name)
				{
					case "AvgPrice":
						((DGVOrderNumberColumn)col).MaxDecimal = maxDecimal;
						doneCount = doneCount + 1;
						break;
					default:
						break;
				}

				if (doneCount == 1)
				{
					break;
				}
			}
		}

		#endregion

		#region - Grid Events -

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			switch (RowColorType)
			{
				case RowColorTypes.Status:
					_currentCellColor = GetStatusColor((Int64)Rows[e.RowIndex].Cells["Status"].Value);
					break;
				case RowColorTypes.Side:
					_currentCellColor = GetSideColor((Int64)Rows[e.RowIndex].Cells["Side"].Value);
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
						case "Status":
						case "Side":
							break;
						default:
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
							break;
					}
				}
			}
			base.OnRowPrePaint(e);
		}

		private Color GetStatusColor(long statusCode)
		{
			switch (statusCode)
			{
				case CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled:
					return ((DGVBaseStatusColumn)Columns["Status"]).PartialFilledColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Filled:
					return ((DGVBaseStatusColumn)Columns["Status"]).FilledColor;
				case CSVEx.CSVFieldIDs.StatusCodes.DoneForDay:
					return ((DGVBaseStatusColumn)Columns["Status"]).DoneForDayColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Canceled:
					return ((DGVBaseStatusColumn)Columns["Status"]).CancelledColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Replaced:
					return ((DGVBaseStatusColumn)Columns["Status"]).ReplacedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.PendingCancel:
					return ((DGVBaseStatusColumn)Columns["Status"]).PendingCancelColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Stopped:
					return ((DGVBaseStatusColumn)Columns["Status"]).StoppedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Rejected:
					return ((DGVBaseStatusColumn)Columns["Status"]).RejectedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Calculated:
					return ((DGVBaseStatusColumn)Columns["Status"]).CalculatedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Expired:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Open:
					return ((DGVBaseStatusColumn)Columns["Status"]).OpenColor;
				case CSVEx.CSVFieldIDs.StatusCodes.CancelRejected:
					return ((DGVBaseStatusColumn)Columns["Status"]).CancelRejectedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Corrected:
					return ((DGVBaseStatusColumn)Columns["Status"]).CorrectedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Busted:
					return ((DGVBaseStatusColumn)Columns["Status"]).BustedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.ReplaceRejected:
					return ((DGVBaseStatusColumn)Columns["Status"]).ReplaceRejectedColor;
				case CSVEx.CSVFieldIDs.StatusCodes.ReplacePending:
					return ((DGVBaseStatusColumn)Columns["Status"]).ReplacePendingColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Exposed:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				case CSVEx.CSVFieldIDs.StatusCodes.DirectedThruIML:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				case CSVEx.CSVFieldIDs.StatusCodes.Directed:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				case CSVEx.CSVFieldIDs.StatusCodes.DirectedReject:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				case CSVEx.CSVFieldIDs.StatusCodes.ROMCancelPending:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				case CSVEx.CSVFieldIDs.StatusCodes.ROMReplacePending:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				case CSVEx.CSVFieldIDs.StatusCodes.FilledAndCancelled:
					return ((DGVBaseStatusColumn)Columns["Status"]).FilledColor;
				case CSVEx.CSVFieldIDs.StatusCodes.ReplacedAndFilled:
					return ((DGVBaseStatusColumn)Columns["Status"]).FilledColor;
				case CSVEx.CSVFieldIDs.StatusCodes.New:
					return ((DGVBaseStatusColumn)Columns["Status"]).OtherColor;
				default:
					return Color.Empty;
					//return ((DGVBaseStatusColumn)Columns["Status"]).DefaultCellStyle.ForeColor;
			}
		}

		private Color GetSideColor(long sideCode)
		{
			switch (sideCode)
			{
				case CSVEx.CSVFieldIDs.SideCodes.Buy:
					return ((DGVBaseSideColumn)Columns["Side"]).BuyColor;
				case CSVEx.CSVFieldIDs.SideCodes.Sell:
					return ((DGVBaseSideColumn)Columns["Side"]).SellColor;
				case CSVEx.CSVFieldIDs.SideCodes.Short:
					return ((DGVBaseSideColumn)Columns["Side"]).SellShortColor;
				case CSVEx.CSVFieldIDs.SideCodes.SSE:
					return ((DGVBaseSideColumn)Columns["Side"]).SSEColor;
				default:
					return Color.Empty;
					//return ((DGVBaseSideColumn)Columns["Side"]).DefaultCellStyle.ForeColor;
			}
		}

		protected override void OnColumnHeaderMouseClick(DataGridViewCellMouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					ShowColumnMenu = true;
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

			if (e.RowIndex >= 0)
			{
				switch (Columns[e.ColumnIndex].Name)
				{
					case "SymbolDetail":
					case "SymbolDisplay":
						_currentCellValue = Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString().Trim();
						if (FilterOutSymbols == _currentCellValue)
						{
							FilterOutSymbols = "";
						}
						else
						{
							FilterOutSymbols = _currentCellValue;
						}
						break;
					default:
						break;
				}
			}
		}

		protected override void OnCellMouseClick(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Right:
						if (CSVEx.CSVFieldIDs.StatusCodes.IsActive((long)Rows[e.RowIndex].Cells["Status"].Value))
						{
							SetRowSelected(Rows[e.RowIndex]);
							ShowMenu = true;
						}
						else
						{
							ShowActionMenu = true;
						}
						break;
					case MouseButtons.Left:
					default:
						switch (Columns[e.ColumnIndex].Name)
						{
							case "Status":
								_selectedRows = RocGridTable.Select("Tag = '" + Rows[e.RowIndex].Cells["OrderID"].Value.ToString() + "'");

								if (_selectedRows.Length > 0 && _selectedRows[0]["Text"] != null)
								{
									_toolTip.Active = true;
									_toolTip.Show(_selectedRows[0]["Text"].ToString(), this, _mousePoint.X + 16, _mousePoint.Y, 2700);
								}
								break;
							case "AvgPrice":
								_currentCellValue = Rows[e.RowIndex].Cells["OrderID"].Value.ToString().Trim();
								if (ShowThisTag == _currentCellValue)
								{
									ShowThisTag = "";
								}
								else
								{
									ShowThisTag = Rows[e.RowIndex].Cells["OrderID"].Value.ToString().Trim();

									_selectedRows = RocGridTable.Select("Tag = '" + Rows[e.RowIndex].Cells["OrderID"].Value.ToString() + "'");

									if (_selectedRows.Length > 0 && _selectedRows[0]["OMTag"] != null)
									{
										ShowThisOMTag = _selectedRows[0]["OMTag"].ToString();
									}
								}
								break;
						}
						break;
				}
			}

			base.OnCellMouseClick(e);
		}

		private void SetRowSelected(DataGridViewRow row)
		{
			ShowMenuSymbol = new RowSelected();
			if (row.Cells["SymbolDetail"].Value != null)
			{
				ShowMenuSymbol.Symbol = row.Cells["SymbolDetail"].Value.ToString();
			}
			if (row.Cells["Symbol"].Value != null)
			{
				ShowMenuSymbol.mdSymbol = row.Cells["Symbol"].Value.ToString();
			}
			if (row.Cells["Exchange"].Value != null)
			{
				ShowMenuSymbol.exchange = row.Cells["Exchange"].Value.ToString();
			}
			if (row.Cells["OrderID"].Value != null)
			{
				ShowMenuSymbol.orderID = row.Cells["OrderID"].Value.ToString();
			}
			if (row.Cells["Side"].Value != null)
			{
				ShowMenuSymbol.side = row.Cells["Side"].Value.ToString();
			}
			if (row.Cells["Qty"].Value != null)
			{
				ShowMenuSymbol.shares = row.Cells["Qty"].Value.ToString();
			}
			if (row.Cells["Price"].Value != null)
			{
				ShowMenuSymbol.price = row.Cells["Price"].Value.ToString();
			}
		}

		protected override void OnMouseClick(MouseEventArgs e)
		{
			_mousePoint.X = e.X;
			_mousePoint.Y = e.Y;

			base.OnMouseClick(e);
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

				_orderStatuses.Clear();
				_orderStatuses = null;

				RocGridTable.Clear();
				RocGridTable = null;

				Columns.Clear();

				_selectedRows = null;
				_toolTip = null;
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
						filter = BuildRowFilterString(filter, string.Concat("SymbolDetail = '", symbol, "'"), true);
					}

					filter = string.Concat("(", filter, ")");
				}
				else
				{
					filter = BuildRowFilterString(filter, string.Concat("SymbolDetail = '", symbols[0], "'"));
				}
			}

			if (FilterOutAccounts != "")
			{
				string[] accts = FilterOutAccounts.Split(new string[] { " " }, StringSplitOptions.RemoveEmptyEntries);

				foreach (string acct in accts)
				{
					filter = BuildRowFilterString(filter, string.Concat("ClearingAcct Not Like '", acct, "%'"));
				}
			}

			if (FilterOutStock)
			{
				filter = BuildRowFilterString(filter, string.Concat("SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.Equity,  "'"));
			}

			if (FilterOutFuture)
			{
				filter = BuildRowFilterString(filter, string.Concat("SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.Future, "'"));
				filter = BuildRowFilterString(filter, string.Concat("SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.SingleStockFuture, "'"));
			}

			if (FilterOutOption)
			{
				filter = BuildRowFilterString(filter, string.Concat("SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.Option, "'"));
				filter = BuildRowFilterString(filter, string.Concat("SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.OptionFuture, "'"));
				filter = BuildRowFilterString(filter, string.Concat("SecType <> '", CSVEx.CSVFieldIDs.SecurityTypes.OptionIndex, "'"));
			}

			if (FilterOutMarket)
			{
				filter = BuildRowFilterString(filter, string.Concat("OrderType <> '", CSVEx.CSVFieldIDs.OrderTypes.Market, "'"));
			}

			if (FilterOutLimit)
			{
				filter = BuildRowFilterString(filter, string.Concat("OrderType <> '", CSVEx.CSVFieldIDs.OrderTypes.Limit, "'"));
			}

			if (FilterOutDay)
			{
				filter = BuildRowFilterString(filter, string.Concat("TIF <> '", CSVEx.CSVFieldIDs.TIFCodes.Day, "'"));
			}

			if (FilterOutCanceled)
			{
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.Canceled, "'"));
			}

			if (FilterOutFilled)
			{
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.Filled, "'"));
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.FilledAndCancelled, "'"));
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.ReplacedAndFilled, "'"));
			}

			if (FilterOutOpen)
			{
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.Open, "'"));
			}

			if (FilterOutPartialFilled)
			{
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.PartiallyFilled, "'"));
			}

			if (FilterOutRejected)
			{
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.Rejected, "'"));
			}

			if (FilterOutReplaced)
			{
				filter = BuildRowFilterString(filter, string.Concat("Status <> '", CSVEx.CSVFieldIDs.StatusCodes.Replaced, "'"));
			}

			try
			{
				RocGridTable.DefaultView.RowFilter = filter;
				LastFilter = RocGridTable.DefaultView.RowFilter;

				RefreshSharedRows = true;
				RefreshAggragation = true;
				ShouldScrollToLastRow = true;
			}
			catch (Exception ex)
			{
				Debug.Assert(false, ex.Message, ex.StackTrace);
			}
		}

		#endregion
	}
}
