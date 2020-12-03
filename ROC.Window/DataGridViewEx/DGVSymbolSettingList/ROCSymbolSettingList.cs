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
	public class ROCSymbolSettingList : DGVBase, INotifyPropertyChanged
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

		private string _currentSecType = CSVEx.CSVFieldIDs.SecutrityTypes.Equity;
		[Browsable(false)]
		public string CurrentSecType
		{
			get
			{
				return _currentSecType;
			}
			set
			{
				_currentSecType = value;
			}
		}

		#region - Data Table -

		private DataTable _rocGridTable;
		[Browsable(false)]
		public DataTable RocGridTable
		{
			get
			{
				SetGridTable(null);
				return _rocGridTable;
			}
			set
			{
				_rocGridTable = value;
			}
		}

		private void SetGridTable(DataTable source)
		{
			if (_rocGridTable == null)
			{
				if (source != null)
				{
					_rocGridTable = source.Clone();
				}
				else
				{
					_rocGridTable = new DataTable("SymbolSettings");

					_rocGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					List<DataColumn> keyColumns = new List<DataColumn>();
					keyColumns.Add(_rocGridTable.Columns["Symbol"]);

					_rocGridTable.Columns.Add(new DataColumn("Quantity", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("QuantityIncrement", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("LimitPrice", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("LimitPriceIncrement", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("StopPrice", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("StopPriceIncrement", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("PegPrice", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("PegPriceIncrement", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("StopLimitOffset", Type.GetType("System.Decimal")));

					_rocGridTable.Columns.Add(new DataColumn("OrderType", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Duration", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("Show", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("ShowIncrement", Type.GetType("System.Decimal")));
					_rocGridTable.Columns.Add(new DataColumn("UseShow", Type.GetType("System.Boolean")));

					_rocGridTable.Columns.Add(new DataColumn("ShortLender", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("Instructions", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("UseInstructions", Type.GetType("System.Boolean")));

					_rocGridTable.Columns.Add(new DataColumn("Trader", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Account", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Exchange", Type.GetType("System.String")));

					// Stock
					_rocGridTable.Columns.Add(new DataColumn("UseLevel2", Type.GetType("System.Boolean")));

					// Future and Option
					_rocGridTable.Columns.Add(new DataColumn("Expiration", Type.GetType("System.String")));

					// Option
					_rocGridTable.Columns.Add(new DataColumn("CallPut", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Strike", Type.GetType("System.Double")));

					_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
					keyColumns.Add(_rocGridTable.Columns["SecType"]);

					_rocGridTable.PrimaryKey = keyColumns.ToArray();

					_rocGridTable.DefaultView.Sort = "Symbol, SecType";

					_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
				}
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

		public void Initialize(DataTable source)
		{
			SetGridTable(source);

			SetGrid(this);
		}

		private void SetGrid(ROCSymbolSettingList grid)
		{
			DGVSymbolSettingTextColumn txtCol;
			DataGridViewTextBoxColumn numCol;
			DataGridViewCheckBoxColumn chkCol;

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
			grid.AllowUserToOrderColumns = false;

			grid.VirtualMode = false;

			// Symbol Setting grid speical
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVSymbolSettingTextColumn();
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

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Qty";
			numCol.Name = "Quantity";
			numCol.DataPropertyName = "Quantity";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "QtyInc";
			numCol.Name = "QuantityIncrement";
			numCol.DataPropertyName = "QuantityIncrement";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.Visible = false;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "LimPri";
			numCol.Name = "LimitPrice";
			numCol.DataPropertyName = "LimitPrice";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "LimPriInc";
			numCol.Name = "LimitPriceIncrement";
			numCol.DataPropertyName = "LimitPriceIncrement";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.Visible = false;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "StpPri";
			numCol.Name = "StopPrice";
			numCol.DataPropertyName = "StopPrice";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "StpPriInc";
			numCol.Name = "StopPriceIncrement";
			numCol.DataPropertyName = "StopPriceIncrement";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.Visible = false;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PegPri";
			numCol.Name = "PegPrice";
			numCol.DataPropertyName = "PegPrice";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "PegPriInc";
			numCol.Name = "PegPriceIncrement";
			numCol.DataPropertyName = "PegPriceIncrement";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "StopLimitOffset";
			numCol.Name = "StopLimitOffset";
			numCol.DataPropertyName = "StopLimitOffset";
			grid.Columns.Add(numCol);

			txtCol = new DGVSymbolSettingTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = false;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "OrdTyp";
			txtCol.Name = "OrderType";
			txtCol.DataPropertyName = "OrderType";
			grid.Columns.Add(txtCol);

			txtCol = new DGVSymbolSettingTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = false;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Dur";
			txtCol.Name = "Duration";
			txtCol.DataPropertyName = "Duration";
			grid.Columns.Add(txtCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Show";
			numCol.Name = "Show";
			numCol.DataPropertyName = "Show";
			grid.Columns.Add(numCol);

			numCol = new DataGridViewTextBoxColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "ShowInc";
			numCol.Name = "ShowIncrement";
			numCol.DataPropertyName = "ShowIncrement";
			grid.Columns.Add(numCol);

			chkCol = new DataGridViewCheckBoxColumn();
			chkCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			chkCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			chkCol.DefaultCellStyle.BackColor = Color.Black;
			chkCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			chkCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			chkCol.SortMode = DataGridViewColumnSortMode.Automatic;
			chkCol.ReadOnly = false;
			chkCol.ValueType = typeof(bool);
			chkCol.DefaultCellStyle.Format = "";
			chkCol.HeaderText = "UseShow";
			chkCol.Name = "UseShow";
			chkCol.DataPropertyName = "UseShow";
			grid.Columns.Add(chkCol);

			txtCol = new DGVSymbolSettingTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = false;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "SLender";
			txtCol.Name = "ShortLender";
			txtCol.DataPropertyName = "ShortLender";
			grid.Columns.Add(txtCol);

			txtCol = new DGVSymbolSettingTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = false;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Instr.";
			txtCol.Name = "Instructions";
			txtCol.DataPropertyName = "Instructions";
			grid.Columns.Add(txtCol);

			chkCol = new DataGridViewCheckBoxColumn();
			chkCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			chkCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			chkCol.DefaultCellStyle.BackColor = Color.Black;
			chkCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			chkCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			chkCol.SortMode = DataGridViewColumnSortMode.Automatic;
			chkCol.Visible = false;
			chkCol.ReadOnly = false;
			chkCol.ValueType = typeof(bool);
			chkCol.DefaultCellStyle.Format = "";
			chkCol.HeaderText = "UseInstr.";
			chkCol.Name = "UseInstructions";
			chkCol.DataPropertyName = "UseInstructions";
			grid.Columns.Add(chkCol);

			txtCol = new DGVSymbolSettingTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = true;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Trader";
			txtCol.Name = "Trader";
			txtCol.DataPropertyName = "Trader";
			grid.Columns.Add(txtCol);

			txtCol = new DGVSymbolSettingTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = true;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Account";
			txtCol.Name = "Account";
			txtCol.DataPropertyName = "Account";
			grid.Columns.Add(txtCol);

			txtCol = new DGVSymbolSettingTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = true;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Exchange";
			txtCol.Name = "Exchange";
			txtCol.DataPropertyName = "Exchange";
			grid.Columns.Add(txtCol);

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Events -

		protected override void OnRowPostPaint(DataGridViewRowPostPaintEventArgs e)
		{
			if (this["Symbol", e.RowIndex].Value != null &&
				this["Symbol", e.RowIndex].Value.ToString().Contains("_DEF"))
			{
				this["Symbol", e.RowIndex].ReadOnly = true;
			}
			else
			{
				this["Symbol", e.RowIndex].ReadOnly = false;
			}
			base.OnRowPostPaint(e);
		}

		protected override void OnKeyDown(KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Delete:
					foreach (DataGridViewRow row in SelectedRows)
					{
						if (row.Cells["Symbol"].Value == null ||
							row.Cells["Symbol"].Value.ToString () == "" ||
							row.Cells["Symbol"].Value.ToString().Contains("_DEF"))
						{
							e.SuppressKeyPress = true;
							break;
						}
						else
						{
							Rows.Remove(row);
						}
					}
					break;
				default:
					base.OnKeyDown(e);
					break;
			}
		}

		protected override void OnDataError(bool displayErrorDialogIfNoHandler, DataGridViewDataErrorEventArgs e)
		{
			//base.OnDataError(displayErrorDialogIfNoHandler, e);
		}

		#endregion
	}
}
