using System;
using System.Drawing;
using System.Windows.Forms;
using System.ComponentModel;

using BindingListEx;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCOrderTradeList : DGVBase
	{
		private ROCBindingList<OrderTradeListData> _rocGridData;
		public ROCBindingList<OrderTradeListData> ROCGridData
		{
			get
			{
				if (_rocGridData == null)
				{
					_rocGridData = new ROCBindingList<OrderTradeListData>();
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

		private void SetGrid(ROCOrderTradeList grid)
		{
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

			grid.VirtualMode = false;

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
		}
	}

	public class OrderTradeListData : INotifyPropertyChanged
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

		public OrderTradeListData()
		{
		}

		public OrderTradeListData(long side, long qty, double price, DateTime execTime)
		{
			_side = side;
			_qty = qty;
			_price = price;
			_execTime = execTime;
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
	}
}
