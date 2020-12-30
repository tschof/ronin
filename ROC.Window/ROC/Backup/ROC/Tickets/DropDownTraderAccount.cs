using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

using RDSEx;

namespace ROC
{
	public class DropDownTraderAccount : INotifyPropertyChanged
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

		private bool _accountChanged = false;
		[Browsable(false)]
		private bool accountChanged
		{
			get
			{
				return _accountChanged;
			}
			set
			{
				_accountChanged = value;
				if (_accountChanged)
				{
					onPropertyChanged(new PropertyChangedEventArgs("AccountChanged"));
				}
			}
		}

		private bool _tradeForChanged = false;
		[Browsable(false)]
		private bool tradeForChanged
		{
			get
			{
				return _tradeForChanged;
			}
			set
			{
				_tradeForChanged = value;
				if (_tradeForChanged)
				{
					onPropertyChanged(new PropertyChangedEventArgs("TradeForChanged"));
				}
			}
		}

		#endregion

		#region - Local Variables -

		private frmStockTicket _stockTicket;
		private frmFutureTicket _futureTicket;
		private frmQuickTicket _quickTicket;
		private frmOptionTicket _optionTicket;
		private frmAutoSpreadTicket _autoSpreadTicket;

		private TicketTypes _ticketType = TicketTypes.None;

