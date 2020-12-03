using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using BindingListEx;

namespace GridEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class DGVEx : DataGridView
	{
		private BindingListEx<WatchListData> _watchList;
		public BindingListEx<WatchListData> WatchList
		{
			get
			{
				if (_watchList == null)
				{
					_watchList = new BindingListEx<WatchListData>();
					_watchList.AddingNew += new AddingNewEventHandler(AddingNew);
					_watchList.ListChanged += new ListChangedEventHandler(ListChanged);
				}
				return _watchList;
			}
			set
			{
				_watchList = value;
			}
		}

		public DGVEx()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.UserPaint | ControlStyles.DoubleBuffer | ControlStyles.OptimizedDoubleBuffer, true);
		}

		public void SetWatchListGrid()
		{
			DGVExColumn col;

			this.BackgroundColor = Color.Black;
			this.GridColor = Color.Black;

			this.MultiSelect = false;
			this.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			this.ShowCellToolTips = false;
			this.AutoGenerateColumns = false;
			this.RowHeadersVisible = false;
			this.AllowUserToResizeRows = false;
			this.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			this.VirtualMode = true;

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(string);
			col.HeaderText = "Symbol";
			col.Name = "Symbol";
			col.DataPropertyName = "Symbol";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.HeaderText = "Bid";
			col.Name = "Bid";
			col.DataPropertyName = "Bid";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Ask";
			col.Name = "Ask";
			col.DataPropertyName = "Ask";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Last";
			col.Name = "Last";
			col.DataPropertyName = "Last";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.NotSortable;
			col.ValueType = typeof(string);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Tick";
			col.Name = "Tick";
			col.DataPropertyName = "Tick";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Net";
			col.Name = "Net";
			col.DataPropertyName = "Net";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Pct";
			col.Name = "Pct";
			col.DataPropertyName = "Pct";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(long);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Volume";
			col.Name = "Volume";
			col.DataPropertyName = "Volume";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "High";
			col.Name = "High";
			col.DataPropertyName = "High";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Low";
			col.Name = "Low";
			col.DataPropertyName = "Low";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Open";
			col.Name = "Open";
			col.DataPropertyName = "Open";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Close";
			col.Name = "Close";
			col.DataPropertyName = "Close";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Vwap";
			col.Name = "Vwap";
			col.DataPropertyName = "Vwap";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(string);
			col.DefaultCellStyle.Format = "";
			col.HeaderText = "Detail";
			col.Name = "Detail";
			col.DataPropertyName = "Detail";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.HeaderText = "TickSize";
			col.Name = "TickSize";
			col.DataPropertyName = "TickSize";
			this.Columns.Add(col);

			col = new DGVExColumn();
			col.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			col.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			col.DefaultCellStyle.BackColor = Color.Black;
			col.DefaultCellStyle.ForeColor = Color.White;
			col.SortMode = DataGridViewColumnSortMode.Automatic;
			col.ValueType = typeof(double);
			col.HeaderText = "DisplayFactor";
			col.Name = "DisplayFactor";
			col.DataPropertyName = "DisplayFactor";
			this.Columns.Add(col);

			this.DataSource = WatchList;
		}

		private void ListChanged(object sender, ListChangedEventArgs e)
		{
			if (e.PropertyDescriptor != null)
			{
				InvalidateCell(Columns[e.PropertyDescriptor.Name].Index, e.NewIndex);
			}
		}

		private void AddingNew(object sender, AddingNewEventArgs e)
		{
		}
	}

	[Serializable]
	public class WatchListData : INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		[field: NonSerialized]
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
}
