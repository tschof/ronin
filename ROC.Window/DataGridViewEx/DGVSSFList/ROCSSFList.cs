using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Data;
using System.Windows.Forms;
using System.Drawing;
using System.Diagnostics;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCSSFList : DGVBase, INotifyPropertyChanged
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

		private bool _priceSelected = false;
		[Browsable(false)]
		public bool PriceSelected
		{
			get
			{
				return _priceSelected;
			}
			set
			{
				_priceSelected = value;
				if (_priceSelected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("PriceSelected"));
				}
			}
		}

		private bool _sizeSelected = false;
		[Browsable(false)]
		public bool SizeSelected
		{
			get
			{
				return _sizeSelected;
			}
			set
			{
				_sizeSelected = value;
				if (_sizeSelected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("SizeSelected"));
				}
			}
		}

		#endregion

		#region - Properties -

		private DataView _searchView;
		[Browsable(false)]
		public DataView SearchView
		{
			get
			{
				if (_searchView == null)
				{
					_searchView = new DataView(RocGridTable);
					_searchView.Sort = "Symbol";
				}
				return _searchView;
			}
		}

		private double _priceSSF = 0;
		[Browsable(false)]
		public double PriceSSF
		{
			get
			{
				if (_priceSSF < 0)
				{
					_priceSSF = 0;
				}
				return _priceSSF;
			}
			set
			{
				_priceSSF = value;
			}
		}

		private long _sizeSSF = 0;
		[Browsable(false)]
		public long SizeSSF
		{
			get
			{
				return _sizeSSF;
			}
			set
			{
				_sizeSSF = value;
			}
		}

		private string _symbolSSF = "";
		[Browsable(false)]
		public string SymbolSSF
		{
			get
			{
				return _symbolSSF;
			}
			set
			{
				_symbolSSF = value;
			}
		}

		private string _secTypeSSF = "";
		[Browsable(false)]
		public string SecTypeSSF
		{
			get
			{
				return _secTypeSSF;
			}
			set
			{
				_secTypeSSF = value;
			}
		}

		private string _expirationSSF = "";
		[Browsable(false)]
		public string ExpirationSSF
		{
			get
			{
				return _expirationSSF;
			}
			set
			{
				_expirationSSF = value;
			}
		}

		private Color _selectedForeColor = Color.LightBlue;
		[Browsable(false)]
		public Color SelectedForeColor
		{
			get
			{
				return _selectedForeColor;
			}
			set
			{
				_selectedForeColor = value;
			}
		}

		#endregion

		#region - Data Table -

		private DataTable _rocGridTable;
		[Browsable(false)]
		public DataTable RocGridTable
		{
			get
			{
				if (_rocGridTable == null)
				{
					SetGridTable();
				}
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
				List<DataColumn> keyColumns = new List<DataColumn>();
				keyColumns.Add(_rocGridTable.Columns["Symbol"]);

				_rocGridTable.Columns.Add(new DataColumn("Expiration", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("BidPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("BidSize", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("AskPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("AskSize", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("LastTraded", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("NetChange", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("PctChange", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("TotalVolume", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));

				_rocGridTable.PrimaryKey = keyColumns.ToArray();

				_rocGridTable.Columns["BidPrice"].DefaultValue = 0;
				_rocGridTable.Columns["BidSize"].DefaultValue = 0;
				_rocGridTable.Columns["AskPrice"].DefaultValue = 0;
				_rocGridTable.Columns["AskSize"].DefaultValue = 0;
				_rocGridTable.Columns["LastTraded"].DefaultValue = 0;

				_rocGridTable.Columns["SecType"].DefaultValue = "F";

				_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
			}

			_rocGridTable.DefaultView.Sort = "Expiration";

			LastSort = _rocGridTable.DefaultView.Sort;
			_rocGridTable.DefaultView.RowFilter = "";
		}

		private void RocGridTable_RowChanged(object sender, DataRowChangeEventArgs e)
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

		private void SetGrid(ROCSSFList grid)
		{
			DGVSSFListTextColumn txtCol;
			DGVSSFListNumberColumn numCol;

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
			grid.AllowUserToOrderColumns = true;

			grid.VirtualMode = true;

			// SSF grid speical
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVSSFListTextColumn();
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

			txtCol = new DGVSSFListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Expiration";
			txtCol.Name = "Expiration";
			txtCol.DataPropertyName = "Expiration";
			grid.Columns.Add(txtCol);

			numCol = new DGVSSFListNumberColumn();
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
			numCol.HeaderText = "BidPice";
			numCol.Name = "BidPrice";
			numCol.DataPropertyName = "BidPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVSSFListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BidSize";
			numCol.Name = "BidSize";
			numCol.DataPropertyName = "BidSize";
			grid.Columns.Add(numCol);

			numCol = new DGVSSFListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "AskPice";
			numCol.Name = "AskPrice";
			numCol.DataPropertyName = "AskPrice";
			grid.Columns.Add(numCol);

			numCol = new DGVSSFListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "AskSize";
			numCol.Name = "AskSize";
			numCol.DataPropertyName = "AskSize";
			grid.Columns.Add(numCol);

			numCol = new DGVSSFListNumberColumn();
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
			numCol.HeaderText = "Last";
			numCol.Name = "LastTraded";
			numCol.DataPropertyName = "LastTraded";
			grid.Columns.Add(numCol);

			numCol = new DGVSSFListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.AutoColor = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Net";
			numCol.Name = "NetChange";
			numCol.DataPropertyName = "NetChange";
			grid.Columns.Add(numCol);

			numCol = new DGVSSFListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Vol";
			numCol.Name = "TotalVolume";
			numCol.DataPropertyName = "TotalVolume";
			grid.Columns.Add(numCol);

			numCol = new DGVSSFListNumberColumn();
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

			txtCol = new DGVSSFListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
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

			foreach (DataGridViewColumn col in Columns)
			{
				col.SortMode = DataGridViewColumnSortMode.NotSortable;
			}

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Event -

		protected override void OnSelectionChanged(EventArgs e)
		{
			// Make sure nothing is selected
			foreach (DataGridViewCell cell in SelectedCells)
			{
				cell.Selected = false;
			}

			base.OnSelectionChanged(e);
		}

		protected override void OnCellMouseDown(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (e.Button == MouseButtons.Left)
				{
					SymbolSSF = (string)Rows[e.RowIndex].Cells["Symbol"].Value;
					SecTypeSSF = (string)Rows[e.RowIndex].Cells["SecType"].Value;
					if (Rows[e.RowIndex].Cells["Expiration"].Value != null)
					{
						ExpirationSSF = (string)Rows[e.RowIndex].Cells["Expiration"].Value;
					}
					else
					{
						ExpirationSSF = "";
					}
					switch (Columns[e.ColumnIndex].Name)
					{
						case "LastTraded":
							Rows[e.RowIndex].Cells["LastTraded"].Style.ForeColor = SelectedForeColor;
							PriceSSF = (double)Rows[e.RowIndex].Cells["LastTraded"].Value;
							PriceSelected = true;
							break;
						case "BidPrice":
							Rows[e.RowIndex].Cells["BidPrice"].Style.ForeColor = SelectedForeColor;
							PriceSSF = (double)Rows[e.RowIndex].Cells["BidPrice"].Value;
							PriceSelected = true;
							break;
						case "AskPrice":
							Rows[e.RowIndex].Cells["AskPrice"].Style.ForeColor = SelectedForeColor;
							PriceSSF = (double)Rows[e.RowIndex].Cells["AskPrice"].Value;
							PriceSelected = true;
							break;
						case "BidSize":
							Rows[e.RowIndex].Cells["BidPrice"].Style.ForeColor = SelectedForeColor;
							Rows[e.RowIndex].Cells["BidSize"].Style.ForeColor = SelectedForeColor;
							PriceSSF = (double)Rows[e.RowIndex].Cells["BidPrice"].Value;
							SizeSSF = (long)Rows[e.RowIndex].Cells["BidSize"].Value;
							SizeSelected = true;
							break;
						case "AskSize":
							Rows[e.RowIndex].Cells["AskPrice"].Style.ForeColor = SelectedForeColor;
							Rows[e.RowIndex].Cells["AskSize"].Style.ForeColor = SelectedForeColor;
							PriceSSF = (double)Rows[e.RowIndex].Cells["AskPrice"].Value;
							SizeSSF = (long)Rows[e.RowIndex].Cells["AskSize"].Value;
							SizeSelected = true;
							break;
					}
				}
			}

			base.OnCellMouseDown(e);
		}

		protected override void OnCellMouseUp(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (e.Button == MouseButtons.Left)
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "LastTraded":
							Rows[e.RowIndex].Cells["LastTraded"].Style.ForeColor = DefaultCellForeColor;
							break;
						case "BidPrice":
							Rows[e.RowIndex].Cells["BidPrice"].Style.ForeColor = DefaultCellForeColor;
							break;
						case "AskPrice":
							Rows[e.RowIndex].Cells["AskPrice"].Style.ForeColor = DefaultCellForeColor;
							break;
						case "BidSize":
							Rows[e.RowIndex].Cells["BidPrice"].Style.ForeColor = DefaultCellForeColor;
							Rows[e.RowIndex].Cells["BidSize"].Style.ForeColor = DefaultCellForeColor;
							break;
						case "AskSize":
							Rows[e.RowIndex].Cells["AskPrice"].Style.ForeColor = DefaultCellForeColor;
							Rows[e.RowIndex].Cells["AskSize"].Style.ForeColor = DefaultCellForeColor;
							break;
					}
				}
			}

			base.OnCellMouseUp(e);
		}

		protected override void OnDataError(bool displayErrorDialogIfNoHandler, DataGridViewDataErrorEventArgs e)
		{
			//base.OnDataError(displayErrorDialogIfNoHandler, e);
		}

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				DataSource = null;

				RocGridTable.Clear();
				RocGridTable = null;

				Columns.Clear();
			}
			base.Dispose(disposing);
		}

		#endregion
	}
}
