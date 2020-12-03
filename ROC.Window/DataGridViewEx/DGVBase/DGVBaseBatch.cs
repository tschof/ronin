using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public abstract class DGVBaseBatch : DGVBase, INotifyPropertyChanged
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

		private bool _showMenu = false;
		[Browsable(false)]
		public bool ShowMenu
		{
			get
			{
				return _showMenu;
			}
			set
			{
				_showMenu = value;
				if (_showMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowMenu"));
				}
			}
		}

		private bool _showStatusMenu = false;
		[Browsable(false)]
		public bool ShowStatusMenu
		{
			get
			{
				return _showStatusMenu;
			}
			set
			{
				_showStatusMenu = value;
				if (_showStatusMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowStatusMenu"));
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

		private bool _showShortLenderMenu = false;
		[Browsable(false)]
		public bool ShowShortLenderMenu
		{
			get
			{
				return _showShortLenderMenu;
			}
			set
			{
				_showShortLenderMenu = value;
				if (_showShortLenderMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowShortLenderMenu"));
				}
			}
		}

		private bool _showSideMenu = false;
		[Browsable(false)]
		public bool ShowSideMenu
		{
			get
			{
				return _showSideMenu;
			}
			set
			{
				_showSideMenu = value;
				if (_showSideMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowSideMenu"));
				}
			}
		}

		private bool _showOptionSideMenu = false;
		[Browsable(false)]
		public bool ShowOptionSideMenu
		{
			get
			{
				return _showOptionSideMenu;
			}
			set
			{
				_showOptionSideMenu = value;
				if (_showOptionSideMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowOptionSideMenu"));
				}
			}
		}

		private bool _showOrderTypeMenu = false;
		[Browsable(false)]
		public bool ShowOrderTypeMenu
		{
			get
			{
				return _showOrderTypeMenu;
			}
			set
			{
				_showOrderTypeMenu = value;
				if (_showOrderTypeMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowOrderTypeMenu"));
				}
			}
		}

		private bool _showAlgoTypeMenu = false;
		[Browsable(false)]
		public bool ShowAlgoTypeMenu
		{
			get
			{
				return _showAlgoTypeMenu;
			}
			set
			{
				_showAlgoTypeMenu = value;
				if (_showAlgoTypeMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowAlgoTypeMenu"));
				}
			}
		}

		private bool _showDurationMenu = false;
		[Browsable(false)]
		public bool ShowDurationMenu
		{
			get
			{
				return _showDurationMenu;
			}
			set
			{
				_showDurationMenu = value;
				if (_showDurationMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowDurationMenu"));
				}
			}
		}

		private bool _showStartTimeMenu = false;
		[Browsable(false)]
		public bool ShowStartTimeMenu
		{
			get
			{
				return _showStartTimeMenu;
			}
			set
			{
				_showStartTimeMenu = value;
				if (_showStartTimeMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowStartTimeMenu"));
				}
			}
		}

		private bool _showEndTimeMenu = false;
		[Browsable(false)]
		public bool ShowEndTimeMenu
		{
			get
			{
				return _showEndTimeMenu;
			}
			set
			{
				_showEndTimeMenu = value;
				if (_showEndTimeMenu == true)
				{
					onPropertyChanged(new PropertyChangedEventArgs("ShowEndTimeMenu"));
				}
			}
		}

		private string _alertMessage = "";
		[Browsable(false)]
		public string AlertMessage
		{
			get
			{
				return _alertMessage;
			}
			set
			{
				_alertMessage = value;
				if (_alertMessage != "")
				{
					onPropertyChanged(new PropertyChangedEventArgs("AlertMessage"));
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

		private string _shortLender = "";
		[Browsable(false)]
		public string ShortLender
		{
			get
			{
				return _shortLender;
			}
		}

		private bool _isMarketDataBatchGrid = false;
		[Browsable(false)]
		public bool IsMarketDataBatchGrid
		{
			get
			{
				return _isMarketDataBatchGrid;
			}
			set
			{
				_isMarketDataBatchGrid = value;
			}
		}

		#endregion

		#region - Data Table -

		[Browsable(false)]
		public abstract DataTable RocGridTable
		{
			get;
			set;
		}

		#endregion

		#region - Aggregation -

		[Browsable(false)]
		public long OrderCount
		{
			get
			{
				try
				{
					DataRow[] rows = RocGridTable.Select("Symbol <> ''");
					return rows.Length;
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _buySharesCount = 0;
		[Browsable(false)]
		public long BuySharesCount
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(Qty)", "Side = 'Buy' Or Side = 'B'").ToString(), out _buySharesCount))
					{
						return _buySharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _sellSharesCount = 0;
		[Browsable(false)]
		public long SellSharesCount
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(Qty)", "Side = 'Sell' Or Side = 'S'").ToString(), out _sellSharesCount))
					{
						return _sellSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		private long _shortSharesCount = 0;
		[Browsable(false)]
		public long ShortSharesCount
		{
			get
			{
				try
				{
					if (Int64.TryParse(RocGridTable.Compute("Sum(Qty)", "Side = 'Short' Or Side = 'Sell Short' Or Side = 'SS'").ToString(), out _shortSharesCount))
					{
						return _shortSharesCount;
					}
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
				return 0;
			}
		}

		#endregion

		#region - Grid -

		public abstract void Initialize();

		#endregion

		#region - Grid Event -

		protected override void OnCellMouseLeave(DataGridViewCellEventArgs e)
		{
			EndEdit();
			base.OnCellMouseLeave(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			OnSingleClick(e);
		}

		protected override void OnCellBeginEdit(DataGridViewCellCancelEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				_rowLocation = e.RowIndex;

				switch (Columns[e.ColumnIndex].Name)
				{
					case "SymbolDetail":
						if (this["SymbolDetail", e.RowIndex].Value != DBNull.Value && this["SymbolDetail", e.RowIndex].Value.ToString() != "")
						{
							// Clean Row
							Rows[e.RowIndex].Cells["SymbolDetail"].Value = "";
							Rows[e.RowIndex].Cells["Qty"].Value = 0;
							Rows[e.RowIndex].Cells["Price"].Value = 0;
							Rows[e.RowIndex].Cells["StopPrice"].Value = 0;
							Rows[e.RowIndex].Cells["PegPrice"].Value = 0;
							Rows[e.RowIndex].Cells["Display"].Value = "";
							Rows[e.RowIndex].Cells["Underlying"].Value = "";

							if (IsMarketDataBatchGrid)
							{
								Rows[e.RowIndex].Cells["Symbol"].Value = "";
								Rows[e.RowIndex].Cells["TickSize"].Value = 0.01;
								Rows[e.RowIndex].Cells["DisplayFactor"].Value = 1;
								Rows[e.RowIndex].Cells["SecType"].Value = "";
								Rows[e.RowIndex].Cells["CallPut"].Value = "";
								Rows[e.RowIndex].Cells["ExpDate"].Value = "";
								Rows[e.RowIndex].Cells["MaturityDay"].Value = "";
								Rows[e.RowIndex].Cells["Strike"].Value = "";
								Rows[e.RowIndex].Cells["BidPrice"].Value = 0;
								Rows[e.RowIndex].Cells["AskPrice"].Value = 0;
								Rows[e.RowIndex].Cells["LastTraded"].Value = 0;
							}
						}
						break;
				}
			}
			base.OnCellBeginEdit(e);
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
						case "Status":
							ShowStatusMenu = true;
							break;
						case "Side":
							if (IsMarketDataBatchGrid && this["SecType", e.RowIndex].Value != DBNull.Value)
							{
								switch (this["SecType", e.RowIndex].Value.ToString())
								{
									case CSVEx.CSVFieldIDs.SecutrityTypes.Option:
									case CSVEx.CSVFieldIDs.SecutrityTypes.OptionFuture:
										ShowOptionSideMenu = true;
										return;
								}
							}
							ShowSideMenu = true;
							break;
						case "Exchange":
							ShowExchangeMenu = true;
							break;
						case "LocalAccountAcrn":
							ShowAccountMenu = true;
							break;
						case "TraderFor":
							ShowTraderMenu = true;
							break;
						case "Duration":
							ShowDurationMenu = true;
							break;
						case "OrderType":
							ShowOrderTypeMenu = true;
							break;
						case "AlgoType":
							ShowAlgoTypeMenu = true;
							break;
						case "StartTime":
							ShowStartTimeMenu = true;
							break;
						case "EndTime":
							ShowEndTimeMenu = true;
							break;
						case "ShortLender":
							if (this["ShortLender", e.RowIndex].Value != null && this["ShortLender", e.RowIndex].Value.ToString() != "")
							{
								_shortLender = this["ShortLender", e.RowIndex].Value.ToString();
							}
							else
							{
								_shortLender = "";
							}
							if (ShortLender != "")
							{
								ShowShortLenderMenu = true;
							}
							break;
						default:
							// Show menu
							ShowMenu = true;
							break;
					}
				}
			}
			else
			{
				base.OnCellMouseClick(e);
			}
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
			RefreshAggragation = true;
			base.OnRowsRemoved(e);
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

		#region - Row Filter -

		public void BuildRowFilter()
		{
		}

		#endregion
	}
}
