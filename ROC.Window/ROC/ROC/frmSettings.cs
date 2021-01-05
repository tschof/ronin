using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Windows.Forms;

using Common;
using FormEx;
using CSVEx;
using ContextMenuEx;
using RDSEx;
using PanelEx;
using System.Media;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmSettings : VistaWindowBorder
	{
		#region - Local Variable -

		private object syncObject = new object();

		// Used by resubscribe
		private string resubscribeStatus = "";
		private int resubscribeRate = 50;

		private bool _stockSymbolsLoaded = false;
		private bool _futureSymbolsLoaded = false;
		private bool _optionSymbolsLoaded = false;
		private bool _windowSettingsLoaded = false;
		private bool _soundSettingsLoaded = false;
		private bool _ordersSettingsLoaded = false;
		private bool _stockTicketQuickButtonSettingsLoaded = false;
		private bool _futureTicketQuickButtonSettingsLoaded = false;
		private bool _optionTicketQuickButtonSettingsLoaded = false;
		private bool _processSettingsLoaded = false;

		#endregion

		#region - Constructor -

		public frmSettings()
		{
			InitializeComponent();

			Initialize();
		}

		private void Initialize()
		{
			FormClosing += new FormClosingEventHandler(Form_FormClosing);
			UITimerTicked += new EventHandler(Form_UITimerTicked);
			Load +=new EventHandler(Form_Load);

			FitClicked += new EventHandler(Form_FitClicked);

			rocSymbolSettingList.Initialize(GLOBAL.HSymbolSettingData.SettingTable);
			rocSymbolSettingList.CellMouseClick += new DataGridViewCellMouseEventHandler(rocSymbolSettingList_CellMouseClick);

			rocQuickButtonList.Initialize(GLOBAL.HQuickButtonSettingData.SettingTable);
			rocQuickButtonList.CellMouseClick += new DataGridViewCellMouseEventHandler(rocQuickButtonList_CellMouseClick);

			DGVAutoFocusSettings.SelectionChanged += new EventHandler(DGVAutoFocusSettings_SelectionChanged);
			DGVAutoFocusSettings.CellMouseDown += new DataGridViewCellMouseEventHandler(DGVAutoFocusSettings_CellMouseDown);
			DGVAutoFocusSettings.MouseEnter += new EventHandler(DGVAutoFocusSettings_MouseEnter);

			DGVSounds.SelectionChanged += new EventHandler(DGVSounds_SelectionChanged);
			DGVSounds.CellMouseDown += new DataGridViewCellMouseEventHandler(DGVSounds_CellMouseDown);
			DGVSounds.MouseEnter += new EventHandler(DGVSounds_MouseEnter);

			resubscribeRate = Int32.Parse(cboResubscribeRate.Text);

			StartTimer();
		}

		#endregion

		#region - Form Event -

		private void Form_Load(object sender, EventArgs e)
		{
			foreach (DataRow row in GLOBAL.HSymbolSettingData.SettingTable.Rows)
			{
				rocSymbolSettingList.RocGridTable.LoadDataRow(row.ItemArray, true);
			}

			foreach (DataRow row in GLOBAL.HQuickButtonSettingData.SettingTable.Rows)
			{
				rocQuickButtonList.RocGridTable.LoadDataRow(row.ItemArray, true);
			}

			SetSettingPanel(managedPanelSymbolSettings);
			LoadStockSymbols();
		}

		private void Form_UITimerTicked(object sender, EventArgs e)
		{
			if (resubscribeStatus != "")
			{
				lblResubscribeStatus.Text = resubscribeStatus;
			}
			else
			{
				if (lblResubscribeStatus.Text != "")
				{
					lblResubscribeStatus.Text = "";
				}
			}
		}

		private void Form_FitClicked(object sender, EventArgs e)
		{
			try
			{
				if (panelManagerSettings.SelectedPanel == managedPanelSymbolSettings)
				{
					lock (rocSymbolSettingList)
					{
						SetColumnMode(rocSymbolSettingList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocSymbolSettingList);
						SetColumnMode(rocSymbolSettingList, DataGridViewAutoSizeColumnMode.None);
					}
				}
				else if (panelManagerSettings.SelectedPanel == managedPaneWindowSettings)
				{
					lock (DGVAutoFocusSettings)
					{
						SetColumnMode(DGVAutoFocusSettings, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(DGVAutoFocusSettings);
						SetColumnMode(DGVAutoFocusSettings, DataGridViewAutoSizeColumnMode.None);
					}
				}
				else if (panelManagerSettings.SelectedPanel == managedPanelSoundSettings)
				{
					lock (DGVSounds)
					{
						SetColumnMode(DGVSounds, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(DGVSounds);
						SetColumnMode(DGVSounds, DataGridViewAutoSizeColumnMode.None);
					}
				}
				else if (panelManagerSettings.SelectedPanel == managedPanelQuickButtonSettings)
				{
					lock (rocQuickButtonList)
					{
						SetColumnMode(rocQuickButtonList, DataGridViewAutoSizeColumnMode.DisplayedCells);
						ResizeToFit(rocQuickButtonList);
						SetColumnMode(rocQuickButtonList, DataGridViewAutoSizeColumnMode.None);
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void Form_FormClosing(object sender, FormClosingEventArgs e)
		{
			StopTimer();

			GLOBAL.HWindows.CloseWindow(this);
			Dispose();
		}

		// Panel Selection
		private void cmdSymbols_Click(object sender, EventArgs e)
		{
			SetSettingPanel(managedPanelSymbolSettings);
			cmdStockSymbols_Click(null, EventArgs.Empty);
		}

		private void cmdWindows_Click(object sender, EventArgs e)
		{
			SetSettingPanel(managedPaneWindowSettings);
			LoadWindowSettings();
		}

		private void cmdSounds_Click(object sender, EventArgs e)
		{
			SetSettingPanel(managedPanelSoundSettings);
			LoadSoundSettings();
		}

		private void cmdOrders_Click(object sender, EventArgs e)
		{
			SetSettingPanel(managedPanelOrderSettings);
			LoadOrdersSettings();
		}

		private void cmdQuickButton_Click(object sender, EventArgs e)
		{
			SetSettingPanel(managedPanelQuickButtonSettings);
			cmdStockTicket_Click(null, EventArgs.Empty);
		}

		private void cmdProcess_Click(object sender, EventArgs e)
		{
			SetSettingPanel(managedPanelProcessSettings);
			LoadProcessSettings();
		}

		// Load
		private void SetSettingPanel(ManagedPanel panel)
		{
			panelManagerSettings.SelectedPanel = panel;

			if (panelManagerSettings.SelectedPanel == managedPanelSymbolSettings)
			{
				cmdSymbols.Enabled = false;
				cmdWindows.Enabled = true;
				cmdSounds.Enabled = true;
				cmdOrders.Enabled = true;
				cmdQuickQty.Enabled = true;
				cmdProcess.Enabled = true;
			}
			else if (panelManagerSettings.SelectedPanel == managedPaneWindowSettings)
			{
				cmdSymbols.Enabled = true;
				cmdWindows.Enabled = false;
				cmdSounds.Enabled = true;
				cmdOrders.Enabled = true;
				cmdQuickQty.Enabled = true;
				cmdProcess.Enabled = true;
			}
			else if (panelManagerSettings.SelectedPanel == managedPanelSoundSettings)
			{
				cmdSymbols.Enabled = true;
				cmdWindows.Enabled = true;
				cmdSounds.Enabled = false;
				cmdOrders.Enabled = true;
				cmdQuickQty.Enabled = true;
				cmdProcess.Enabled = true;
			}
			else if (panelManagerSettings.SelectedPanel == managedPanelOrderSettings)
			{
				cmdSymbols.Enabled = true;
				cmdWindows.Enabled = true;
				cmdSounds.Enabled = true;
				cmdOrders.Enabled = false;
				cmdQuickQty.Enabled = true;
				cmdProcess.Enabled = true;
			}
			else if (panelManagerSettings.SelectedPanel == managedPanelQuickButtonSettings)
			{
				cmdSymbols.Enabled = true;
				cmdWindows.Enabled = true;
				cmdSounds.Enabled = true;
				cmdOrders.Enabled = true;
				cmdQuickQty.Enabled = false;
				cmdProcess.Enabled = true;
			}
			else if (panelManagerSettings.SelectedPanel == managedPanelProcessSettings)
			{
				cmdSymbols.Enabled = true;
				cmdWindows.Enabled = true;
				cmdSounds.Enabled = true;
				cmdOrders.Enabled = true;
				cmdQuickQty.Enabled = true;
				cmdProcess.Enabled = false;
			}
		}

		// Exit
		private void cmdApply_Click(object sender, EventArgs e)
		{
			SaveSymbolSettings();
			SaveWindowSettings();
			SaveSoundSettings();
			SaveOrdersSettings();
			SaveQucikButtonSettings();
			SaveProcessSettings();

			GLOBAL.UserUIProfile.ReloadUI();

			Close();
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		#endregion

		#region - Symbols -

		private void cmdStockSymbols_Click(object sender, EventArgs e)
		{
			cmdStockSymbols.Enabled = false;
			cmdFutureSymbols.Enabled = true;
			cmdOptionSymbols.Enabled = true;

			LoadStockSymbols();
		}

		private void cmdFutureSymbols_Click(object sender, EventArgs e)
		{
			cmdStockSymbols.Enabled = true;
			cmdFutureSymbols.Enabled = false;
			cmdOptionSymbols.Enabled = true;

			LoadFutureSymbols();
		}

		private void cmdOptionSymbols_Click(object sender, EventArgs e)
		{
			cmdStockSymbols.Enabled = true;
			cmdFutureSymbols.Enabled = true;
			cmdOptionSymbols.Enabled = false;

			LoadOptionSymbols();
		}

		#region - Symbol Grid Event -

		private void rocSymbolSettingList_CellMouseClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Right:
						switch (rocSymbolSettingList.Columns[e.ColumnIndex].Name)
						{
							case "Trader":
								ShowTraderMenu();
								break;
							case "Account":
								if (rocSymbolSettingList["Trader", e.RowIndex].Value != null &&
									rocSymbolSettingList["Trader", e.RowIndex].Value.ToString() != "")
								{
									ShowAccountMenu(rocSymbolSettingList["Trader", e.RowIndex].Value.ToString().ToUpper());
								}
								break;
							case "Exchange":
								if (rocSymbolSettingList["Trader", e.RowIndex].Value != null &&
									rocSymbolSettingList["Trader", e.RowIndex].Value.ToString() != "" &&
									rocSymbolSettingList["Account", e.RowIndex].Value != null &&
									rocSymbolSettingList["Account", e.RowIndex].Value.ToString() != "")
								{
									ShowExchangeMenu(
										rocSymbolSettingList["Trader", e.RowIndex].Value.ToString().ToUpper(),
										rocSymbolSettingList["Account", e.RowIndex].Value.ToString().ToUpper());
								}
								break;
						}
						break;
				}
			}
		}

		#endregion

		#region - Row TraderFor Menu -

		private menuBaseAction _menuTraderActions;
		private PropertyContainer _menuTraderActionsContainer;
		private void ShowTraderMenu()
		{
			if (_menuTraderActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("", "");
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					items.Add(trader.tradeFor.ToUpper(), trader.tradeFor.ToUpper());
				}

				_menuTraderActions = new menuBaseAction(items, true);
				_menuTraderActions.PropertyChanged += new PropertyChangedEventHandler(menuTraderActions_PropertyChanged);
			}

			if (_menuTraderActionsContainer == null)
			{
				_menuTraderActionsContainer = new PropertyContainer(_menuTraderActions);
				_menuTraderActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuTraderActionsContainer_Closed);
			}

			_menuTraderActionsContainer.Show(Cursor.Position);
		}

		private void menuTraderActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			string tradeFor = e.PropertyName;

			foreach (DataGridViewRow row in rocSymbolSettingList.SelectedRows)
			{
				if (row.Cells["Symbol"].Value != null &&
					row.Cells["Symbol"].Value.ToString() != "")
				{
					UpdateDataRowBySymbol(row.Cells["Symbol"].Value.ToString().ToUpper(), "Trader", tradeFor);
				}
			}

			_menuTraderActionsContainer.Close();
		}

		private void menuTraderActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuTraderActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuTraderActionsContainer_Closed);
			_menuTraderActionsContainer = null;
			_menuTraderActions.PropertyChanged -= new PropertyChangedEventHandler(menuTraderActions_PropertyChanged);
			_menuTraderActions = null;
		}

		#endregion

		#region - Row Account Menu -

		private menuBaseAction _menuAccountActions;
		private PropertyContainer _menuAccountActionsContainer;
		private void ShowAccountMenu(string tradeFor)
		{
			if (_menuAccountActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("", "");
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (trader.tradeFor.ToUpper() == tradeFor)
					{
						foreach (AccountMap acctMap in trader.CSAccounts.Values)
							items.TryAdd(acctMap.account, acctMap.account);

						foreach (AccountMap acctMap in trader.FUTAccounts.Values)
							items.TryAdd(acctMap.account, acctMap.account);

						foreach (AccountMap acctMap in trader.OPTAccounts.Values)
							items.TryAdd(acctMap.account, acctMap.account);

						break;
					}
				}

				_menuAccountActions = new menuBaseAction(items, true);
				_menuAccountActions.PropertyChanged += new PropertyChangedEventHandler(menuAccountActions_PropertyChanged);
			}

			if (_menuAccountActionsContainer == null)
			{
				_menuAccountActionsContainer = new PropertyContainer(_menuAccountActions);
				_menuAccountActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuAccountActionsContainer_Closed);
			}

			_menuAccountActionsContainer.Show(Cursor.Position);
		}

		private void menuAccountActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			string account = e.PropertyName;

			foreach (DataGridViewRow row in rocSymbolSettingList.SelectedRows)
			{
				if (row.Cells["Symbol"].Value != null &&
					row.Cells["Symbol"].Value.ToString() != "")
				{
					UpdateDataRowBySymbol(row.Cells["Symbol"].Value.ToString().ToUpper(), "Account", account);
				}
			}

			_menuAccountActionsContainer.Close();
		}

		private void menuAccountActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuAccountActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuAccountActionsContainer_Closed);
			_menuAccountActionsContainer = null;
			_menuAccountActions.PropertyChanged -= new PropertyChangedEventHandler(menuAccountActions_PropertyChanged);
			_menuAccountActions = null;
		}

		#endregion

		#region - Row Exchange Menu -

		private menuBaseAction _menuExchangeActions;
		private PropertyContainer _menuExchangeActionsContainer;
		private void ShowExchangeMenu(string tradeFor, string account)
		{
			if (_menuExchangeActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("", "");
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (trader.tradeFor.ToUpper() == tradeFor)
					{
						foreach (AccountMap acctMap in trader.CSAccounts.Values)
						{
							if (acctMap.account.ToUpper() == account)
							{
								foreach (DestinationMap destMap in acctMap.Destinations)
									items.TryAdd(destMap.shortName, destMap.shortName);
							}
						}

						foreach (AccountMap acctMap in trader.FUTAccounts.Values)
						{
							if (acctMap.account.ToUpper() == account)
							{
								foreach (DestinationMap destMap in acctMap.Destinations)
									items.TryAdd(destMap.shortName, destMap.shortName);
							}
						}

						foreach (AccountMap acctMap in trader.OPTAccounts.Values)
						{
							if (acctMap.account.ToUpper() == account)
							{
								foreach (DestinationMap destMap in acctMap.Destinations)
									items.TryAdd(destMap.shortName, destMap.shortName);
							}
						}

						break;
					}
				}

				_menuExchangeActions = new menuBaseAction(items, true);
				_menuExchangeActions.PropertyChanged += new PropertyChangedEventHandler(menuExchangeActions_PropertyChanged);
			}

			if (_menuExchangeActionsContainer == null)
			{
				_menuExchangeActionsContainer = new PropertyContainer(_menuExchangeActions);
				_menuExchangeActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuExchangeActionsContainer_Closed);
			}

			_menuExchangeActionsContainer.Show(Cursor.Position);
		}

		private void menuExchangeActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			string exchange = e.PropertyName;
			DataRow[] symbolRows = new DataRow[0];

			foreach (DataGridViewRow row in rocSymbolSettingList.SelectedRows)
			{
				if (row.Cells["Symbol"].Value != null &&
					row.Cells["Symbol"].Value.ToString() != "")
				{
					UpdateDataRowBySymbol(row.Cells["Symbol"].Value.ToString().ToUpper(), "Exchange", exchange);
				}
			}

			_menuExchangeActionsContainer.Close();
		}

		private void menuExchangeActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuExchangeActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuExchangeActionsContainer_Closed);
			_menuExchangeActionsContainer = null;
			_menuExchangeActions.PropertyChanged -= new PropertyChangedEventHandler(menuExchangeActions_PropertyChanged);
			_menuExchangeActions = null;
		}

		#endregion

		private void UpdateDataRowBySymbol(string symbol, string colName, string value)
		{
			DataRow[] symbolRows = rocSymbolSettingList.RocGridTable.Select("Symbol = '" + symbol + "'");

			foreach (DataRow symbolRow in symbolRows)
			{
				switch (colName)
				{
					case "Trader":
						symbolRow["Account"] = "";
						symbolRow["Exchange"] = "";
						if (symbolRow["Symbol"].ToString().Contains("_DEF"))
						{
							rocSymbolSettingList.RocGridTable.Columns["Trader"].DefaultValue = value;
							rocSymbolSettingList.RocGridTable.Columns["Account"].DefaultValue = "";
							rocSymbolSettingList.RocGridTable.Columns["Exchange"].DefaultValue = "";
						}
						break;
					case "Account":
						symbolRow["Exchange"] = "";
						if (symbolRow["Symbol"].ToString().Contains("_DEF"))
						{
							rocSymbolSettingList.RocGridTable.Columns["Account"].DefaultValue = value;
							rocSymbolSettingList.RocGridTable.Columns["Exchange"].DefaultValue = "";
						}
						break;
					case "Exchange":
						if (symbolRow["Symbol"].ToString().Contains("_DEF"))
						{
							rocSymbolSettingList.RocGridTable.Columns["Exchange"].DefaultValue = value;
						}
						break;
				}
				symbolRow[colName] = value;
			}
		}

		private void LoadStockSymbols()
		{
			_stockSymbolsLoaded = true;

			rocSymbolSettingList.RocGridTable.DefaultView.RowFilter = string.Format("SecType = '{0}'", CSVFieldIDs.SecurityTypes.Equity);
			rocSymbolSettingList.RocGridTable.Columns["SecType"].DefaultValue = CSVFieldIDs.SecurityTypes.Equity;

			rocSymbolSettingList.RocGridTable.Columns["Quantity"].DefaultValue = 100;
			rocSymbolSettingList.RocGridTable.Columns["QuantityIncrement"].DefaultValue = 100;

			rocSymbolSettingList.Columns["StopLimitOffset"].Visible = false;
			rocSymbolSettingList.Columns["PegPriceIncrement"].Visible = true;
			rocSymbolSettingList.Columns["Show"].Visible = true;
			rocSymbolSettingList.Columns["ShowIncrement"].Visible = true;
			rocSymbolSettingList.Columns["UseShow"].Visible = true;
			rocSymbolSettingList.Columns["ShortLender"].Visible = true;

			SetDefaultUserInfo(TicketDefaults.Stock);

			Form_FitClicked(null, EventArgs.Empty);
		}

		private void LoadFutureSymbols()
		{
			_futureSymbolsLoaded = true;

			rocSymbolSettingList.RocGridTable.DefaultView.RowFilter = string.Format("SecType = '{0}'", CSVFieldIDs.SecurityTypes.Future);
			rocSymbolSettingList.RocGridTable.Columns["SecType"].DefaultValue = CSVFieldIDs.SecurityTypes.Future;

			rocSymbolSettingList.RocGridTable.Columns["Quantity"].DefaultValue = 1;
			rocSymbolSettingList.RocGridTable.Columns["QuantityIncrement"].DefaultValue = 1;

			rocSymbolSettingList.Columns["StopLimitOffset"].Visible = true;

			rocSymbolSettingList.Columns["PegPriceIncrement"].Visible = false;
			rocSymbolSettingList.Columns["Show"].Visible = false;
			rocSymbolSettingList.Columns["ShowIncrement"].Visible = false;
			rocSymbolSettingList.Columns["UseShow"].Visible = false;
			rocSymbolSettingList.Columns["ShortLender"].Visible = false;

			SetDefaultUserInfo(TicketDefaults.Future);

			Form_FitClicked(null, EventArgs.Empty);
		}

		private void LoadOptionSymbols()
		{
			_optionSymbolsLoaded = true;

			rocSymbolSettingList.RocGridTable.DefaultView.RowFilter = string.Format("SecType = '{0}'", CSVFieldIDs.SecurityTypes.Option);
			rocSymbolSettingList.RocGridTable.Columns["SecType"].DefaultValue = CSVFieldIDs.SecurityTypes.Option;

			rocSymbolSettingList.RocGridTable.Columns["Quantity"].DefaultValue = 1;
			rocSymbolSettingList.RocGridTable.Columns["QuantityIncrement"].DefaultValue = 1;

			rocSymbolSettingList.Columns["StopLimitOffset"].Visible = false;
			rocSymbolSettingList.Columns["PegPriceIncrement"].Visible = false;
			rocSymbolSettingList.Columns["Show"].Visible = false;
			rocSymbolSettingList.Columns["ShowIncrement"].Visible = false;
			rocSymbolSettingList.Columns["UseShow"].Visible = false;
			rocSymbolSettingList.Columns["ShortLender"].Visible = false;

			SetDefaultUserInfo(TicketDefaults.Option);

			Form_FitClicked(null, EventArgs.Empty);
		}

		private void SaveSymbolSettings()
		{
			if (_stockSymbolsLoaded ||
				_futureSymbolsLoaded ||
				_optionSymbolsLoaded)
			{
				try
				{
					GLOBAL.HSymbolSettingData.SettingTable.Clear();
					rocSymbolSettingList.RocGridTable.AcceptChanges();
					foreach (DataRow row in rocSymbolSettingList.RocGridTable.Rows)
					{
						GLOBAL.HSymbolSettingData.SettingTable.LoadDataRow(row.ItemArray, true);
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void SetDefaultUserInfo(string defaultSymbol)
		{
			DataRow[] rows = rocSymbolSettingList.RocGridTable.Select("Symbol = '" + defaultSymbol + "'");
			if (rows.Length > 0)
			{
				SetDefaultUserInfo("Trader", rows[0]);
				SetDefaultUserInfo("Account", rows[0]);
				SetDefaultUserInfo("Exchange", rows[0]);
			}
		}

		private void SetDefaultUserInfo(string field, DataRow row)
		{
			if (row[field] != null)
			{
				rocSymbolSettingList.RocGridTable.Columns[field].DefaultValue = row[field].ToString().ToUpper();
			}
		}

		#endregion

		#region - Windows -

		private void LoadWindowSettings()
		{
			_windowSettingsLoaded = true;

			DGVAutoFocusSettings.Rows.Clear();
			DGVAutoFocusSettings.Rows.Add("Watch List", Configuration.ROC.Default.EnableAutoFocusWatchList);
			DGVAutoFocusSettings.Rows.Add("Plot List", Configuration.ROC.Default.EnableAutoFocusPlotList);
			DGVAutoFocusSettings.Rows.Add("Stock Ticket", Configuration.ROC.Default.EnableAutoFocusStockTicket);
			DGVAutoFocusSettings.Rows.Add("Future Ticket", Configuration.ROC.Default.EnableAutoFocusFutureTicket);
			DGVAutoFocusSettings.Rows.Add("Option Ticket", Configuration.ROC.Default.EnableAutoFocusOptionTicket);
			DGVAutoFocusSettings.Rows.Add("Quick Ticket", Configuration.ROC.Default.EnableAutoFocusQuickTicket);
			DGVAutoFocusSettings.Rows.Add("Batch Ticket", Configuration.ROC.Default.EnableAutoFocusBatchTicket);
			DGVAutoFocusSettings.Rows.Add("MDBatch Ticket", Configuration.ROC.Default.EnableAutoFocusMarketDataBatchTicket);
			DGVAutoFocusSettings.Rows.Add("Auto Spread Ticket", Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket);
			DGVAutoFocusSettings.Rows.Add("Orders", Configuration.ROC.Default.EnableAutoFocusOrders);
			DGVAutoFocusSettings.Rows.Add("Trades", Configuration.ROC.Default.EnableAutoFocusTrades);
			DGVAutoFocusSettings.Rows.Add("Positions", Configuration.ROC.Default.EnableAutoFocusPositions);
			DGVAutoFocusSettings.Rows.Add("Show Level2 As Default", Configuration.ROC.Default.ShowLevel2AsDefault);
			DGVAutoFocusSettings.Rows.Add("Show QuickTicket As Default", Configuration.ROC.Default.ShowQuickTicketAsDefault);
			DGVAutoFocusSettings.Rows.Add("QuickTicket AutoCenter", Configuration.ROC.Default.EnableQuickTicketAutoCenter);

			Form_FitClicked(null, EventArgs.Empty);
		}

		private void SaveWindowSettings()
		{
			if (_windowSettingsLoaded)
			{
				try
				{
					for (int rowCount = 0; rowCount < DGVAutoFocusSettings.Rows.Count; rowCount++)
					{
						switch (DGVAutoFocusSettings.Rows[rowCount].Cells["Window"].Value.ToString())
						{
							case "Watch List":
								Configuration.ROC.Default.EnableAutoFocusWatchList = GetEnableFlag(rowCount);
								break;
							case "Plot List":
								Configuration.ROC.Default.EnableAutoFocusPlotList = GetEnableFlag(rowCount);
								break;
							case "Stock Ticket":
								Configuration.ROC.Default.EnableAutoFocusStockTicket = GetEnableFlag(rowCount);
								break;
							case "Future Ticket":
								Configuration.ROC.Default.EnableAutoFocusFutureTicket = GetEnableFlag(rowCount);
								break;
							case "Option Ticket":
								Configuration.ROC.Default.EnableAutoFocusOptionTicket = GetEnableFlag(rowCount);
								break;
							case "Quick Ticket":
								Configuration.ROC.Default.EnableAutoFocusQuickTicket = GetEnableFlag(rowCount);
								break;
							case "Batch Ticket":
								Configuration.ROC.Default.EnableAutoFocusBatchTicket = GetEnableFlag(rowCount);
								break;
							case "MDBatch Ticket":
								Configuration.ROC.Default.EnableAutoFocusMarketDataBatchTicket = GetEnableFlag(rowCount);
								break;
							case "Auto Spread Ticket":
								Configuration.ROC.Default.EnableAutoFocusAutoSpreadTicket = GetEnableFlag(rowCount);
								break;
							case "Orders":
								Configuration.ROC.Default.EnableAutoFocusOrders = GetEnableFlag(rowCount);
								break;
							case "Trades":
								Configuration.ROC.Default.EnableAutoFocusTrades = GetEnableFlag(rowCount);
								break;
							case "Positions":
								Configuration.ROC.Default.EnableAutoFocusPositions = GetEnableFlag(rowCount);
								break;
							case "Show Level2 As Default":
								Configuration.ROC.Default.ShowLevel2AsDefault = GetEnableFlag(rowCount);
								break;
							case "Show QuickTicket As Default":
								Configuration.ROC.Default.ShowQuickTicketAsDefault = GetEnableFlag(rowCount);
								break;
							case "QuickTicket AutoCenter":
								Configuration.ROC.Default.EnableQuickTicketAutoCenter = GetEnableFlag(rowCount);
								break;
						}
					}
					Configuration.ROC.Default.Save();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		#region - Window Grid Event -

		private void DGVAutoFocusSettings_SelectionChanged(object sender, EventArgs e)
		{
			// Make sure nothing is selected
			foreach (DataGridViewCell cell in DGVAutoFocusSettings.SelectedCells)
			{
				cell.Selected = false;
			}
		}

		private void DGVAutoFocusSettings_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
			}
		}

		private void DGVAutoFocusSettings_MouseEnter(object sender, EventArgs e)
		{
			ActivateGrid(DGVAutoFocusSettings);
		}

		#endregion

		private bool GetEnableFlag(int rowCount)
		{
			if (DGVAutoFocusSettings.Rows[rowCount].Cells["Enable"].Value != null)
			{
				return (bool)DGVAutoFocusSettings.Rows[rowCount].Cells["Enable"].Value;
			}
			else
			{
				return false;
			}
		}

		#endregion

		#region - Sounds -

		private void LoadSoundSettings()
		{
			_soundSettingsLoaded = true;

			DGVSounds.Rows.Clear();
			DGVSounds.Rows.Add("Open", Configuration.SOUND.Default.Open.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableOpen);
			DGVSounds.Rows.Add("Replaced", Configuration.SOUND.Default.Replaced.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableReplaced);
			DGVSounds.Rows.Add("Filled", Configuration.SOUND.Default.Filled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableFilled);
			DGVSounds.Rows.Add("FilledAndCanceled", Configuration.SOUND.Default.FilledAndCanceled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableCanceled);
			DGVSounds.Rows.Add("PartiallyFilled", Configuration.SOUND.Default.PartiallyFilled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnablePartiallyFilled);
			DGVSounds.Rows.Add("Rejected", Configuration.SOUND.Default.Rejected.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableRejected);
			DGVSounds.Rows.Add("Canceled", Configuration.SOUND.Default.Canceled.Replace(@"..\", ""), "New...", "Listen", Configuration.SOUND.Default.EnableCanceled);
			
			Form_FitClicked(null, EventArgs.Empty);
		}

		private void SaveSoundSettings()
		{
			if (_soundSettingsLoaded)
			{
				try
				{
					for (int rowCount = 0; rowCount < DGVSounds.Rows.Count; rowCount++)
					{
						switch (DGVSounds.Rows[rowCount].Cells["Type"].Value.ToString())
						{
							case "Open":
								Configuration.SOUND.Default.Open = GetSoundPath(rowCount);
								Configuration.SOUND.Default.EnableOpen = GetSoundEnableFlag(rowCount);
								break;
							case "Replaced":
								Configuration.SOUND.Default.Replaced = GetSoundPath(rowCount);
								Configuration.SOUND.Default.EnableReplaced = GetSoundEnableFlag(rowCount);
								break;
							case "Filled":
								Configuration.SOUND.Default.Filled = GetSoundPath(rowCount);
								Configuration.SOUND.Default.EnableFilled = GetSoundEnableFlag(rowCount);
								break;
							case "FilledAndCanceled":
								Configuration.SOUND.Default.FilledAndCanceled = GetSoundPath(rowCount);
								Configuration.SOUND.Default.EnableFilledAndCanceled = GetSoundEnableFlag(rowCount);
								break;
							case "PartiallyFilled":
								Configuration.SOUND.Default.PartiallyFilled = GetSoundPath(rowCount);
								Configuration.SOUND.Default.EnablePartiallyFilled = GetSoundEnableFlag(rowCount);
								break;
							case "Rejected":
								Configuration.SOUND.Default.Rejected = GetSoundPath(rowCount);
								Configuration.SOUND.Default.EnableRejected = GetSoundEnableFlag(rowCount);
								break;
							case "Canceled":
								Configuration.SOUND.Default.Canceled = GetSoundPath(rowCount);
								Configuration.SOUND.Default.EnableCanceled = GetSoundEnableFlag(rowCount);
								break;
						}
					}
					Configuration.SOUND.Default.Save();
					GLOBAL.ROCSounds.ReloadSound();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		#region - Sound Grid Event -

		private void DGVSounds_SelectionChanged(object sender, EventArgs e)
		{
			// Make sure nothing is selected
			foreach (DataGridViewCell cell in DGVSounds.SelectedCells)
			{
				cell.Selected = false;
			}
		}

		private void DGVSounds_CellMouseDown(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (DGVSounds.Columns[e.ColumnIndex].Name)
				{
					case "Browse":
						OpenFileDialog ofd = new OpenFileDialog();
						ofd.DefaultExt = "Sound";
						ofd.Filter = "Wav File (*.wav)|*.wav";
						ofd.AddExtension = true;
						ofd.RestoreDirectory = true;
						ofd.Title = "Load New Sound From";

						if (DGVSounds.Rows[e.RowIndex].Cells["Path"].Value != null && DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString() != "")
						{
							ofd.InitialDirectory = DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString();
						}
						else
						{
							ofd.InitialDirectory = "Sounds";
						}

						if (ofd.ShowDialog() == DialogResult.OK)
						{
							DGVSounds.Rows[e.RowIndex].Cells["Path"].Value = ofd.FileName;
						}
						break;
					case "Test":
						if (DGVSounds.Rows[e.RowIndex].Cells["Path"].Value != null && DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString() != "")
						{
							SoundPlayer sp = new SoundPlayer(DGVSounds.Rows[e.RowIndex].Cells["Path"].Value.ToString().Replace(@"..\", ""));
							sp.Play();
						}
						break;
				}
			}
		}

		private void DGVSounds_MouseEnter(object sender, EventArgs e)
		{
			ActivateGrid(DGVSounds);
		}

		#endregion

		private string GetSoundPath(int rowCount)
		{
			if (DGVSounds.Rows[rowCount].Cells["Path"].Value != null)
			{
				return DGVSounds.Rows[rowCount].Cells["Path"].Value.ToString();
			}
			else
			{
				return @"..\Sounds\NoSound.WAV";
			}
		}

		private bool GetSoundEnableFlag(int rowCount)
		{
			if (DGVSounds.Rows[rowCount].Cells["SoundEnable"].Value != null)
			{
				return (bool)DGVSounds.Rows[rowCount].Cells["SoundEnable"].Value;
			}
			else
			{
				return false;
			}
		}

		#endregion

		#region - Quick Button -

		private string _currentQuickButtonSecurityType = CSVFieldIDs.SecurityTypes.Equity;

		private void cmdStockTicket_Click(object sender, EventArgs e)
		{
			cmdStockTicket.Enabled = false;
			cmdFutureTicket.Enabled = true;
			cmdOptionTicket.Enabled = true;

			_currentQuickButtonSecurityType = CSVFieldIDs.SecurityTypes.Equity;

			LoadStockTicketQuickButtonSettings();
		}

		private void cmdFutureTicket_Click(object sender, EventArgs e)
		{
			cmdStockTicket.Enabled = true;
			cmdFutureTicket.Enabled = false;
			cmdOptionTicket.Enabled = true;

			_currentQuickButtonSecurityType = CSVFieldIDs.SecurityTypes.Future;

			LoadFutureTicketQuickButtonSettings();
		}

		private void cmdOptionTicket_Click(object sender, EventArgs e)
		{
			cmdStockTicket.Enabled = true;
			cmdFutureTicket.Enabled = true;
			cmdOptionTicket.Enabled = false;

			_currentQuickButtonSecurityType = CSVFieldIDs.SecurityTypes.Option;

			LoadOptionTicketQuickButtonSettings();
		}

		private void LoadStockTicketQuickButtonSettings()
		{
			_stockTicketQuickButtonSettingsLoaded = true;

			rocQuickButtonList.RocGridTable.DefaultView.RowFilter = string.Format("SecType = '{0}'", CSVFieldIDs.SecurityTypes.Equity);
			rocQuickButtonList.RocGridTable.Columns["SecType"].DefaultValue = CSVFieldIDs.SecurityTypes.Equity;

			rocQuickButtonList.RocGridTable.Columns["Symbol"].DefaultValue = TicketDefaults.Stock;
			rocQuickButtonList.RocGridTable.Columns["Quantity"].DefaultValue = 100;
			rocQuickButtonList.RocGridTable.Columns["BasePriceOffset"].DefaultValue = 0;

			rocQuickButtonList.Columns["BasePriceOffset"].Visible = true;
			rocQuickButtonList.Columns["BasePriceSource"].Visible = true;
			rocQuickButtonList.Columns["ShortLocate"].Visible = true;

			Form_FitClicked(null, EventArgs.Empty);
		}

		private void LoadFutureTicketQuickButtonSettings()
		{
			_futureTicketQuickButtonSettingsLoaded = true;

			rocQuickButtonList.RocGridTable.DefaultView.RowFilter = string.Format("SecType = '{0}'", CSVFieldIDs.SecurityTypes.Future);
			rocQuickButtonList.RocGridTable.Columns["SecType"].DefaultValue = CSVFieldIDs.SecurityTypes.Future;

			rocQuickButtonList.RocGridTable.Columns["Symbol"].DefaultValue = TicketDefaults.Future;
			rocQuickButtonList.RocGridTable.Columns["Quantity"].DefaultValue = 1;
			rocQuickButtonList.RocGridTable.Columns["BasePriceOffset"].DefaultValue = 0;

			rocQuickButtonList.Columns["BasePriceOffset"].Visible = true;
			rocQuickButtonList.Columns["BasePriceSource"].Visible = true;
			rocQuickButtonList.Columns["ShortLocate"].Visible = false;

			Form_FitClicked(null, EventArgs.Empty);
		}

		private void LoadOptionTicketQuickButtonSettings()
		{
			_optionTicketQuickButtonSettingsLoaded = true;

			rocQuickButtonList.RocGridTable.DefaultView.RowFilter = string.Format("SecType = '{0}'", CSVFieldIDs.SecurityTypes.Option);
			rocQuickButtonList.RocGridTable.Columns["SecType"].DefaultValue = CSVFieldIDs.SecurityTypes.Option;

			rocQuickButtonList.RocGridTable.Columns["Symbol"].DefaultValue = TicketDefaults.Option;
			rocQuickButtonList.RocGridTable.Columns["Quantity"].DefaultValue = 1;

			rocQuickButtonList.Columns["BasePriceOffset"].Visible = false;
			rocQuickButtonList.Columns["BasePriceSource"].Visible = false;
			rocQuickButtonList.Columns["ShortLocate"].Visible = false;

			Form_FitClicked(null, EventArgs.Empty);
		}

		private void SaveQucikButtonSettings()
		{
			if (_stockTicketQuickButtonSettingsLoaded ||
				_futureTicketQuickButtonSettingsLoaded ||
				_optionTicketQuickButtonSettingsLoaded)
			{
				try
				{
					GLOBAL.HQuickButtonSettingData.SettingTable.Clear();
					rocQuickButtonList.RocGridTable.AcceptChanges();
					foreach (DataRow row in rocQuickButtonList.RocGridTable.Rows)
					{
						GLOBAL.HQuickButtonSettingData.SettingTable.LoadDataRow(row.ItemArray, true);
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		#region - Quick Button Grid Event -

		private void rocQuickButtonList_CellMouseClick(object sender, DataGridViewCellMouseEventArgs e)
		{
			if (e.RowIndex >= 0)
			{
				switch (e.Button)
				{
					case MouseButtons.Right:
						switch (rocQuickButtonList.Columns[e.ColumnIndex].Name)
						{
							case "Side":
								ShowSideMenu();
								break;
							case "BasePriceSource":
								ShowBasePriceSourceMenu();
								break;
						}
						break;
				}
			}
		}

		#endregion

		#region - Row Side Menu -

		private menuBaseAction _menuSideActions;
		private PropertyContainer _menuSideActionsContainer;
		private void ShowSideMenu()
		{
			if (_menuSideActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("", "");

				switch (_currentQuickButtonSecurityType)
				{
					case CSVFieldIDs.SecurityTypes.Future:
						items.Add(QuickButtonSides.Buy, QuickButtonSides.Buy);
						items.Add(QuickButtonSides.Sell, QuickButtonSides.Sell);
						break;
					case CSVFieldIDs.SecurityTypes.Option:
						items.Add(QuickButtonSides.BuyOpen, QuickButtonSides.BuyOpen);
						items.Add(QuickButtonSides.BuyClose, QuickButtonSides.BuyClose);
						items.Add(QuickButtonSides.SellOpen, QuickButtonSides.SellOpen);
						items.Add(QuickButtonSides.SellClose, QuickButtonSides.SellClose);
						break;
					case CSVFieldIDs.SecurityTypes.Equity:
						items.Add(QuickButtonSides.Buy, QuickButtonSides.Buy);
						items.Add(QuickButtonSides.Sell, QuickButtonSides.Sell);
						items.Add(QuickButtonSides.Short, QuickButtonSides.Short);
						break;
				}

				_menuSideActions = new menuBaseAction(items, true);
				_menuSideActions.PropertyChanged += new PropertyChangedEventHandler(menuSideActions_PropertyChanged);
			}

			if (_menuSideActionsContainer == null)
			{
				_menuSideActionsContainer = new PropertyContainer(_menuSideActions);
				_menuSideActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuSideActionsContainer_Closed);
			}

			_menuSideActionsContainer.Show(Cursor.Position);
		}

		private void menuSideActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			string side = e.PropertyName.ToUpper();

			foreach (DataGridViewRow row in rocQuickButtonList.SelectedRows)
			{
				if (row.Cells["Symbol"].Value != null &&
					row.Cells["Symbol"].Value.ToString() != "")
				{
					row.Cells["Side"].Value = side;
				}
			}

			_menuSideActionsContainer.Close();
		}

		private void menuSideActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuSideActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuSideActionsContainer_Closed);
			_menuSideActionsContainer = null;
			_menuSideActions.PropertyChanged -= new PropertyChangedEventHandler(menuSideActions_PropertyChanged);
			_menuSideActions = null;
		}

		#endregion

		#region - Row BasePriceSource Menu -

		private menuBaseAction _menuBasePriceSourceActions;
		private PropertyContainer _menuBasePriceSourceActionsContainer;
		private void ShowBasePriceSourceMenu()
		{
			if (_menuBasePriceSourceActions == null)
			{
				Dictionary<string, string> items = new Dictionary<string, string>();
				items.Add("", "");

				items.Add(QuickButtonBasePriceSource.Bid, QuickButtonBasePriceSource.Bid);
				items.Add(QuickButtonBasePriceSource.Ask, QuickButtonBasePriceSource.Ask);
				items.Add(QuickButtonBasePriceSource.Last, QuickButtonBasePriceSource.Last);
				items.Add(QuickButtonBasePriceSource.High, QuickButtonBasePriceSource.High);
				items.Add(QuickButtonBasePriceSource.Low, QuickButtonBasePriceSource.Low);

				_menuBasePriceSourceActions = new menuBaseAction(items, true);
				_menuBasePriceSourceActions.PropertyChanged += new PropertyChangedEventHandler(menuBasePriceSourceActions_PropertyChanged);
			}

			if (_menuBasePriceSourceActionsContainer == null)
			{
				_menuBasePriceSourceActionsContainer = new PropertyContainer(_menuBasePriceSourceActions);
				_menuBasePriceSourceActionsContainer.Closed += new ToolStripDropDownClosedEventHandler(menuBasePriceSourceActionsContainer_Closed);
			}

			_menuBasePriceSourceActionsContainer.Show(Cursor.Position);
		}

		private void menuBasePriceSourceActions_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			string basePriceSource = e.PropertyName.ToUpper();

			foreach (DataGridViewRow row in rocQuickButtonList.SelectedRows)
			{
				if (row.Cells["Symbol"].Value != null &&
					row.Cells["Symbol"].Value.ToString() != "")
				{
					row.Cells["BasePriceSource"].Value = basePriceSource;
				}
			}

			_menuBasePriceSourceActionsContainer.Close();
		}

		private void menuBasePriceSourceActionsContainer_Closed(object sender, ToolStripDropDownClosedEventArgs e)
		{
			_menuBasePriceSourceActionsContainer.Closed -= new ToolStripDropDownClosedEventHandler(menuBasePriceSourceActionsContainer_Closed);
			_menuBasePriceSourceActionsContainer = null;
			_menuBasePriceSourceActions.PropertyChanged -= new PropertyChangedEventHandler(menuBasePriceSourceActions_PropertyChanged);
			_menuBasePriceSourceActions = null;
		}

		#endregion

		#endregion

		#region - Orders -

		private void LoadOrdersSettings()
		{
			_ordersSettingsLoaded = true;

			Width = MinimumSize.Width;

			dtpStockAutoCancel.Value = GLOBAL.StockAutoCancelTime;
			chkStockAutoCancel.Checked = Configuration.User.Default.UseStockAutoCancel;

			dtpFutureAutoCancel.Value = GLOBAL.FutureAutoCancelTime;
			chkFutureAutoCancel.Checked = Configuration.User.Default.UseFutureAutoCancel;

			dtpOptionAutoCancel.Value = GLOBAL.OptionAutoCancelTime;
			chkOptionAutoCancel.Checked = Configuration.User.Default.UseOptionAutoCancel;

			chkSkipGTCandGTDonAuto.Checked = Configuration.User.Default.SkipGTCandGTDonAuto;
			chkOrderAutoFocusLast.Checked = Configuration.User.Default.OrderAutoFocusLast;

			switch (Configuration.User.Default.OrderAvgPriceResolution)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
				case 5:
				case 6:
				case 7:
					cboAvgPriceResolution.Text = Configuration.User.Default.OrderAvgPriceResolution.ToString();
					break;
				default:
					cboAvgPriceResolution.Text = "2";
					break;
			}
		}

		private void SaveOrdersSettings()
		{
			if (_ordersSettingsLoaded)
			{
				try
				{
					#region - Stock -

					GLOBAL.StockAutoCancelTime = dtpStockAutoCancel.Value;
					Configuration.User.Default.StockAutoCancelTime = dtpStockAutoCancel.Value.ToString("HH:mm:ss");
					Configuration.User.Default.Save();

					Configuration.User.Default.UseStockAutoCancel = chkStockAutoCancel.Checked;
					Configuration.User.Default.Save();

					#endregion

					#region - Future -

					GLOBAL.FutureAutoCancelTime = dtpFutureAutoCancel.Value;
					Configuration.User.Default.FutureAutoCancelTime = dtpFutureAutoCancel.Value.ToString("HH:mm:ss");
					Configuration.User.Default.Save();

					Configuration.User.Default.UseFutureAutoCancel = chkFutureAutoCancel.Checked;
					Configuration.User.Default.Save();

					#endregion

					#region - Option -

					GLOBAL.OptionAutoCancelTime = dtpOptionAutoCancel.Value;
					Configuration.User.Default.OptionAutoCancelTime = dtpOptionAutoCancel.Value.ToString("HH:mm:ss");
					Configuration.User.Default.Save();

					Configuration.User.Default.UseOptionAutoCancel = chkOptionAutoCancel.Checked;
					Configuration.User.Default.Save();

					#endregion

					Configuration.User.Default.SkipGTCandGTDonAuto = chkSkipGTCandGTDonAuto.Checked;
					Configuration.User.Default.Save();

					Int32 i = 2;
					if (int.TryParse(cboAvgPriceResolution.Text, out i))
					{
						if (Configuration.User.Default.OrderAvgPriceResolution != i)
						{
							Configuration.User.Default.OrderAvgPriceResolution = i;
							Configuration.User.Default.Save();
						}
					}

					if (Configuration.User.Default.OrderAutoFocusLast != chkOrderAutoFocusLast.Checked)
					{
						Configuration.User.Default.OrderAutoFocusLast = chkOrderAutoFocusLast.Checked;
						Configuration.User.Default.Save();
					}
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		#endregion

		#region - Process -

		private void LoadProcessSettings()
		{
			_processSettingsLoaded = true;

			Width = MinimumSize.Width;

			panelAdmin.Visible = GLOBAL.AdminMode;

			switch (Configuration.User.Default.CSVThreadPriority)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
					cboCSVThreadPriority.SelectedIndex = Configuration.User.Default.CSVThreadPriority;
					break;
			}

			switch (Configuration.User.Default.MDThreadPriority)
			{
				case 0:
				case 1:
				case 2:
				case 3:
				case 4:
					cboMDThreadPriority.SelectedIndex = Configuration.User.Default.MDThreadPriority;
					break;
			}

			trackBarUIUpdateRate.Value = Configuration.User.Default.UIUpdateRate;
			chkUserUIUpdateRate.Checked = Configuration.User.Default.UseUIUpdateRate;

			chkMDS.Checked = true;
			dspMDS.Text = Configuration.MDS.Default.IP + " : " + Configuration.MDS.Default.Port;

			chkMDSBackup.Checked = false;
			dspMDSBackup.Text = Configuration.MDS.Default.IPBackup + " : " + Configuration.MDS.Default.PortBackup; 

			lblUIUpdateRate.Enabled = panelTrackBarUI.Enabled = chkUserUIUpdateRate.Checked;
		}

		private void SaveProcessSettings()
		{
			if (_processSettingsLoaded)
			{
				try
				{
					switch (cboCSVThreadPriority.SelectedIndex)
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
							if (Configuration.User.Default.CSVThreadPriority != cboCSVThreadPriority.SelectedIndex)
							{
								Configuration.User.Default.CSVThreadPriority = cboCSVThreadPriority.SelectedIndex;
								GLOBAL.ReloadProcessSettings = true;
							}
							break;
					}

					switch (cboMDThreadPriority.SelectedIndex)
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
							if (Configuration.User.Default.MDThreadPriority != cboMDThreadPriority.SelectedIndex)
							{
								Configuration.User.Default.MDThreadPriority = cboMDThreadPriority.SelectedIndex;
								GLOBAL.ReloadProcessSettings = true;
							}
							break;
					}

					if (Configuration.User.Default.UIUpdateRate != trackBarUIUpdateRate.Value)
					{
						Configuration.User.Default.UIUpdateRate = trackBarUIUpdateRate.Value;
						GLOBAL.ReloadProcessSettings = true;
					}

					if (Configuration.User.Default.UseUIUpdateRate != chkUserUIUpdateRate.Checked)
					{
						Configuration.User.Default.UseUIUpdateRate = chkUserUIUpdateRate.Checked;
						GLOBAL.ReloadProcessSettings = true;
					}

					Configuration.User.Default.Save();
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}
		}

		private void chkUserUIUpdateRate_CheckedChanged(object sender, EventArgs e)
		{
			lblUIUpdateRate.Enabled = panelTrackBarUI.Enabled = chkUserUIUpdateRate.Checked;
		}

		#endregion

		#region - Admin -

		private void cmdResubscrib_Click(object sender, EventArgs e)
		{
			HelperResubscribe.LoadSubscriptionCache();

			System.Threading.ThreadPool.QueueUserWorkItem(
				ProcessSubscriptionCache, txtSourceFilter.Text.Trim().ToUpper());
		}

		private void ProcessSubscriptionCache(object input)
		{
			string sourceFilter = (string)input;
			int total = HelperResubscribe.CacheTable.Rows.Count;
			int count = 0;
			int resubCount = 0;
			resubscribeStatus = "";

			if (total > 0 && sourceFilter != "")
			{
				lock (HelperResubscribe.CacheTable)
				{
					foreach (DataRow row in HelperResubscribe.CacheTable.Rows)
					{
						if (row["Source"].ToString().ToUpper() == sourceFilter)
						{
							SubscribeByDataRow(row);
							lock (syncObject)
							{
								System.Threading.Thread.Sleep(resubscribeRate);
							}
							resubCount = resubCount + 1;
						}
						count = count + 1;

						resubscribeStatus = string.Format("{0}/{1}/{2}", new object[] { total, count, resubCount });
					}
				}
			}

			resubscribeStatus = "";
		}

		private void SubscribeByDataRow(DataRow row)
		{
			try
			{
				switch (row["MsgType"].ToString().ToUpper())
				{
					case "S":
						HelperSubscriber.ReSubscribe(
							row["Symbol"].ToString(),
							row["Source"].ToString().ToUpper(),
							row["SubscriptionType"].ToString().ToUpper(),
							row["SecType"].ToString().ToUpper());
						break;
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private void cboResubscribeRate_SelectedIndexChanged(object sender, EventArgs e)
		{
			lock (syncObject)
			{
				resubscribeRate = Int32.Parse(cboResubscribeRate.Text);
			}
		}

		#endregion
	}
}