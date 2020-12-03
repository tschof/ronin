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
	public class ROCWatchList : DGVBase, INotifyPropertyChanged
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
			Performence
		}

		#region - Properties With Events -

		private string _subscribeSymbolDetail = "";
		[Browsable(false)]
		public string SubscribeSymbolDetail
		{
			get
			{
				return _subscribeSymbolDetail;
			}
			set
			{
				_subscribeSymbolDetail = value;
				onPropertyChanged(new PropertyChangedEventArgs("SubscribeSymbolDetail"));
			}
		}

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

		private bool _rowDragged = false;
		[Browsable(false)]
		public bool RowDragged
		{
			get
			{
				return _rowDragged;
			}
			set
			{
				_rowDragged = value;
				if (_rowDragged)
				{
					onPropertyChanged(new PropertyChangedEventArgs("RowDragged"));
				}
			}
		}

		#endregion

		#region - Properties -

		private RowColorTypes _rowColorType = RowColorTypes.None;
		[DefaultValue(typeof(DataGridViewEx.ROCWatchList.RowColorTypes), "None"), Category("Appearance")]
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

		private int _rowLocation = 0;
		[Browsable(false)]
		public int RowLocation
		{
			get
			{
				return _rowLocation;
			}
		}

		private int _rowOriginalLocation = 0;
		[Browsable(false)]
		public int RowOriginalLocation
		{
			get
			{
				return _rowOriginalLocation;
			}
		}

		private int _rowNewLocation = 0;
		[Browsable(false)]
		public int RowNewLocation
		{
			get
			{
				return _rowNewLocation;
			}
		}

		private string _rowOrigionalSymbol = "";
		[Browsable(false)]
		public string RowOrigionalSymbol
		{
			get
			{
				return _rowOrigionalSymbol;
			}
		}

		#endregion

		#region - Local Variable -

		private Color _currentCellColor = Color.Empty;

		private bool _isRowSelected = false;

		#endregion

		#region - Data Table -

		private DataTable _rocGridTable;
		[Browsable(false)]
		public DataTable RocGridTable
		{
			get
			{
				SetGridTable();
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

				_rocGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("SymbolDetail", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("BidPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("BidSize", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("BidPartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("BidHigh", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("BidLow", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("AskPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("AskSize", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("AskPartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("AskHigh", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("AskLow", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("LastTraded", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TradeVolume", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("TradePartID", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Tick", Type.GetType("System.String")));
				
				_rocGridTable.Columns.Add(new DataColumn("NetChange", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("PctChange", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Volume", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("High", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Low", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Open", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Close", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Vwap", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Name", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("OpenInterest", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("SettlePrice", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("SecurityStatus", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("LineTime", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("DecodeTime", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("OnMsgTime", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("ServerSentTime", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("ClientRecivedTime", Type.GetType("System.String")));

				_rocGridTable.DefaultView.Sort = "";
				LastSort = _rocGridTable.DefaultView.Sort;

				_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
				_rocGridTable.ColumnChanged += new DataColumnChangeEventHandler(RocGridTable_ColumnChanged);
			}
		}

		private void RocGridTable_ColumnChanged(object sender, DataColumnChangeEventArgs e)
		{
			switch (e.Column.ColumnName)
			{
				case "SymbolDetail":
					try
					{
						//Debug.Print(e.Row.RowState.ToString());
						if (e.Row.RowState != DataRowState.Detached)
						{
							e.Row.AcceptChanges();
							if (e.Row["SymbolDetail"] != null && e.Row["SymbolDetail"].ToString() != "")
							{
								SubscribeSymbolDetail = e.Row["SymbolDetail"].ToString().ToUpper().Trim();
							}
						}
					}
					catch (Exception ex)
					{
						Debug.Assert(false, ex.Message, ex.StackTrace);
					}
					break;
			}
		}

		private void RocGridTable_RowChanged(object sender, DataRowChangeEventArgs e)
		{
			switch (e.Action)
			{
				case DataRowAction.Add:
					if (e.Row["SymbolDetail"] != null && e.Row["SymbolDetail"].ToString() != "")
					{
						SubscribeSymbolDetail = e.Row["SymbolDetail"].ToString().ToUpper().Trim();
					}
					break;
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

		private void SetGrid(ROCWatchList grid)
		{
			DGVWatchListTextColumn txtCol;
			DGVWatchListTextOverlayColumn txtoverlayCol;
			DGVWatchListNumberColumn numCol;
			DGVWatchListTickColumn ticCol;

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

			grid.AllowUserToAddRows = true;
			grid.AllowUserToDeleteRows = true;
			grid.AllowUserToOrderColumns = true;

			grid.VirtualMode = true;

			// Watch List grid special
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Symbol";
			txtCol.Name = "Symbol";
			txtCol.DataPropertyName = "Symbol";
			grid.Columns.Add(txtCol);

			txtoverlayCol = new DGVWatchListTextOverlayColumn();
			txtoverlayCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtoverlayCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtoverlayCol.DefaultCellStyle.BackColor = Color.Black;
			txtoverlayCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtoverlayCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtoverlayCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtoverlayCol.ReadOnly = false;
			txtoverlayCol.ValueType = typeof(string);
			txtoverlayCol.HeaderText = "Detail";
			txtoverlayCol.Name = "SymbolDetail";
			txtoverlayCol.DataPropertyName = "SymbolDetail";
			grid.Columns.Add(txtoverlayCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Bid";
			numCol.Name = "BidPrice";
			numCol.DataPropertyName = "BidPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "BidID";
			txtCol.Name = "BidPartID";
			txtCol.DataPropertyName = "BidPartID";
			grid.Columns.Add(txtCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "BidHigh";
			numCol.Name = "BidHigh";
			numCol.DataPropertyName = "BidHigh";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "BidLow";
			numCol.Name = "BidLow";
			numCol.DataPropertyName = "BidLow";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = true;
			numCol.NotifyType = NotifyTypes.Border;
			numCol.IsNotified = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Ask";
			numCol.Name = "AskPrice";
			numCol.DataPropertyName = "AskPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "AskID";
			txtCol.Name = "AskPartID";
			txtCol.DataPropertyName = "AskPartID";
			grid.Columns.Add(txtCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "AskHigh";
			numCol.Name = "AskHigh";
			numCol.DataPropertyName = "AskHigh";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "AskLow";
			numCol.Name = "AskLow";
			numCol.DataPropertyName = "AskLow";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Last";
			numCol.Name = "LastTraded";
			numCol.DataPropertyName = "LastTraded";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "TradeVolume";
			numCol.Name = "TradeVolume";
			numCol.DataPropertyName = "TradeVolume";
			grid.Columns.Add(numCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "TradeID";
			txtCol.Name = "TradePartID";
			txtCol.DataPropertyName = "TradePartID";
			grid.Columns.Add(txtCol);

			ticCol = new DGVWatchListTickColumn();
			ticCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			ticCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			ticCol.DefaultCellStyle.BackColor = Color.Black;
			ticCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			ticCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			ticCol.SortMode = DataGridViewColumnSortMode.Automatic;
			ticCol.ReadOnly = true;
			ticCol.IsNotified = true;
			ticCol.Resizable = DataGridViewTriState.False;
			ticCol.MinimumWidth = 35;
			ticCol.Width = 35;
			ticCol.NotifyType = NotifyTypes.Border;
			ticCol.ValueType = typeof(string);
			ticCol.DefaultCellStyle.Format = "";
			ticCol.HeaderText = "Tick";
			ticCol.Name = "Tick";
			ticCol.DataPropertyName = "Tick";
			grid.Columns.Add(ticCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.NotifyType = NotifyTypes.Border;
			numCol.IsFraction = true;
			numCol.AutoColor = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Net";
			numCol.Name = "NetChange";
			numCol.DataPropertyName = "NetChange";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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

			numCol = new DGVWatchListNumberColumn();
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

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "High";
			numCol.Name = "High";
			numCol.DataPropertyName = "High";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "Low";
			numCol.Name = "Low";
			numCol.DataPropertyName = "Low";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "Open";
			numCol.Name = "Open";
			numCol.DataPropertyName = "Open";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "Close";
			numCol.Name = "Close";
			numCol.DataPropertyName = "Close";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "F";
			numCol.MaxDecimal = 7;
			numCol.AllowResolutionChange = true;
			numCol.HeaderText = "Vwap";
			numCol.Name = "Vwap";
			numCol.DataPropertyName = "Vwap";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "OpenInterest";
			numCol.Name = "OpenInterest";
			numCol.DataPropertyName = "OpenInterest";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
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
			numCol.HeaderText = "SettlePrice";
			numCol.Name = "SettlePrice";
			numCol.DataPropertyName = "SettlePrice";
			grid.Columns.Add(numCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Name";
			txtCol.Name = "Name";
			txtCol.DataPropertyName = "Name";
			grid.Columns.Add(txtCol);

			numCol = new DGVWatchListNumberColumn();
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

			numCol = new DGVWatchListNumberColumn();
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

			txtCol = new DGVWatchListTextColumn();
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

			txtCol = new DGVWatchListTextColumn();
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

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "LineTime";
			txtCol.Name = "LineTime";
			txtCol.DataPropertyName = "LineTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "OnMsgTime";
			txtCol.Name = "OnMsgTime";
			txtCol.DataPropertyName = "OnMsgTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "DecodeTime";
			txtCol.Name = "DecodeTime";
			txtCol.DataPropertyName = "DecodeTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "ServerSentTime";
			txtCol.Name = "ServerSentTime";
			txtCol.DataPropertyName = "ServerSentTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "ClientRecivedTime";
			txtCol.Name = "ClientRecivedTime";
			txtCol.DataPropertyName = "ClientRecivedTime";
			grid.Columns.Add(txtCol);

			SingleClickColumns.Clear();
			foreach (DataGridViewColumn col in Columns)
			{
				if (!col.ReadOnly && col.Name != "SymbolDetail")
				{
					SingleClickColumns.Add(col);
				}
				col.SortMode = DataGridViewColumnSortMode.NotSortable;
			}

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Event -

		protected override void OnMouseDown(MouseEventArgs e)
		{
			OnSingleClick(e);
		}

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			if (e.RowIndex >= 0 &&
				!((DGVBaseTextOverlayCell)this["SymbolDetail", e.RowIndex]).Flaged &&
				this["SecurityStatus", e.RowIndex].Value != null &&
				this["SecurityStatus", e.RowIndex].Value.ToString() != "")
			{
				InvalidateCell(Columns["SymbolDetail"].Index, e.RowIndex);
			}
			base.OnRowPrePaint(e);
		}

		protected override void OnRowPostPaint(DataGridViewRowPostPaintEventArgs e)
		{
			switch (RowColorType)
			{
				case RowColorTypes.Performence:
					_currentCellColor = ((DGVBaseNotifiedNumberCell)Rows[e.RowIndex].Cells["NetChange"]).CurrentForeColor;
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
				switch (Columns[i].Name)
				{
					case "BidPrice":
					case "AskPrice":
					case "LastTraded":
					case "NetChange":
					case "PctChange":
						if (((DGVWatchListNumberColumn)Columns[i]).NotifyType == NotifyTypes.None)
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

					RefreshAggragation = true;

					break;
			}

			base.OnColumnHeaderMouseClick(e);
		}

		//protected override void OnCellMouseClick(DataGridViewCellMouseEventArgs e)
		//{
		//    base.OnCellMouseClick(e);
		//}

		protected override void OnCellMouseDown(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Left:
						if (Rows[e.RowIndex].Cells["SymbolDetail"].Value != null && Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString() != "")
						{
							// Drag Starting Point
							_rowOriginalLocation = e.RowIndex;
							_rowOrigionalSymbol = Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString();
						}
						break;
					case MouseButtons.Right:
						_rowLocation = e.RowIndex;
						if (Rows[e.RowIndex].Cells["SymbolDetail"].Value != null && Rows[e.RowIndex].Cells["SymbolDetail"].Value.ToString() != "")
						{
							// Show menu
							ShowMenu = true;
						}
						else
						{
							ShowActionMenu = true;
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
						if (_rowOrigionalSymbol != "" && e.RowIndex != _rowOriginalLocation)
						{
							_rowNewLocation = e.RowIndex;
							RowDragged = true;
						}
						else
						{
							ResetDrag();
						}
						break;
				}
			}

			base.OnCellMouseUp(e);
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

		#region - Grid Functions -

		public void AddDataRow()
		{
			lock (RocGridTable)
			{
				RocGridTable.Rows.InsertAt(RocGridTable.NewRow(), RowLocation);
			}
		}

		public void DeleteDataRow()
		{
			lock (RocGridTable)
			{
				SuspendLayout();
				RocGridTable.BeginLoadData();

				if (Rows.Count > RowLocation)
				{
					if (!Rows[RowLocation].IsNewRow)
					{
						Rows.Remove(Rows[RowLocation]);
					}
				}

				Refresh();

				RocGridTable.EndLoadData();
				ResumeLayout();
			}
		}

		public void DeleteSelectedDataRow()
		{
			lock (RocGridTable)
			{
				SuspendLayout();
				RocGridTable.BeginLoadData();

				foreach (DataGridViewRow row in SelectedRows)
				{
					if (!row.IsNewRow)
					{
						Rows.Remove(row);
					}
				}

				Refresh();

				RocGridTable.EndLoadData();
				ResumeLayout();
			}
		}

		public void DraggedDataRow()
		{
			lock (RocGridTable)
			{
				SuspendLayout();
				RocGridTable.BeginLoadData();

				if (Rows.Count > RowOriginalLocation)
				{
					if (!Rows[RowOriginalLocation].IsNewRow)
					{
						Rows.Remove(Rows[RowOriginalLocation]);
					}
				}

				DataRow row = RocGridTable.NewRow();
				row["SymbolDetail"] = RowOrigionalSymbol;
				RocGridTable.Rows.InsertAt(row, RowNewLocation);

				ResetDrag();

				Refresh();

				RocGridTable.EndLoadData();
				ResumeLayout();
			}
		}

		private void ResetDrag()
		{
			_rowOriginalLocation = 0;
			_rowNewLocation = 0;
			_rowOrigionalSymbol = "";
		}

		#endregion
	}
}
