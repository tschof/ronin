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
	public class ROCTradeList : DGVBase, INotifyPropertyChanged
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
			Side,
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

		#endregion

		#region - Properties -

		private RowColorTypes _rowColorType = RowColorTypes.None;
		[DefaultValue(typeof(DataGridViewEx.ROCTradeList.RowColorTypes), "None"), Category("Appearance")]
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
				if (_showThisOMTag != value)
				{
					_showThisOMTag = value;
					onPropertyChanged(new PropertyChangedEventArgs("ShowThisOMTag"));
				}
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

		private DataRow[] _selectedRows = new DataRow[0];

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

					_rocGridTable.Columns.Add(new DataColumn("ExecID", Type.GetType("System.String")));
					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_rocGridTable.Columns["ExecID"]);
					_rocGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SymbolDetail", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SymbolDisplay", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Side", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("Qty", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("Price", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("TradeValue", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("DestID", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("OmTime", Type.GetType("System.DateTime")));
					_rocGridTable.Columns.Add(new DataColumn("ClearingAcct", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
					_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

					_rocGridTable.Columns.Add(new DataColumn("OMTag", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("TPOS", Type.GetType("System.Int64")));

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

		private string FilterBySide(long side)
		{
			if (RocGridTable.DefaultView.RowFilter != "")
			{
				return string.Concat(new object[] { RocGridTable.DefaultView.RowFilter, " and Side = ", side.ToString() });
			}
			else
			{
				return string.Concat(new object[] { "Side = ", side.ToString() });
			}
		}

		private double _buyValue = 0;
		[Browsable(false)]
		public double BuyValue
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(TradeValue)", FilterBySide(CSVEx.CSVFieldIDs.SideCodes.Buy)).ToString(), out _buyValue))
					{
						return Math.Round(_buyValue, 7);
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _buyQty = 0;
		[Browsable(false)]
		public long BuyQty
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(Qty)", FilterBySide(CSVEx.CSVFieldIDs.SideCodes.Buy)).ToString(), out _buyQty))
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

		private double _sellValue = 0;
		[Browsable(false)]
		public double SellValue
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(TradeValue)", FilterBySide(CSVEx.CSVFieldIDs.SideCodes.Sell)).ToString(), out _sellValue))
					{
						return Math.Round(_sellValue, 7);
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _sellQty = 0;
		[Browsable(false)]
		public long SellQty
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(Qty)", FilterBySide(CSVEx.CSVFieldIDs.SideCodes.Sell)).ToString(), out _sellQty))
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

		private double _shortValue = 0;
		[Browsable(false)]
		public double ShortValue
		{
			get
			{
				try
				{
					if (Double.TryParse(RocGridTable.Compute("Sum(TradeValue)", FilterBySide(CSVEx.CSVFieldIDs.SideCodes.Short)).ToString(), out _shortValue))
					{
						return Math.Round(_shortValue, 7);
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _shortQty = 0;
		[Browsable(false)]
		public long ShortQty
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(Qty)", FilterBySide(CSVEx.CSVFieldIDs.SideCodes.Short)).ToString(), out _shortQty))
					{
						return _shortQty;
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

		private void SetGrid(ROCTradeList grid)
		{
			DGVTradeTextColumn txtCol;
			DGVTradeNumberColumn numCol;
			DGVTradeSideColumn sidCol;
			DGVTradeDestinationColumn desCol;
			DGVTradeTimeColumn timCol;

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

			// Trade grid speical
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVTradeTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Exec ID";
			txtCol.Name = "ExecID";
			txtCol.DataPropertyName = "ExecID";  // OMExecTag, ExecID
			grid.Columns.Add(txtCol);

			txtCol = new DGVTradeTextColumn();
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

			txtCol = new DGVTradeTextColumn();
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

			txtCol = new DGVTradeTextColumn();
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

			sidCol = new DGVTradeSideColumn();
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
			
			numCol = new DGVTradeNumberColumn();
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

			numCol = new DGVTradeNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Price";
			numCol.Name = "Price";
			numCol.DataPropertyName = "Price";
			grid.Columns.Add(numCol);

			numCol = new DGVTradeNumberColumn();
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
			numCol.HeaderText = "Value";
			numCol.Name = "Value";
			numCol.DataPropertyName = "TradeValue";
			grid.Columns.Add(numCol);

			desCol = new DGVTradeDestinationColumn();
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

			timCol = new DGVTradeTimeColumn();
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

			txtCol = new DGVTradeTextColumn();
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

			numCol = new DGVTradeNumberColumn();
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

			numCol = new DGVTradeNumberColumn();
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

			numCol = new DGVTradeNumberColumn();
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

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Events -

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			switch (RowColorType)
			{
				case RowColorTypes.Side:
					if (Rows[e.RowIndex].Cells["Side"].Visible)
					{
						_currentCellColor = ((DGVBaseSideCell)Rows[e.RowIndex].Cells["Side"]).CurrentForeColor;
					}
					else
					{
						_currentCellColor = GetSideColor((Int64)Rows[e.RowIndex].Cells["Side"].Value);
					} break;
				case RowColorTypes.None:
				default:
					_currentCellColor = Color.Empty;
					//_currentCellColor = DefaultCellForeColor;
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
					return ((DGVBaseSideColumn)Columns["Side"]).DefaultCellStyle.ForeColor;
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
							FilterOutSymbols = Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString().Trim();
						}
						break;
					case "ExecID":
						_currentCellValue = Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString().Trim();
						if (ShowThisTag == _currentCellValue)
						{
							ShowThisTag = "";
						}
						else
						{
							ShowThisTag = Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString().Trim();

							_selectedRows = RocGridTable.Select("ExecID = '" + Rows[e.RowIndex].Cells[e.ColumnIndex].Value.ToString() + "'");

							if (_selectedRows.Length > 0 && _selectedRows[0]["OMTag"] != null)
							{
								ShowThisOMTag = _selectedRows[0]["OMTag"].ToString();
							}
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
						ShowActionMenu = true;
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
			if (!Importing)
			{
				string filter = "";

				if (FilterOutSymbols != "")
				{
					string[] symbols = FilterOutSymbols.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries);

					if (symbols.Length > 1)
					{
						foreach (string symbol in symbols)
						{
							filter = BuildRowFilterString(filter, String.Concat(new object[] { "SymbolDetail = '", symbol, "'" }), true);
						}

						filter = String.Concat(new object[] { "(", filter, ")" });
					}
					else
					{
						filter = BuildRowFilterString(filter, String.Concat(new object[] { "SymbolDetail = '", symbols[0], "'" }));
					}
				}

				if (FilterOutAccounts != "")
				{
					string[] accts = FilterOutAccounts.Split(new string[] { " " },  StringSplitOptions.RemoveEmptyEntries);

					foreach (string acct in accts)
					{
						filter = BuildRowFilterString(filter, String.Concat(new object[] { "ClearingAcct Not Like '", acct, "%'" }));
					}
				}

				if (FilterOutStock)
				{
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecutrityTypes.Equity, "'" }));
				}

				if (FilterOutFuture)
				{
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecutrityTypes.Future, "'" }));
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecutrityTypes.SingleStockFuture, "'" }));
				}

				if (FilterOutOption)
				{
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecutrityTypes.Option, "'" }));
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecutrityTypes.OptionFuture, "'" }));
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "SecType <> '", CSVEx.CSVFieldIDs.SecutrityTypes.OptionIndex, "'" }));
				}

				if (FilterOutTPOS)
				{
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "TPOS <> 1" }));
				}

				if (FilterOutROC)
				{
					filter = BuildRowFilterString(filter, String.Concat(new object[] { "TPOS <> 0" }));
				}

				try
				{
					RocGridTable.DefaultView.RowFilter = filter;

					RefreshSharedRows = true;
					RefreshAggragation = true;
					ShouldScrollToLastRow = true;
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
			}
		}

		#endregion
	}
}
