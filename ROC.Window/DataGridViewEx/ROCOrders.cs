using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Collections.Generic;

using BindingListEx;
using System.Data;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCOrders : DGVEx
	{
		private DataTable _ordersTable;
		public DataTable OrdersTable
		{
			get
			{
				SetTable();
				return _ordersTable;
			}
			set
			{
				_ordersTable = value;
			}
		}

		private void SetTable()
		{
			if (_ordersTable == null)
			{
				_ordersTable = new DataTable();

				_ordersTable.Columns.Add(new DataColumn("Tag", Type.GetType("System.String")));
				_ordersTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
				_ordersTable.Columns.Add(new DataColumn("Status", Type.GetType("System.Int64")));
				_ordersTable.Columns.Add(new DataColumn("Side", Type.GetType("System.Int64")));
				_ordersTable.Columns.Add(new DataColumn("Qty", Type.GetType("System.Int64")));
				_ordersTable.Columns.Add(new DataColumn("Price", Type.GetType("System.Double")));
				_ordersTable.Columns.Add(new DataColumn("LeaveQty", Type.GetType("System.Int64")));
				_ordersTable.Columns.Add(new DataColumn("CumQty", Type.GetType("System.Int64")));
				_ordersTable.Columns.Add(new DataColumn("AvgPrice", Type.GetType("System.Double")));
				_ordersTable.Columns.Add(new DataColumn("OrderType", Type.GetType("System.Int64")));
				_ordersTable.Columns.Add(new DataColumn("TIF", Type.GetType("System.Int64")));
				_ordersTable.Columns.Add(new DataColumn("OmTime", Type.GetType("System.DateTime")));
				_ordersTable.Columns.Add(new DataColumn("ClearingAcct", Type.GetType("System.String")));
			}
		}

		public void Initialize()
		{
			SetGrid(this);
		}

		private void SetGrid(ROCOrders grid)
		{
			DGVTextColumn txtCol;

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
			txtCol.HeaderText = "Order ID";
			txtCol.Name = "OrderID";
			txtCol.DataPropertyName = "Tag";
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

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(long);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Status";
			txtCol.Name = "Status";
			txtCol.DataPropertyName = "Status";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(long);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Side";
			txtCol.Name = "Side";
			txtCol.DataPropertyName = "Side";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(long);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Qty";
			txtCol.Name = "Qty";
			txtCol.DataPropertyName = "Qty";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(long);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Leave";
			txtCol.Name = "Leave";
			txtCol.DataPropertyName = "LeaveQty";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(double);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Price";
			txtCol.Name = "Price";
			txtCol.DataPropertyName = "Price";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(long);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Filled";
			txtCol.Name = "Filled";
			txtCol.DataPropertyName = "CumQty";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(double);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "AvgPrice";
			txtCol.Name = "AvgPrice";
			txtCol.DataPropertyName = "AvgPrice";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(double);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Type";
			txtCol.Name = "Type";
			txtCol.DataPropertyName = "OrderType";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(double);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Duration";
			txtCol.Name = "Duration";
			txtCol.DataPropertyName = "TIF";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(double);
			txtCol.DefaultCellStyle.Format = "";
			txtCol.HeaderText = "Exchange";
			txtCol.Name = "Exchange";
			txtCol.DataPropertyName = "DestID";
			grid.Columns.Add(txtCol);

			txtCol = new DGVTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.ForeColor = Color.White;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ValueType = typeof(DateTime);
			txtCol.DefaultCellStyle.Format = "HH:mm:ss";
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

			grid.DataSource = OrdersTable;
		}
	}

	[Serializable]
	public class ROCOrdersProfile
	{
		private Color _gridBackColor;
		private Color _gridColor;
		private Font _gridFont;

		private string _gridDatas;

		private Dictionary<string, DGVColumnProfile> _profileColumns;

		public ROCOrdersProfile()
		{
		}

		public ROCOrdersProfile(ROCWatchList grid)
		{
			Export(grid);
		}

		public ROCOrdersProfile(ROCWatchList grid, WatchListProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCWatchList grid)
		{
			_gridBackColor = grid.BackgroundColor;
			_gridColor = grid.GridColor;
			_gridFont = grid.Font;

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DGVTextColumn txtCol in grid.Columns)
			{
				switch (txtCol.Name)
				{
					case "OrderID":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Symbol":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Status":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Side":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Qty":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Leave":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Price":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Filled":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "AvgPrice":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Type":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Duration":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Exchange":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Time":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
					case "Account":
						ProfileColumns.Add(txtCol.Name,
							new DGVColumnProfile(
							txtCol.DefaultCellStyle.ForeColor,
							txtCol.DefaultCellStyle.Font,
							txtCol.DisplayIndex,
							txtCol.Name,
							txtCol.Visible,
							txtCol.Width));
						break;
				}
			}

			#endregion
		}

		public void Import(ROCWatchList grid, WatchListProfile prof)
		{
			lock (prof)
			{
				_gridBackColor = prof.GridBackColor;
				_gridColor = prof.GridColor;
				_gridFont = prof.GridFont;

				_profileColumns = prof.ProfileColumns;
			}

			grid.DefaultCellStyle.BackColor = _gridBackColor;
			grid.GridColor = _gridColor;
			grid.Font = _gridFont;

			#region - Profile Columns -

			DGVTextColumn txtCol;

			foreach (DGVColumnProfile val in _profileColumns.Values)
			{
				switch (val.ColumnName)
				{
					case "OrderID":
						txtCol = grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Symbol":
						txtCol = grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Status":
						txtCol = grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Side":
						txtCol = grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.DefaultForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
				}
			}

			#endregion

			grid.ROCGridData.Import(_gridDatas);

			if (grid.ROCGridData.Count > 0)
			{
				grid.Rows.Add(grid.ROCGridData.Count);
				for (int i = 0; i < grid.ROCGridData.Count; i++)
				{
					if (!grid.Rows[i].IsNewRow)
					{
						grid.Rows[i].Cells["Symbol"].Value = grid.ROCGridData[i].Symbol;
						grid.Rows[i].Cells["Bid"].Value = grid.ROCGridData[i].Bid;
						grid.Rows[i].Cells["Ask"].Value = grid.ROCGridData[i].Ask;
						grid.Rows[i].Cells["Last"].Value = grid.ROCGridData[i].Last;
						grid.Rows[i].Cells["Tick"].Value = grid.ROCGridData[i].Tick;
						grid.Rows[i].Cells["Net"].Value = grid.ROCGridData[i].Net;
						grid.Rows[i].Cells["Pct"].Value = grid.ROCGridData[i].Pct;
						grid.Rows[i].Cells["Volume"].Value = grid.ROCGridData[i].Volume;
						grid.Rows[i].Cells["High"].Value = grid.ROCGridData[i].High;
						grid.Rows[i].Cells["Low"].Value = grid.ROCGridData[i].Low;
						grid.Rows[i].Cells["Open"].Value = grid.ROCGridData[i].Open;
						grid.Rows[i].Cells["Close"].Value = grid.ROCGridData[i].Close;
						grid.Rows[i].Cells["Vwap"].Value = grid.ROCGridData[i].Vwap;
						grid.Rows[i].Cells["Detail"].Value = grid.ROCGridData[i].Detail;
						grid.Rows[i].Cells["TickSize"].Value = grid.ROCGridData[i].TickSize;
						grid.Rows[i].Cells["DisplayFactor"].Value = grid.ROCGridData[i].DisplayFactor;
					}
				}
			}
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

		public Dictionary<string, DGVColumnProfile> ProfileColumns
		{
			get
			{
				if (_profileColumns == null)
				{
					_profileColumns = new Dictionary<string, DGVColumnProfile>();
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
}
