using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

using BindingListEx;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCSubTradesList : DGVBase
	{
		private ROCBindingList<OrderSubTradesListData> _rocGridData;
		[Browsable(false)]
		public ROCBindingList<OrderSubTradesListData> ROCGridData
		{
			get
			{
				if (_rocGridData == null)
				{
					_rocGridData = new ROCBindingList<OrderSubTradesListData>();
				}
				return _rocGridData;
			}
			set
			{
				_rocGridData = value;
			}
		}

		public void Initialize()
		{
			SetGrid(this);
		}

		private void SetGrid(ROCSubTradesList grid)
		{
			DGVOrderTextColumn txtCol;
			DGVOrderNumberColumn numCol;
			DGVOrderSideColumn sidCol;
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

			grid.AllowUserToOrderColumns = false;

			grid.VirtualMode = true;

			// Sub Trade grid special
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
			txtCol.HeaderText = "Exec ID";
			txtCol.Name = "ExecID";
			txtCol.DataPropertyName = "OmExecTag";
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
			txtCol.Name = "Symbol";
			txtCol.DataPropertyName = "Symbol";
			grid.Columns.Add(txtCol);

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
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Price";
			numCol.Name = "Price";
			numCol.DataPropertyName = "Price";
			grid.Columns.Add(numCol);

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
			timCol.DataPropertyName = "ExecTime";
			grid.Columns.Add(timCol);

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
			numCol.HeaderText = "ContractSize";
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

			grid.DataSource = ROCGridData;
		}

		public void SyncGUI(DGVBaseProfile prof)
		{
			lock (prof)
			{
				DefaultCellForeColor = prof.GridForeColor;
				DefaultSelectedBackgroundColor = prof.GridSelectedBackColor;
				BackgroundColor = prof.GridBackColor;
				GridColor = prof.GridColor;
				Font = prof.GridFont;

				foreach (DataGridViewColumn col in Columns)
				{
					col.DefaultCellStyle.SelectionBackColor = prof.GridSelectedBackColor;
					col.DefaultCellStyle.BackColor = prof.GridBackColor;
					col.DefaultCellStyle.ForeColor = prof.GridForeColor;
					col.DefaultCellStyle.Font = prof.GridFont;
				}
			}
		}
	}

	public class OrderSubTradesListData : INotifyPropertyChanged
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

		public OrderSubTradesListData()
		{
		}

		public OrderSubTradesListData(string omExecTag, string symbol, long side, long qty, double price, DateTime execTime, double contractSize, double tickSize, double displayFactor)
		{
			_omExecTag = omExecTag;
			_symbol = symbol;
			_side = side;
			_qty = qty;
			_price = price;
			_execTime = execTime;
			_contractSize = contractSize;
			_tickSize = tickSize;
			_displayFactor = displayFactor;
		}

		private string _omExecTag = "";
		public string OmExecTag
		{
			get
			{
				return _omExecTag;
			}
			set
			{
				if (_omExecTag != value)
				{
					_omExecTag = value;
					onPropertyChanged(new PropertyChangedEventArgs("OmExecTag"));
				}
			}
		}

		private string _symbol = "";
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

		private long _side = 0;
		public long Side
		{
			get
			{
				return _side;
			}
			set
			{
				if (_side != value)
				{
					_side = value;
					onPropertyChanged(new PropertyChangedEventArgs("Side"));
				}
			}
		}

		private long _qty = 0;
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
					onPropertyChanged(new PropertyChangedEventArgs("Qty"));
				}
			}
		}

		private double _price = 0;
		public double Price
		{
			get
			{
				return _price;
			}
			set
			{
				if (_price != value)
				{
					_price = value;
					onPropertyChanged(new PropertyChangedEventArgs("Price"));
				}
			}
		}

		private DateTime _execTime;
		public DateTime ExecTime
		{
			get
			{
				return _execTime;
			}
			set
			{
				if (_execTime != value)
				{
					_execTime = value;
					onPropertyChanged(new PropertyChangedEventArgs("ExecTime"));
				}
			}
		}

		private double _contractSize = 1;
		public double ContractSize
		{
			get
			{
				return _contractSize;
			}
			set
			{
				if (_contractSize != value)
				{
					_contractSize = value;
					onPropertyChanged(new PropertyChangedEventArgs("ContractSize"));
				}
			}
		}

		private double _tickSize = 0.01;
		public double TickSize
		{
			get
			{
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

		private double _displayFactor = 1;
		public double DisplayFactor
		{
			get
			{
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
