using System;
using System.ComponentModel;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Linq;

using Common;
using RDSEx;

namespace ROC
{
	internal class DropDownTraderAccount : INotifyPropertyChanged
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

		internal DropDownTraderAccount(frmStockTicket stockTicket)
		{
			_stockTicket = stockTicket;
			_ticketType = TicketTypes.Stock;
		}

		internal DropDownTraderAccount(frmFutureTicket futureTicket)
		{
			_futureTicket = futureTicket;
			_ticketType = TicketTypes.Future;
		}

		internal DropDownTraderAccount(frmQuickTicket quickTicket)
		{
			_quickTicket = quickTicket;
			_ticketType = TicketTypes.Quick;
		}

		internal DropDownTraderAccount(frmOptionTicket optionTicket)
		{
			_optionTicket = optionTicket;
			_ticketType = TicketTypes.Option;
		}

		internal DropDownTraderAccount(frmAutoSpreadTicket autoSpreadTicket)
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
		internal string SelectedTraderFor
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
		internal string SelectedLocalAccountAcrn
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
		internal string SelectedExchange
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
		internal string SelectedAlgoExchange
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

		internal string CurrentTradeFor
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

		internal string CurrentAccount
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

		internal AccountMap CurrentAcctountInfo => _currentAccounts.TryGetValue(CurrentAccount, out var found) ? found : null;

		internal DestinationMap CurrentDestinationInfo => _currentDestinations.TryGetValue(CurrentExchange, out var found) ? found : null;

		internal bool GetCurrentAccount(ref AccountMap map)
		{
			return GetCurrentAccount(CurrentAccount, ref map);
		}

		internal bool GetCurrentAccount(string currentAccount, ref AccountMap map)
		{
			if (_currentAccounts.TryGetValue(currentAccount, out var found))
			{
				map = found;
				return true;
			}
			return false;
		}

		internal string CurrentExchange
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

		internal string CurrentAlgoExchange
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

		internal bool GetCurrentExchange(ref DestinationMap map)
		{
			return GetCurrentExchange(CurrentExchange, ref map);
		}
		internal bool GetCurrentExchange(string currentExchange, ref DestinationMap map)
		{
			if (_currentDestinations.TryGetValue(currentExchange, out var found))
			{
				map = found;
				return true;
			}
			return false;
		}

		internal bool GetCurrentAlgoExchange(ref DestinationMap map)
		{
			return GetCurrentAlgoExchange(CurrentAlgoExchange, ref map);
		}
		internal bool GetCurrentAlgoExchange(string currentAlgoExchange, ref DestinationMap map)
		{
			if (_currentDestinations.TryGetValue(currentAlgoExchange, out var found))
			{
				map = found;
				return true;
			}
			return false;
		}

		internal void LoadUserAccounts()
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

		private void addTradeForDropDownItem(string tradeFor, Dictionary<string, AccountMap> accounts)
		{
			if (accounts.Count > 0) {
				_tradeForList.Add(tradeFor);
				cmdTradeFor.DropDownItems.Add(tradeFor, null, new EventHandler(TradeFor_Changed));
			}
		}

		private void LoadTradeFors()
		{
			if (cmdTradeFor != null)
			{
				cmdTradeFor.DropDownItems.Clear();
				_tradeForList.Clear();
				foreach ((string _, TraderMap trader) in GLOBAL.HRDS.UserProfiles) {
					if (!_tradeForList.Contains(trader.TradeFor))
					{
						switch (_ticketType)
						{
							case TicketTypes.Future:
								addTradeForDropDownItem(trader.TradeFor, trader.FUTAccounts);
								break;
							case TicketTypes.Option:
								addTradeForDropDownItem(trader.TradeFor, trader.OPTAccounts);
								break;
							case TicketTypes.Stock:
								addTradeForDropDownItem(trader.TradeFor, trader.CSAccounts);
								break;
							case TicketTypes.Quick:
								addTradeForDropDownItem(trader.TradeFor, _quickTicket.IsStock ? trader.CSAccounts : trader.FUTAccounts);
								break;
							case TicketTypes.AutoSpread:
								addTradeForDropDownItem(trader.TradeFor, trader.FUTAccounts);
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
				foreach ((string _, TraderMap trader) in GLOBAL.HRDS.UserProfiles)
				{
					if (trader.TradeFor == tradeFor)
					{
						_currentTraders.TryAdd(tradeFor, trader);
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
			if (cmdAccount != null) {
				forEachAccountByTicketType(trader, n => {
					if (!_accountList.Contains(n.account)) {
						_accountList.Add(n.account);
						cmdAccount.DropDownItems.Add(n.account, null, new EventHandler(Account_Changed));
					}
				});
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
				cmdAccount.Text = account;

			string lastSelectedExchange = "";
			if (cboExchange != null)
			{
				lastSelectedExchange = (_selectedExchange != "") ? _selectedExchange : cboExchange.Text;
				cboExchange.Items.Clear();
				foreach ((string _, TraderMap trader) in GLOBAL.HRDS.UserProfiles)
				{
					if (_currentTraders.ContainsKey(trader.TradeFor))
						SetExchangeByTicketType(trader, account);
				}
			}

			if (cboExchange != null && cboExchange.Items.Count > 0)
			{
				if (cboExchange.Items.Contains(lastSelectedExchange))
					cboExchange.Text = lastSelectedExchange;
				else
					cboExchange.SelectedIndex = 0;
			} else
			{
				if (cboExchange != null)
					cboExchange.Text = "";
				UserProfileFailed();
			}
		}

		private void forEachAccountByTicketType(TraderMap trader, System.Action<AccountMap> action)
		{
			Dictionary<string, AccountMap> accounts = null;
			if (_ticketType == TicketTypes.Stock)
				accounts = trader.CSAccounts;
			else if ((_ticketType == TicketTypes.Future) || (_ticketType == TicketTypes.AutoSpread))
				accounts = trader.FUTAccounts;
			else if (_ticketType == TicketTypes.Option)
				accounts = trader.OPTAccounts;
			else if (_ticketType == TicketTypes.Quick)
				accounts = _quickTicket.IsStock ? trader.CSAccounts : trader.FUTAccounts;

			if (accounts != null) {
				foreach ((string _, AccountMap account) in accounts)
					action(account);
			}
		}

		private void SetExchangeByTicketType(TraderMap trader, string account)
		{
			forEachAccountByTicketType(trader, n => {
				if (n.account == account)
					SetDestinationsByAccount(n);
			});
		}

		private void SetDestinationsByAccount(AccountMap acct)
		{
			_currentAccounts.TryAdd(acct.account, acct);

			foreach (DestinationMap dest in acct.Destinations)
			{
				_currentDestinations.TryAdd(dest.shortName, dest);

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
