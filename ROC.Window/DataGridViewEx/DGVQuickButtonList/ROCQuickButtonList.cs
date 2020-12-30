using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Collections.Generic;
using System.Data;
using System.Diagnostics;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCQuickButtonList : DGVBase, INotifyPropertyChanged
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

		private string _currentSecType = CSVEx.CSVFieldIDs.SecurityTypes.Equity;
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
					_rocGridTable = new DataTable("QuickButtonSettings");
					
					_rocGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("Quantity", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("Side", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("ShortLocate", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("BasePriceSource", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("BasePriceOffset", Type.GetType("System.Int64")));
					_rocGridTable.Columns.Add(new DataColumn("DisplayName", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("HotKey", Type.GetType("System.String")));
					_rocGridTable.Columns.Add(new DataColumn("Special", Type.GetType("System.String")));

					_rocGridTable.Columns.Add(new DataColumn("LoadOnly", Type.GetType("System.Boolean")));
					_rocGridTable.Columns.Add(new DataColumn("Active", Type.GetType("System.Boolean")));

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

		private void SetGrid(ROCQuickButtonList grid)
		{
			DGVQuickButtonTextColumn txtCol;
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

			txtCol = new DGVQuickButtonTextColumn();
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

			txtCol = new DGVQuickButtonTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = true;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Side";
			txtCol.Name = "Side";
			txtCol.DataPropertyName = "Side";
			grid.Columns.Add(txtCol);

			txtCol = new DGVQuickButtonTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "ShortLocate";
			txtCol.Name = "ShortLocate";
			txtCol.DataPropertyName = "ShortLocate";
			grid.Columns.Add(txtCol);

			txtCol = new DGVQuickButtonTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = true;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "BasePriceSource";
			txtCol.Name = "BasePriceSource";
			txtCol.DataPropertyName = "BasePriceSource";
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
			numCol.HeaderText = "BasePriceOffset";
			numCol.Name = "BasePriceOffset";
			numCol.DataPropertyName = "BasePriceOffset";
			grid.Columns.Add(numCol);

			txtCol = new DGVQuickButtonTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "DisplayName";
			txtCol.Name = "DisplayName";
			txtCol.DataPropertyName = "DisplayName";
			grid.Columns.Add(txtCol);

			txtCol = new DGVQuickButtonTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = false;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "HotKey";
			txtCol.Name = "HotKey";
			txtCol.DataPropertyName = "HotKey";
			grid.Columns.Add(txtCol);

			txtCol = new DGVQuickButtonTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.Visible = false;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Special";
			txtCol.Name = "Special";
			txtCol.DataPropertyName = "Special";
			grid.Columns.Add(txtCol);

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
			chkCol.HeaderText = "LoadOnly";
			chkCol.Name = "LoadOnly";
			chkCol.DataPropertyName = "LoadOnly";
			grid.Columns.Add(chkCol);

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
			chkCol.HeaderText = "Enable";
			chkCol.Name = "Active";
			chkCol.DataPropertyName = "Active";
			grid.Columns.Add(chkCol);

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Events -

		protected override void OnDataError(bool displayErrorDialogIfNoHandler, DataGridViewDataErrorEventArgs e)
		{
			//base.OnDataError(displayErrorDialogIfNoHandler, e);
		}

		#endregion
	}
}
