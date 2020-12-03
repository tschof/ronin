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
	public class ROCPositionList : DGVBase
	{
		private DataTable _stdPositionTable;
		public DataTable StdPositionTable
		{
			get
			{
				SetStdTable();
				return _stdPositionTable;
			}
			set
			{
				_stdPositionTable = value;
			}
		}

		private void SetStdTable()
		{
		}

		private DataTable _extPositionTable;
		public DataTable ExtPositionTable
		{
			get
			{
				SetExtTable();
				return _extPositionTable;
			}
			set
			{
				_extPositionTable = value;
			}
		}

		private void SetExtTable()
		{
		}
		
		public void Initialize()
		{
			SetGrid(this);
		}

		private void SetGrid(ROCPositionList grid)
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
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "SymbolDetail";
			txtCol.Name = "SymbolDetail";
			txtCol.DataPropertyName = "SymbolDetail";
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
	}

	[Serializable]
	public class ROCPositionsProfile
	{
		private Color _gridBackColor;
		private Color _gridColor;
		private Font _gridFont;

		private Dictionary<string, DGVColumnProfile> _profileColumns;

		public ROCPositionsProfile()
		{
		}

		public ROCPositionsProfile(ROCPositionList grid)
		{
			Export(grid);
		}

		public ROCPositionsProfile(ROCPositionList grid, ROCPositionsProfile prof)
		{
			Import(grid, prof);
		}

		public void Export(ROCPositionList grid)
		{
			_gridBackColor = grid.BackgroundColor;
			_gridColor = grid.GridColor;
			_gridFont = grid.Font;

			#region - Profile Columns -

			ProfileColumns = new Dictionary<string, DGVColumnProfile>();
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (col.Name)
				{
					default:
						AddProfile(col);
						break;
				}
			}

			#endregion
		}

		private void AddProfile(DataGridViewColumn col)
		{
			ProfileColumns.Add(col.Name, new DGVColumnProfile());
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.ForeColor, col.DefaultCellStyle.ForeColor);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.BackColor, col.DefaultCellStyle.BackColor);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.Font, col.DefaultCellStyle.Font);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.HeaderForeColor, col.HeaderCell.Style.ForeColor);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.HeaderBackColor, col.HeaderCell.Style.BackColor);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.HeaderFont, col.HeaderCell.Style.Font);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.DisplayIndex, col.DisplayIndex);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.Name, col.Name);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.Visiable, col.Visible);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.Width, col.Width);
			ProfileColumns[col.Name].Add(DGVColumnProfileFieldID.Column.SelectedBackColor, col.DefaultCellStyle.SelectionBackColor);
		}

		public void Import(ROCPositionList grid, ROCPositionsProfile prof)
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
					case "Symbol":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "SymbolDetail":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Status":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Side":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Qty":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Leave":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Price":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Filled":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "AvgPrice":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Type":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Duration":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Exchange":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Time":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
					case "Account":
						txtCol = (DGVTextColumn)grid.Columns[val.ColumnName];
						txtCol.DefaultCellStyle.BackColor = _gridBackColor;
						txtCol.DefaultCellStyle.ForeColor = val.ColumnForeColor;
						txtCol.DefaultCellStyle.Font = val.ColumnFont;
						txtCol.DisplayIndex = val.ColumnDisplayIndex;
						txtCol.Visible = val.ColumnVisiable;
						txtCol.Width = val.ColumnWidth;
						break;
				}
			}

			#endregion
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
	}
}