		private ToolStripDropDownButton cmdTradeFor
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.cmdTradeFor;
					case TicketTypes.Future:
						return _futureTicket.cmdTradeFor;
					case TicketTypes.Quick:
						return _quickTicket.cmdTradeFor;
					case TicketTypes.Option:
						return _optionTicket.cmdTradeFor;
					case TicketTypes.AutoSpread:
						return _autoSpreadTicket.cmdTradeFor;
					default:
						return null;
				}
			}
		}

		private ToolStripDropDownButton cmdAccount
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.cmdAccount;
					case TicketTypes.Future:
						return _futureTicket.cmdAccount;
					case TicketTypes.Quick:
						return _quickTicket.cmdAccount;
					case TicketTypes.Option:
						return _optionTicket.cmdAccount;
					case TicketTypes.AutoSpread:
						return _autoSpreadTicket.cmdAccount;
					default:
						return null;
				}
			}
		}

		private ComboBox cboExchange
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.cboExchange;
					case TicketTypes.Future:
						return _futureTicket.cboExchange;
					case TicketTypes.Quick:
						return _quickTicket.cboExchange;
					case TicketTypes.Option:
						return _optionTicket.cboExchange;
					case TicketTypes.AutoSpread:
						return _autoSpreadTicket.cboExchange;
					default:
						return null;
				}
			}
		}

		private ComboBox cboAlgoExchange
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.cboAlgoExchange;
					default:
						return null;
				}
			}
		}

		private Label lblExchange
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.lblExchange;
					case TicketTypes.Future:
						return _futureTicket.lblExchange;
					case TicketTypes.Quick:
						return _quickTicket.lblExchange;
					case TicketTypes.Option:
						return _optionTicket.lblExchange;
					case TicketTypes.AutoSpread:
						return _autoSpreadTicket.lblExchange;
					default:
						return null;
				}
			}
		}

		#endregion

		#region - Constructor -

		public DropDownTraderAccount(frmStockTicket stockTicket)
		{
			_stockTicket = stockTicket;
			_ticketType = TicketTypes.Stock;
		}

		public DropDownTraderAccount(frmFutureTicket futureTicket)
		{
			_futureTicket = futureTicket;
			_ticketType = TicketTypes.Future;
		}

		public DropDownTraderAccount(frmQuickTicket quickTicket)
		{
			_quickTicket = quickTicket;
			_ticketType = TicketTypes.Quick;
		}

		public DropDownTraderAccount(frmOptionTicket optionTicket)
		{
			_optionTicket = optionTicket;
			_ticketType = TicketTypes.Option;
		}

		public DropDownTraderAccount(frmAutoSpreadTicket autoSpreadTicket)
		{
			_autoSpreadTicket = autoSpreadTicket;
			_ticketType = TicketTypes.AutoSpread;
		}

		#endregion

		#region - Trader Account Selection -

		private List<string> _tradeForList = new List<string>();
		private List<string> _accountList = new List<string>();

		private Dictionary<string, TraderMap> _currentTraders = new Dictionary<string, TraderMap>();
		private Dictionary<string, AccountMap> _currentAccounts = new Dictionary<string, AccountMap>();
		private Dictionary<string, DestinationMap> _currentDestinations = new Dictionary<string, DestinationMap>();

		private string _selectedTradeFor = "";
		public string SelectedTraderFor
		{
			get
			{
				return _selectedTradeFor;
			}
			set
			{
				_selectedTradeFor = value;
			}
		}

		private string _selectedLocalAccountAcrn = "";
		public string SelectedLocalAccountAcrn
		{
			get
			{
				return _selectedLocalAccountAcrn;
			}
			set
			{
				_selectedLocalAccountAcrn = value;
			}
		}

		private string _selectedExchange = "";
		public string SelectedExchange
		{
			get
			{
				return _selectedExchange;
			}
			set
			{
				_selectedExchange = value;
			}
		}

		private string _selectedAlgoExchange = "";
		public string SelectedAlgoExchange
		{
			get
			{
				return _selectedAlgoExchange;
			}
			set
			{
				_selectedAlgoExchange = value;
			}
		}

		public string CurrentTradeFor
		{
			get
			{
				return cmdTradeFor.Text;
			}
			set
			{
				_selectedTradeFor = value;
			}
		}

		public string CurrentAccount
		{
			get
			{
				return cmdAccount.Text;
			}
			set
			{
				_selectedLocalAccountAcrn = value;
			}
		}

		public AccountMap CurrentAcctountInfo
		{
			get
			{
				if (_currentAccounts.ContainsKey(CurrentAccount))
				{
					return _currentAccounts[CurrentAccount];
				}
				return null;
			}
		}

		public DestinationMap CurrentDestinationInfo
		{
			get
			{
				if (_currentDestinations.ContainsKey(CurrentExchange))
				{
					return _currentDestinations[CurrentExchange];
				}
				return null;
			}
		}

		public bool GetCurrentAccount(ref AccountMap map)
		{
			return GetCurrentAccount(CurrentAccount, ref map);
		}
		public bool GetCurrentAccount(string currentAccount, ref AccountMap map)
		{
			if (_currentAccounts.ContainsKey(currentAccount))
			{
				map = _currentAccounts[currentAccount];
				return true;
			}
			return false;
		}

		public string CurrentExchange
		{
			get
			{
				return cboExchange.Text;
			}
			set
			{
				_selectedExchange = value;
			}
		}

		public string CurrentAlgoExchange
		{
			get
			{
				return cboAlgoExchange.Text;
			}
			set
			{
				_selectedAlgoExchange = value;
			}
		}

		public bool GetCurrentExchange(ref DestinationMap map)
		{
			return GetCurrentExchange(CurrentExchange, ref map);
		}
		public bool GetCurrentExchange(string currentExchange, ref DestinationMap map)
		{
			if (_currentDestinations.ContainsKey(currentExchange))
			{
				map = _currentDestinations[currentExchange];
				return true;
			}
			return false;
		}

		public bool GetCurrentAlgoExchange(ref DestinationMap map)
		{
			return GetCurrentAlgoExchange(CurrentAlgoExchange, ref map);
		}
		public bool GetCurrentAlgoExchange(string currentAlgoExchange, ref DestinationMap map)
		{
			if (_currentDestinations.ContainsKey(currentAlgoExchange))
			{
				map = _currentDestinations[currentAlgoExchange];
				return true;
			}
			return false;
		}

		public void LoadUserAccounts()
		{
			_tradeForList.Clear();
			_accountList.Clear();

			_currentTraders.Clear();
			_currentAccounts.Clear();
			_currentDestinations.Clear();

			if (GLOBAL.HRDS.GotUserProfiles)
			{
				LoadTradeFors();
			}
			else
			{
				UserProfileFailed();
			}
		}

		private void UserProfileFailed()
		{
			TicketSupport.SetState(cmdTradeFor, true);
			TicketSupport.SetState(cmdAccount, true);
			TicketSupport.SetState(lblExchange, true);
		}

		private void LoadTradeFors()
		{
			if (cmdTradeFor != null)
			{
				cmdTradeFor.DropDownItems.Clear();
				_tradeForList.Clear();
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (!_tradeForList.Contains(trader.tradeFor))
					{
						switch (_ticketType)
						{
							case TicketTypes.Future:
								if (trader.FUTAccounts.Count > 0)
								{
									_tradeForList.Add(trader.tradeFor);
									cmdTradeFor.DropDownItems.Add(trader.tradeFor, null, new EventHandler(TradeFor_Changed));
								}
								break;
							case TicketTypes.Option:
								//if (_optionTicket.IsFuture)
								//{
								//    if (trader.FUTAccounts.Count > 0)
								//    {
								//        _tradeForList.Add(trader.tradeFor);
								//        cmdTradeFor.DropDownItems.Add(trader.tradeFor, null, new EventHandler(TradeFor_Changed));
								//    }
								//}
								//else
								//{
									if (trader.OPTAccounts.Count > 0)
									{
										_tradeForList.Add(trader.tradeFor);
										cmdTradeFor.DropDownItems.Add(trader.tradeFor, null, new EventHandler(TradeFor_Changed));
									}
								//}
								break;
							case TicketTypes.Stock:
								if (trader.CSAccounts.Count > 0)
								{
									_tradeForList.Add(trader.tradeFor);
									cmdTradeFor.DropDownItems.Add(trader.tradeFor, null, new EventHandler(TradeFor_Changed));
								}
								break;
							case TicketTypes.Quick:
								if (_quickTicket.IsStock)
								{
									if (trader.CSAccounts.Count > 0)
									{
										_tradeForList.Add(trader.tradeFor);
										cmdTradeFor.DropDownItems.Add(trader.tradeFor, null, new EventHandler(TradeFor_Changed));
									}
								}
								else
								{
									if (trader.FUTAccounts.Count > 0)
									{
										_tradeForList.Add(trader.tradeFor);
										cmdTradeFor.DropDownItems.Add(trader.tradeFor, null, new EventHandler(TradeFor_Changed));
									}
								}
								break;
							case TicketTypes.AutoSpread:
								if (trader.FUTAccounts.Count > 0)
								{
									_tradeForList.Add(trader.tradeFor);
									cmdTradeFor.DropDownItems.Add(trader.tradeFor, null, new EventHandler(TradeFor_Changed));
								}
								break;
						}
					}
				}
			}

			if (cmdTradeFor != null && cmdTradeFor.DropDownItems.Count > 0)
			{
				if (_selectedTradeFor != "")
				{
					foreach (ToolStripDropDownItem item in cmdTradeFor.DropDownItems)
					{
						if (_selectedTradeFor == item.Text)
						{
							item.PerformClick();
							return;
						}
					}
				}

				cmdTradeFor.DropDownItems[0].PerformClick();
			}
			else
			{
				if (cmdTradeFor != null)
				{
					cmdTradeFor.Text = "N/A";
				}
				UserProfileFailed();
			}
		}

		private void TradeFor_Changed(object sender, EventArgs e)
		{
			LoadAccount(((ToolStripMenuItem)sender).Text);
			tradeForChanged = true;
		}

		private void LoadAccount(string tradeFor)
		{
			if (cmdTradeFor != null)
			{
				cmdTradeFor.Text = tradeFor;
			}

			if (cmdAccount != null)
			{
				cmdAccount.DropDownItems.Clear();
				_accountList.Clear();
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (trader.tradeFor == tradeFor)
					{
						if (!_currentTraders.ContainsKey(tradeFor))
						{
							_currentTraders.Add(tradeFor, trader);
						}

						SetAccountByTicketType(trader);
					}
				}
			}

			if (cmdAccount != null && cmdAccount.DropDownItems.Count > 0)
			{
				if (_selectedLocalAccountAcrn != "")
				{
					foreach (ToolStripDropDownItem item in cmdAccount.DropDownItems)
					{
						if (_selectedLocalAccountAcrn == item.Text)
						{
							item.PerformClick();
							return;
						}
					}
				}

				cmdAccount.DropDownItems[0].PerformClick();
			}
			else
			{
				if (cmdAccount != null)
				{
					cmdAccount.Text = "N/A";
				}
				UserProfileFailed();
			}
		}

		private void SetAccountByTicketType(TraderMap trader)
		{
			if (cmdAccount != null)
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						foreach (AccountMap acct in trader.CSAccounts.Values)
						{
							AddAccountToDropDown(acct);
						}
						break;
					case TicketTypes.Future:
						foreach (AccountMap acct in trader.FUTAccounts.Values)
						{
							AddAccountToDropDown(acct);
						}
						break;
					case TicketTypes.Option:
						//if (_optionTicket.IsFuture)
						//{
						//    foreach (AccountMap acct in trader.FUTAccounts.Values)
						//    {
						//        AddAccountToDropDown(acct);
						//    }
						//}
						//else
						//{
							foreach (AccountMap acct in trader.OPTAccounts.Values)
							{
								AddAccountToDropDown(acct);
							}
						//}
						break;
					case TicketTypes.Quick:
						if (_quickTicket.IsStock)
						{
							foreach (AccountMap acct in trader.CSAccounts.Values)
							{
								AddAccountToDropDown(acct);
							}
						}
						else
						{
							foreach (AccountMap acct in trader.FUTAccounts.Values)
							{
								AddAccountToDropDown(acct);
							}
						}
						break;
					case TicketTypes.AutoSpread:
						foreach (AccountMap acct in trader.FUTAccounts.Values)
						{
							AddAccountToDropDown(acct);
						}
						break;
				}
			}
		}

		private void AddAccountToDropDown(AccountMap acct)
		{
			if (!_accountList.Contains(acct.account))
			{
				_accountList.Add(acct.account);
				cmdAccount.DropDownItems.Add(acct.account, null, new EventHandler(Account_Changed));
			}
		}

		private void Account_Changed(object sender, EventArgs e)
		{
			LoadExchange(((ToolStripMenuItem)sender).Text);
			accountChanged = true;
		}

		private void LoadExchange(string account)
		{
			if (cmdAccount != null)
			{
				cmdAccount.Text = account;
			}

			string lastSelectedExchange = "";
			if (cboExchange != null)
			{
				if (_selectedExchange != "")
				{
					lastSelectedExchange = _selectedExchange;
				}
				else
				{
					lastSelectedExchange = cboExchange.Text;
				}
				cboExchange.Items.Clear();
				foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
				{
					if (_currentTraders.ContainsKey(trader.tradeFor))
					{
						SetExchangeByTicketType(trader, account);
					}
				}
			}

			if (cboExchange != null && cboExchange.Items.Count > 0)
			{
				if (cboExchange.Items.Contains(lastSelectedExchange))
				{
					cboExchange.Text = lastSelectedExchange;
				}
				else
				{
					cboExchange.SelectedIndex = 0;
				}
			}
			else
			{
				if (cboExchange != null)
				{
					cboExchange.Text = "";
				}
				UserProfileFailed();
			}
		}

		private void SetExchangeByTicketType(TraderMap trader, string account)
		{
			switch (_ticketType)
			{
				case TicketTypes.Stock:
					foreach (AccountMap acct in trader.CSAccounts.Values)
					{
						if (acct.account == account)
						{
							SetDestinationsByAccount(acct);
						}
					}
					break;
				case TicketTypes.Future:
					foreach (AccountMap acct in trader.FUTAccounts.Values)
					{
						if (acct.account == account)
						{
							SetDestinationsByAccount(acct);
						}
					}
					break;
				case TicketTypes.Option:
					//if (_optionTicket.IsFuture)
					//{
					//    foreach (AccountMap acct in trader.FUTAccounts.Values)
					//    {
					//        if (acct.account == account)
					//        {
					//            SetDestinationsByAccount(acct);
					//        }
					//    }
					//}
					//else
					//{
						foreach (AccountMap acct in trader.OPTAccounts.Values)
						{
							if (acct.account == account)
							{
								SetDestinationsByAccount(acct);
							}
						}
					//}
					break;
				case TicketTypes.Quick:
					if (_quickTicket.IsStock)
					{
						foreach (AccountMap acct in trader.CSAccounts.Values)
						{
							if (acct.account == account)
							{
								SetDestinationsByAccount(acct);
							}
						}
					}
					else
					{
						foreach (AccountMap acct in trader.FUTAccounts.Values)
						{
							if (acct.account == account)
							{
								SetDestinationsByAccount(acct);
							}
						}
					}
					break;
				case TicketTypes.AutoSpread:
					foreach (AccountMap acct in trader.FUTAccounts.Values)
					{
						if (acct.account == account)
						{
							SetDestinationsByAccount(acct);
						}
					}
					break;
			}
		}

		private void SetDestinationsByAccount(AccountMap acct)
		{
			if (!_currentAccounts.ContainsKey(acct.account))
			{
				_currentAccounts.Add(acct.account, acct);
			}

			foreach (DestinationMap dest in acct.Destinations.Values)
			{
				if (!_currentDestinations.ContainsKey(dest.shortName))
				{
					_currentDestinations.Add(dest.shortName, dest);
				}

				if (!cboExchange.Items.Contains(dest.shortName))
				{
					cboExchange.Items.Add(dest.shortName);
				}

				if (cboAlgoExchange != null)
				{
					if (!dest.shortName.ToUpper().Contains("_ALGO") && !cboAlgoExchange.Items.Contains(dest.shortName))
					{
						cboAlgoExchange.Items.Add(dest.shortName);
					}
				}
			}
		}

		#endregion
	}
}
