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
	public class ROCAutoSpreadListSettings : DGVBase, INotifyPropertyChanged
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

		private bool _refreshRation = false;
		[Browsable(false)]
		public bool RefreshRation
		{
			get
			{
				return _refreshRation;
			}
			set
			{
				_refreshRation = value;
				if (_refreshRation)
				{
					OnRefreshingRation(EventArgs.Empty);
				}
			}
		}

		private bool _showColumnMenu = false;
		[Browsable(false)]
		public bool ShowColumnMenu
		{
			get
			{
				return _showColumnMenu;
			}
			set
			{
				_showColumnMenu = value;
				if (_showColumnMenu)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowColumnMenu"));
				}
			}
		}

		private bool _showTraderMenu = false;
		[Browsable(false)]
		public bool ShowTraderMenu
		{
			get
			{
				return _showTraderMenu;
			}
			set
			{
				_showTraderMenu = value;
				if (_showTraderMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowTraderMenu"));
				}
			}
		}

		private bool _showAccountMenu = false;
		[Browsable(false)]
		public bool ShowAccountMenu
		{
			get
			{
				return _showAccountMenu;
			}
			set
			{
				_showAccountMenu = value;
				if (_showAccountMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowAccountMenu"));
				}
			}
		}

		private bool _showExchangeMenu = false;
		[Browsable(false)]
		public bool ShowExchangeMenu
		{
			get
			{
				return _showExchangeMenu;
			}
			set
			{
				_showExchangeMenu = value;
				if (_showExchangeMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowExchangeMenu"));
				}
			}
		}

		#endregion

		#region - Properties -

		private int _rowLocation = 0;
		[Browsable(false)]
		public int RowLocation
		{
			get
			{
				return _rowLocation;
			}
		}

		#endregion

		#region - Event -

		[Description("Event raised when the value of the Refeshing Ration property is changed."), Category("Property Changed")]
		public event EventHandler RefreshingRation;

		#endregion

		#region - OnEvent -

		protected virtual void OnRefreshingRation(EventArgs e)
		{
			if (RefreshingRation != null)
			{
				RefreshingRation(this, e);
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

				_rocGridTable.Columns.Add(new DataColumn("Factor", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Ratio", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("UserRatio", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("Name", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Lean", Type.GetType("System.Boolean")));

				_rocGridTable.Columns.Add(new DataColumn("TopAskBookPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TopAskBookPriceRowIndex", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("TopBidBookPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TopBidBookPriceRowIndex", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("Valid", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("Exchange", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("LocalAccountAcrn", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("TraderFor", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("ShortLender", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("TickLimit", Type.GetType("System.Int64")));

				_rocGridTable.Columns["Symbol"].DefaultValue = "";
				_rocGridTable.Columns["SymbolDetail"].DefaultValue = "";
				_rocGridTable.Columns["Factor"].DefaultValue = 1;
				_rocGridTable.Columns["ContractSize"].DefaultValue = 1;
				_rocGridTable.Columns["TickSize"].DefaultValue = 0.01;
				_rocGridTable.Columns["Ratio"].DefaultValue = 0;
				_rocGridTable.Columns["UserRatio"].DefaultValue = 0;
				_rocGridTable.Columns["Lean"].DefaultValue = false;
				_rocGridTable.Columns["TickLimit"].DefaultValue = 5;
				_rocGridTable.Columns["DisplayFactor"].DefaultValue = 1;
				_rocGridTable.Columns["SecType"].DefaultValue = "";
				_rocGridTable.Columns["Valid"].DefaultValue = 0;

				_rocGridTable.Columns["Exchange"].DefaultValue = "";
				_rocGridTable.Columns["LocalAccountAcrn"].DefaultValue = "";
				_rocGridTable.Columns["TraderFor"].DefaultValue = "";
				_rocGridTable.Columns["ShortLender"].DefaultValue = "";

				_rocGridTable.DefaultView.Sort = "";
				LastSort = _rocGridTable.DefaultView.Sort;

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
					if (e.Row["SymbolDetail"] != null && e.Row["SymbolDetail"].ToString() != "")
					{
						SubscribeSymbolDetail = e.Row["SymbolDetail"].ToString().ToUpper().Trim();
					}
					break;
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

		private void SetGrid(ROCAutoSpreadListSettings grid)
		{
			DGVAutoSpreadListTextColumn txtCol;
			DGVAutoSpreadListNumberColumn numCol;
			DataGridViewCheckBoxColumn chkCol;

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
			grid.AllowUserToOrderColumns = true;

			grid.VirtualMode = true;

			// Setting grid specials
			grid.ScrollBars = ScrollBars.Horizontal;
			grid.CellBorderStyle = DataGridViewCellBorderStyle.SingleHorizontal;
			grid.ColumnHeadersDefaultCellStyle.BackColor = HeaderBackColor;
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVAutoSpreadListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "MDSymbol";
			txtCol.Name = "Symbol";
			txtCol.DataPropertyName = "Symbol";
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVAutoSpreadListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = Color.Gold;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Symbol";
			txtCol.Name = "SymbolDetail";
			txtCol.DataPropertyName = "SymbolDetail";
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			numCol = new DGVAutoSpreadListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.MaxDecimal = 0;
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "QTY";
			numCol.Name = "Factor";
			numCol.DataPropertyName = "Factor";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			numCol = new DGVAutoSpreadListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Mult";
			numCol.Name = "ContractSize";
			numCol.DataPropertyName = "ContractSize";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			numCol = new DGVAutoSpreadListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "TSize";
			numCol.Name = "TickSize";
			numCol.DataPropertyName = "TickSize";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			numCol = new DGVAutoSpreadListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(double);
			numCol.MaxDecimal = 4;
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "DEF";
			numCol.Name = "Ratio";
			numCol.DataPropertyName = "Ratio";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			numCol = new DGVAutoSpreadListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.ValueType = typeof(double);
			numCol.MaxDecimal = 4;
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "OVRD";
			numCol.Name = "UserRatio";
			numCol.DataPropertyName = "UserRatio";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			chkCol = new DataGridViewCheckBoxColumn();
			chkCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			chkCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			chkCol.DefaultCellStyle.BackColor = Color.Black;
			chkCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			chkCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			chkCol.SortMode = DataGridViewColumnSortMode.Automatic;
			chkCol.ReadOnly = false;
			chkCol.ValueType = typeof(bool);
			chkCol.DefaultCellStyle.Format = "";
			chkCol.HeaderText = "Lean";
			chkCol.Name = "Lean";
			chkCol.DataPropertyName = "Lean";
			chkCol.Width = 55;
			chkCol.FillWeight = 55F / 369F;
			grid.Columns.Add(chkCol);

			numCol = new DGVAutoSpreadListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = false;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.MaxDecimal = 4;
			numCol.DefaultCellStyle.Format = "F0";
			numCol.HeaderText = "TLimit";
			numCol.Name = "TickLimit";
			numCol.DataPropertyName = "TickLimit";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			txtCol = new DGVAutoSpreadListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = Color.Gold;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Exch";
			txtCol.Name = "Exchange";
			txtCol.DataPropertyName = "Exchange";
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVAutoSpreadListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = Color.Gold;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Acct";
			txtCol.Name = "LocalAccountAcrn";
			txtCol.DataPropertyName = "LocalAccountAcrn";
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVAutoSpreadListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = Color.Gold;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Trader";
			txtCol.Name = "TraderFor";
			txtCol.DataPropertyName = "TraderFor";
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVAutoSpreadListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = false;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "SLender";
			txtCol.Name = "ShortLender";
			txtCol.DataPropertyName = "ShortLender";
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVAutoSpreadListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleLeft;
			txtCol.DefaultCellStyle.BackColor = Color.FromKnownColor(KnownColor.ControlDarkDark);
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = Color.Gold;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Name";
			txtCol.Name = "Name";
			txtCol.DataPropertyName = "Name";
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			numCol = new DGVAutoSpreadListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Valid";
			numCol.Name = "Valid";
			numCol.DataPropertyName = "Valid";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			numCol = new DGVAutoSpreadListNumberColumn();
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
			numCol.HeaderText = "Display Factor";
			numCol.Name = "DisplayFactor";
			numCol.DataPropertyName = "DisplayFactor";
			numCol.Width = 55;
			numCol.FillWeight = 55F / 369F;
			grid.Columns.Add(numCol);

			txtCol = new DGVAutoSpreadListTextColumn();
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
			txtCol.Width = 55;
			txtCol.FillWeight = 55F / 369F;
			grid.Columns.Add(txtCol);

			foreach (DataGridViewColumn col in Columns)
			{
				col.SortMode = DataGridViewColumnSortMode.NotSortable;
			}

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Event -

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				if (Rows[e.RowIndex].Cells["Valid"].Value == null || (long)Rows[e.RowIndex].Cells["Valid"].Value != 1)
				{
					Rows[e.RowIndex].Cells["SymbolDetail"].Style.ForeColor = Color.Red;
				}
				else
				{
					Rows[e.RowIndex].Cells["SymbolDetail"].Style.ForeColor = Color.Gold;
				}
			}
			base.OnRowPrePaint(e);
		}

		protected override void OnCellMouseUp(DataGridViewCellMouseEventArgs e)
		{
			base.OnCellMouseUp(e);
		}

		protected override void OnCellEndEdit(DataGridViewCellEventArgs e)
		{
			switch (Columns[e.ColumnIndex].Name)
			{
				case "Factor":
				case "UserRatio":
				case "Lean":
				case "TickLimit":
				case "ShortLender":
					RefreshRation = true;
					break;
			}
			base.OnCellEndEdit(e);
		}

		protected override void OnCellMouseClick(DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				_rowLocation = e.RowIndex;

				if (e.Button == MouseButtons.Right)
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "Exchange":
							ShowExchangeMenu = true;
							break;
						case "LocalAccountAcrn":
							ShowAccountMenu = true;
							break;
						case "TraderFor":
							ShowTraderMenu = true;
							break;
					}
				}
				else
				{
					switch (Columns[e.ColumnIndex].Name)
					{
						case "Lean":
							try
							{
								if (this["SymbolDetail", e.RowIndex].Value != null && this["SymbolDetail", e.RowIndex].Value.ToString() != "")
								{
									lock (RocGridTable)
									{
										DataRow[] rows = RocGridTable.Select("SymbolDetail = '" + this["SymbolDetail", e.RowIndex].Value.ToString() + "'");
										if (rows.Length > 0)
										{
											foreach (DataRow row in rows)
											{
												row["Lean"] = ((DataGridViewCheckBoxCell)Rows[e.RowIndex].Cells[e.ColumnIndex]).EditingCellFormattedValue;
											}
										}
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
			}
			base.OnCellMouseClick(e);
		}

		protected override void OnColumnHeaderMouseClick(DataGridViewCellMouseEventArgs e)
		{
			switch (e.Button)
			{
				case MouseButtons.Right:
					ShowColumnMenu = true;
					break;
			}

			base.OnColumnHeaderMouseClick(e);
		}

		protected override void OnRowsRemoved(DataGridViewRowsRemovedEventArgs e)
		{
			if (e.RowIndex < Rows.Count)
			{
				RefreshRation = true;
			}
			base.OnRowsRemoved(e);
		}

		protected override void OnDataError(bool displayErrorDialogIfNoHandler, DataGridViewDataErrorEventArgs e)
		{
			//base.OnDataError(displayErrorDialogIfNoHandler, e);
		}

		#endregion
	}
}
