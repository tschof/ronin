using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Windows.Forms;
using System.Drawing;
using System.Diagnostics;
using MarketData;
using System.Linq;

using Price = Common.Price;

namespace DataGridViewEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class ROCOptionList : DGVBase, INotifyPropertyChanged
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

		#region - Local Variables -

		private int _red = 0;
		private int _green = 0;
		private int _blue = 0;

		[Browsable(false)]
		public Color DefaultAtMoneyColor
		{
			get
			{
				return Color.FromArgb(RedDefault, GreenDefault, BlueDefault);
			}
			set
			{
				RedDefault = Convert.ToInt32(value.R);
				GreenDefault = Convert.ToInt32(value.G);
				BlueDefault = Convert.ToInt32(value.B);
			}			
		}

		private Dictionary<int, Color> _shiftedColors;
		private Dictionary<int, Color> shiftedColors
		{
			get
			{
				if (_shiftedColors == null)
				{
					RedDefault = 0;
					GreenDefault = 125;
					BlueDefault = 125;

					_shiftedColors = new Dictionary<int, Color>();

					for (int _shiftMultiples = 1; _shiftMultiples < ColorShiftIntervalMax; _shiftMultiples++)
					{
						_red = (RedDefault - (ColorShiftInterval * _shiftMultiples) < 0 ? 0 : RedDefault - (ColorShiftInterval * _shiftMultiples));
						_green = (GreenDefault - (ColorShiftInterval * _shiftMultiples) < 0 ? 0 : GreenDefault - (ColorShiftInterval * _shiftMultiples));
						_blue = (BlueDefault - (ColorShiftInterval * _shiftMultiples) < 0 ? 0 : BlueDefault - (ColorShiftInterval * _shiftMultiples));

						_shiftedColors.Add(_shiftMultiples, Color.FromArgb(_red, _green, _blue));
					}
				}

				return _shiftedColors;
			}
		}

		#endregion

		#region - Properties With Events -

		private bool _subscribe = false;
		[Browsable(false)]
		public bool Subscribe
		{
			get
			{
				return _subscribe;
			}
			set
			{
				_subscribe = value;
				if (_subscribe)
				{
					onPropertyChanged(new PropertyChangedEventArgs("Subscribe"));
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

		private bool _refreshNBBO = false;
		[Browsable(false)]
		public bool RefreshNBBO
		{
			get
			{
				return _refreshNBBO;
			}
			set
			{
				_refreshNBBO = value;
				if (_refreshNBBO)
				{
					onPropertyChanged(new PropertyChangedEventArgs("RefreshNBBO"));
				}
			}
		}

		private bool _optionPriceSelected = false;
		[Browsable(false)]
		public bool OptionPriceSelected
		{
			get
			{
				return _optionPriceSelected;
			}
			set
			{
				_optionPriceSelected = value;
				if (_optionPriceSelected)
				{
					onPropertyChanged(new PropertyChangedEventArgs("OptionPriceSelected"));
				}
			}
		}

		#endregion

		#region - Properties -

		private string _mdPutSymbol = "";
		[Browsable(false)]
		public string MDPutSymbol
		{
			get
			{
				return _mdPutSymbol;
			}
			set
			{
				_mdPutSymbol = value;
			}
		}

		private string _mdPutSymbolOPRA = "";
		[Browsable(false)]
		public string MDPutSymbolOPRA
		{
			get
			{
				return _mdPutSymbolOPRA;
			}
			set
			{
				_mdPutSymbolOPRA = value;
			}
		}

		private string _mdCallSymbol = "";
		[Browsable(false)]
		public string MDCallSymbol
		{
			get
			{
				return _mdCallSymbol;
			}
			set
			{
				_mdCallSymbol = value;
			}
		}

		private string _mdCallSymbolOPRA = "";
		[Browsable(false)]
		public string MDCallSymbolOPRA
		{
			get
			{
				return _mdCallSymbolOPRA;
			}
			set
			{
				_mdCallSymbolOPRA = value;
			}
		}

		private string _mdSource = "";
		[Browsable(false)]
		public string MDSource
		{
			get
			{
				return _mdSource;
			}
			set
			{
				_mdSource = value;
			}
		}

		private string _secType = "";
		[Browsable(false)]
		public string SecType
		{
			get
			{
				return _secType;
			}
			set
			{
				_secType = value;
			}
		}

		private string _defaultRowFilter = "PutPartID = '' And CallPartID = ''";
		[Browsable(false)]
		public string DefaultRowFilter
		{
			get
			{
				return _defaultRowFilter;
			}
		}

		private Dictionary<string, Color> _exchangeColors;
		[Browsable(false)]
		public Dictionary<string, Color> ExchangeColors
		{
			get
			{
				if (_exchangeColors == null)
				{
					_exchangeColors = new Dictionary<string, Color>();

					_exchangeColors.Add("", Color.RoyalBlue); 	// COMP
					_exchangeColors.Add(OptionExchangeCode.PHLX, Color.MediumPurple);
					_exchangeColors.Add(OptionExchangeCode.BOX, Color.OrangeRed);
					_exchangeColors.Add(OptionExchangeCode.ISE, Color.DeepSkyBlue);
					_exchangeColors.Add(OptionExchangeCode.CBOE, Color.GreenYellow);
					_exchangeColors.Add(OptionExchangeCode.C2, Color.GreenYellow);
					_exchangeColors.Add(OptionExchangeCode.AMEX, Color.Yellow);
					_exchangeColors.Add(OptionExchangeCode.PCX, Color.Orange);
					_exchangeColors.Add(OptionExchangeCode.NSDQ, Color.White);
					_exchangeColors.Add(OptionExchangeCode.BATS, Color.Violet);
				}
				return _exchangeColors;
			}
			set
			{
				// Override From Config File
				_exchangeColors = value;
			}
		}

		private DataView _searchView;
		[Browsable(false)]
		public DataView SearchView
		{
			get
			{
				if (_searchView == null && RocGridTable != null)
				{
					lock (RocGridTable)
					{
						_searchView = new DataView(RocGridTable);
						_searchView.Sort = "Expiration, Strike";
					}
				}
				return _searchView;
			}
			set
			{
				_searchView = value;
			}
		}

		private DataView _searchViewFull;
		[Browsable(false)]
		public DataView SearchViewFull
		{
			get
			{
				if (_searchViewFull == null && RocGridTable != null)
				{
					lock (RocGridTable)
					{
						_searchViewFull = new DataView(RocGridTable);
						_searchViewFull.Sort = "Expiration, Strike, OptionSymbol";
					}
				}
				return _searchViewFull;
			}
			set
			{
				_searchViewFull = value;
			}
		}

		private DataView _strikeSearchView;
		[Browsable(false)]
		public DataView StrikeSearchView
		{
			get
			{
				if (_strikeSearchView == null && RocGridTable != null)
				{
					lock (RocGridTable)
					{
						_strikeSearchView = new DataView(RocGridTable);
						_strikeSearchView.Sort = "Strike";
					}
				}
				return _strikeSearchView;
			}
			set
			{
				_strikeSearchView = value;
			}
		}

		private DataView _callSearchView;
		[Browsable(false)]
		public DataView CallSearchView
		{
			get
			{
				if (_callSearchView == null && RocGridTable != null)
				{
					lock (RocGridTable)
					{
						_callSearchView = new DataView(RocGridTable);
						_callSearchView.Sort = "CallSymbol, CallPartID";
					}
				}
				return _callSearchView;
			}
			set
			{
				_callSearchView = value;
			}
		}

		private DataView _putSearchView;
		[Browsable(false)]
		public DataView PutSearchView
		{
			get
			{
				if (_putSearchView == null && RocGridTable != null)
				{
					lock (RocGridTable)
					{
						_putSearchView = new DataView(RocGridTable);
						_putSearchView.Sort = "PutSymbol, PutPartID";
					}
				}
				return _putSearchView;
			}
			set
			{
				_putSearchView = value;
			}
		}

		private List<string> _callKeys;
		[Browsable(false)]
		public List<string> CallKeys
		{
			get
			{
				if (_callKeys == null)
				{
					_callKeys = new List<string>();
				}
				return _callKeys;
			}
			set
			{
				_callKeys = value;
			}
		}

		private List<string> _putKeys;
		[Browsable(false)]
		public List<string> PutKeys
		{
			get
			{
				if (_putKeys == null)
				{
					_putKeys = new List<string>();
				}
				return _putKeys;
			}
			set
			{
				_putKeys = value;
			}
		}

		private Dictionary<int, Color> _monthColors;
		private Dictionary<int, Color> monthColors
		{
			get
			{
				if (_monthColors == null)
				{
					_monthColors = new Dictionary<int, Color>();

					_monthColors.Add(0, Color.Blue);
					_monthColors.Add(1, Color.GreenYellow);
				}
				return _monthColors;
			}
		}

		private Color _strikeColor = Color.Blue;

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

		private string _exchangeFilter = "";
		[Browsable(false)]
		public string ExchangeFilter
		{
			get
			{
				return _exchangeFilter;
			}
		}

		[Browsable(false)]
		public int RowsToShow
		{
			get
			{
				return Height / RowTemplate.Height;
			}
		}

		private string _optionCallPut = CSVEx.CSVFieldIDs.OptionTypes.Call;
		[Browsable(false)]
		public string OptionCallPut
		{
			get
			{
				return _optionCallPut;
			}
			set
			{
				_optionCallPut = value;
			}
		}

		private double _optionPrice = 0;
		[Browsable(false)]
		public double OptionPrice
		{
			get
			{
				return _optionPrice;
			}
			set
			{
				_optionPrice = value;
			}
		}

		private string _optionPutSymbol = "";
		[Browsable(false)]
		public string OptionPutSymbol
		{
			get
			{
				return _optionPutSymbol;
			}
			set
			{
				_optionPutSymbol = value;
			}
		}

		private string _optionCallSymbol = "";
		[Browsable(false)]
		public string OptionCallSymbol
		{
			get
			{
				return _optionCallSymbol;
			}
			set
			{
				_optionCallSymbol = value;
			}
		}

		private double _optionStrike = 0;
		[Browsable(false)]
		public double OptionStrike
		{
			get
			{
				return _optionStrike;
			}
			set
			{
				_optionStrike = value;
			}
		}

		private DateTime _optionExpiration = DateTime.Now;
		[Browsable(false)]
		public DateTime OptionExpiration
		{
			get
			{
				return _optionExpiration;
			}
			set
			{
				_optionExpiration = value;
			}
		}

		private string _optionExchange = "";
		[Browsable(false)]
		public string OptionExchange
		{
			get
			{
				return _optionExchange;
			}
			set
			{
				_optionExchange = value;
			}
		}

		[Browsable(false)]
		public string OptionExchangeName => OptionExchangeCode.TryGetName(_optionExchange, out string name) ? name : _optionExchange;

		[Browsable(false)]
		public ScrollBars OriginalScrollBars
		{
			get
			{
				return ScrollBars.Both;
			}
		}

		#endregion

		#region - Filter Properties -

		private bool _showOnlySelectedExchange = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool ShowOnlySelectedExchange
		{
			get
			{
				return _showOnlySelectedExchange;
			}
			set
			{
				_showOnlySelectedExchange = value;
			}
		}

		private bool _filterOutCBOE = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutCBOE
		{
			get
			{
				return _filterOutCBOE;
			}
			set
			{
				if (_filterOutCBOE != value)
				{
					_filterOutCBOE = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutBOX = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutBOX
		{
			get
			{
				return _filterOutBOX;
			}
			set
			{
				if (_filterOutBOX != value)
				{
					_filterOutBOX = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutAMEX = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutAMEX
		{
			get
			{
				return _filterOutAMEX;
			}
			set
			{
				if (_filterOutAMEX != value)
				{
					_filterOutAMEX = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutPHLX = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutPHLX
		{
			get
			{
				return _filterOutPHLX;
			}
			set
			{
				if (_filterOutPHLX != value)
				{
					_filterOutPHLX = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutPCX = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutPCX
		{
			get
			{
				return _filterOutPCX;
			}
			set
			{
				if (_filterOutPCX != value)
				{
					_filterOutPCX = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutISE = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutISE
		{
			get
			{
				return _filterOutISE;
			}
			set
			{
				if (_filterOutISE != value)
				{
					_filterOutISE = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutNSDQ = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutNSDQ
		{
			get
			{
				return _filterOutNSDQ;
			}
			set
			{
				if (_filterOutNSDQ != value)
				{
					_filterOutNSDQ = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutBATS = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutBATS
		{
			get
			{
				return _filterOutBATS;
			}
			set
			{
				if (_filterOutBATS != value)
				{
					_filterOutBATS = value;
					BuildRowFilter();
				}
			}
		}

		private bool _filterOutC2 = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance")]
		public bool FilterOutC2
		{
			get
			{
				return _filterOutC2;
			}
			set
			{
				if (_filterOutC2 != value)
				{
					_filterOutC2 = value;
					BuildRowFilter();
				}
			}
		}

		[Browsable(false)]
		public bool FilterOutAll
		{
			get
			{
				return FilterOutC2 && FilterOutCBOE && FilterOutBOX && FilterOutAMEX && FilterOutPHLX && FilterOutPCX && FilterOutISE && FilterOutNSDQ && FilterOutBATS;
			}
			set
			{
				_filterOutC2 = value;
				_filterOutCBOE = value;
				_filterOutBOX = value;
				_filterOutAMEX = value;
				_filterOutPHLX = value;
				_filterOutPCX = value;
				_filterOutISE = value;
				_filterOutNSDQ = value;
				_filterOutBATS = value;

				BuildRowFilter();
			}
		}

		[Browsable(false)]
		public bool FilterOutAllWithoutFilter
		{
			get
			{
				return FilterOutC2 && FilterOutCBOE && FilterOutBOX && FilterOutAMEX && FilterOutPHLX && FilterOutPCX && FilterOutISE && FilterOutNSDQ && FilterOutBATS;
			}
			set
			{
				_filterOutC2 = value;
				_filterOutCBOE = value;
				_filterOutBOX = value;
				_filterOutAMEX = value;
				_filterOutPHLX = value;
				_filterOutPCX = value;
				_filterOutISE = value;
				_filterOutNSDQ = value;
				_filterOutBATS = value;
			}
		}

		[Browsable(false)]
		public bool FilterInAll
		{
			get
			{
				return !FilterOutC2 && !FilterOutCBOE && !FilterOutBOX && !FilterOutAMEX && !FilterOutPHLX && !FilterOutPCX && !FilterOutISE && !FilterOutNSDQ && !FilterOutBATS;
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

				_rocGridTable.Columns.Add(new DataColumn("PutSymbol", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("PutPartID", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("PutOpenQty", Type.GetType("System.Int64")));

				_rocGridTable.Columns.Add(new DataColumn("PutLastTraded", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("PutLastPartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("PutPctChanged", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("PutVolume", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("PutBidPartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("PutBidSize", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("PutBidPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("PutAskSize", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("PutAskPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("PutAskPartID", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("CallLastTraded", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("CallLastPartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("CallPctChanged", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("CallVolume", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("CallBidPartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("CallBidSize", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("CallBidPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("CallAskSize", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("CallAskPrice", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("CallAskPartID", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("CallOpenQty", Type.GetType("System.Int64")));
				
				_rocGridTable.Columns.Add(new DataColumn("CallPartID", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("CallSymbol", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("ContractSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("TickSize", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayFactor", Type.GetType("System.Double")));

				_rocGridTable.Columns.Add(new DataColumn("Strike", Type.GetType("System.Double")));
				_rocGridTable.Columns.Add(new DataColumn("StringStrike", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Expiration", Type.GetType("System.DateTime")));
				_rocGridTable.Columns.Add(new DataColumn("DisplayExpiration", Type.GetType("System.DateTime")));
				_rocGridTable.Columns.Add(new DataColumn("OptionSymbol", Type.GetType("System.String")));

				_rocGridTable.Columns.Add(new DataColumn("SecType", Type.GetType("System.String")));
				
				_rocGridTable.Columns.Add(new DataColumn("MDSource", Type.GetType("System.String")));
				_rocGridTable.Columns.Add(new DataColumn("Subscribed", Type.GetType("System.Int64")));
				_rocGridTable.Columns.Add(new DataColumn("MoneyFlag", Type.GetType("System.Int64")));


				_rocGridTable.Columns["PutSymbol"].DefaultValue = "";
				_rocGridTable.Columns["PutPartID"].DefaultValue = "";

				_rocGridTable.Columns["CallSymbol"].DefaultValue = "";
				_rocGridTable.Columns["CallPartID"].DefaultValue = "";

				_rocGridTable.Columns["TickSize"].DefaultValue = 0.01;
				_rocGridTable.Columns["ContractSize"].DefaultValue = 100;
				_rocGridTable.Columns["DisplayFactor"].DefaultValue = 1;

				_rocGridTable.Columns["Subscribed"].DefaultValue = 0;
				_rocGridTable.Columns["MoneyFlag"].DefaultValue = 0;

				_rocGridTable.Columns["MDSource"].DefaultValue = "";

				_rocGridTable.RowChanged += new DataRowChangeEventHandler(RocGridTable_RowChanged);
			}

			_rocGridTable.DefaultView.Sort = "Expiration, Strike";
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

		public void Initialize()
		{
			SetGridTable();

			SetGrid(this);
		}

		private void SetGrid(ROCOptionList grid)
		{
			DGVOptionListTextColumn txtCol;
			DGVOptionListNumberColumn numCol;
			DGVOptionListNotifiedNumberColumn nnumCol;
			DGVOptionListTimeColumn timCol;

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

			// Option grid specials
			grid.ScrollBars = OriginalScrollBars;
			grid.CellBorderStyle = DataGridViewCellBorderStyle.SingleHorizontal;
			grid.ColumnHeadersDefaultCellStyle.BackColor = HeaderBackColor;
			grid.ColumnHeadersBorderStyle = DataGridViewHeaderBorderStyle.Single;
			grid.ColumnHeadersDefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;

			txtCol = new DGVOptionListTextColumn();
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
			txtCol.Name = "CallSymbol";
			txtCol.DataPropertyName = "CallSymbol";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "PID";
			txtCol.Name = "CallPartID";
			txtCol.DataPropertyName = "CallPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Open";
			numCol.Name = "CallOpenQty";
			numCol.DataPropertyName = "CallOpenQty";
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			nnumCol = new DGVOptionListNotifiedNumberColumn();
			nnumCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			nnumCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			nnumCol.DefaultCellStyle.BackColor = Color.Black;
			nnumCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			nnumCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			nnumCol.SortMode = DataGridViewColumnSortMode.Automatic;
			nnumCol.ReadOnly = true;
			nnumCol.IsPercent = true;
			nnumCol.MaxDecimal = 2;
			nnumCol.AutoColor = true;
			nnumCol.Visible = false;
			nnumCol.ValueType = typeof(double);
			nnumCol.DefaultCellStyle.Format = "";
			nnumCol.HeaderText = "%";
			nnumCol.Name = "CallPctChanged";
			nnumCol.DataPropertyName = "CallPctChanged";
			nnumCol.Width = 40;
			nnumCol.MinimumWidth = 40;
			nnumCol.FillWeight = 40F / 558F;
			grid.Columns.Add(nnumCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Vol";
			numCol.Name = "CallVolume";
			numCol.DataPropertyName = "CallVolume";
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "BP";
			txtCol.Name = "CallBidPartID";
			txtCol.DataPropertyName = "CallBidPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BSize";
			numCol.Name = "CallBidSize";
			numCol.DataPropertyName = "CallBidSize";
			numCol.Width = 45;
			numCol.MinimumWidth = 45;
			numCol.FillWeight = 45F / 558F;
			grid.Columns.Add(numCol);

			nnumCol = new DGVOptionListNotifiedNumberColumn();
			nnumCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			nnumCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			nnumCol.DefaultCellStyle.BackColor = Color.Black;
			nnumCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			nnumCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			nnumCol.ReadOnly = true;
			nnumCol.IsFraction = true;
			nnumCol.IsNotified = false;
			nnumCol.ValueType = typeof(double);
			nnumCol.DefaultCellStyle.Format = "";
			nnumCol.HeaderText = "Bid";
			nnumCol.Name = "CallBidPrice";
			nnumCol.DataPropertyName = "CallBidPrice";
			nnumCol.Width = 45;
			nnumCol.MinimumWidth = 45;
			nnumCol.FillWeight = 45F / 558F;
			grid.Columns.Add(nnumCol);

			nnumCol = new DGVOptionListNotifiedNumberColumn();
			nnumCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			nnumCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			nnumCol.DefaultCellStyle.BackColor = Color.Black;
			nnumCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			nnumCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			nnumCol.SortMode = DataGridViewColumnSortMode.Automatic;
			nnumCol.ReadOnly = true;
			nnumCol.IsFraction = true;
			nnumCol.IsNotified = false;
			nnumCol.ValueType = typeof(double);
			nnumCol.DefaultCellStyle.Format = "";
			nnumCol.HeaderText = "Ask";
			nnumCol.Name = "CallAskPrice";
			nnumCol.DataPropertyName = "CallAskPrice";
			nnumCol.Width = 45;
			nnumCol.MinimumWidth = 45;
			nnumCol.FillWeight = 45F / 558F;
			grid.Columns.Add(nnumCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "ASize";
			numCol.Name = "CallAskSize";
			numCol.DataPropertyName = "CallAskSize";
			numCol.Width = 45;
			numCol.MinimumWidth = 45;
			numCol.FillWeight = 45F / 558F;
			grid.Columns.Add(numCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "AP";
			txtCol.Name = "CallAskPartID";
			txtCol.DataPropertyName = "CallAskPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "CLP";
			txtCol.Name = "CallLastPartID";
			txtCol.DataPropertyName = "CallLastPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Last";
			numCol.Name = "CallLastTraded";
			numCol.DataPropertyName = "CallLastTraded";
			numCol.Width = 45;
			numCol.MinimumWidth = 45;
			numCol.FillWeight = 45F / 558F;
			grid.Columns.Add(numCol);

			timCol = new DGVOptionListTimeColumn();
			timCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			timCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			timCol.DefaultCellStyle.BackColor = Color.Gainsboro;
			timCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			timCol.DefaultCellStyle.ForeColor = Color.DarkSlateBlue;
			timCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			timCol.SortMode = DataGridViewColumnSortMode.Automatic;
			timCol.ReadOnly = true;
			timCol.Visible = false;
			timCol.FormatString = "yy MMM dd";
			timCol.ValueType = typeof(DateTime);
			timCol.HeaderText = "EXP";
			timCol.Name = "Expiration";
			timCol.DataPropertyName = "Expiration";
			timCol.Width = 85;
			timCol.MinimumWidth = 85;
			timCol.FillWeight = 85F / 558F;
			grid.Columns.Add(timCol);

			timCol = new DGVOptionListTimeColumn();
			timCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			timCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			timCol.DefaultCellStyle.BackColor = Color.Gainsboro;
			timCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			timCol.DefaultCellStyle.ForeColor = Color.DarkSlateBlue;
			timCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			timCol.SortMode = DataGridViewColumnSortMode.Automatic;
			timCol.ReadOnly = true;
			timCol.Visible = true;
			timCol.FormatString = "yy MMM dd";
			timCol.ValueType = typeof(DateTime);
			timCol.HeaderText = "MONTH";
			timCol.Name = "DisplayExpiration";
			timCol.DataPropertyName = "DisplayExpiration";
			timCol.Width = 85;
			timCol.MinimumWidth = 85;
			timCol.FillWeight = 85F / 558F;
			grid.Columns.Add(timCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.WhiteSmoke;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = Color.DarkBlue;
			numCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "F";
			numCol.HeaderText = "STK";
			numCol.Name = "Strike";
			numCol.DataPropertyName = "Strike";
			numCol.Width = 65;
			numCol.MinimumWidth = 65;
			numCol.FillWeight = 65F / 558F;
			grid.Columns.Add(numCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "SSTK";
			txtCol.Name = "StringStrike";
			txtCol.DataPropertyName = "StringStrike";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			txtCol.DefaultCellStyle.BackColor = Color.WhiteSmoke;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = Color.DarkBlue;
			txtCol.DefaultCellStyle.Font = new Font(Font, FontStyle.Bold);
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "OPT";
			txtCol.Name = "OptionSymbol";
			txtCol.DataPropertyName = "OptionSymbol";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.IsFraction = true;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Last";
			numCol.Name = "PutLastTraded";
			numCol.DataPropertyName = "PutLastTraded";
			numCol.Width = 45;
			numCol.MinimumWidth = 45;
			numCol.FillWeight = 45F / 558F;
			grid.Columns.Add(numCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "PLP";
			txtCol.Name = "PutLastPartID";
			txtCol.DataPropertyName = "PutLastPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "BP";
			txtCol.Name = "PutBidPartID";
			txtCol.DataPropertyName = "PutBidPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "BSize";
			numCol.Name = "PutBidSize";
			numCol.DataPropertyName = "PutBidSize";
			numCol.Width = 45;
			numCol.MinimumWidth = 45;
			numCol.FillWeight = 45F / 558F;
			grid.Columns.Add(numCol);

			nnumCol = new DGVOptionListNotifiedNumberColumn();
			nnumCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			nnumCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			nnumCol.DefaultCellStyle.BackColor = Color.Black;
			nnumCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			nnumCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			nnumCol.SortMode = DataGridViewColumnSortMode.Automatic;
			nnumCol.ReadOnly = true;
			nnumCol.IsFraction = true;
			nnumCol.IsNotified = false;
			nnumCol.ValueType = typeof(double);
			nnumCol.DefaultCellStyle.Format = "";
			nnumCol.HeaderText = "Bid";
			nnumCol.Name = "PutBidPrice";
			nnumCol.DataPropertyName = "PutBidPrice";
			nnumCol.Width = 45;
			nnumCol.MinimumWidth = 45;
			nnumCol.FillWeight = 45F / 558F;
			grid.Columns.Add(nnumCol);

			nnumCol = new DGVOptionListNotifiedNumberColumn();
			nnumCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			nnumCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			nnumCol.DefaultCellStyle.BackColor = Color.Black;
			nnumCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			nnumCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			nnumCol.SortMode = DataGridViewColumnSortMode.Automatic;
			nnumCol.ReadOnly = true;
			nnumCol.IsFraction = true;
			nnumCol.IsNotified = false;
			nnumCol.ValueType = typeof(double);
			nnumCol.DefaultCellStyle.Format = "";
			nnumCol.HeaderText = "Ask";
			nnumCol.Name = "PutAskPrice";
			nnumCol.DataPropertyName = "PutAskPrice";
			nnumCol.Width = 45;
			nnumCol.MinimumWidth = 45;
			nnumCol.FillWeight = 45F / 558F;
			grid.Columns.Add(nnumCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "ASize";
			numCol.Name = "PutAskSize";
			numCol.DataPropertyName = "PutAskSize";
			numCol.Width = 45;
			numCol.MinimumWidth = 45;
			numCol.FillWeight = 45F / 558F;
			grid.Columns.Add(numCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "AP";
			txtCol.Name = "PutAskPartID";
			txtCol.DataPropertyName = "PutAskPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Vol";
			numCol.Name = "PutVolume";
			numCol.DataPropertyName = "PutVolume";
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			nnumCol = new DGVOptionListNotifiedNumberColumn();
			nnumCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			nnumCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			nnumCol.DefaultCellStyle.BackColor = Color.Black;
			nnumCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			nnumCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			nnumCol.SortMode = DataGridViewColumnSortMode.Automatic;
			nnumCol.ReadOnly = true;
			nnumCol.IsPercent = true;
			nnumCol.MaxDecimal = 2;
			nnumCol.AutoColor = true;
			nnumCol.Visible = false;
			nnumCol.ValueType = typeof(double);
			nnumCol.DefaultCellStyle.Format = "";
			nnumCol.HeaderText = "%";
			nnumCol.Name = "PutPctChanged";
			nnumCol.DataPropertyName = "PutPctChanged";
			nnumCol.Width = 40;
			nnumCol.MinimumWidth = 40;
			nnumCol.FillWeight = 40F / 558F;
			grid.Columns.Add(nnumCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Open";
			numCol.Name = "PutOpenQty";
			numCol.DataPropertyName = "PutOpenQty";
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "PID";
			txtCol.Name = "PutPartID";
			txtCol.DataPropertyName = "PutPartID";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "Name";
			txtCol.Name = "PutSymbol";
			txtCol.DataPropertyName = "PutSymbol";
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "Subscribed";
			numCol.Name = "Subscribed";
			numCol.DataPropertyName = "Subscribed";
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.ReadOnly = true;
			numCol.Visible = false;
			numCol.ValueType = typeof(long);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "MoneyFlag";
			numCol.Name = "MoneyFlag";
			numCol.DataPropertyName = "MoneyFlag";
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
			txtCol.DefaultCellStyle.BackColor = Color.Black;
			txtCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			txtCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			txtCol.SortMode = DataGridViewColumnSortMode.Automatic;
			txtCol.ReadOnly = true;
			txtCol.Visible = false;
			txtCol.ValueType = typeof(string);
			txtCol.HeaderText = "MDSource";
			txtCol.Name = "MDSource";
			txtCol.DataPropertyName = "MDSource";
			grid.Columns.Add(txtCol);

			txtCol = new DGVOptionListTextColumn();
			txtCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			txtCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
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
			txtCol.Width = 40;
			txtCol.MinimumWidth = 40;
			txtCol.FillWeight = 40F / 558F;
			grid.Columns.Add(txtCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleCenter;
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
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			numCol = new DGVOptionListNumberColumn();
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
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			numCol = new DGVOptionListNumberColumn();
			numCol.AutoSizeMode = DataGridViewAutoSizeColumnMode.None;
			numCol.DefaultCellStyle.Alignment = DataGridViewContentAlignment.MiddleRight;
			numCol.DefaultCellStyle.BackColor = Color.Black;
			numCol.DefaultCellStyle.SelectionBackColor = DefaultSelectedBackgroundColor;
			numCol.DefaultCellStyle.ForeColor = DefaultCellForeColor;
			numCol.SortMode = DataGridViewColumnSortMode.Automatic;
			numCol.Visible = false;
			numCol.ValueType = typeof(double);
			numCol.DefaultCellStyle.Format = "";
			numCol.HeaderText = "DisplayFactor";
			numCol.Name = "DisplayFactor";
			numCol.DataPropertyName = "DisplayFactor";
			numCol.Width = 40;
			numCol.MinimumWidth = 40;
			numCol.FillWeight = 40F / 558F;
			grid.Columns.Add(numCol);

			foreach (DataGridViewColumn col in Columns)
			{
				col.SortMode = DataGridViewColumnSortMode.NotSortable;
			}

			grid.DataSource = RocGridTable.DefaultView;
		}

		#endregion

		#region - Grid Event -

		protected override void OnMouseWheel(MouseEventArgs e)
		{
			ScrollByWheel(e.Delta / 120);
			base.OnMouseWheel(e);
		}

		protected override void OnColumnHeaderMouseClick(DataGridViewCellMouseEventArgs e)
		{
			if (e.Button == MouseButtons.Right)
			{
				ShowColumnMenu = true;
			}
			base.OnColumnHeaderMouseClick(e);
		}

		protected override void OnCellMouseDown(DataGridViewCellMouseEventArgs e)
		{
			try
			{
				if (e.RowIndex >= 0)
				{
					if (e.Button == MouseButtons.Left)
					{
						switch (Columns[e.ColumnIndex].Name)
						{
							case "PutBidPrice":
								Rows[e.RowIndex].Cells["PutBidPrice"].Style.SelectionBackColor = SelectedForeColor;

								if (Rows[e.RowIndex].Cells["PutBidPrice"].Value != null && Rows[e.RowIndex].Cells["PutBidPrice"].Value.ToString() != "")
								{
									OptionPrice = (double)Rows[e.RowIndex].Cells["PutBidPrice"].Value;
								}
								if (Rows[e.RowIndex].Cells["PutSymbol"].Value != null && Rows[e.RowIndex].Cells["PutSymbol"].Value.ToString() != "")
								{
									OptionPutSymbol = (string)Rows[e.RowIndex].Cells["PutSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["CallSymbol"].Value != null && Rows[e.RowIndex].Cells["CallSymbol"].Value.ToString() != "")
								{
									OptionCallSymbol = (string)Rows[e.RowIndex].Cells["CallSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["Strike"].Value != null && Rows[e.RowIndex].Cells["Strike"].Value.ToString() != "")
								{
									OptionStrike = (double)Rows[e.RowIndex].Cells["Strike"].Value;
								}
								if (Rows[e.RowIndex].Cells["Expiration"].Value != null && Rows[e.RowIndex].Cells["Expiration"].Value.ToString() != "")
								{
									OptionExpiration = (DateTime)Rows[e.RowIndex].Cells["Expiration"].Value;
								}
								if (Rows[e.RowIndex].Cells["PutBidPartID"].Value != null && Rows[e.RowIndex].Cells["PutBidPartID"].Value.ToString() != "")
								{
									OptionExchange = (string)Rows[e.RowIndex].Cells["PutBidPartID"].Value;
								}
								OptionCallPut = CSVEx.CSVFieldIDs.OptionTypes.Put;

								OptionPriceSelected = true;
								break;
							case "PutAskPrice":
								Rows[e.RowIndex].Cells["PutAskPrice"].Style.SelectionBackColor = SelectedForeColor;

								if (Rows[e.RowIndex].Cells["PutAskPrice"].Value != null && Rows[e.RowIndex].Cells["PutAskPrice"].Value.ToString() != "")
								{
									OptionPrice = (double)Rows[e.RowIndex].Cells["PutAskPrice"].Value;
								}
								if (Rows[e.RowIndex].Cells["PutSymbol"].Value != null && Rows[e.RowIndex].Cells["PutSymbol"].Value.ToString() != "")
								{
									OptionPutSymbol = (string)Rows[e.RowIndex].Cells["PutSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["CallSymbol"].Value != null && Rows[e.RowIndex].Cells["CallSymbol"].Value.ToString() != "")
								{
									OptionCallSymbol = (string)Rows[e.RowIndex].Cells["CallSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["Strike"].Value != null && Rows[e.RowIndex].Cells["Strike"].Value.ToString() != "")
								{
									OptionStrike = (double)Rows[e.RowIndex].Cells["Strike"].Value;
								}
								if (Rows[e.RowIndex].Cells["Expiration"].Value != null && Rows[e.RowIndex].Cells["Expiration"].Value.ToString() != "")
								{
									OptionExpiration = (DateTime)Rows[e.RowIndex].Cells["Expiration"].Value;
								}
								if (Rows[e.RowIndex].Cells["PutAskPartID"].Value != null && Rows[e.RowIndex].Cells["PutAskPartID"].Value.ToString() != "")
								{
									OptionExchange = (string)Rows[e.RowIndex].Cells["PutAskPartID"].Value;
								}
								OptionCallPut = CSVEx.CSVFieldIDs.OptionTypes.Put;

								OptionPriceSelected = true;
								break;
							case "CallBidPrice":
								Rows[e.RowIndex].Cells["CallBidPrice"].Style.SelectionBackColor = SelectedForeColor;

								if (Rows[e.RowIndex].Cells["CallBidPrice"].Value != null && Rows[e.RowIndex].Cells["CallBidPrice"].Value.ToString() != "")
								{
									OptionPrice = (double)Rows[e.RowIndex].Cells["CallBidPrice"].Value;
								}
								if (Rows[e.RowIndex].Cells["PutSymbol"].Value != null && Rows[e.RowIndex].Cells["PutSymbol"].Value.ToString() != "")
								{
									OptionPutSymbol = (string)Rows[e.RowIndex].Cells["PutSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["CallSymbol"].Value != null && Rows[e.RowIndex].Cells["CallSymbol"].Value.ToString() != "")
								{
									OptionCallSymbol = (string)Rows[e.RowIndex].Cells["CallSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["Strike"].Value != null && Rows[e.RowIndex].Cells["Strike"].Value.ToString() != "")
								{
									OptionStrike = (double)Rows[e.RowIndex].Cells["Strike"].Value;
								}
								if (Rows[e.RowIndex].Cells["Expiration"].Value != null && Rows[e.RowIndex].Cells["Expiration"].Value.ToString() != "")
								{
									OptionExpiration = (DateTime)Rows[e.RowIndex].Cells["Expiration"].Value;
								}
								if (Rows[e.RowIndex].Cells["CallBidPartID"].Value != null && Rows[e.RowIndex].Cells["CallBidPartID"].Value.ToString() != "")
								{
									OptionExchange = (string)Rows[e.RowIndex].Cells["CallBidPartID"].Value;
								}
								OptionCallPut = CSVEx.CSVFieldIDs.OptionTypes.Call;

								OptionPriceSelected = true;
								break;
							case "CallAskPrice":
								Rows[e.RowIndex].Cells["CallAskPrice"].Style.SelectionBackColor = SelectedForeColor;

								if (Rows[e.RowIndex].Cells["CallAskPrice"].Value != null && Rows[e.RowIndex].Cells["CallAskPrice"].Value.ToString() != "")
								{
									OptionPrice = (double)Rows[e.RowIndex].Cells["CallAskPrice"].Value;
								}
								if (Rows[e.RowIndex].Cells["PutSymbol"].Value != null && Rows[e.RowIndex].Cells["PutSymbol"].Value.ToString() != "")
								{
									OptionPutSymbol = (string)Rows[e.RowIndex].Cells["PutSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["CallSymbol"].Value != null && Rows[e.RowIndex].Cells["CallSymbol"].Value.ToString() != "")
								{
									OptionCallSymbol = (string)Rows[e.RowIndex].Cells["CallSymbol"].Value;
								}
								if (Rows[e.RowIndex].Cells["Strike"].Value != null && Rows[e.RowIndex].Cells["Strike"].Value.ToString() != "")
								{
									OptionStrike = (double)Rows[e.RowIndex].Cells["Strike"].Value;
								}
								if (Rows[e.RowIndex].Cells["Expiration"].Value != null && Rows[e.RowIndex].Cells["Expiration"].Value.ToString() != "")
								{
									OptionExpiration = (DateTime)Rows[e.RowIndex].Cells["Expiration"].Value;
								}
								if (Rows[e.RowIndex].Cells["CallAskPartID"].Value != null && Rows[e.RowIndex].Cells["CallAskPartID"].Value.ToString() != "")
								{
									OptionExchange = (string)Rows[e.RowIndex].Cells["CallAskPartID"].Value;
								}
								OptionCallPut = CSVEx.CSVFieldIDs.OptionTypes.Call;

								OptionPriceSelected = true;
								break;
						}
					}
					else if (e.Button == MouseButtons.Middle)
					{
						CenterOnRow(AtMoney);
					}
				}
				base.OnCellMouseDown(e);
			}
			catch (Exception ex)
			{
				Debug.Print(ex.Message);
			}
		}

		protected override void OnCellMouseUp(DataGridViewCellMouseEventArgs e)
		{
			resetCellForColor(e.RowIndex, e.ColumnIndex);
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
				switch (Columns[columnIndex].Name)
				{
					case "PutBidPrice":
						Rows[rowIndex].Cells["PutBidPrice"].Style.SelectionBackColor = Color.Blue;
						break;
					case "PutAskPrice":
						Rows[rowIndex].Cells["PutAskPrice"].Style.SelectionBackColor = Color.Blue;
						break;
					case "CallBidPrice":
						Rows[rowIndex].Cells["CallBidPrice"].Style.SelectionBackColor = Color.Blue;
						break;
					case "CallAskPrice":
						Rows[rowIndex].Cells["CallAskPrice"].Style.SelectionBackColor = Color.Blue;
						break;
				}
			}
		}

		protected override void OnRowPrePaint(DataGridViewRowPrePaintEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				SubscribeCheck(e.RowIndex);

				if (Rows[e.RowIndex].Cells["MoneyFlag"].Value != null && Rows[e.RowIndex].Cells["MoneyFlag"].Value.ToString() != "")
				{
					switch ((long)Rows[e.RowIndex].Cells["MoneyFlag"].Value)
					{
						case 1:
							for (int colCount = 0; colCount < Columns.Count; colCount++)
							{
								if (Columns[colCount].Visible && IsMoneyColorColumn(colCount))
								{
									if (Rows[e.RowIndex].Cells[colCount].Style.BackColor != shiftedColors[1])
									{
										Rows[e.RowIndex].Cells[colCount].Style.BackColor = shiftedColors[1];
									}
								}
							}
							break;
						case 2:
							for (int colCount = 0; colCount < Columns.Count; colCount++)
							{
								if (Columns[colCount].Visible && IsMoneyColorColumn(colCount))
								{
									if (Rows[e.RowIndex].Cells[colCount].Style.BackColor != shiftedColors[2])
									{
										Rows[e.RowIndex].Cells[colCount].Style.BackColor = shiftedColors[2];
									}
								}
							}
							break;
						case 3:
							for (int colCount = 0; colCount < Columns.Count; colCount++)
							{
								if (Columns[colCount].Visible && IsMoneyColorColumn(colCount))
								{
									if (Rows[e.RowIndex].Cells[colCount].Style.BackColor != shiftedColors[3])
									{
										Rows[e.RowIndex].Cells[colCount].Style.BackColor = shiftedColors[3];
									}
								}
							}
							break;
						case 4:
							for (int colCount = 0; colCount < Columns.Count; colCount++)
							{
								if (Columns[colCount].Visible && IsMoneyColorColumn(colCount))
								{
									if (Rows[e.RowIndex].Cells[colCount].Style.BackColor != shiftedColors[4])
									{
										Rows[e.RowIndex].Cells[colCount].Style.BackColor = shiftedColors[4];
									}
								}
							}
							break;
						case 5:
							for (int colCount = 0; colCount < Columns.Count; colCount++)
							{
								if (Columns[colCount].Visible && IsMoneyColorColumn(colCount))
								{
									if (Rows[e.RowIndex].Cells[colCount].Style.BackColor != shiftedColors[5])
									{
										Rows[e.RowIndex].Cells[colCount].Style.BackColor = shiftedColors[5];
									}
								}
							}
							break;
						default:
							for (int colCount = 0; colCount < Columns.Count; colCount++)
							{
								if (Columns[colCount].Visible && IsMoneyColorColumn(colCount))
								{
									if (Rows[e.RowIndex].Cells[colCount].Style.BackColor != Color.Black)
									{
										Rows[e.RowIndex].Cells[colCount].Style.BackColor = Color.Black;
									}
								}
							}
							break;
					}
				}

				SetCellForeColor("Put", "Last", e.RowIndex);
				SetCellForeColor("Put", "Bid", e.RowIndex);
				SetCellForeColor("Put", "Ask", e.RowIndex);
				SetCellForeColor("Call", "Bid", e.RowIndex);
				SetCellForeColor("Call", "Ask", e.RowIndex);
				SetCellForeColor("Call", "Last", e.RowIndex);
			}
			base.OnRowPrePaint(e);
		}

		private bool IsMoneyColorColumn(int colCount)
		{
			switch (Columns[colCount].Name)
			{
				case "Strike":
				case "DisplayExpiration":
				case "Expiration":
				case "OptionSymbol":
					return false;
				default:
					return true;
			}
		}

		private void SetCellForeColor(string callPut, string bidAsk, int rowIndex)
		{
			switch (bidAsk)
			{
				case "Last":
					if (Rows[rowIndex].Cells[callPut + bidAsk + "PartID"].Value != null && Rows[rowIndex].Cells[callPut + bidAsk + "PartID"].Value.ToString() != "")
					{
						Rows[rowIndex].Cells[callPut + bidAsk + "Traded"].Style.ForeColor = GetExchangeColor((string)Rows[rowIndex].Cells[callPut + bidAsk + "PartID"].Value);
						Rows[rowIndex].Cells[callPut + bidAsk + "Traded"].Style.SelectionForeColor = Rows[rowIndex].Cells[callPut + bidAsk + "Traded"].Style.ForeColor;
					}
					break;
				case "Bid":
				case "Ask":
					if (Rows[rowIndex].Cells[callPut + bidAsk + "PartID"].Value != null && Rows[rowIndex].Cells[callPut + bidAsk + "PartID"].Value.ToString() != "")
					{
						Rows[rowIndex].Cells[callPut + bidAsk + "Price"].Style.ForeColor = GetExchangeColor((string)Rows[rowIndex].Cells[callPut + bidAsk + "PartID"].Value);
						Rows[rowIndex].Cells[callPut + bidAsk + "Price"].Style.SelectionForeColor = Rows[rowIndex].Cells[callPut + bidAsk + "Price"].Style.ForeColor;

						Rows[rowIndex].Cells[callPut + bidAsk + "Size"].Style.ForeColor = GetExchangeColor((string)Rows[rowIndex].Cells[callPut + bidAsk + "PartID"].Value);
						Rows[rowIndex].Cells[callPut + bidAsk + "Size"].Style.SelectionForeColor = Rows[rowIndex].Cells[callPut + bidAsk + "Size"].Style.ForeColor;
					}
					break;
			}
		}

		private Color GetExchangeColor(string pid)
		{
			return ExchangeColors.TryGetValue(pid, out Color value) ? value : DefaultCellForeColor;
		}

		// This will subscribe option chain as symbol scroll into view.
		private void SubscribeCheck(int rowIndex)
		{
			try
			{
				if (ShouldSubscribe(rowIndex))
				{
					DataGridViewRow row = Rows[rowIndex];

					MDPutSymbol = (string)row.Cells["PutSymbol"].Value;
					if (row.Cells["PutPartID"].Value.ToString() == "")
					{
						MDPutSymbolOPRA = MDPutSymbol;
					}
					else
					{
						MDPutSymbolOPRA = (string)row.Cells["PutSymbol"].Value + "." + (string)row.Cells["PutPartID"].Value;
					}

					MDCallSymbol = (string)row.Cells["CallSymbol"].Value;
					if (row.Cells["CallPartID"].Value.ToString() == "")
					{
						// NBBO
						MDCallSymbolOPRA = MDCallSymbol;
					}
					else
					{
						MDCallSymbolOPRA = MDCallSymbol + "." + (string)row.Cells["CallPartID"].Value;
					}

					MDSource = (string)row.Cells["MDSource"].Value;
					SecType = (string)row.Cells["SecType"].Value;
					row.Cells["Subscribed"].Value = 1;

					Subscribe = true;
				}
			}
			catch
			{
			}
		}

		private bool ShouldSubscribe(int rowIndex)
		{
			if (ShowOnlySelectedExchange)
			{
				if ((long)Rows[rowIndex].Cells["Subscribed"].Value == 0 &&
					(string)Rows[rowIndex].Cells["CallSymbol"].Value != "" &&
					(string)Rows[rowIndex].Cells["PutSymbol"].Value != "" &&
					(string)Rows[rowIndex].Cells["MDSource"].Value != "" &&
					(string)Rows[rowIndex].Cells["SecType"].Value != "")
				{
					return true;
				}
			}
			else
			{
				if ((long)Rows[rowIndex].Cells["Subscribed"].Value == 0 &&
					(string)Rows[rowIndex].Cells["CallSymbol"].Value != "" &&
					(string)Rows[rowIndex].Cells["PutSymbol"].Value != "" &&
					(string)Rows[rowIndex].Cells["CallPartID"].Value == "" &&
					(string)Rows[rowIndex].Cells["PutPartID"].Value == "" &&
					(string)Rows[rowIndex].Cells["MDSource"].Value != "" &&
					(string)Rows[rowIndex].Cells["SecType"].Value != "")
				{
					return true;
				}
			}

			return false;
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

				SearchView = null;
				CallSearchView = null;
				PutSearchView = null;

				CallKeys = null;
				PutKeys = null;

				lock (RocGridTable)
				{
					RocGridTable.Clear();
					RocGridTable = null;
				}

				lock (Columns)
				{
					Columns.Clear();
				}

				_monthColors = null;
				_exchangeColors = null;
			}
			base.Dispose(disposing);
		}

		#endregion

		#region - Row Filter -

		private void BuildRowFilter()
		{
			if (!Importing)
			{
				string filter = "";

				if (ShowOnlySelectedExchange)
				{
					if (FilterOutCBOE)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.CBOE, "' And CallPartID = '", OptionExchangeCode.CBOE, "'"));
					}
					else if (FilterOutBOX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.BOX, "' And CallPartID = '", OptionExchangeCode.BOX, "'"));
					}
					else if (FilterOutAMEX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.AMEX, "' And CallPartID = '", OptionExchangeCode.AMEX, "'"));
					}
					else if (FilterOutPHLX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.PHLX, "' And CallPartID = '", OptionExchangeCode.PHLX, "'"));
					}
					else if (FilterOutPCX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.PCX, "' And CallPartID = '", OptionExchangeCode.PCX, "'"));
					}
					else if (FilterOutISE)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.ISE, "' And CallPartID = '", OptionExchangeCode.ISE, "'"));
					}
					else if (FilterOutNSDQ)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.NSDQ, "' And CallPartID = '", OptionExchangeCode.NSDQ, "'"));
					}
					else if (FilterOutBATS)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.BATS, "' And CallPartID = '", OptionExchangeCode.BATS, "'"));
					}
					else if (FilterOutC2)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID = '", OptionExchangeCode.C2, "' And CallPartID = '", OptionExchangeCode.C2, "'"));
					}
				}
				else
				{
					if (FilterOutCBOE)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.CBOE, "' And CallPartID <> '", OptionExchangeCode.CBOE, "'"));
					}
					if (FilterOutBOX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.BOX, "' And CallPartID <> '", OptionExchangeCode.BOX, "'"));
					}
					if (FilterOutAMEX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.AMEX, "' And CallPartID <> '", OptionExchangeCode.AMEX, "'"));
					}
					if (FilterOutPHLX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.PHLX, "' And CallPartID <> '", OptionExchangeCode.PHLX, "'"));
					}
					if (FilterOutPCX)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.PCX, "' And CallPartID <> '", OptionExchangeCode.PCX, "'"));
					}
					if (FilterOutISE)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.ISE, "' And CallPartID <> '", OptionExchangeCode.ISE, "'"));
					}
					if (FilterOutNSDQ)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.NSDQ, "' And CallPartID <> '", OptionExchangeCode.NSDQ, "'"));
					}
					if (FilterOutBATS)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.BATS, "' And CallPartID <> '", OptionExchangeCode.BATS, "'"));
					}
					if (FilterOutC2)
					{
						filter = BuildRowFilterString(filter, string.Concat("PutPartID <> '", OptionExchangeCode.C2, "' And CallPartID <> '", OptionExchangeCode.C2, "'"));
					}
				}

				try
				{
					_exchangeFilter = filter;
					if (filter != "")
					{
						if (!ShowOnlySelectedExchange)
						{
							filter = BuildRowFilterString(filter, DefaultRowFilter);
						}
					}
					else
					{
						filter = BuildRowFilterString(filter, DefaultRowFilter);
					}

					if (RocGridTable.DefaultView.RowFilter != filter)
					{
						RocGridTable.DefaultView.RowFilter = filter;
					}

					RefreshNBBO = true;

					//RefreshSharedRows = true;
					//RefreshAggragation = true;
					//ShouldScrollToLastRow = true;
				}
				catch (Exception ex)
				{
					Debug.Assert(false, ex.Message, ex.StackTrace);
				}
			}
		}

		#endregion

		#region - Option Grid Functions -

		private SortedSet<Price> _strikes = new SortedSet<Price>();
		public void ClearStrikes()
		{
			lock (_strikes) {
				_strikes.Clear();
			}
		}
		public bool TryAddStrike(Price strike)
		{
			lock (_strikes) {
				if (_strikes.Contains(strike))
					return false;
				_strikes.Add(strike);
				return true;
			}
		}
		public Price LowStrike => _strikes.Count > 0 ? _strikes.Min : Price.UnsetPrice;
		public Price HighStrike => _strikes.Count > 0 ? _strikes.Max : Price.UnsetPrice;
		public double MidStrike =>  _strikes.Count > 0 ? (_strikes.Max.Value + _strikes.Min.Value) / 2.0 : 0;
		public int StrikeCount => _strikes.Count;
		public IEnumerator<Price> GetStrikeEnumerator() => _strikes.GetEnumerator();
		public Price GetStrikeAt(int index)
		{
			foreach (Price strike in _strikes) {
				if (index-- == 0)
					return strike;
			}
			return Price.UnsetPrice;
		}

		private int _strikeRange = 20;
		[Browsable(false)]
		public int StrikeRange
		{
			get
			{
				return _strikeRange;
			}
			set
			{
				_strikeRange = value;
			}
		}

		private int _expirationRange = 1;
		[Browsable(false)]
		public int ExpirationRange
		{
			get
			{
				return _expirationRange;
			}
			set
			{
				_expirationRange = value;
			}
		}

		private SortedSet<DateTime> _expirations = new SortedSet<DateTime>();
		public bool HasExpiration(DateTime when) => _expirations.Contains(when);
		public void AddExpiration(DateTime when)
		{
			lock (_expirations) {
				if (!_expirations.Contains(when))
					_expirations.Add(when);
			}
		}
		public void ClearExpirations()
		{
			lock (_expirations) {
				_expirations.Clear();
			}
		}
		public int ExpirationCount => _expirations.Count;
		public DateTime HighExpiration => _expirations.Max;
		public DateTime LowExpiration => _expirations.Min;
		public IEnumerator<DateTime> GetExpirationEnumerator() => _expirations.GetEnumerator();
		public DateTime GetExpirationAt(int index)
		{
			foreach (DateTime when in _expirations) {
				if (index-- == 0)
					return when;
			}
			return default;
		}

		private bool _optionLoaded = false;
		[Browsable(false)]
		public bool OptionLoaded
		{
			get
			{
				return _optionLoaded;
			}
			set
			{
				_optionLoaded = value;
			}
		}

		private double _commonFactor = 1;
		[Browsable(false)]
		public double CommonFactor
		{
			get
			{
				return _commonFactor;
			}
			set
			{
				_commonFactor = value;
			}
		}

		private double _atMoney = 0;
		[Browsable(false)]
		public double AtMoney
		{
			get
			{
				return _atMoney;
			}
			set
			{
				_atMoney = value;
			}
		}

		private DateTime _currentExpDate;
		[Browsable(false)]
		public DateTime CurrentExpDate
		{
			get
			{
				return _currentExpDate;
			}
			set
			{
				if (_currentExpDate != value)
				{
					_currentExpDate = value;
					if (!CenterOnRow(_currentExpDate, _atMoney))
					{
						onPropertyChanged(new PropertyChangedEventArgs("RelodOptionChain"));
					}
				}
			}
		}

		private double _currentStrike;
		[Browsable(false)]
		public double CurrentStrike
		{
			get
			{
				return _currentStrike;
			}
			set
			{
				if (_currentStrike != value)
				{
					_currentStrike = value;
					//CenterOnRow(_currentExpDate, _currentStrike);
				}
			}
		}

		private List<string> _optionSymbols;
		[Browsable(false)]
		public List<string> OptionSymbols
		{
			get
			{
				if (_optionSymbols == null)
				{
					_optionSymbols = new List<string>();
				}
				return _optionSymbols;
			}
			set
			{
				_optionSymbols = value;
			}
		}

		private int rowsToShow
		{
			get
			{
				return Height / RowTemplate.Height;
			}
		}

		public void CenterOnStrike()
		{
			CenterOnRow(_currentExpDate, _currentStrike);
		}

		private bool CenterOnRow(DateTime expDate, double price)
		{
			try
			{
				lock (RocGridTable)
				{
					DataRow[] rows = RocGridTable.Select("Expiration = '#" + expDate.ToString() + "#' And Strike >= " + price);
					if (rows.Length > 0)
					{
						if (rows[0]["Strike"] != null && (double)rows[0]["Strike"] != price)
						{
							price = (double)rows[0]["Strike"];
							OptionStrike = price;
						}
					}
				}
			}
			catch (Exception ex)
			{
				Debug.Print(ex.Message);
			}

			bool result = false;
			if (expDate != null && _atMoney > 0)
			{
				foreach (DataGridViewRow row in Rows)
				{
					if (row.Cells["Expiration"].Value != null &&
						row.Cells["Strike"].Value != null &&
						((DateTime)row.Cells["Expiration"].Value).Month == expDate.Month &&
						((DateTime)row.Cells["Expiration"].Value).Day == expDate.Day &&
						(double)row.Cells["Strike"].Value == price)
					{
						CenterOnRow(row.Index);
						result = true;
						break;
					}
				}
			}
			return result;
		}

		public void CenterOnRow(double price)
		{
			CenterOnRow(_currentExpDate, price);
			//foreach (DataGridViewRow row in Rows)
			//{
			//    if (row.Cells["Strike"].Value != null && (double)row.Cells["Strike"].Value == price)
			//    {
			//        CenterOnRow(row.Index);
			//        break;
			//    }
			//}
		}

		private void CenterOnRow(int rowIndex)
		{
			if (Rows.Count > 0 && Rows.Count > rowIndex)
			{
				if (rowIndex >= 0)
				{
					try
					{
						int firstDisplayRow = (rowIndex - (rowsToShow / 2) < 0 ? 0 : rowIndex - (rowsToShow / 2));
						if (Math.Abs(firstDisplayRow - FirstDisplayedScrollingRowIndex) > 2)
						{
							FirstDisplayedScrollingRowIndex = firstDisplayRow;
						}
					}
					catch (Exception ex)
					{
						if (Debugger.IsAttached)
						{
							Debug.Print(ex.Message);
							Debug.Print(ex.StackTrace);
						}
					}
				}
			}
		}

		private bool _show64Th = false;
		public bool Show64Th
		{
			get
			{
				return _show64Th;
			}
			set
			{
				_show64Th = value;
				lock (Columns)
				{
					if (Columns.Contains("CallBidPrice"))
					{
						((DGVOptionListNotifiedNumberColumn)Columns["CallBidPrice"]).Is64Th = _show64Th;
					}
					if (Columns.Contains("CallAskPrice"))
					{
						((DGVOptionListNotifiedNumberColumn)Columns["CallAskPrice"]).Is64Th = _show64Th;
					}
					if (Columns.Contains("PutBidPrice"))
					{
						((DGVOptionListNotifiedNumberColumn)Columns["PutBidPrice"]).Is64Th = _show64Th;
					}
					if (Columns.Contains("PutAskPrice"))
					{
						((DGVOptionListNotifiedNumberColumn)Columns["PutAskPrice"]).Is64Th = _show64Th;
					}
				}
				Invalidate();
			}
		}

		private bool _showOptionSymbolColumn = false;
		public bool ShowOptionSymbolColumn
		{
			get
			{
				return _showOptionSymbolColumn;
			}
			set
			{
				_showOptionSymbolColumn = value;
				lock (Columns)
				{
					if (Columns.Contains("OptionSymbol"))
					{
						if (Columns["OptionSymbol"].Visible != _showOptionSymbolColumn)
						{
							Columns["OptionSymbol"].Visible = _showOptionSymbolColumn;
							Invalidate();
						}
					}
				}
			}
		}

		#endregion
	}
}
