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
	public class ROCBookList : DGVBase, INotifyPropertyChanged
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

		public enum GridSides
		{
			Bid,
			Ask,
		}

		#region - Local Variables -

		private GridSides _side = GridSides.Bid;

		private int _shiftMultiples = 0;
		private int _red = 0;
		private int _green = 0;
		private int _blue = 0;

		private Dictionary<int, Color> _shiftedColors = new Dictionary<int, Color>();

		#endregion

		#region - Properties With Events -

		private bool _showToolTip = false;
		[Browsable(false)]
		public bool ShowToolTip
		{
			get
			{
				return _showToolTip;
			}
			set
			{
				_showToolTip = value;
				if (_showToolTip == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowToolTip"));
				}
			}
		}

		private bool _showOrderMenu = false;
		[Browsable(false)]
		public bool ShowOrderMenu
		{
			get
			{
				return _showOrderMenu;
			}
			set
			{
				_showOrderMenu = value;
				if (_showOrderMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowOrderMenu"));
				}
			}
		}

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

		private string _showTip = "";
		[Browsable(false)]
		public string ShowTip
		{
			get
			{
				return _showTip;
			}
			set
			{
				_showTip = value;
			}
		}

		private string _showOrderID = "";
		[Browsable(false)]
		public string ShowOrderID
		{
			get
			{
				return _showOrderID;
			}
			set
			{
				_showOrderID = value;
			}
		}

		private double _priceBook = 0;
		[Browsable(false)]
		public double PriceBook
		{
			get
			{
				return _priceBook;
			}
			set
			{
				_priceBook = value;
			}
		}

		private long _sizeBook = 0;
		[Browsable(false)]
		public long SizeBook
		{
			get
			{
				return _sizeBook;
			}
			set
			{
				_sizeBook = value;
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

		private Color _bidColor = Color.LimeGreen;
		[Browsable(false)]
		public Color BidColor
		{
			get
			{
				return _bidColor;
			}
			set
			{
				_bidColor = value;
			}
		}

		private Color _askColor = Color.Red;
		[Browsable(false)]
		public Color AskColor
		{
			get
			{
				return _askColor;
			}
			set
			{
				_askColor = value;
			}
		}

		private int _colorShiftInterval = 18;
		[Browsable(false)]
		public int ColorShiftInterval
		{
			get
			{
				return _colorShiftInterval;
			}
			set
			{
				_colorShiftInterval = value;
			}
		}

		private int _colorShiftIntervalMax = 10;
		[Browsable(false)]
		public int ColorShiftIntervalMax
		{
			get
			{
				return _colorShiftIntervalMax;
			}
			set
			{
				_colorShiftIntervalMax = value;
			}
		}

		private string _defaultRowFilter = string.Format(
			"Price <> 0 And Size <> 0 And ({0})", 
			CSVEx.CSVFieldIDs.StatusCodes.ActiveStatusFilter.Replace("Status", "OrderStatus"));
		[Browsable(false)]
		public string DefaultRowFilter
		{
			get
			{
				return _defaultRowFilter;
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
				
				_rocGridTable.Columns.Add(new DataColumn("Tag", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Price", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Size", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("OrderStatus", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));

				_rocGridTable.Columns["OrderStatus"].DefaultValue = -1;
				_rocGridTable.Columns["Price"].DefaultValue = 0;
				_rocGridTable.Columns["Size"].DefaultValue = 0;

				_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
			}

			if (_side == GridSides.Bid)
			{
				_rocGridTable.DefaultView.Sort = "Price DESC";
			}
			else
			{
				_rocGridTable.DefaultView.Sort = "Price";
			}
			LastSort = _rocGridTable.DefaultView.Sort;

			_rocGridTable.DefaultView.RowFilter = DefaultRowFilter;
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

		public void Initialize(GridSides side)
		{
			_side = side;

			SetGridTable();

			SetGrid(this);
		}

		private void SetGrid(ROCBookList grid)
		{
			DGVBookListTextColumn txtCol;
			DGVBookListNumberColumn numCol;

			DefaultCellForeColor = Color.Black;
			grid.DefaultCellForeColor = DefaultCellForeColor;

			grid.BackgroundColor = Color.Black;
			grid.GridColor = Color.Black;

			grid.ShowCellToolTips = false;
			grid.MultiSelect = false;
			grid.SelectionMode = DataGridViewSelectionMode.CellSelect;

			grid.ShowCellToolTips = false;
			grid.AutoGenerateColumns = false;
			grid.RowHeadersVisible = false;
			grid.AllowUserToResizeRows = false;
			grid.ColumnHeadersHeightSizeMode = DataGridViewColumnHeadersHeightSizeMode.DisableResizing;

			grid.AllowUserToAddRows = false;
			grid.AllowUserToDeleteRows = false;
			grid.AllowUserToOrderColumns = true;

			grid.VirtualMode = true;

			// Book grid specials
			grid.ScrollBars = ScrollBars.None;
			grid.CellBorderStyle = DataGridViewCellBorderStyle.SingleHorizontal;
			grid.ColumnHeadersDefaultCellStyle.BackColor = HeaderBackColor;
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;

			if (_side == GridSides.Bid)
			{
				grid.ColumnHeadersDefaultCellStyle.ForeColor = BidColor;
				grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			}
			else
			{
				grid.ColumnHeadersDefaultCellStyle.ForeColor = AskColor;
				grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			}

			txtCol = new DGVBookListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Order ID";
			txtCol.Name = "Tag";
			txtCol.DataPropertyName = "Tag";
			grid.Columns.Add(txtCol);

			numCol = new DGVBookListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			if (_side == GridSides.Bid)
			{
				numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			}
			else
			{
				numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			}
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Price";
			numCol.Name = "Price";
			numCol.DataPropertyName = "Price";
			numCol.Width = 88;
			numCol.FillWeight = 0.5F;
			grid.Columns.Add(numCol);

			numCol = new DGVBookListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			if (_side == GridSides.Bid)
			{
				numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			}
			else
			{
				numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			}
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Size";
			numCol.Name = "Size";
			numCol.DataPropertyName = "Size";
			numCol.Width = 87;
			numCol.FillWeight = 0.5F;
			grid.Columns.Add(numCol);

			numCol = new DGVBookListNumberColumn();
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
			numCol.HeaderText = "Mult";
			numCol.Name = "ContractSize";
			numCol.DataPropertyName = "ContractSize";
			grid.Columns.Add(numCol);

			numCol = new DGVBookListNumberColumn();
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

			numCol = new DGVBookListNumberColumn();
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
			numCol.HeaderText = "DisplayFactor";
			numCol.Name = "DisplayFactor";
			numCol.DataPropertyName = "DisplayFactor";
			grid.Columns.Add(numCol);


			foreach (DataGridViewColumn col in Columns)
			{
				col.SortMode = DataGridViewColumnSortMode.NotSortable;
			}

			if (_side == GridSides.Bid)
			{
				grid.Columns["Price"].DisplayIndex = 2;
			}

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Event -

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				Rows[e.RowIndex].Cells["TickSize"].Style.BackColor = Color.Black;

				if (Rows[e.RowIndex].Cells["Tag"].Value == null || Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
				{
					// Market Data
					_shiftMultiples = e.RowIndex;
					if (_shiftMultiples > ColorShiftIntervalMax) _shiftMultiples = ColorShiftIntervalMax;

					if (!_shiftedColors.ContainsKey(_shiftMultiples))
					{
						// Make new shifted color
						_red = RedDefault - (ColorShiftInterval * _shiftMultiples);
						_green = GreenDefault - (ColorShiftInterval * _shiftMultiples);
						_blue = BlueDefault - (ColorShiftInterval * _shiftMultiples);

						_shiftedColors.Add(_shiftMultiples, Color.FromArgb(_red, _green, _blue));
					}

					if (Rows[e.RowIndex].DefaultCellStyle.BackColor != _shiftedColors[_shiftMultiples])
					{
						Rows[e.RowIndex].DefaultCellStyle.BackColor = _shiftedColors[_shiftMultiples];
					}
					if (Rows[e.RowIndex].DefaultCellStyle.ForeColor != Color.Black)
					{
						Rows[e.RowIndex].DefaultCellStyle.ForeColor = Color.Black;
					}

					//for (int i = 0; i < Columns.Count; i++)
					//{
					//    if (Columns[i].Visible)
					//    {
					//        if (Rows[e.RowIndex].Cells[i].Style.BackColor != _shiftedColors[_shiftMultiples])
					//        {
					//            Rows[e.RowIndex].Cells[i].Style.BackColor = _shiftedColors[_shiftMultiples];
					//            Rows[e.RowIndex].Cells[i].Style.ForeColor = Color.Black;
					//        }
					//    }
					//}
				}
				else
				{
					// User Orders
					if (_side == GridSides.Bid)
					{
						if (Rows[e.RowIndex].DefaultCellStyle.BackColor != BidColor)
						{
							Rows[e.RowIndex].DefaultCellStyle.BackColor = BidColor;
						}
						if (Rows[e.RowIndex].DefaultCellStyle.ForeColor != Color.Gainsboro)
						{
							Rows[e.RowIndex].DefaultCellStyle.ForeColor = Color.Gainsboro;
						}

						//for (int i = 0; i < Columns.Count; i++)
						//{
						//    if (Columns[i].Visible)
						//    {
						//        if (Rows[e.RowIndex].Cells[i].Style.BackColor != BidColor)
						//        {
						//            Rows[e.RowIndex].Cells[i].Style.BackColor = BidColor;
						//            Rows[e.RowIndex].Cells[i].Style.ForeColor = Color.Gainsboro;
						//        }
						//    }
						//}
					}
					else
					{
						if (Rows[e.RowIndex].DefaultCellStyle.BackColor != AskColor)
						{
							Rows[e.RowIndex].DefaultCellStyle.BackColor = AskColor;
						}
						if (Rows[e.RowIndex].DefaultCellStyle.ForeColor != Color.Gainsboro)
						{
							Rows[e.RowIndex].DefaultCellStyle.ForeColor = Color.Gainsboro;
						}

						//for (int i = 0; i < Columns.Count; i++)
						//{
						//    if (Columns[i].Visible)
						//    {
						//        if (Rows[e.RowIndex].Cells[i].Style.BackColor != AskColor)
						//        {
						//            Rows[e.RowIndex].Cells[i].Style.BackColor = AskColor;
						//            Rows[e.RowIndex].Cells[i].Style.ForeColor = Color.Gainsboro;
						//        }
						//    }
						//}
					}
				}
			}

			base.OnRowPrePaint(e);
		}

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
				if (e.Button == MouseButtons.Left && Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "Price":
							Rows[e.RowIndex].Cells["Price"].Style.ForeColor = SelectedForeColor;
							break;
						case "Size":
							Rows[e.RowIndex].Cells["Price"].Style.ForeColor = SelectedForeColor;
							Rows[e.RowIndex].Cells["Size"].Style.ForeColor = SelectedForeColor;
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
				if (e.Button == MouseButtons.Left && Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "Price":
							Rows[e.RowIndex].Cells["Price"].Style.ForeColor = DefaultCellForeColor;
							break;
						case "Size":
							Rows[e.RowIndex].Cells["Price"].Style.ForeColor = DefaultCellForeColor;
							Rows[e.RowIndex].Cells["Size"].Style.ForeColor = DefaultCellForeColor;
							break;
					}
				}
			}

			base.OnCellMouseUp(e);
		}

		protected override void OnCellMouseClick(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (e.Button == MouseButtons.Left)
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "Price":
							if (Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
							{
								PriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;

								PriceSelected = true;
							}
							else
							{
								ShowTip = String.Concat(new object[] { 
									Rows[e.RowIndex].Cells["Tag"].Value.ToString(), 
									" - ", 
									Rows[e.RowIndex].Cells["Size"].Value.ToString(),
									"@",
									Rows[e.RowIndex].Cells["Price"].Value.ToString() });
								ShowToolTip = true;
							}
							break;
						case "Size":
							if (Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
							{
								PriceBook = (double)Rows[e.RowIndex].Cells["Price"].Value;
								SizeBook = (long)Rows[e.RowIndex].Cells["Size"].Value;

								SizeSelected = true;
							}
							break;
					}
				}
				else if (e.Button == MouseButtons.Right)
				{
					if (Rows[e.RowIndex].Cells["Tag"].Value.ToString() != "")
					{
						ShowOrderID = Rows[e.RowIndex].Cells["Tag"].Value.ToString();
						ShowOrderMenu = true;
					}
				}
			}

			base.OnCellMouseDown(e);
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

				_shiftedColors.Clear();
				_shiftedColors = null;

				RocGridTable.Clear();
				RocGridTable = null;

				Columns.Clear();
			}
			base.Dispose(disposing);
		}

		#endregion
	}
}
