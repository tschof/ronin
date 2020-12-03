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
	public class ROCBatch : DGVBaseBatch, INotifyPropertyChanged
	{
		#region - Data Table -

		private DataTable _rocGridTable;
		[Browsable(false)]
		public override DataTable RocGridTable
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
				_rocGridTable.Columns.Add(new DataColumn("Qty", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("Side", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Price", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Exchange", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("LocalAccountAcrn", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("TraderFor", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Display", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("ShortLender", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Duration", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("OrderType", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Instruction", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("ExecInstruction", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("StopPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("PegPrice", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("AlgoType", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("StartTime", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("EndTime", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Note", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("ProgramTrade", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Status", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("OrderID", Type.GetType("System.String")));

				_rocGridTable.Columns["Symbol"].DefaultValue = "";

				_rocGridTable.DefaultView.Sort = "";

				_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
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

		#region - Grid -

		public override void Initialize()
		{
			SetGridTable();

			SetGrid(this);
		}

		private void SetGrid(ROCBatch grid)
		{
			DGVBatchTextColumn txtCol;
			DGVBatchNumberColumn numCol;
			DGVBatchStatusColumn staCol;

			grid.BackgroundColor = Color.Black;
			grid.GridColor = Color.Black;

			grid.ShowCellToolTips = false;
			grid.MultiSelect = true;
			grid.SelectionMode = DataGridViewSelectionMode.FullRowSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.AllowUserToAddRows = true;
			grid.AllowUserToDeleteRows = true;
			grid.AllowUserToOrderColumns = false;

			grid.VirtualMode = true;

			// Batch grid specials
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Symbol";
			txtCol.Name = "Symbol";
			txtCol.DataPropertyName = "Symbol";
			grid.Columns.Add(txtCol);

			numCol = new DGVBatchNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "Qty";
			numCol.Name = "Qty";
			numCol.DataPropertyName = "Qty";
			grid.Columns.Add(numCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Side";
			txtCol.Name = "Side";
			txtCol.DataPropertyName = "Side";
			grid.Columns.Add(txtCol);

			numCol = new DGVBatchNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Price";
			numCol.Name = "Price";
			numCol.DataPropertyName = "Price";
			grid.Columns.Add(numCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Exchange";
			txtCol.Name = "Exchange";
			txtCol.DataPropertyName = "Exchange";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Account";
			txtCol.Name = "LocalAccountAcrn";
			txtCol.DataPropertyName = "LocalAccountAcrn";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Trader";
			txtCol.Name = "TraderFor";
			txtCol.DataPropertyName = "TraderFor";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Display";
			txtCol.Name = "Display";
			txtCol.DataPropertyName = "Display";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "SLender";
			txtCol.Name = "ShortLender";
			txtCol.DataPropertyName = "ShortLender";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "TIF";
			txtCol.Name = "Duration";
			txtCol.DataPropertyName = "Duration";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Type";
			txtCol.Name = "OrderType";
			txtCol.DataPropertyName = "OrderType";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Instruction";
			txtCol.Name = "Instruction";
			txtCol.DataPropertyName = "Instruction";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "AlgoType";
			txtCol.Name = "AlgoType";
			txtCol.DataPropertyName = "AlgoType";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Exec Instruction";
			txtCol.Name = "ExecInstruction";
			txtCol.DataPropertyName = "ExecInstruction";
			grid.Columns.Add(txtCol);
			SingleClickColumns.Add(txtCol);

			numCol = new DGVBatchNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "StopPrice";
			numCol.Name = "StopPrice";
			numCol.DataPropertyName = "StopPrice";
			grid.Columns.Add(numCol);
			SingleClickColumns.Add(txtCol);

			numCol = new DGVBatchNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PegPrice";
			numCol.Name = "PegPrice";
			numCol.DataPropertyName = "PegPrice";
			grid.Columns.Add(numCol);
			SingleClickColumns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "StartTime";
			txtCol.Name = "StartTime";
			txtCol.DataPropertyName = "StartTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "EndTime";
			txtCol.Name = "EndTime";
			txtCol.DataPropertyName = "EndTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Note";
			txtCol.Name = "Note";
			txtCol.DataPropertyName = "Note";
			grid.Columns.Add(txtCol);
			SingleClickColumns.Add(txtCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "ProgramTrade";
			txtCol.Name = "ProgramTrade";
			txtCol.DataPropertyName = "ProgramTrade";
			grid.Columns.Add(txtCol);
			SingleClickColumns.Add(txtCol);

			staCol = new DGVBatchStatusColumn();
			staCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			staCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			staCol.DefaultCellStyle.BackColor = Color.Black;
			staCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			staCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			staCol.SortMode = DataGridViewColumnSortMode.Automatic;
			staCol.ReadOnly = true;
			staCol.ValueType = typeof(long);
			staCol.DefaultCellStyle.Format = "";
			staCol.HeaderText = "Status";
			staCol.Name = "Status";
			staCol.DataPropertyName = "Status";
			grid.Columns.Add(staCol);

			txtCol = new DGVBatchTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Order ID";
			txtCol.Name = "OrderID";
			txtCol.DataPropertyName = "OrderID";
			grid.Columns.Add(txtCol);

			SingleClickColumns.Clear();
			foreach (DataGridViewColumn col in Columns)
			{
				if (!col.ReadOnly && col.Name != "Symbol")
				{
					SingleClickColumns.Add(col);
				}
				col.SortMode = DataGridViewColumnSortMode.NotSortable;

				if (col.ReadOnly)
				{
					col.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
					col.DefaultCellStyle.SelectionBackColor = Color.DarkBlue;
				}
				else
				{
					col.DefaultCellStyle.BackColor = Color.Black;
					col.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
				}
			}

			grid.AlternatingRowsDefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion
	}
}
