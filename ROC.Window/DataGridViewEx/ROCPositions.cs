using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Collections.Generic;

using BindingListEx;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCPositions : DGVEx
	{
		private ROCBindingList<WatchListData> _rocGridData;
		public ROCBindingList<WatchListData> ROCGridData
		{
			get
			{
				if (_rocGridData == null)
				{
					_rocGridData = new ROCBindingList<WatchListData>();
					_rocGridData.AddingNew += new AddingNewEventHandler(AddingNew);
					_rocGridData.ListChanged += new ListChangedEventHandler(ListChanged);
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

		private void SetGrid(ROCPositions grid)
		{
			DGVTextColumn txtCol;
			DGVNumberColumn numCol;

			grid.BackgroundColor = Color.Black;
			grid.GridColor = Color.Black;

			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.AllowUserToAddRows = false;
			grid.AllowUserToDeleteRows = false;

			grid.ReadOnly = true;

			grid.VirtualMode = true;

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "OrderID";
			txtCol.Name = "OrderID";
			txtCol.DataPropertyName = "OrderID";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
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
			numCol.IsNotified = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Status";
			numCol.Name = "Status";
			numCol.DataPropertyName = "Status";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Side";
			numCol.Name = "Side";
			numCol.DataPropertyName = "Side";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Qty";
			numCol.Name = "Qty";
			numCol.DataPropertyName = "Qty";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Leave";
			numCol.Name = "Leave";
			numCol.DataPropertyName = "LeaveQty";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Price";
			numCol.Name = "Price";
			numCol.DataPropertyName = "Price";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Filled";
			numCol.Name = "Filled";
			numCol.DataPropertyName = "CumQty";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "AvgPrice";
			numCol.Name = "AvgPrice";
			numCol.DataPropertyName = "AvgPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Type";
			numCol.Name = "Type";
			numCol.DataPropertyName = "OrderType";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Duration";
			numCol.Name = "Duration";
			numCol.DataPropertyName = "TIF";
			grid.Columns.Add(numCol);

			numCol = new DGVNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.ForeColor = Color.White;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Exchange";
			numCol.Name = "Exchange";
			numCol.DataPropertyName = "DestID";
			grid.Columns.Add(numCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(DateTime);
			txtCol.DefaultCellStyle.Format = "hh:mm:ss";
			txtCol.HeaderText = "Time";
			txtCol.Name = "Time";
			txtCol.DataPropertyName = "OmTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(string);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Account";
			txtCol.Name = "Account";
			txtCol.DataPropertyName = "ClearingAcct";
			grid.Columns.Add(txtCol);
		}

		private void ListChanged(object sender, ListChangedEventArgs e)
		{
		}

		private void AddingNew(object sender, AddingNewEventArgs e)
		{
		}
	}
}
