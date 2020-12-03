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
	public class ROCBatchMarket : DGVBaseBatch, INotifyPropertyChanged
	{
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

		#endregion

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
				_rocGridTable.Columns.Add(new DataColumn("SymbolDetail", Type.GetType("System.String")));
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

				// Security Columns
				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));
				
				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("CallPut", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("ExpDate", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("MaturityDay", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Strike", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Underlying", Type.GetType("System.String")));

				// Market Data Columns
				_rocGridTable.Columns.Add(new DataColumn("BidPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("AskPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("LastTraded", Type.GetType("System.Double")));

				_rocGridTable.Columns["Symbol"].DefaultValue = "";
				_rocGridTable.Columns["SymbolDetail"].DefaultValue = "";
				_rocGridTable.Columns["TickSize"].DefaultValue = 0.01;
				_rocGridTable.Columns["DisplayFactor"].DefaultValue = 1;

				_rocGridTable.Columns["SecType"].DefaultValue = "";
				_rocGridTable.Columns["CallPut"].DefaultValue = "";
				_rocGridTable.Columns["ExpDate"].DefaultValue = "";
				_rocGridTable.Columns["MaturityDay"].DefaultValue = "";
				_rocGridTable.Columns["Strike"].DefaultValue = "";
				_rocGridTable.Columns["Underlying"].DefaultValue = "";

				_rocGridTable.DefaultView.Sort = "";

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
					if (Columns.Contains("SymbolDetail"))
					{
						if (e.Row["SymbolDetail"] != null && e.Row["SymbolDetail"].ToString() != "")
						{
							SubscribeSymbolDetail = e.Row["SymbolDetail"].ToString().ToUpper().Trim();
						}
					}
					break;
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
			IsMarketDataBatchGrid = true;

			SetGridTable();

			SetGrid(this);
		}

		private void SetGrid(ROCBatchMarket grid)
		{
			DGVBatchMarketTextColumn txtCol;
			DGVBatchMarketNumberColumn numCol;
			DGVBatchMarketNumericUpDownColumn numUpDownCol;
			DGVBatchMarketStatusColumn staCol;

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

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Detail";
			txtCol.Name = "SymbolDetail";
			txtCol.DataPropertyName = "SymbolDetail";
			grid.Columns.Add(txtCol);

			numUpDownCol = new DGVBatchMarketNumericUpDownColumn();
			numUpDownCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numUpDownCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numUpDownCol.DefaultCellStyle.BackColor = Color.Black;
			numUpDownCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numUpDownCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numUpDownCol.DefaultCellStyle.SelectionForeColor = Color.White;
			numUpDownCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numUpDownCol.ReadOnly = false;
			numUpDownCol.MaxDecimal = 0;
			numUpDownCol.ValueType = typeof(long);
			numUpDownCol.DefaultCellStyle.Format = "F";
			numUpDownCol.HeaderText = "Qty";
			numUpDownCol.Name = "Qty";
			numUpDownCol.DataPropertyName = "Qty";
			grid.Columns.Add(numUpDownCol);

			txtCol = new DGVBatchMarketTextColumn();
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

			numUpDownCol = new DGVBatchMarketNumericUpDownColumn();
			numUpDownCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numUpDownCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numUpDownCol.DefaultCellStyle.BackColor = Color.Black;
			numUpDownCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numUpDownCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numUpDownCol.DefaultCellStyle.SelectionForeColor = Color.White;
			numUpDownCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numUpDownCol.ReadOnly = false;
			numUpDownCol.ValueType = typeof(double);
			numUpDownCol.DefaultCellStyle.Format = "";
			numUpDownCol.HeaderText = "Price";
			numUpDownCol.Name = "Price";
			numUpDownCol.DataPropertyName = "Price";
			grid.Columns.Add(numUpDownCol);

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			#region - Optional Fields -

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			numUpDownCol = new DGVBatchMarketNumericUpDownColumn();
			numUpDownCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numUpDownCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numUpDownCol.DefaultCellStyle.BackColor = Color.Black;
			numUpDownCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numUpDownCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numUpDownCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numUpDownCol.ReadOnly = false;
			numUpDownCol.ValueType = typeof(double);
			numUpDownCol.DefaultCellStyle.Format = "";
			numUpDownCol.HeaderText = "StopPrice";
			numUpDownCol.Name = "StopPrice";
			numUpDownCol.DataPropertyName = "StopPrice";
			grid.Columns.Add(numUpDownCol);

			numUpDownCol = new DGVBatchMarketNumericUpDownColumn();
			numUpDownCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numUpDownCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numUpDownCol.DefaultCellStyle.BackColor = Color.Black;
			numUpDownCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numUpDownCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numUpDownCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numUpDownCol.ReadOnly = false;
			numUpDownCol.ValueType = typeof(double);
			numUpDownCol.DefaultCellStyle.Format = "";
			numUpDownCol.HeaderText = "PegPrice";
			numUpDownCol.Name = "PegPrice";
			numUpDownCol.DataPropertyName = "PegPrice";
			grid.Columns.Add(numUpDownCol);

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			txtCol = new DGVBatchMarketTextColumn();
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

			#endregion

			#region - Status Fields -

			staCol = new DGVBatchMarketStatusColumn();
			staCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			staCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			staCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
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

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Order ID";
			txtCol.Name = "OrderID";
			txtCol.DataPropertyName = "OrderID";
			grid.Columns.Add(txtCol);

			#endregion

			#region - Security Info -

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
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

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Underlying";
			txtCol.Name = "Underlying";
			txtCol.DataPropertyName = "Underlying";
			grid.Columns.Add(txtCol);

			numCol = new DGVBatchMarketNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
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

			numCol = new DGVBatchMarketNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
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

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
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

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "CallPut";
			txtCol.Name = "CallPut";
			txtCol.DataPropertyName = "CallPut";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "ExpDate";
			txtCol.Name = "ExpDate";
			txtCol.DataPropertyName = "ExpDate";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "MaturityDay";
			txtCol.Name = "MaturityDay";
			txtCol.DataPropertyName = "MaturityDay";
			grid.Columns.Add(txtCol);

			txtCol = new DGVBatchMarketTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Strike";
			txtCol.Name = "Strike";
			txtCol.DataPropertyName = "Strike";
			grid.Columns.Add(txtCol);

			#endregion

			#region - Market Data -

			numCol = new DGVBatchMarketNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			//numCol.NotifyType = NotifyTypes.None;
			//numCol.IsNotified = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Bid";
			numCol.Name = "BidPrice";
			numCol.DataPropertyName = "BidPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVBatchMarketNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			//numCol.NotifyType = NotifyTypes.None;
			//numCol.IsNotified = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Ask";
			numCol.Name = "AskPrice";
			numCol.DataPropertyName = "AskPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVBatchMarketNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			//numCol.NotifyType = NotifyTypes.None;
			//numCol.IsNotified = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Last";
			numCol.Name = "LastTraded";
			numCol.DataPropertyName = "LastTraded";
			grid.Columns.Add(numCol);

			#endregion

			SingleClickColumns.Clear();
			foreach (DataGridViewColumn col in Columns)
			{
				if (!col.ReadOnly && col.Name != "SymbolDetail")
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

		#region - Tick Sizes -

		public void UpdateMultiplier(string symbol, double tickSize)
		{
			if (TickSizes.ContainsKey(symbol))
			{
				TickSizes[symbol] = tickSize;
			}
			else
			{
				TickSizes.Add(symbol, tickSize);
			}
		}

		#endregion

		#region - Grid Event -

		protected override void OnCurrentCellDirtyStateChanged(EventArgs e)
		{
			if (CurrentCell.IsInEditMode && Columns[CurrentCell.ColumnIndex].Name != "SymbolDetail")
			{
				CurrentCell.Value = EditingControl.Text;
			}
			base.OnCurrentCellDirtyStateChanged(e);
		}

		#endregion
	}
}
