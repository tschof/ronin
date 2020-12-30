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
	public class ROCLevel2List : DGVBase, INotifyPropertyChanged
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

		private bool _priceL2Selected = false;
		[Browsable(false)]
		public bool PriceL2Selected
		{
			get
			{
				return _priceL2Selected;
			}
			set
			{
				_priceL2Selected = value;
				if (_priceL2Selected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("PriceL2Selected"));
				}
			}
		}

		private bool _sizeL2Selected = false;
		[Browsable(false)]
		public bool SizeL2Selected
		{
			get
			{
				return _sizeL2Selected;
			}
			set
			{
				_sizeL2Selected = value;
				if (_sizeL2Selected == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("SizeL2Selected"));
				}
			}
		}

		private bool _replaceOrderNow = false;
		[Browsable(false)]
		public bool ReplaceOrderNow
		{
			get
			{
				return _replaceOrderNow;
			}
			set
			{
				_replaceOrderNow = value;
				if (_replaceOrderNow == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ReplaceOrderNow"));
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

		private string _workingOrderID = "";
		[Browsable(false)]
		public string WorkingOrderID
		{
			get
			{
				return _workingOrderID;
			}
			set
			{
				_workingOrderID = value;
			}
		}

		private double _newPrice = 0;
		[Browsable(false)]
		public double NewPrice
		{
			get
			{
				return _newPrice;
			}
		}

		private double _orgPrice = 0;
		[Browsable(false)]
		public double OrgPrice
		{
			get
			{
				return _orgPrice;
			}
		}

		private long _orgSize = 0;
		[Browsable(false)]
		public long OrgSize
		{
			get
			{
				return _orgSize;
			}
		}

		private double _priceL2 = 0;
		[Browsable(false)]
		public double PriceL2
		{
			get
			{
				return _priceL2;
			}
			set
			{
				_priceL2 = value;
			}
		}

		private long _sizeL2 = 0;
		[Browsable(false)]
		public long SizeL2
		{
			get
			{
				return _sizeL2;
			}
			set
			{
				_sizeL2 = value;
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

		private int _colorShiftIntervalMax = 7;
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

		private ROCLevel2List _pairDGV = null;
		[Browsable(false)]
		public ROCLevel2List PairDGV
		{
			get
			{
				return _pairDGV;
			}
			set
			{
				_pairDGV = value;
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

				_rocGridTable.Columns.Add(new DataColumn("Symbol", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("SymbolDetail", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Tag", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("PartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("PartName", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Price", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Size", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("RealSize", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("OrderStatus", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));

				_rocGridTable.Columns["OrderStatus"].DefaultValue = -1;

				_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
			}

			if (_side == GridSides.Bid)
			{
				_rocGridTable.DefaultView.Sort = "Price DESC, RealSize DESC";
			}
			else
			{
				_rocGridTable.DefaultView.Sort = "Price, RealSize DESC";
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

		#region - Aggregation -

		private long _totalSize = 0;
		public long GetTotalSizeAtPrice(double price)
		{
			try
			{
				if (RocGridTable.Rows.Count > 0)
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(Size)", "Price = " + price).ToString(), out _totalSize))
					{
						return _totalSize;
					}
				}
				else
				{
					return 0;
				}
			}
			catch (Exception ex)
			{
				Debug.Assert(false, ex.Message, ex.StackTrace);
			}
			return 0;
		}

		#endregion

		#region - Grid -

		public void Initialize(GridSides side)
		{
			_side = side;

			SetGridTable();

			SetGrid(this);
		}

		private void SetGrid(ROCLevel2List grid)
		{
			DGVLevel2ListTextColumn txtCol;
			DGVLevel2ListNumberColumn numCol;

			DefaultCellForeColor = Color.Black;
			grid.DefaultCellForeColor = DefaultCellForeColor;

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

			grid.AllowUserToAddRows = false;
			grid.AllowUserToDeleteRows = false;
			grid.AllowUserToOrderColumns = true;

			grid.VirtualMode = true;
			
			// Level 2 grid specials
			grid.ScrollBars = ScrollBars.None;
			grid.CellBorderStyle = DataGridViewCellBorderStyle.SingleHorizontal;
			grid.ColumnHeadersDefaultCellStyle.BackColor = HeaderBackColor;
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;

			if (_side == GridSides.Bid)
			{
				grid.ColumnHeadersDefaultCellStyle.ForeColor = BidColor;
				grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight; //DataGridViewContentAlignment.MiddleLeft;
			}
			else
			{
				grid.ColumnHeadersDefaultCellStyle.ForeColor = AskColor;
				grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			}

			txtCol = new DGVLevel2ListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
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

			txtCol = new DGVLevel2ListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Detail";
			txtCol.Name = "SymbolDetail";
			txtCol.DataPropertyName = "SymbolDetail";
			grid.Columns.Add(txtCol);

			txtCol = new DGVLevel2ListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "OrderID";
			txtCol.Name = "Tag";
			txtCol.DataPropertyName = "Tag";
			grid.Columns.Add(txtCol);

			txtCol = new DGVLevel2ListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Name";
			txtCol.Name = "PartName";
			txtCol.DataPropertyName = "PartName";
			grid.Columns.Add(txtCol);

			txtCol = new DGVLevel2ListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			if (_side == GridSides.Bid)
			{
				txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			}
			else
			{
				txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			}
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			//txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.ValueType = typeof(string);
			if (_side == GridSides.Bid)
			{
				txtCol.HeaderText = "Bid";
			}
			else
			{
				txtCol.HeaderText = "Ask";
			}
			txtCol.Name = "PartID";
			txtCol.DataPropertyName = "PartID";
			txtCol.MinimumWidth = 35;
			txtCol.Width = 35;
			txtCol.FillWeight = 35 / 151F;
			grid.Columns.Add(txtCol);

			numCol = new DGVLevel2ListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			if (_side == GridSides.Bid)
			{
				numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight; //MiddleLeft
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
			numCol.MinimumWidth = 57;
			numCol.Width = 57;
			numCol.FillWeight = 57 / 151F;
			grid.Columns.Add(numCol);

			numCol = new DGVLevel2ListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			if (_side == GridSides.Bid)
			{
				numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight; // MiddleLeft
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
			numCol.MinimumWidth = 57;
			numCol.Width = 57;
			numCol.FillWeight = 57 / 151F;
			grid.Columns.Add(numCol);

			numCol = new DGVLevel2ListNumberColumn();
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

			numCol = new DGVLevel2ListNumberColumn();
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

			numCol = new DGVLevel2ListNumberColumn();
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

			//if (_side == GridSides.Bid)
			//{
			//    grid.Columns["PartID"].DisplayIndex = 6;
			//    grid.Columns["Size"].DisplayIndex = 4;
			//}

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
					_shiftMultiples = Convert.ToInt32(Math.Abs((double)Rows[0].Cells["Price"].Value - (double)Rows[e.RowIndex].Cells["Price"].Value) * 100);
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
				if (e.Button == MouseButtons.Left)
				{
					if (Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
					{
						switch (Columns[e.ColumnIndex].Name)
						{
							case "Price":
							case "Size":
								Rows[e.RowIndex].Cells["Price"].Style.ForeColor = SelectedForeColor;
								break;
							//case "Size":
							//    Rows[e.RowIndex].Cells["Price"].Style.ForeColor = SelectedForeColor;
							//    Rows[e.RowIndex].Cells["Size"].Style.ForeColor = SelectedForeColor;
							//    break;
						}
					}
					else
					{
						WorkingOrderID = "";
						if (Rows[e.RowIndex].Cells["Tag"].Value != null && Rows[e.RowIndex].Cells["Price"].Value != null)
						{
							Cursor.Current = Cursors.Hand;
							WorkingOrderID = Rows[e.RowIndex].Cells["Tag"].Value.ToString();
							_orgPrice = (double)Rows[e.RowIndex].Cells["Price"].Value;
							_orgSize = (long)Rows[e.RowIndex].Cells["Size"].Value;
						}
					}
				}
			}

			base.OnCellMouseDown(e);
		}

		protected override void OnCellMouseUp(DataGridViewCellMouseEventArgs e)
		{
			Cursor.Current = Cursors.Default;
			resetCellForColor(e.RowIndex, e.ColumnIndex);

			if (e.RowIndex >= 0 && e.Button == MouseButtons.Left)
			{
				if (WorkingOrderID != "")
				{
					// Replace Order with in the same grid
					_newPrice = 0;
					if (Rows[e.RowIndex].Cells["Price"].Value != null)
					{
						_newPrice = (double)Rows[e.RowIndex].Cells["Price"].Value;
					}
					ReplaceOrderNow = true;
				}
			}
			base.OnCellMouseUp(e);
		}

		protected override void OnCellMouseMove(DataGridViewCellMouseEventArgs e)
		{
			resetCellForColor(e.RowIndex, e.ColumnIndex);
			base.OnCellMouseMove(e);
		}

		private void resetCellForColor(int rowIndex, int columnIndex)
		{
			if (rowIndex >= 0)
			{
				if (Rows[rowIndex].Cells["Tag"].Value.ToString() == "")
				{
					switch (Columns[columnIndex].Name)
					{
						case "Price":
						case "Size":
							Rows[rowIndex].Cells["Price"].Style.ForeColor = DefaultCellForeColor;
							break;
						//case "Size":
						//    Rows[rowIndex].Cells["Price"].Style.ForeColor = DefaultCellForeColor;
						//    Rows[rowIndex].Cells["Size"].Style.ForeColor = DefaultCellForeColor;
						//    break;
					}
				}
			}
		}

		protected override void OnCellMouseClick(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (e.Button == MouseButtons.Left)
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "PartID":
							if (Rows[e.RowIndex].Cells["Tag"].Value.ToString() != "")
							{
								ShowTip = String.Concat(new object[] { 
									Rows[e.RowIndex].Cells["Tag"].Value.ToString(), 
									" - ", 
									Rows[e.RowIndex].Cells["Size"].Value.ToString(),
									"@",
									Rows[e.RowIndex].Cells["Price"].Value.ToString() });
							}
							else
							{
								ShowTip = String.Concat(new object[] { 
									Rows[e.RowIndex].Cells["PartID"].Value.ToString(), 
									" - ", 
									Rows[e.RowIndex].Cells["PartName"].Value.ToString() } );
							}
							ShowToolTip = true;
							break;
						case "Price":
						case "Size":
							if (Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
							{
								PriceL2 = (double)Rows[e.RowIndex].Cells["Price"].Value;

								PriceL2Selected = true;
							}
							break;
						//case "Size":
						//    if (Rows[e.RowIndex].Cells["Tag"].Value.ToString() == "")
						//    {
						//        PriceL2 = (double)Rows[e.RowIndex].Cells["Price"].Value;
						//        SizeL2 = (long)Rows[e.RowIndex].Cells["Size"].Value;

						//        SizeL2Selected = true;
						//    }
						//    break;
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

		public int GetRowIndex(Point s)
		{
			Point p = PointToClient(s);
			HitTestInfo hit = HitTest(p.X, p.Y);
			return hit.RowIndex;	
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

		#region - Public Functions -

		public void ReplaceOrderCleanUp()
		{
			WorkingOrderID = "";
			ReplaceOrderNow = false;
			_newPrice = 0;
			_orgPrice = 0;
			_orgSize = 0;
		}

		#endregion
	}
}
