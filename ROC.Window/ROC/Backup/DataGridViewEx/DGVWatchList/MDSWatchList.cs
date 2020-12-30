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
	public class MDSWatchList : DGVBase, INotifyPropertyChanged, IDisposable
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

		#region - Properties With Events -

		private string _resubscribeIssueSymbol = "";
		[Browsable(false)]
		public string ResubscribeIssueSymbol
		{
			get
			{
				return _resubscribeIssueSymbol;
			}
			set
			{
				_resubscribeIssueSymbol = value;
				onPropertyChanged(new PropertyChangedEventArgs("ResubscribeIssueSymbol"));
			}
		}

		#endregion

		#region - Properties -

		private string _resubscribeSource = "";
		[Browsable(false)]
		public string ResubscribeSource
		{
			get
			{
				return _resubscribeSource;
			}
			set
			{
				_resubscribeSource = value;
			}
		}

		private string _resubscribeBookSymbol = "";
		[Browsable(false)]
		public string ResubscribeBookSymbol
		{
			get
			{
				return _resubscribeBookSymbol;
			}
			set
			{
				_resubscribeBookSymbol = value;
			}
		}

		private long _resubscribeBook = 0;
		[Browsable(false)]
		public long ResubscribeBook
		{
			get
			{
				return _resubscribeBook;
			}
			set
			{
				_resubscribeBook = value;
			}
		}

		#endregion

		#region - Data Table -

		private DataTable _mdsGridTable;
		[Browsable(false)]
		public DataTable MdsGridTable
		{
			get
			{
				SetGridTable();
				return _mdsGridTable;
			}
			set
			{
				_mdsGridTable = value;
			}
		}

		private void SetGridTable()
		{
			if (_mdsGridTable == null)
			{
				_mdsGridTable = new DataTable();

				_mdsGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
				_mdsGridTable.Columns.Add(new DataColumn("IssueSymbol", Type.GetType("System.String")));
				_mdsGridTable.Columns.Add(new DataColumn("Source", Type.GetType("System.String")));
				_mdsGridTable.Columns.Add(new DataColumn("Status", Type.GetType("System.String")));

				_mdsGridTable.Columns.Add(new DataColumn("BidPartID", Type.GetType("System.String")));
				_mdsGridTable.Columns.Add(new DataColumn("BidPrice", Type.GetType("System.Double")));
				_mdsGridTable.Columns.Add(new DataColumn("BidSize", Type.GetType("System.Int64")));

				_mdsGridTable.Columns.Add(new DataColumn("AskPartID", Type.GetType("System.String")));
				_mdsGridTable.Columns.Add(new DataColumn("AskPrice", Type.GetType("System.Double")));
				_mdsGridTable.Columns.Add(new DataColumn("AskSize", Type.GetType("System.Int64")));

				_mdsGridTable.Columns.Add(new DataColumn("TradePartID", Type.GetType("System.String")));
				_mdsGridTable.Columns.Add(new DataColumn("TradePrice", Type.GetType("System.Double")));
				_mdsGridTable.Columns.Add(new DataColumn("TradeVolume", Type.GetType("System.Int64")));

				_mdsGridTable.Columns.Add(new DataColumn("LineTime", Type.GetType("System.String")));
				_mdsGridTable.Columns.Add(new DataColumn("DecodeTime", Type.GetType("System.Double")));

				_mdsGridTable.Columns.Add(new DataColumn("IsBook", Type.GetType("System.Int64")));

				_mdsGridTable.Columns["IsBook"].DefaultValue = 0;

				_mdsGridTable.DefaultView.Sort = "Symbol";
				LastSort = _mdsGridTable.DefaultView.Sort;

				_mdsGridTable.RowChanged += new DataRowChangeEventHandler(MdsGridTable_RowChanged);
			}
		}

		private void MdsGridTable_RowChanged(object sender, DataRowChangeEventArgs e)
		{
			switch (e.Action)
			{
				case DataRowAction.Add:
				case DataRowAction.Change:
				case DataRowAction.Delete:
				case DataRowAction.Commit:
				case DataRowAction.ChangeCurrentAndOriginal:
				case DataRowAction.ChangeOriginal:
				case DataRowAction.Nothing:
				case DataRowAction.Rollback:
				default:
					break;
			}
		}

		#endregion

		#region - Grid -

		public void Initialize()
		{
			SetGridTable();

			SetGrid(this);
		}

		private void SetGrid(MDSWatchList grid)
		{
			DGVWatchListTextColumn txtCol;
			DGVWatchListNumberColumn numCol;

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

			grid.AllowUserToAddRows = false;
			grid.AllowUserToDeleteRows = false;
			grid.AllowUserToOrderColumns = false;

			grid.AllowUserToOrderColumns = false;

			grid.VirtualMode = true;

			// MDS Symbol grid special
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVWatchListTextColumn();
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

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "IssueSymbol";
			txtCol.Name = "IssueSymbol";
			txtCol.DataPropertyName = "IssueSymbol";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Source";
			txtCol.Name = "Source";
			txtCol.DataPropertyName = "Source";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Status";
			txtCol.Name = "Status";
			txtCol.DataPropertyName = "Status";
			grid.Columns.Add(txtCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Bid";
			numCol.Name = "BidPrice";
			numCol.DataPropertyName = "BidPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BidSize";
			numCol.Name = "BidSize";
			numCol.DataPropertyName = "BidSize";
			grid.Columns.Add(numCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "BidID";
			txtCol.Name = "BidPartID";
			txtCol.DataPropertyName = "BidPartID";
			grid.Columns.Add(txtCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Ask";
			numCol.Name = "AskPrice";
			numCol.DataPropertyName = "AskPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "AskSize";
			numCol.Name = "AskSize";
			numCol.DataPropertyName = "AskSize";
			grid.Columns.Add(numCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "AskID";
			txtCol.Name = "AskPartID";
			txtCol.DataPropertyName = "AskPartID";
			grid.Columns.Add(txtCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "TradePrice";
			numCol.Name = "TradePrice";
			numCol.DataPropertyName = "TradePrice";
			grid.Columns.Add(numCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "TradeVolume";
			numCol.Name = "TradeVolume";
			numCol.DataPropertyName = "TradeVolume";
			grid.Columns.Add(numCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "TradePartID";
			txtCol.Name = "TradePartID";
			txtCol.DataPropertyName = "TradePartID";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "LineTime";
			txtCol.Name = "LineTime";
			txtCol.DataPropertyName = "LineTime";
			grid.Columns.Add(txtCol);

			txtCol = new DGVWatchListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(double);
			txtCol.HeaderText = "DecodeTime";
			txtCol.Name = "DecodeTime";
			txtCol.DataPropertyName = "DecodeTime";
			grid.Columns.Add(txtCol);

			numCol = new DGVWatchListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "IsBook";
			numCol.Name = "IsBook";
			numCol.DataPropertyName = "IsBook";
			grid.Columns.Add(numCol);

			grid.DataSource = MdsGridTable.DefaultView;
		}

		#endregion

		#region - Grid Event -

		protected override void OnCellMouseDoubleClick(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0 && Columns[e.ColumnIndex].Name == "IssueSymbol")
			{
				ResubscribeSource = Rows[e.RowIndex].Cells["Source"].Value.ToString();
				ResubscribeBook = (long)Rows[e.RowIndex].Cells["IsBook"].Value;
				if (ResubscribeBook == 1)
				{
					ResubscribeBookSymbol = Rows[e.RowIndex].Cells["Symbol"].Value.ToString();
				}
				Rows[e.RowIndex].Cells["Status"].Value = "ReSub";

				// Notify Event
				ResubscribeIssueSymbol = Rows[e.RowIndex].Cells["IssueSymbol"].Value.ToString();
			}

			base.OnCellMouseDoubleClick(e);
		}

		#endregion

		#region - IDisposable Members -

		void IDisposable.Dispose()
		{
			MdsGridTable.Clear();
			MdsGridTable = null;

			Columns.Clear();
		}

		#endregion
	}
}
