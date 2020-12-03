using System;
using System.Windows.Forms;
using System.ComponentModel;

using BindingListEx;
using DataGridViewEx.Properties;
using System.Drawing;
using System.Collections.Generic;
using System.Collections;
using System.Runtime.Serialization;

using NumberEx;
using System.Diagnostics;

namespace DataGridViewEx
{
	public delegate void WatchListEventHandler(object sender, WatchListEventArgs e);

	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCWatchList : DataGridView
	{
		[Description("Event raised event when new symbols are added."), Category("Property Changed")]
		public event WatchListEventHandler SymbolAdded;

		protected virtual void OnSymbolAdded(WatchListEventArgs e)
		{
			if (SymbolAdded != null)
			{
				SymbolAdded(this, e);
			}
		}

		private BindingListWatch<WatchListData> _watchDatas;
		public BindingListWatch<WatchListData> WatchDatas
		{
			get
			{
				if (_watchDatas == null)
				{
					_watchDatas = new BindingListWatch<WatchListData>();
					_watchDatas.AddingNew += new AddingNewEventHandler(AddingNew);
					_watchDatas.ListChanged += new ListChangedEventHandler(ListChanged);
				}
				return _watchDatas;
			}
			set
			{
				_watchDatas = value;
			}
		}

		private WatchListData _inEditRowData;
		private int _inEditRowIndex = -1;
		private bool _rowScopeCommit = true;

		public ROCWatchList()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer | ControlStyles.OptimizedDoubleBuffer, true);
		}

		public void Initialize()
		{
			SetGrid(this);
		}
	
		private void SetGrid(ROCWatchList grid)
		{
			DGVTextColumn txtCol;
			DGVNumberColumn numCol;
			DGVTickColumn imgCol;

			grid.BackgroundColor = Color.Black;
			grid.GridColor = Color.Black;

			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.VirtualMode = true;

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Symbol";
			txtCol.Name = "Symbol";
			txtCol.DataPropertyName = "Symbol";
			grid.Columns.Add(txtCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.IsNotified = true;
			numCol.ValueType = typeof(double);
			numCol.HeaderText = "Bid";
			numCol.Name = "Bid";
			numCol.DataPropertyName = "Bid";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.IsNotified = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Ask";
			numCol.Name = "Ask";
			numCol.DataPropertyName = "Ask";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.IsNotified = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Last";
			numCol.Name = "Last";
			numCol.DataPropertyName = "Last";
			grid.Columns.Add(numCol);

			imgCol = new DGVTickColumn();
			imgCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			imgCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			imgCol.DefaultCellStyle.BackColor = Color.Black;
			imgCol.DefaultCellStyle.ForeColor = Color.White;
			imgCol.SortMode = DataGridViewColumnSortMode.NotSortable;
			imgCol.IsNotified = true;
			imgCol.ValueType = typeof(string);
			imgCol.DefaultCellStyle.Format = "";
			imgCol.HeaderText = "Tick";
			imgCol.Name = "Tick";
			imgCol.DataPropertyName = "Tick";
			grid.Columns.Add(imgCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Net";
			numCol.Name = "Net";
			numCol.DataPropertyName = "Net";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.IsPercent = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Pct";
			numCol.Name = "Pct";
			numCol.DataPropertyName = "Pct";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Volume";
			numCol.Name = "Volume";
			numCol.DataPropertyName = "Volume";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "High";
			numCol.Name = "High";
			numCol.DataPropertyName = "High";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Low";
			numCol.Name = "Low";
			numCol.DataPropertyName = "Low";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Open";
			numCol.Name = "Open";
			numCol.DataPropertyName = "Open";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Close";
			numCol.Name = "Close";
			numCol.DataPropertyName = "Close";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Vwap";
			numCol.Name = "Vwap";
			numCol.DataPropertyName = "Vwap";
			grid.Columns.Add(numCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(string);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Detail";
			txtCol.Name = "Detail";
			txtCol.DataPropertyName = "Detail";
			grid.Columns.Add(txtCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.HeaderText = "TickSize";
			numCol.Name = "TickSize";
			numCol.DataPropertyName = "TickSize";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.HeaderText = "DisplayFactor";
			numCol.Name = "DisplayFactor";
			numCol.DataPropertyName = "DisplayFactor";
			grid.Columns.Add(numCol);

			grid.DataSource = WatchDatas;
		}

		public void ReorderColumn()
		{
			ReorderColumn(this, "Symbol", 0);
			ReorderColumn(this, "Bid", 1);
			ReorderColumn(this, "Ask", 2);
			ReorderColumn(this, "Last", 3);
			ReorderColumn(this, "Tick", 4);
			ReorderColumn(this, "Net", 5);
			ReorderColumn(this, "Pct", 6);
			ReorderColumn(this, "Volume", 7);
			ReorderColumn(this, "High", 8);
			ReorderColumn(this, "Low", 9);
			ReorderColumn(this, "Open", 10);
			ReorderColumn(this, "Close", 11);
			ReorderColumn(this, "Vwap", 12);
			ReorderColumn(this, "Detail", 13);
			ReorderColumn(this, "TickSize", 14);
			ReorderColumn(this, "DisplayFactor", 15);
		}
		public void ReorderColumn(string colName, int displayIndex)
		{
			ReorderColumn(this, colName, displayIndex);
		}
		private void ReorderColumn(ROCWatchList grid, string colName, int displayIndex)
		{
			grid.Columns[colName].DisplayIndex = displayIndex;
		}

		#region - Not Used -

		protected override void OnCellValueNeeded(DataGridViewCellValueEventArgs e)
		{
			// If this is the row for new records, no values are needed.
			if (e.RowIndex == RowCount - 1)
			{
				return;
			}

			WatchListData watchData;

			// Store a reference to the Customer object for the row being painted.
			if (e.RowIndex == _inEditRowIndex)
			{
				watchData = _inEditRowData;
			}
			else
			{
				watchData = WatchDatas[e.RowIndex];
			}

			// Set the cell value to paint using the Customer object retrieved.
			switch (Columns[e.ColumnIndex].Name)
			{
				case "Symbol":
					e.Value = watchData.Symbol;
					break;
				case "Bid":
					e.Value = watchData.Bid;
					break;
				case "Ask":
					e.Value = watchData.Ask;
					break;
				case "Last":
					e.Value = watchData.Last;
					break;
				case "Tick":
					e.Value = watchData.Tick;
					break;
				case "Net":
					e.Value = watchData.Net;
					break;
				case "Pct":
					e.Value = watchData.Pct;
					break;
				case "Volume":
					e.Value = watchData.Volume;
					break;
				case "High":
					e.Value = watchData.High;
					break;
				case "Low":
					e.Value = watchData.Low;
					break;
				case "Open":
					e.Value = watchData.Open;
					break;
				case "Close":
					e.Value = watchData.Close;
					break;
				case "Vwap":
					e.Value = watchData.Vwap;
					break;
				case "Detail":
					e.Value = watchData.Detail;
					break;
				case "TickSize":
					e.Value = watchData.TickSize;
					break;
				case "DisplayFactor":
					e.Value = watchData.DisplayFactor;
					break;
			}

			base.OnCellValueNeeded(e);
		}

		protected override void OnCellValuePushed(DataGridViewCellValueEventArgs e)
		{
			WatchListData watchData;

			// Store a reference to the Customer object for the row being edited.
			if (e.RowIndex <= WatchDatas.Count)
			{
				// If the user is editing a new row, create a new Customer object.
				if (_inEditRowData == null)
				{
					_inEditRowData = new WatchListData(
						WatchDatas[e.RowIndex].Symbol,
						WatchDatas[e.RowIndex].Bid,
						WatchDatas[e.RowIndex].Ask,
						WatchDatas[e.RowIndex].Last,
						WatchDatas[e.RowIndex].Tick,
						WatchDatas[e.RowIndex].Net,
						WatchDatas[e.RowIndex].Pct,
						WatchDatas[e.RowIndex].Volume,
						WatchDatas[e.RowIndex].High,
						WatchDatas[e.RowIndex].Low,
						WatchDatas[e.RowIndex].Open,
						WatchDatas[e.RowIndex].Close,
						WatchDatas[e.RowIndex].Vwap,
						WatchDatas[e.RowIndex].Detail,
						WatchDatas[e.RowIndex].TickSize,
						WatchDatas[e.RowIndex].DisplayFactor);
				}
				watchData = _inEditRowData;
				_inEditRowIndex = e.RowIndex;
			}
			else
			{
				watchData = _inEditRowData;
			}

			// Set the appropriate Customer property to the cell value entered.
			switch (Columns[e.ColumnIndex].Name)
			{
				case "Symbol":
					watchData.Symbol = e.Value as string;
					break;
				case "Bid":
					watchData.Bid = e.Value as Nullable<double>;
					break;
				case "Ask":
					watchData.Ask = e.Value as Nullable<double>;
					break;
				case "Last":
					watchData.Last = e.Value as Nullable<double>;
					break;
				case "Tick":
					watchData.Tick = e.Value as string;
					break;
				case "Net":
					watchData.Net = e.Value as Nullable<double>;
					break;
				case "Pct":
					watchData.Pct = e.Value as Nullable<double>;
					break;
				case "Volume":
					watchData.Volume = e.Value as Nullable<long>;
					break;
				case "High":
					watchData.High = e.Value as Nullable<double>;
					break;
				case "Low":
					watchData.Low = e.Value as Nullable<double>;
					break;
				case "Open":
					watchData.Open = e.Value as Nullable<double>;
					break;
				case "Close":
					watchData.Close = e.Value as Nullable<double>;
					break;
				case "Vwap":
					watchData.Vwap = e.Value as Nullable<double>;
					break;
				case "Detail":
					watchData.Detail = e.Value as string;
					break;
				case "TickSize":
					watchData.TickSize = e.Value as Nullable<double>;
					break;
				case "DisplayFactor":
					watchData.DisplayFactor = e.Value as Nullable<double>;
					break;
			}

			base.OnCellValuePushed(e);
		}

		protected override void OnNewRowNeeded(DataGridViewRowEventArgs e)
		{
			// Create a new Customer object when the user edits
			// the row for new records.
			_inEditRowData = new WatchListData();
			_inEditRowIndex = Rows.Count - 1;
			base.OnNewRowNeeded(e);
		}

		protected override void OnRowValidated(DataGridViewCellEventArgs e)
		{
			// Save row changes if any were made and release the edited 
			// Customer object if there is one.
			if (e.RowIndex >= WatchDatas.Count && e.RowIndex != Rows.Count - 1)
			{
				// Add the new Customer object to the data store.
				WatchDatas.Add(_inEditRowData);
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}
			else if (_inEditRowData != null && e.RowIndex < WatchDatas.Count)
			{
				if (this["Symbol", e.RowIndex].Value != null)
				{
					_inEditRowData.Symbol = (string)this["Symbol", e.RowIndex].Value;
				}
				if (this["Bid", e.RowIndex].Value != null)
				{
					_inEditRowData.Bid = (double)this["Bid", e.RowIndex].Value;
				}
				if (this["Ask", e.RowIndex].Value != null)
				{
					_inEditRowData.Ask = (double)this["Ask", e.RowIndex].Value;
				}
				if (this["Last", e.RowIndex].Value != null)
				{
					_inEditRowData.Last = (double)this["Last", e.RowIndex].Value;
				}
				if (this["Tick", e.RowIndex].Value != null)
				{
					_inEditRowData.Tick = (string)this["Tick", e.RowIndex].Value;
				}
				if (this["Net", e.RowIndex].Value != null)
				{
					_inEditRowData.Net = (double)this["Net", e.RowIndex].Value;
				}
				if (this["Pct", e.RowIndex].Value != null)
				{
					_inEditRowData.Pct = (double)this["Pct", e.RowIndex].Value;
				}
				if (this["Volume", e.RowIndex].Value != null)
				{
					_inEditRowData.Volume = (long)this["Volume", e.RowIndex].Value;
				}
				if (this["High", e.RowIndex].Value != null)
				{
					_inEditRowData.High = (double)this["High", e.RowIndex].Value;
				}
				if (this["Low", e.RowIndex].Value != null)
				{
					_inEditRowData.Low = (double)this["Low", e.RowIndex].Value;
				}
				if (this["Open", e.RowIndex].Value != null)
				{
					_inEditRowData.Open = (double)this["Open", e.RowIndex].Value;
				}
				if (this["Close", e.RowIndex].Value != null)
				{
					_inEditRowData.Close = (double)this["Close", e.RowIndex].Value;
				}
				if (this["Vwap", e.RowIndex].Value != null)
				{
					_inEditRowData.Vwap = (double)this["Vwap", e.RowIndex].Value;
				}
				if (this["Detail", e.RowIndex].Value != null)
				{
					_inEditRowData.Detail = (string)this["Detail", e.RowIndex].Value;
				}
				if (this["TickSize", e.RowIndex].Value != null)
				{
					_inEditRowData.TickSize = (double)this["TickSize", e.RowIndex].Value;
				}
				if (this["DisplayFactor", e.RowIndex].Value != null)
				{
					_inEditRowData.DisplayFactor = (double)this["DisplayFactor", e.RowIndex].Value;
				}

				// Save the modified Customer object in the data store.
				WatchDatas[e.RowIndex] = _inEditRowData;
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}
			else if (ContainsFocus)
			{
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}

			base.OnRowValidated(e);
		}

		protected override void OnRowDirtyStateNeeded(QuestionEventArgs e)
		{
			if (!_rowScopeCommit)
			{
				// In cell-level commit scope, indicate whether the value
				// of the current cell has been modified.
				e.Response = IsCurrentCellDirty;
			}

			base.OnRowDirtyStateNeeded(e);
		}

		protected override void OnCancelRowEdit(QuestionEventArgs e)
		{
			if (_inEditRowIndex == Rows.Count - 2 && _inEditRowIndex == WatchDatas.Count)
			{
				// If the user has canceled the edit of a newly created row, 
				// replace the corresponding Customer object with a new, empty one.
				_inEditRowData = new WatchListData();
			}
			else
			{
				// If the user has canceled the edit of an existing row, 
				// release the corresponding Customer object.
				_inEditRowData = null;
				_inEditRowIndex = -1;
			}

			base.OnCancelRowEdit(e);
		}

		protected override void OnUserDeletedRow(DataGridViewRowEventArgs e)
		{
			if (e.Row.Index != -1)
			{
				if (e.Row.Index < WatchDatas.Count)
				{
					// If the user has deleted an existing row, remove the 
					// corresponding Customer object from the data store.
					WatchDatas.RemoveAt(e.Row.Index);
				}

				if (e.Row.Index == _inEditRowIndex)
				{
					// If the user has deleted a newly created row, release
					// the corresponding Customer object. 
					_inEditRowData = null;
					_inEditRowIndex = -1;
				}
			}
			else
			{
				foreach (DataGridViewRow row in SelectedRows)
				{
					if (row.Index != NewRowIndex)
					{
						// Don't remove when the row are a databounded
						//WatchDatas.RemoveAt(row.Index);
					}
				}
			}

			base.OnUserDeletedRow(e);
		}

		protected override void OnDataError(bool displayErrorDialogIfNoHandler, DataGridViewDataErrorEventArgs e)
		{
			base.OnDataError(false, e);
		}

		#endregion

		private void ListChanged(object sender, ListChangedEventArgs e)
		{
			if (e.PropertyDescriptor != null)
			{
				Debug.Print(e.PropertyDescriptor.Name, e.NewIndex.ToString());
				InvalidateCell(Columns[e.PropertyDescriptor.Name].Index, e.NewIndex);
			}
		}

		private void AddingNew(object sender, AddingNewEventArgs e)
		{
			if (e.NewObject != null)
			{
				string symbol = ((WatchListData)e.NewObject).Symbol;
				if (symbol != null && symbol != "")
				{
					OnSymbolAdded(new WatchListEventArgs(symbol));
				}
			}
		}
	}

	[Serializable]
	public class WatchListData : INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -
		
		[field:NonSerialized]
		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, e);
			}
		}

		#endregion

		public WatchListData()
		{
		}

		public WatchListData(
			string symbol,
			Nullable<double> bid,
			Nullable<double> ask,
			Nullable<double> last,
			string tick,
			Nullable<double> net,
			Nullable<double> pct,
			Nullable<long> volume,
			Nullable<double> high,
			Nullable<double> low,
			Nullable<double> open,
			Nullable<double> close,
			Nullable<double> vwap,
			string detail,
			Nullable<double> tickSize,
			Nullable<double> displayFactor)
		{
			_symbol = symbol;
			_bid = bid;
			_ask = ask;
			_last = last;
			_tick = tick;
			_net = net;
			_pct = pct;
			_volume = volume;
			_high = high;
			_low = low;
			_open = open;
			_close = close;
			_vwap = vwap;
			_detail = detail;
			_tickSize = tickSize;
			_displayFactor = displayFactor;
		}

		private string _symbol;
		public string Symbol
		{
			get
			{
				return _symbol;
			}
			set
			{
				if (_symbol != value)
				{
					_symbol = value;
					onPropertyChanged(new PropertyChangedEventArgs("Symbol"));
				}
			}
		}

		private Nullable<double> _bid;
		public Nullable<double> Bid
		{
			get
			{
				return _bid;
			}
			set
			{
				if (_bid != value)
				{
					_bid = value;
					onPropertyChanged(new PropertyChangedEventArgs("Bid"));
				}
			}
		}

		private Nullable<double> _ask;
		public Nullable<double> Ask
		{
			get
			{
				return _ask;
			}
			set
			{
				if (_ask != value)
				{
					_ask = value;
					onPropertyChanged(new PropertyChangedEventArgs("Ask"));
				}
			}
		}

		private Nullable<double> _last;
		public Nullable<double> Last
		{
			get
			{
				return _last;
			}
			set
			{
				if (_last != value)
				{
					_last = value;
					onPropertyChanged(new PropertyChangedEventArgs("Last"));
				}
			}
		}

		private string _tick;
		public string Tick
		{
			get
			{
				return _tick;
			}
			set
			{
				if (_tick != value)
				{
					_tick = value;
					onPropertyChanged(new PropertyChangedEventArgs("Tick"));
				}
			}
		}

		private Nullable<double> _net;
		public Nullable<double> Net
		{
			get
			{
				return _net;
			}
			set
			{
				if (_net != value)
				{
					_net = value;
					onPropertyChanged(new PropertyChangedEventArgs("Net"));
				}
			}
		}

		private Nullable<double> _pct;
		public Nullable<double> Pct
		{
			get
			{
				return _pct;
			}
			set
			{
				if (_pct != value)
				{
					_pct = value;
					onPropertyChanged(new PropertyChangedEventArgs("Pct"));
				}
			}
		}

		private Nullable<long> _volume;
		public Nullable<long> Volume
		{
			get
			{
				return _volume;
			}
			set
			{
				if (_volume != value)
				{
					_volume = value;
					onPropertyChanged(new PropertyChangedEventArgs("Volume"));
				}
			}
		}

		private Nullable<double> _high;
		public Nullable<double> High
		{
			get
			{
				return _high;
			}
			set
			{
				if (_high != value)
				{
					_high = value;
					onPropertyChanged(new PropertyChangedEventArgs("High"));
				}
			}
		}

		private Nullable<double> _low;
		public Nullable<double> Low
		{
			get
			{
				return _low;
			}
			set
			{
				if (_low != value)
				{
					_low = value;
					onPropertyChanged(new PropertyChangedEventArgs("Low"));
				}
			}
		}

		private Nullable<double> _open;
		public Nullable<double> Open
		{
			get
			{
				return _open;
			}
			set
			{
				if (_open != value)
				{
					_open = value;
					onPropertyChanged(new PropertyChangedEventArgs("Open"));
				}
			}
		}

		private Nullable<double> _close;
		public Nullable<double> Close
		{
			get
			{
				return _close;
			}
			set
			{
				if (_close != value)
				{
					_close = value;
					onPropertyChanged(new PropertyChangedEventArgs("Close"));
				}
			}
		}

		private Nullable<double> _vwap;
		public Nullable<double> Vwap
		{
			get
			{
				return _vwap;
			}
			set
			{
				if (_vwap != value)
				{
					_vwap = value;
					onPropertyChanged(new PropertyChangedEventArgs("Vwap"));
				}
			}
		}

		private string _detail;
		public string Detail
		{
			get
			{
				return _detail;
			}
			set
			{
				if (_detail != value)
				{
					_detail = value;
					onPropertyChanged(new PropertyChangedEventArgs("Detail"));
				}
			}
		}

		private Nullable<double> _tickSize;
		public Nullable<double> TickSize
		{
			get
			{
				if (_tickSize == null)
				{
					_tickSize = 0.01;
				}
				return _tickSize;
			}
			set
			{
				if (_tickSize != value)
				{
					_tickSize = value;
					onPropertyChanged(new PropertyChangedEventArgs("TickSize"));
				}
			}
		}

		private Nullable<double> _displayFactor;
		public Nullable<double> DisplayFactor
		{
			get
			{
				if (_displayFactor == null)
				{
					_displayFactor = 1;
				}
				return _displayFactor;
			}
			set
			{
				if (_displayFactor != value)
				{
					_displayFactor = value;
					onPropertyChanged(new PropertyChangedEventArgs("DisplayFactor"));
				}
			}
		}
	}

	[Serializable]
	public class WatchListProfile
	{
		private Color _gridBackColor;
		private Color _gridColor;
		private Font _gridFont;

		private string _gridDatas;

		private Dictionary<string, WatchListProfileColumn> _profileColumns;

		public WatchListProfile(ROCWatchList grid)
		{
			Export(grid);
		}

		public WatchListProfile(ROCWatchList grid, WatchListProfile prof)
		{
			Import(grid, prof);
		}

		private void Export(ROCWatchList grid)
		{
			_gridBackColor = grid.BackgroundColor;
			_gridColor = grid.GridColor;
			_gridFont = grid.Font;

			#region - Profile Columns -

			DGVNumberColumn numCol;
			DGVTickColumn imgCol;

			ProfileColumns = new Dictionary<string, WatchListProfileColumn>();
			foreach (DataGridViewColumn txtCol in grid.Columns)
			{
				switch (txtCol.Name)
				{
					case "Symbol":
						ProfileColumns.Add(txtCol.Name,
							new WatchListProfileColumn(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Bid":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.IsNotified,
							numCol.DefaultCellStyle.ForeColor,
							numCol.NotifyType,
							numCol.NotifyPositiveColor,
							numCol.NotifyPositiveBorderWidth,
							numCol.NotifyNegativeColor,
							numCol.NotifyNegativeBorderWidth,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Ask":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.IsNotified,
							numCol.DefaultCellStyle.ForeColor,
							numCol.NotifyType,
							numCol.NotifyPositiveColor,
							numCol.NotifyPositiveBorderWidth,
							numCol.NotifyNegativeColor,
							numCol.NotifyNegativeBorderWidth,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Last":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.IsNotified,
							numCol.DefaultCellStyle.ForeColor,
							numCol.NotifyType,
							numCol.NotifyPositiveColor,
							numCol.NotifyPositiveBorderWidth,
							numCol.NotifyNegativeColor,
							numCol.NotifyNegativeBorderWidth,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Tick":
						imgCol = (DGVTickColumn)txtCol;
						ProfileColumns.Add(imgCol.Name,
							new WatchListProfileColumn(
							imgCol.DefaultCellStyle.ForeColor,
							imgCol.DefaultCellStyle.Font,
							imgCol.DisplayIndex,
							imgCol.Name,
							imgCol.Visible,
							imgCol.Width));
						break;
					case "Net":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Pct":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Volume":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "High":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Low":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Open":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Close":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Vwap":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "Detail":
						ProfileColumns.Add(txtCol.Name,
							new WatchListProfileColumn(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "TickSize":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
					case "DisplayFactor":
						numCol = (DGVNumberColumn)txtCol;
						ProfileColumns.Add(numCol.Name,
							new WatchListProfileColumn(
							numCol.IsFraction,
							numCol.DefaultCellStyle.ForeColor,
							numCol.DefaultCellStyle.Font,
							numCol.DisplayIndex,
							numCol.Name,
							numCol.Visible,
							numCol.Width));
						break;
				}
			}

			#endregion

			_gridDatas = grid.WatchDatas.Export();
		}

		private void Import(ROCWatchList grid, WatchListProfile prof)
		{
			lock (prof)
			{
				_gridBackColor = prof.GridBackColor;
				_gridColor = prof.GridColor;
				_gridFont = prof.GridFont;

				_profileColumns = prof.ProfileColumns;

				_gridDatas = prof.GridDatas;
			}

			grid.DefaultCellStyle.BackColor = _gridBackColor;
			grid.GridColor = _gridColor;
			grid.Font = _gridFont;

			#region - Profile Columns -

			DataGridViewColumn txtCol;
			DGVNumberColumn numCol;
			DGVTickColumn imgCol;

			foreach (WatchListProfileColumn val in _profileColumns.Values)
			{
				switch (val.ColumnName)
				{
					case "Symbol":
						txtCol = grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Bid":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.IsNotified = val.IsNotified;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.NotifyType = val.NotifyType;
						numCol.NotifyPositiveColor = val.NotifyPositiveColor;
						numCol.NotifyPositiveBorderWidth = val.NotifyPositiveBorderWidth;
						numCol.NotifyNegativeColor = val.NotifyNegativeColor;
						numCol.NotifyNegativeBorderWidth = val.NotifyNegativeBorderWidth;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Ask":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.IsNotified = val.IsNotified;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.NotifyType = val.NotifyType;
						numCol.NotifyPositiveColor = val.NotifyPositiveColor;
						numCol.NotifyPositiveBorderWidth = val.NotifyPositiveBorderWidth;
						numCol.NotifyNegativeColor = val.NotifyNegativeColor;
						numCol.NotifyNegativeBorderWidth = val.NotifyNegativeBorderWidth;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Last":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.IsNotified = val.IsNotified;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.NotifyType = val.NotifyType;
						numCol.NotifyPositiveColor = val.NotifyPositiveColor;
						numCol.NotifyPositiveBorderWidth = val.NotifyPositiveBorderWidth;
						numCol.NotifyNegativeColor = val.NotifyNegativeColor;
						numCol.NotifyNegativeBorderWidth = val.NotifyNegativeBorderWidth;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Tick":
						imgCol = (DGVTickColumn)grid.Columns[val.ColumnName];
						imgCol.DefaultCellStyle.BackColor = _gridBackColor;
						imgCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						imgCol.DefaultCellStyle.Font = val.ColumnFont;
						imgCol.DisplayIndex = val.ColumnDisplayIndex;
						imgCol.Visible = val.ColumnVisiable;
						imgCol.Width = val.ColumnWidth;
						break;
					case "Net":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Pct":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Volume":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "High":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Low":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Open":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Close":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.IsFraction = val.IsFraction;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Vwap":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "Detail":
						txtCol = grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "TickSize":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
					case "DisplayFactor":
						numCol = (DGVNumberColumn)grid.Columns[val.ColumnName];
						numCol.DefaultCellStyle.BackColor = _gridBackColor;
						numCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						numCol.DefaultCellStyle.Font = val.ColumnFont;
						numCol.DisplayIndex = val.ColumnDisplayIndex;
						numCol.Visible = val.ColumnVisiable;
						numCol.Width = val.ColumnWidth;
						break;
				}
			}

			#endregion

			grid.WatchDatas.Import(_gridDatas);
		}

		public Color GridBackColor
		{
			get
			{
				return _gridBackColor;
			}
		}

		public Color GridColor
		{
			get
			{
				return _gridColor;
			}
		}

		public Font GridFont
		{
			get
			{
				return _gridFont;
			}
		}

		public Dictionary<string, WatchListProfileColumn> ProfileColumns
		{
			get
			{
				if (_profileColumns == null)
				{
					_profileColumns = new Dictionary<string, WatchListProfileColumn>();
				}
				return _profileColumns;
			}
			set
			{
				_profileColumns = value;
			}
		}

		public string GridDatas
		{
			get
			{
				return _gridDatas;
			}
		}
	}

	[Serializable]
	public class WatchListProfileColumn
	{
		private bool _isFraction;
		private bool _isNotified;
		private Color _defaultForeColor;
		private NotifyTypes _notifyType;
		private Color _notifyPositiveColor;
		private float _notifyPositiveBorderWidth;
		private Color _notifyNegativeColor;
		private float _notifyNegativeBorderWidth;
		private Font _columnFont;
		private int _columnDisplayIndex;
		private string _columnName;
		private bool _columnVisiable;
		private int _columnWidth;

		public WatchListProfileColumn(
			Color defaultForeColor,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			Initializ(
				false,
				false,
				defaultForeColor,
				NotifyTypes.None,
				Color.Empty,
				1.5f,
				Color.Empty,
				1.5f,
				columnFont,
				columnDisplayIndex,
				columnName,
				columnVisiable,
				columnWidth);
		}

		public WatchListProfileColumn(
			bool isFraction,
			Color defaultForeColor,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			Initializ(
				isFraction,
				false,
				defaultForeColor,
				NotifyTypes.None,
				Color.Empty,
				1.5f,
				Color.Empty,
				1.5f,
				columnFont,
				columnDisplayIndex,
				columnName,
				columnVisiable,
				columnWidth);
		}

		public WatchListProfileColumn(
			bool isFraction,
			bool isNotified,
			Color defaultForeColor,
			NotifyTypes notifyType,
			Color notifyPositiveColor,
			float notifyPositiveBorderWidth,
			Color notifyNegativeColor,
			float notifyNegativeBorderWidth,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			Initializ(
				isFraction,
				isNotified,
				defaultForeColor,
				notifyType,
				notifyPositiveColor,
				notifyPositiveBorderWidth,
				notifyNegativeColor,
				notifyNegativeBorderWidth,
				columnFont,
				columnDisplayIndex,
				columnName,
				columnVisiable,
				columnWidth);
		}

		private void Initializ(
			bool isFraction,
			bool isNotified,
			Color defaultForeColor,
			NotifyTypes notifyType,
			Color notifyPositiveColor,
			float notifyPositiveBorderWidth,
			Color notifyNegativeColor,
			float notifyNegativeBorderWidth,
			Font columnFont,
			int columnDisplayIndex,
			string columnName,
			bool columnVisiable,
			int columnWidth)
		{
			_isFraction = isFraction;
			_isNotified = isNotified;
			_defaultForeColor = defaultForeColor;
			_notifyType = notifyType;
			_notifyPositiveColor = notifyPositiveColor;
			_notifyPositiveBorderWidth = notifyPositiveBorderWidth;
			_notifyNegativeColor = notifyNegativeColor;
			_notifyNegativeBorderWidth = notifyNegativeBorderWidth;
			_columnFont = columnFont;
			_columnDisplayIndex = columnDisplayIndex;
			_columnName = columnName;
			_columnVisiable = columnVisiable;
			_columnWidth = columnWidth;
		}

		public bool IsFraction
		{
			get
			{
				return _isFraction;
			}
		}

		public bool IsNotified
		{
			get
			{
				return _isNotified;
			}
		}

		public Color DefaultForeColor
		{
			get
			{
				return _defaultForeColor;
			}
		}

		public NotifyTypes NotifyType
		{
			get
			{
				return _notifyType;
			}
		}

		public Color NotifyPositiveColor
		{
			get
			{
				return _notifyPositiveColor;
			}
		}

		public float NotifyPositiveBorderWidth
		{
			get
			{
				return _notifyPositiveBorderWidth;
			}
		}

		public Color NotifyNegativeColor
		{
			get
			{
				return _notifyNegativeColor;
			}
		}

		public float NotifyNegativeBorderWidth
		{
			get
			{
				return _notifyNegativeBorderWidth;
			}
		}

		public Font ColumnFont
		{
			get
			{
				return _columnFont;
			}
		}

		public int ColumnDisplayIndex
		{
			get
			{
				return _columnDisplayIndex;
			}
		}

		public string ColumnName
		{
			get
			{
				return _columnName;
			}
		}

		public bool ColumnVisiable
		{
			get
			{
				return _columnVisiable;
			}
		}

		public int ColumnWidth
		{
			get
			{
				return _columnWidth;
			}
		}
	}
	
	public class WatchListEventArgs : EventArgs
	{
		private string _symbol;

		public WatchListEventArgs(string symbol)
		{
			_symbol = symbol;
		}

		public string Symbol
		{
			get
			{
				return _symbol;
			}
		}
	}
}
