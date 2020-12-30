using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;
using MarketData;
using Price = Common.Price;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCQuickList : DGVBase, INotifyPropertyChanged
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

		private bool _showQtyMenu = false;
		[Browsable(false)]
		public bool ShowQtyMenu
		{
			get
			{
				return _showQtyMenu;
			}
			set
			{
				_showQtyMenu = value;
				if (_showQtyMenu)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowQtyMenu"));
				}
			}
		}

		private bool _priceSelected = false;
		[Browsable(false)]
		public bool PriceSelected
		{
			get
			{
				return _priceSelected;
			}
			set
			{
				_priceSelected = value;
				if (_priceSelected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("PriceSelected"));
				}
			}
		}

		private bool _buySelected = false;
		[Browsable(false)]
		public bool BuySelected
		{
			get
			{
				return _buySelected;
			}
			set
			{
				_buySelected = value;
				if (_buySelected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("BuySelected"));
				}
			}
		}

		private bool _sellSelected = false;
		[Browsable(false)]
		public bool SellSelected
		{
			get
			{
				return _sellSelected;
			}
			set
			{
				_sellSelected = value;
				if (_sellSelected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("SellSelected"));
				}
			}
		}

		private bool _buyStopSelected = false;
		[Browsable(false)]
		public bool BuyStopSelected
		{
			get
			{
				return _buyStopSelected;
			}
			set
			{
				_buyStopSelected = value;
				if (_buyStopSelected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("BuyStopSelected"));
				}
			}
		}

		private bool _sellStopSelected = false;
		[Browsable(false)]
		public bool SellStopSelected
		{
			get
			{
				return _sellStopSelected;
			}
			set
			{
				_sellStopSelected = value;
				if (_sellStopSelected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("SellStopSelected"));
				}
			}
		}

		private bool _buyCancel = false;
		[Browsable(false)]
		public bool BuyCancel
		{
			get
			{
				return _buyCancel;
			}
			set
			{
				_buyCancel = value;
				if (_buyCancel == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("BuyCancel"));
				}
			}
		}

		private bool _sellCancel = false;
		[Browsable(false)]
		public bool SellCancel
		{
			get
			{
				return _sellCancel;
			}
			set
			{
				_sellCancel = value;
				if (_sellCancel == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("SellCancel"));
				}
			}
		}

		private bool _buyReplace = false;
		[Browsable(false)]
		public bool BuyReplace
		{
			get
			{
				return _buyReplace;
			}
			set
			{
				_buyReplace = value;
				if (_buyReplace == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("BuyReplace"));
				}
			}
		}

		private bool _sellReplace = false;
		[Browsable(false)]
		public bool SellReplace
		{
			get
			{
				return _sellReplace;
			}
			set
			{
				_sellReplace = value;
				if (_sellReplace == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("SellReplace"));
				}
			}
		}

		private bool _showBuyStopLimit = false;
		[Browsable(false)]
		public bool ShowBuyStopLimit
		{
			get
			{
				return _showBuyStopLimit;
			}
			set
			{
				_showBuyStopLimit = value;
				if (_showBuyStopLimit)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowBuyStopLimit"));
				}
			}
		}

		private bool _clearBuyStopLimit = false;
		[Browsable(false)]
		public bool ClearBuyStopLimit
		{
			get
			{
				return _clearBuyStopLimit;
			}
			set
			{
				_clearBuyStopLimit = value;
				if (_clearBuyStopLimit)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ClearBuyStopLimit"));
				}
			}
		}

		private bool _showSellStopLimit = false;
		[Browsable(false)]
		public bool ShowSellStopLimit
		{
			get
			{
				return _showSellStopLimit;
			}
			set
			{
				_showSellStopLimit = value;
				if (_showSellStopLimit)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowSellStopLimit"));
				}
			}
		}

		private bool _clearSellStopLimit = false;
		[Browsable(false)]
		public bool ClearSellStopLimit
		{
			get
			{
				return _clearSellStopLimit;
			}
			set
			{
				_clearSellStopLimit = value;
				if (_clearSellStopLimit)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ClearSellStopLimit"));
				}
			}
		}

		#endregion

		#region - Properties -

		private Color _bidColor = Color.LimeGreen;
		[Browsable(false)]
		public Color BidColor
		{
			get
			{
				return _bidColor;
			}
			set
			{
				_bidColor = value;
			}
		}

		private Color _askColor = Color.Red;
		[Browsable(false)]
		public Color AskColor
		{
			get
			{
				return _askColor;
			}
			set
			{
				_askColor = value;
			}
		}

		private string _defaultRowFilter = ""; //"BidSize <> 0 Or AskSize <> 0";
		[Browsable(false)]
		public string DefaultRowFilter
		{
			get
			{
				return _defaultRowFilter;
			}
		}

		private DataView _priceSearchView;
		[Browsable(false)]
		public DataView PriceSearchView
		{
			get
			{
				if (_priceSearchView == null)
				{
					_priceSearchView = new DataView(RocGridTable);
					_priceSearchView.Sort = "Price DESC";
				}
				return _priceSearchView;
			}
			set
			{
				_priceSearchView = value;
			}
		}

		private Color _selectedForeColor = Color.LightBlue;
		[Browsable(false)]
		public Color SelectedForeColor
		{
			get
			{
				return _selectedForeColor;
			}
			set
			{
				_selectedForeColor = value;
			}
		}

		private Color _selectedStopLimitBackColor = Color.FromArgb(34, 34, 34);
		[Browsable(false)]
		public Color SelectedStopLimitBackColor
		{
			get
			{
				return _selectedStopLimitBackColor;
			}
			set
			{
				_selectedStopLimitBackColor = value;
			}
		}

		private double _priceBook = 0;
		[Browsable(false)]
		public double PriceBook
		{
			get
			{
				return _priceBook;
			}
			set
			{
				_priceBook = value;
			}
		}

		private double _stopPriceBook = 0;
		[Browsable(false)]
		public double StopPriceBook
		{
			get
			{
				return _stopPriceBook;
			}
			set
			{
				_stopPriceBook = value;
			}
		}

		private double _startReplacePrice = 0;
		[Browsable(false)]
		public double StartReplacePrice
		{
			get
			{
				return _startReplacePrice;
			}
			set
			{
				_startReplacePrice = value;
			}
		}

		private double _endReplacePrice = 0;
		[Browsable(false)]
		public double EndReplacePrice
		{
			get
			{
				return _endReplacePrice;
			}
			set
			{
				_endReplacePrice = value;
			}
		}

		private bool _hasFlagChange = false;
		[Browsable(false)]
		public bool HasFlagChange
		{
			get
			{
				return _hasFlagChange;
			}
			set
			{
				_hasFlagChange = value;
			}
		}

		#endregion

		#region - Data Table -

		private DataTable _rocGridTable;
		[Browsable(false)]
		public DataTable RocGridTable
		{
			get
			{
				if (_rocGridTable == null)
				{
					SetGridTable();
				}
				return _rocGridTable;
			}
			set
			{
				_rocGridTable = value;
			}
		}

		private void SetGridTable()
		{
			if (_rocGridTable == null)
			{
				_rocGridTable = new DataTable();

				_rocGridTable.Columns.Add(new DataColumn("BuyTags", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("BuyOpenQty", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("BidQty", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("BidSize", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("Price", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("AskSize", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("AskQty", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("SellOpenQty", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("SellTags", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("PriceFlag", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("PriceHighFlag", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("PriceLowFlag", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("PriceBidFlag", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("PriceAskFlag", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("BuyStopLimitFlag", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("BuyStopFlag", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("SellStopLimitFlag", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("SellStopFlag", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("Volume", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("TradedVolume", Type.GetType("System.Int64")));

				_rocGridTable.Columns["TickSize"].DefaultValue = 0.01;
				_rocGridTable.Columns["PriceFlag"].DefaultValue = 0;

				_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
			}

			_rocGridTable.DefaultView.Sort = "Price DESC";
			LastSort = _rocGridTable.DefaultView.Sort;

			_rocGridTable.DefaultView.RowFilter = DefaultRowFilter;
		}

		private void RocGridTable_RowChanged(object sender, DataRowChangeEventArgs e)
		{
			switch (e.Action)
			{
				case DataRowAction.Add:
				case DataRowAction.Change:
				case DataRowAction.Delete:
				case DataRowAction.Commit:
				case DataRowAction.ChangeCurrentAndOriginal:
				case DataRowAction.ChangeOriginal:
				case DataRowAction.Nothing:
				case DataRowAction.Rollback:
				default:
					break;
			}
		}

		#endregion

		#region - Grid -

		public void Initialize()
		{
			SetGridTable();

			SetGrid(this);
		}

		private void SetGrid(ROCQuickList grid)
		{
			DGVQuickListTextColumn txtCol;
			DGVQuickListNumberColumn numCol;

			DefaultCellForeColor = Color.Black;
			grid.DefaultCellForeColor = DefaultCellForeColor;

			grid.BackgroundColor = Color.Black;
			grid.GridColor = Color.Gainsboro;

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

			// Quick grid specials
			grid.ScrollBars = ScrollBars.Horizontal;
			grid.CellBorderStyle = DataGridViewCellBorderStyle.SingleHorizontal;
			grid.ColumnHeadersDefaultCellStyle.BackColor = HeaderBackColor;
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVQuickListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Tags";
			txtCol.Name = "BuyTags";
			txtCol.DataPropertyName = "BuyTags";
			grid.Columns.Add(txtCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = BidColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.FlagStopOpen = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Open";
			numCol.Name = "BuyOpenQty";
			numCol.DataPropertyName = "BuyOpenQty";
			numCol.Width = 45;
			numCol.FillWeight = 45F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = BidColor;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = Color.WhiteSmoke;
			numCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Qty";
			numCol.Name = "BidQty";
			numCol.DataPropertyName = "BidQty";
			numCol.Width = 42;
			numCol.MinimumWidth = 35;
			numCol.FillWeight = 42F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = BidColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ShowBar = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Size";
			numCol.Name = "BidSize";
			numCol.DataPropertyName = "BidSize";
			numCol.Width = 55;
			numCol.MinimumWidth = 50;
			numCol.FillWeight = 55F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.WhiteSmoke;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = Color.Black;
			numCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.IsTradePrice = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "Price";
			numCol.Name = "Price";
			numCol.DataPropertyName = "Price";
			numCol.Width = 75;
			numCol.MinimumWidth = 70;
			numCol.FillWeight = 75F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Silver;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = Color.DarkBlue;
			numCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "Vol";
			numCol.Name = "Volume";
			numCol.DataPropertyName = "Volume";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Silver;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = Color.Black;
			numCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "TV";
			numCol.Name = "TradedVolume";
			numCol.DataPropertyName = "TradedVolume";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = AskColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ShowBar = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Size";
			numCol.Name = "AskSize";
			numCol.DataPropertyName = "AskSize";
			numCol.Width = 55;
			numCol.MinimumWidth = 50;
			numCol.FillWeight = 55F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = AskColor;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = Color.WhiteSmoke;
			numCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Qty";
			numCol.Name = "AskQty";
			numCol.DataPropertyName = "AskQty";
			numCol.Width = 42;
			numCol.MinimumWidth = 35;
			numCol.FillWeight = 42F / 361F;
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = AskColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.FlagStopOpen = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Open";
			numCol.Name = "SellOpenQty";
			numCol.DataPropertyName = "SellOpenQty";
			numCol.Width = 45;
			numCol.FillWeight = 45F / 361F;
			grid.Columns.Add(numCol);

			txtCol = new DGVQuickListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Tags";
			txtCol.Name = "SellTags";
			txtCol.DataPropertyName = "SellTags";
			grid.Columns.Add(txtCol);

			numCol = new DGVQuickListNumberColumn();
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

			numCol = new DGVQuickListNumberColumn();
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

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "DisplayFactor";
			numCol.Name = "DisplayFactor";
			numCol.DataPropertyName = "DisplayFactor";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PriceFlag";
			numCol.Name = "PriceFlag";
			numCol.DataPropertyName = "PriceFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PriceHighFlag";
			numCol.Name = "PriceHighFlag";
			numCol.DataPropertyName = "PriceHighFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PriceLowFlag";
			numCol.Name = "PriceLowFlag";
			numCol.DataPropertyName = "PriceLowFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PriceBidFlag";
			numCol.Name = "PriceBidFlag";
			numCol.DataPropertyName = "PriceBidFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PriceAskFlag";
			numCol.Name = "PriceAskFlag";
			numCol.DataPropertyName = "PriceAskFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BuyStopLimitFlag";
			numCol.Name = "BuyStopLimitFlag";
			numCol.DataPropertyName = "BuyStopLimitFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "SellStopLimitFlag";
			numCol.Name = "SellStopLimitFlag";
			numCol.DataPropertyName = "SellStopLimitFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BuyStopFlag";
			numCol.Name = "BuyStopFlag";
			numCol.DataPropertyName = "BuyStopFlag";
			grid.Columns.Add(numCol);

			numCol = new DGVQuickListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "SellStopFlag";
			numCol.Name = "SellStopFlag";
			numCol.DataPropertyName = "SellStopFlag";
			grid.Columns.Add(numCol);

			foreach (DataGridViewColumn col in Columns)
			{
				col.SortMode = DataGridViewColumnSortMode.NotSortable;
			}

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Event -

		protected override void OnMouseWheel(MouseEventArgs e)
		{
			ScrollByWheel(e.Delta / 120);
			base.OnMouseWheel(e);
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
			if (ReadyForTickTable)
			{
				if (e.Button == MouseButtons.Middle)
				{
					CenterOnPrice(TradePrice);
				}
			}
			base.OnMouseDown(e);
		}

		protected override void OnCellMouseDown(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Left:
						switch (Columns[e.ColumnIndex].Name)
						{
							case "Price":
								Rows[e.RowIndex].Cells["Price"].Style.ForeColor = SelectedForeColor;

								PriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;

								PriceSelected = true;
								break;
							case "BidQty":
								Rows[e.RowIndex].Cells["BidQty"].Style.ForeColor = SelectedForeColor;

								PriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;

								BuySelected = true;
								break;
							case "AskQty":
								Rows[e.RowIndex].Cells["AskQty"].Style.ForeColor = SelectedForeColor;

								PriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;

								SellSelected = true;
								break;
							case "BuyOpenQty":
								Rows[e.RowIndex].Cells["BuyOpenQty"].Style.ForeColor = SelectedForeColor;
								StartReplacePrice = (double)Rows[e.RowIndex].Cells["Price"].Value;
								break;
							case "SellOpenQty":
								Rows[e.RowIndex].Cells["SellOpenQty"].Style.ForeColor = SelectedForeColor;
								StartReplacePrice = (double)Rows[e.RowIndex].Cells["Price"].Value;
								break;
						}
						break;
					case MouseButtons.Right:
						switch (Columns[e.ColumnIndex].Name)
						{
							case "BidQty":
								ShowQtyMenu = true;
								break;
							case "AskQty":
								ShowQtyMenu = true;
								break;
							case "BuyOpenQty":
								Rows[e.RowIndex].Cells["BuyOpenQty"].Style.BackColor = SelectedForeColor;

								PriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;

								BuyStopSelected = true;
								break;
							case "SellOpenQty":
								Rows[e.RowIndex].Cells["SellOpenQty"].Style.ForeColor = SelectedForeColor;

								PriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;

								SellStopSelected = true;
								break;
						}
						break;
				}
			}

			base.OnCellMouseDown(e);
		}

		protected override void OnCellMouseUp(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Left:
						resetCellForColor(e.RowIndex, e.ColumnIndex);

						switch (Columns[e.ColumnIndex].Name)
						{
							case "BuyOpenQty":
								EndReplacePrice = (double)Rows[e.RowIndex].Cells["Price"].Value;

								if (StartReplacePrice == EndReplacePrice)
								{
									PriceBook = EndReplacePrice;
									BuyCancel = true;
								}
								else
								{
									BuyReplace = true;
								}
								break;
							case "SellOpenQty":
								EndReplacePrice = (double)Rows[e.RowIndex].Cells["Price"].Value;

								if (StartReplacePrice == EndReplacePrice)
								{
									PriceBook = EndReplacePrice;
									SellCancel = true;
								}
								else
								{
									SellReplace = true;
								}
								break;
						}
						break;
					case MouseButtons.Right:
						break;
				}
			}
			base.OnCellMouseUp(e);
		}

		protected override void OnCellMouseMove(DataGridViewCellMouseEventArgs e)
		{
			resetCellForColor(e.RowIndex, e.ColumnIndex);
			base.OnCellMouseMove(e);
		}

		protected override void OnCellMouseEnter(DataGridViewCellEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (Rows[e.RowIndex].Cells["Price"].Value != null && Rows[e.RowIndex].Cells["Price"].Value.ToString() != "")
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "BuyOpenQty":
							StopPriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;
							ShowBuyStopLimit = true;
							break;
						case "SellOpenQty":
							StopPriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;
							ShowSellStopLimit = true;
							break;
					}
				}
			}
			base.OnCellMouseEnter(e);
		}

		protected override void OnCellMouseLeave(DataGridViewCellEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (Columns[e.ColumnIndex].Name)
				{
					case "BuyOpenQty":
						ClearBuyStopLimit = true;
						break;
					case "SellOpenQty":
						ClearSellStopLimit = true;
						break;
				}

			}
			base.OnCellMouseLeave(e);
		}

		private void resetCellForColor(int rowIndex, int columnIndex)
		{
			if (rowIndex >= 0)
			{
				switch (Columns[columnIndex].Name)
				{
					case "Price":
						Rows[rowIndex].Cells["Price"].Style.ForeColor = Color.Black;
						break;
					case "BidQty":
						Rows[rowIndex].Cells["BidQty"].Style.ForeColor = Color.White;
						break;
					case "AskQty":
						Rows[rowIndex].Cells["AskQty"].Style.ForeColor = Color.White;
						break;
					case "BuyOpenQty":
						Rows[rowIndex].Cells["BuyOpenQty"].Style.ForeColor = BidColor;
						break;
					case "SellOpenQty":
						Rows[rowIndex].Cells["SellOpenQty"].Style.ForeColor = AskColor;
						break;
				}
			}
		}

		protected override void OnSelectionChanged(EventArgs e)
		{
			// Make sure nothing is selected
			foreach (DataGridViewCell cell in SelectedCells)
			{
				if (Columns[cell.ColumnIndex].Name != "Price")
				{
					cell.Selected = false;
				}
			}

			base.OnSelectionChanged(e);
		}

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (hasFlag(Rows[e.RowIndex], "PriceFlag"))
				{
					switch (getFlagValue(Rows[e.RowIndex], "PriceFlag"))
					{
						case PriceFlages.tradeUp:
							Rows[e.RowIndex].Cells["Price"].Style.BackColor = BidColor;
							Rows[e.RowIndex].Cells["TradedVolume"].Style.BackColor = BidColor;
							break;
						case PriceFlages.tradeDown:
							Rows[e.RowIndex].Cells["Price"].Style.BackColor = AskColor;
							Rows[e.RowIndex].Cells["TradedVolume"].Style.BackColor = AskColor;
							break;
						default:
							Rows[e.RowIndex].Cells["Price"].Style.BackColor = Columns["Price"].DefaultCellStyle.BackColor;
							Rows[e.RowIndex].Cells["TradedVolume"].Style.BackColor = Columns["TradedVolume"].DefaultCellStyle.BackColor;
							break;
					}
				}
				else
				{
					Rows[e.RowIndex].Cells["Price"].Style.BackColor = Columns["Price"].DefaultCellStyle.BackColor;
					Rows[e.RowIndex].Cells["TradedVolume"].Style.BackColor = Columns["TradedVolume"].DefaultCellStyle.BackColor;
				}


				if (hasFlag(Rows[e.RowIndex], "BuyStopLimitFlag"))
				{
					switch (getFlagValue(Rows[e.RowIndex], "BuyStopLimitFlag"))
					{
						case PriceFlages.stopLimit:
							Rows[e.RowIndex].Cells["BuyOpenQty"].Style.BackColor = SelectedStopLimitBackColor;
							break;
						default:
							if (Rows[e.RowIndex].Cells["BuyOpenQty"].Style.BackColor != Columns["BuyOpenQty"].DefaultCellStyle.BackColor)
							{
								Rows[e.RowIndex].Cells["BuyOpenQty"].Style.BackColor = Columns["BuyOpenQty"].DefaultCellStyle.BackColor;
							}
							break;
					}
				}
				else
				{
					if (Rows[e.RowIndex].Cells["BuyOpenQty"].Style.BackColor != Columns["BuyOpenQty"].DefaultCellStyle.BackColor)
					{
						Rows[e.RowIndex].Cells["BuyOpenQty"].Style.BackColor = Columns["BuyOpenQty"].DefaultCellStyle.BackColor;
					}
				}

				if (hasFlag(Rows[e.RowIndex], "SellStopLimitFlag"))
				{
					switch (getFlagValue(Rows[e.RowIndex], "SellStopLimitFlag"))
					{
						case PriceFlages.stopLimit:
							Rows[e.RowIndex].Cells["SellOpenQty"].Style.BackColor = SelectedStopLimitBackColor;
							break;
						default:
							if (Rows[e.RowIndex].Cells["SellOpenQty"].Style.BackColor != Columns["SellOpenQty"].DefaultCellStyle.BackColor)
							{
								Rows[e.RowIndex].Cells["SellOpenQty"].Style.BackColor = Columns["SellOpenQty"].DefaultCellStyle.BackColor;
							} 
							break;
					}
				}
				else
				{
					if (Rows[e.RowIndex].Cells["SellOpenQty"].Style.BackColor != Columns["SellOpenQty"].DefaultCellStyle.BackColor)
					{
						Rows[e.RowIndex].Cells["SellOpenQty"].Style.BackColor = Columns["SellOpenQty"].DefaultCellStyle.BackColor;
					}
				}
			}

			base.OnRowPrePaint(e);
		}

		private long getFlagValue(DataGridViewRow row, string colName)
		{
			return (long)row.Cells[colName].Value;
		}

		private bool hasFlag(DataGridViewRow row, string colName)
		{
			if (row.Cells[colName].Value != null && row.Cells[colName].Value.ToString() != "")
			{
				return true;
			}
			return false;
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

				PriceSearchView = null;

				RocGridTable.Clear();
				RocGridTable = null;

				Columns.Clear();

				bidBookPrices = null;
				askBookPrices = null;
			}
			base.Dispose(disposing);
		}

		#endregion

		#region - Quick Grid Functions -

		[Browsable(false)]
		public bool AutoCenterOnTraded = false;

		[Browsable(false)]
		public long AutoCenterRange = 3;

		[Browsable(false)]
		public double TradePrice = 0;

		[Browsable(false)]
		public string TradeTick = "";

		[Browsable(false)]
		public double HighPrice = 0;

		[Browsable(false)]
		public double LowPrice = 0;

		[Browsable(false)]
		public double AskPrice = 0;

		[Browsable(false)]
		public double BidPrice = 0;

		[Browsable(false)]
		public double TickSize = 0;

		private long _qty = 0;
		[Browsable(false)]
		public long Qty
		{
			get
			{
				return _qty;
			}
			set
			{
				if (_qty != value)
				{
					_qty = value;
					SetQty();
				}
			}
		}

		private void SetQty()
		{
			lock (RocGridTable)
			{
				foreach (DataRow row in RocGridTable.Rows)
				{
					row["BidQty"] = Qty;
					row["AskQty"] = Qty;
				}
			}
		}

		private bool _readyForTickTable = false;
		[Browsable(false)]
		public bool ReadyForTickTable
		{
			get
			{
				return _readyForTickTable;
			}
			set
			{
				_readyForTickTable = value;
			}
		}

		private int _bookDepthLimit = 10;

		private int rowsToShow
		{
			get
			{
				return Height / RowTemplate.Height;
			}
		}

		private List<Price> _bidBookPrices;
		public List<Price> bidBookPrices
		{
			get
			{
				if (_bidBookPrices == null)
				{
					_bidBookPrices = new List<Price>();
				}
				return _bidBookPrices;
			}
			set
			{
				_bidBookPrices = value;
			}
		}

		private List<Price> _askBookPrices;
		public List<Price> askBookPrices
		{
			get
			{
				if (_askBookPrices == null)
				{
					_askBookPrices = new List<Price>();
				}
				return _askBookPrices;
			}
			set
			{
				_askBookPrices = value;
			}
		}

		public void SetupTickTable(Price price, int bookDepthLimit)
		{
			price = price.Round(7);

			bidBookPrices = new List<Price>();
			askBookPrices = new List<Price>();

			_bookDepthLimit = bookDepthLimit;

			if (Rows.Count == 0)
			{
				lock (RocGridTable)
				{
					//SuspendLayout();
					//RocGridTable.BeginLoadData();

					FirstPad(price);

					//RocGridTable.EndLoadData();
					//ResumeLayout();

					CenterOnRow(500);
				}
			}
		}

		private void FirstPad(Price price)
		{
			Price newPrice = price.Round(7) + TickSize * 500;

			//SetFirstRowFilter(price);
			
			for (int i = 0; i < 1000; i++)
			{
				DataRow row = RocGridTable.NewRow();
				row["TickSize"] = TickSize;
				row["DisplayFactor"] = 1;

				row["BidQty"] = Qty;
				row["Price"] = newPrice;
				row["AskQty"] = Qty;

				RocGridTable.Rows.Add(row);
				newPrice -= TickSize;
			}

			// TODO Fix the visiblity problem when the first column is not visiable
			if (Columns["BuyTags"].Visible)
			{
				Columns["BuyTags"].Visible = false;
			}
		}

		public void PadByLimit()
		{
			if (Rows.Count > 0)
			{
				if (FirstDisplayedScrollingRowIndex < 50)
				{
					Pad((double)Rows[0].Cells["Price"].Value, true);
				}
				else if (FirstDisplayedScrollingRowIndex + (Height / Rows[0].Height) > Rows.Count - 50)
				{
					Pad((double)Rows[Rows.Count - 1].Cells["Price"].Value, false);
				}
			}
		}

		public void Pad(Price price, bool high)
		{
			price = price.Round(7);

			if (RocGridTable.Rows.Count > 0)
			{
				lock (RocGridTable)
				{
					double limitPrice = 0;
					if (high)
					{
						if (RocGridTable.Rows[0]["Price"] != null && RocGridTable.Rows[0]["Price"].ToString() != "")
						{
							limitPrice = (double)RocGridTable.Rows[0]["Price"];
						}
					}
					else
					{
						if (RocGridTable.Rows[Rows.Count - 1]["Price"] != null && RocGridTable.Rows[Rows.Count - 1]["Price"].ToString() != "")
						{
							limitPrice = (double)RocGridTable.Rows[Rows.Count - 1]["Price"];
						}
					}

					Price newPrice = 0;
					if ((high && limitPrice < (price + (TickSize * 50))) ||
						(!high && limitPrice > (price - (TickSize * 50))))
					{
						if (limitPrice != 0 && (limitPrice % TickSize) == 0)
						{
							newPrice = limitPrice;
						}
						else
						{
							newPrice = price;
						}

						Price newLimitPrice = 0;
						if (high)
						{
							newLimitPrice = price + TickSize * 100;
						}
						else
						{
							newLimitPrice = price - TickSize * 100;
						}

						while (PriceSearchView.FindRows(newLimitPrice).Length == 0)
						{
							for (int i = 0; i < 100; i++)
							{
								if (PriceSearchView.FindRows(newPrice).Length == 0)
								{
									DataRow row = RocGridTable.NewRow();
									row["TickSize"] = TickSize;
									row["DisplayFactor"] = 1;

									row["BidQty"] = Qty;
									row["Price"] = newPrice;
									row["AskQty"] = Qty;

									RocGridTable.Rows.Add(row);

									//if (high)
									//{
									//    RocGridTable.Rows.InsertAt(row, 0);
									//}
									//else
									//{
									//    RocGridTable.Rows.Add(row);
									//}
								}

								if (high)
								{
									newPrice += TickSize;
								}
								else
								{
									newPrice -= TickSize;
								}
							}
						}
					}
				}
			}
		}

		public void FlagTradePrice(Price oldPrice, Price newPrice, string tick)
		{
			oldPrice = oldPrice.Round(7);
			newPrice = newPrice.Round(7);

			if (tick == null) tick = "";

			FlagReset(oldPrice, "PriceFlag");

			DataRowView[] rows = PriceSearchView.FindRows(newPrice);

			if (rows.Length > 0)
			{
				foreach (DataRowView row in rows)
				{
					if (tick.Contains("+"))
					{
						row["PriceFlag"] = PriceFlages.tradeUp;
					}
					else if (tick.Contains("-"))
					{
						row["PriceFlag"] = PriceFlages.tradeDown;
					}
					else
					{
						row["PriceFlag"] = PriceFlages.tradeDown;
					}
				}

				TradePrice = newPrice.Value;
			}
		}

		public void FlagHighPrice(double oldPrice, double newPrice)
		{
			oldPrice = Math.Round(oldPrice, 7);
			newPrice = Math.Round(newPrice, 7);

			FlagReset(oldPrice, "PriceHighFlag");

			DataRowView[] rows = PriceSearchView.FindRows(newPrice);

			if (rows.Length > 0)
			{
				foreach (DataRowView row in rows)
				{
					row["PriceHighFlag"] = PriceFlages.high;
				}

				HighPrice = newPrice;
			}
		}

		public void FlagLowPrice(double oldPrice, double newPrice)
		{
			oldPrice = Math.Round(oldPrice, 7);
			newPrice = Math.Round(newPrice, 7);

			FlagReset(oldPrice, "PriceLowFlag");

			DataRowView[] rows = PriceSearchView.FindRows(newPrice);

			if (rows.Length > 0)
			{
				foreach (DataRowView row in rows)
				{
					row["PriceLowFlag"] = PriceFlages.low;
				}

				LowPrice = newPrice;
			}
		}

		public void FlagBidPrice(Price oldPrice, Price newPrice)
		{
			oldPrice = oldPrice.Round(7);
			newPrice = newPrice.Round(7);

			FlagReset(oldPrice, "PriceBidFlag");

			DataRowView[] rows = PriceSearchView.FindRows(newPrice);

			if (rows.Length > 0)
			{
				foreach (DataRowView row in rows)
				{
					row["PriceBidFlag"] = PriceFlages.bid;
				}

				BidPrice = newPrice.Value;
			}
		}

		public void FlagAskPrice(Price oldPrice, Price newPrice)
		{
			oldPrice = oldPrice.Round(7);
			newPrice = newPrice.Round(7);

			FlagReset(oldPrice, "PriceAskFlag");

			DataRowView[] rows = PriceSearchView.FindRows(newPrice);

			if (rows.Length > 0)
			{
				foreach (DataRowView row in rows)
				{
					row["PriceAskFlag"] = PriceFlages.ask;
				}

				AskPrice = newPrice.Value;
			}
		}

		public void FlagStopLimitPrice(string colName, double stopPrice, double stopLimitPrice)
		{
			DataRow[] rows = RocGridTable.Select(string.Format("Price >= '{0}' And Price <= '{1}'", new object[] {stopPrice, stopLimitPrice}));

			foreach (DataRow row in rows)
			{
				row[colName] = PriceFlages.stopLimit;
			}

			Invalidate();
		}

		public void ClearStopLimit(string colName)
		{
			DataRow[] rows = RocGridTable.Select(string.Format("{0} = '{1}'", new object[] { colName, PriceFlages.stopLimit }));

			foreach (DataRow row in rows)
			{
				row[colName] = DBNull.Value;
			}

			Invalidate();
		}

		private void FlagReset(Price price, string colName)
		{
			//price = Math.Round(price, 7);

			DataRowView[] rows = PriceSearchView.FindRows(price.Value);

			if (rows.Length > 0)
			{
				foreach (DataRowView row in rows)
				{
					row[colName] = DBNull.Value;
				}
			}
		}

		public void CenterOnPrice(Price price)
		{
			CenterOnPrice(price, false);
		}

		public void CenterOnPrice(Price price, bool isAuto)
		{
			price = price.Round(7);
			
			if (Rows.Count > 0)
			{
				int rowIndex = FindPriceRow(price);

				Pad(price, true);

				if (rowIndex > 0)
				{
					try
					{
						int num = rowsToShow;

						if (isAuto)
						{
							int diff = (rowIndex - FirstDisplayedScrollingRowIndex);
							if (diff <= AutoCenterRange || diff >= num - (AutoCenterRange * 2))
							{
								FirstDisplayedScrollingRowIndex = rowIndex - (num / 2);
								this["Price", rowIndex].Selected = true;
							}
						}
						else
						{
							FirstDisplayedScrollingRowIndex = rowIndex - (num / 2);
							this["Price", rowIndex].Selected = true;
						}
					}
					catch (Exception ex)
					{
						if (Debugger.IsAttached)
						{
							Debug.Print(ex.Message);
							Debug.Print(ex.StackTrace);
						}
					}
				}
			}
		}

		public void CenterOnRow(int rowIndex)
		{
			if (Rows.Count > 0 && Rows.Count > rowIndex)
			{
				if (rowIndex > 0)
				{
					try
					{
						FirstDisplayedScrollingRowIndex = rowIndex - (rowsToShow / 2);
					}
					catch (Exception ex)
					{
						if (Debugger.IsAttached)
						{
							Debug.Print(ex.Message);
							Debug.Print(ex.StackTrace);
						}
					}
				}
			}
		}

		private int FindPriceRow(Price price)
		{
			price = price.Round(7);

			if (Rows.Count > 0)
			{
				if (PriceSearchView.FindRows(price).Length > 0)
				{
					Price startPrice = (double)Rows[0].Cells["Price"].Value;
					return Convert.ToInt32((startPrice - price).Value / TickSize);
				}
				
				//foreach (DataGridViewRow row in Rows)
				//{
				//    if (row.Cells["Price"].Value != null &&
				//        row.Cells["Price"].ToString() != "" &&
				//        (double)row.Cells["Price"].Value == price)
				//    {
				//        return row.Index;
				//    }
				//}
			}

			return -1;
		}

		public void CheckBook(Price limitPrice, string side)
		{
			limitPrice = limitPrice.Round(7);
			int bookcount = 0;
			List<Price> prices;
			System.Converter<Price, bool> validate;

			switch (side)
			{
				case "Bid":
					Price.Sort(bidBookPrices, Price.SortOrder.Bid);
					prices = new List<Price>(bidBookPrices);
					validate = n => n > limitPrice;
					break;
				case "Ask":
					Price.Sort(askBookPrices, Price.SortOrder.Ask);
					prices = new List<Price>(askBookPrices);
					validate = n => n < limitPrice;
					break;
				default:
					return;
			}

			foreach (Price price in prices) {
				if (validate(price) || limitPrice.IsZero) {
					ResetBookSize(price, side);
				} else {
					if (bookcount >= _bookDepthLimit) {
						ResetBookSize(price, side);
					}
					bookcount++;
				}
			}
		}

		public void UpdateBook(QuoteCollection quotes, string side)
		{
			List<Price> prices = null;
			Price.SortOrder sortOrder = Price.SortOrder.Ask;

			switch (side)
			{
				case "Bid":
					prices = bidBookPrices;
					sortOrder = Price.SortOrder.Bid;
					break;
				case "Ask":
					prices = askBookPrices;
					sortOrder = Price.SortOrder.Ask;
					break;
			}

			if ((prices != null) && (prices.Count > 0)) {
				Price.Sort(prices, sortOrder);
				foreach (Price price in prices) {
					if (!quotes.HasLevel(price))
						ResetBookSize(price, side);
				}
			}

			foreach (Quote quote in quotes)
				SetBookSize(quote.QuotePrice, quote.Size, side);
		}

		public virtual void SetBookSize(Price price, long size, string side)
		{
			DataRowView[] rows;

			lock (RocGridTable)
			{
				if (RocGridTable.Rows.Count > 0)
				{
					rows = PriceSearchView.FindRows(price.Round(7).Value);

					if (rows.Length > 0)
					{
						foreach (DataRowView row in rows)
						{
							if (size == 0)
							{
								row[side + "Size"] = DBNull.Value;
							}
							else
							{
								row[side + "Size"] = size;
							}
						}
					}
				}

				SetMaxBookSize(size, side);

				switch (side)
				{
					case "Bid":
						if (!bidBookPrices.Exists(n => price.Equals(n)))
							bidBookPrices.Add(price.Value);
						break;
					case "Ask":
						if (!askBookPrices.Exists(n => price.Equals(n)))
							askBookPrices.Add(price.Value);
						break;
				}
			}
		}

		public void ResetBookSize(Price price, string side)
		{
			price = price.Round(7);
			DataRowView[] rows = new DataRowView[0];
			long size = 0;

			lock (RocGridTable)
			{
				if (RocGridTable.Rows.Count > 0)
				{
					rows = PriceSearchView.FindRows(price);
				}

				if (rows.Length > 0)
				{
					foreach (DataRowView row in rows)
					{
						if (row[side + "Size"] != null && row[side + "Size"].ToString() != "")
						{
							if (size < (long)row[side + "Size"])
							{
								size = (long)row[side + "Size"];
							}
							row[side + "Size"] = DBNull.Value;
						}
					}
				}

				switch (side)
				{
					case "Bid":
						if (bidBookPrices.Contains(price))
							bidBookPrices.Remove(price);
						break;
					case "Ask":
						if (askBookPrices.Contains(price))
							askBookPrices.Remove(price);
						break;
				}
			}
		}

		public void SetMaxBookSize(long size, string side)
		{
			if (((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax < size)
			{
				((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = size;

				if (size < 100 && ((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax != 100)
				{
					((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = 100;
				}
				else if (size < 500 && ((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax != 500)
				{
					((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = 500;
				}
				else if (size < 1000 && ((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax != 1000)
				{
					((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = 1000;
				}
				else if (size < 2000 && ((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax != 2000)
				{
					((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = 2000;
				}
				else if (size < 3000 && ((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax != 3000)
				{
					((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = 3000;
				}
				else if (size < 4000 && ((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax != 4000)
				{
					((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = 4000;
				}
				else if (size < 5000 && ((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax != 5000)
				{
					((DGVQuickListNumberColumn)Columns[side + "Size"]).ShowBarMax = 5000;
				}
			}
		}

		public void ResetMaxBookSize()
		{
			((DGVQuickListNumberColumn)Columns["BidSize"]).ShowBarMax = 100;
			((DGVQuickListNumberColumn)Columns["AskSize"]).ShowBarMax = 100;
		}

		public void UpdateTotalTradedVolumeByPrice(double price, long volume)
		{
			UpdateTotalTradedVolumeByPrice(price, volume, false);
		}
		public void UpdateTotalTradedVolumeByPrice(double price, long volume, bool updateTradedVolume)
		{
			price = Math.Round(price, 7);

			if (updateTradedVolume)
			{
				ResetTradedVolume();
			}

			DataRowView[] rows = PriceSearchView.FindRows(price);

			foreach (DataRowView row in rows)
			{
				if (updateTradedVolume)
				{
					row["TradedVolume"] = volume;
				}

				if (row["Volume"] == null || row["Volume"].ToString() == "")
				{
					row["Volume"] = volume;
				}
				else
				{
					row["Volume"] = (long)row["Volume"] + volume;
				}
			}
		}

		private void ResetTradedVolume()
		{
			DataRow[] rows = RocGridTable.Select("TradedVolume Is Not Null");

			foreach (DataRow row in rows)
			{
				row["TradedVolume"] = DBNull.Value;
			}
		}

		#endregion
	}
}