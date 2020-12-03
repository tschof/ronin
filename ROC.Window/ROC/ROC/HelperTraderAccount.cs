using System;
using System.Collections.Generic;
using System.Text;
using ButtonEx;
using RDSEx;

namespace ROC
{
	public class HelperTraderAccount
	{
		#region - Local Variable -
		
		private TicketTypes _ticketType = TicketTypes.None;

		private AccountTypeToGet _toGet;
		private AccountTypeToGet toGet
		{
			get
			{
				if (_toGet == null)
				{
					switch (_ticketType)
					{
						case TicketTypes.AutoSpread:
							_toGet = new AccountTypeToGet(true, true, false);
							break;
						case TicketTypes.None:
						default:
							_toGet = new AccountTypeToGet(false, false, false);
							break;
					}
				}
				return _toGet;
			}
		}

		#endregion

		#region - Constructor -

		public HelperTraderAccount()
		{
			_ticketType = TicketTypes.None;
		}
		public HelperTraderAccount(frmAutoSpreadTicket ticket)
		{
			_ticketType = TicketTypes.AutoSpread;
		}

		#endregion

		#region - TradeFors -

		public List<string> GetTradeFors()
		{
			return GetTradeFors(toGet);
		}

		private List<string> GetTradeFors(AccountTypeToGet toGet)
		{
			List<string> result = new List<string>();

			if (toGet.CS)
			{
				GetCSTradeFors(ref result);
			}

			if (toGet.FUT)
			{
				GetFUTTradeFors(ref result);
			}

			if (toGet.OPT)
			{
				GetOPTTradeFors(ref result);
			}

			return result;
		}

		private void GetCSTradeFors(ref List<string> result)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.CSAccounts.Count > 0 && !result.Contains(trader.tradeFor))
				{
					result.Add(trader.tradeFor);
				}
			}
		}

		private void GetFUTTradeFors(ref List<string> result)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.FUTAccounts.Count > 0 && !result.Contains(trader.tradeFor))
				{
					result.Add(trader.tradeFor);
				}
			}
		}

		private void GetOPTTradeFors(ref List<string> result)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.OPTAccounts.Count > 0 && !result.Contains(trader.tradeFor))
				{
					result.Add(trader.tradeFor);
				}
			}
		}

		#endregion

		#region - Accounts -

		public List<string> GetAccounts(string tradeFor)
		{
			return GetAccounts(toGet, tradeFor);
		}

		private List<string> GetAccounts(AccountTypeToGet toGet, string tradeFor)
		{
			List<string> result = new List<string>();

			if (toGet.CS)
			{
				GetCSAccounts(ref result, tradeFor);
			}

			if (toGet.FUT)
			{
				GetFUTAccounts(ref result, tradeFor);
			}

			if (toGet.OPT)
			{
				GetOPTAccounts(ref result, tradeFor);
			}

			return result;
		}

		private void GetCSAccounts(ref List<string> result, string tradeFor)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.tradeFor == tradeFor && trader.CSAccounts.Count > 0)
				{
					GetAccounts(ref result, trader.CSAccounts);
				}
			}
		}

		private void GetFUTAccounts(ref List<string> result, string tradeFor)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.tradeFor == tradeFor && trader.FUTAccounts.Count > 0)
				{
					GetAccounts(ref result, trader.FUTAccounts);
				}
			}
		}

		private void GetOPTAccounts(ref List<string> result, string tradeFor)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.tradeFor == tradeFor && trader.OPTAccounts.Count > 0)
				{
					GetAccounts(ref result, trader.OPTAccounts);
				}
			}
		}

		private void GetAccounts(ref List<string> result, Dictionary<string, AccountMap> accts)
		{
			foreach (AccountMap acct in accts.Values)
			{
				if (!result.Contains(acct.account))
				{
					result.Add(acct.account);
				}
			}
		}

		#endregion

		#region - Exchanges -

		public List<string> GetExchanges(string tradeFor, string account)
		{
			return GetExchanges(toGet, tradeFor, account);
		}

		private List<string> GetExchanges(AccountTypeToGet toGet, string tradeFor, string account)
		{
			List<string> result = new List<string>();

			if (toGet.CS)
			{
				GetCSExchanges(ref result, tradeFor, account);
			}

			if (toGet.FUT)
			{
				GetFUTExchanges(ref result, tradeFor, account);
			}

			if (toGet.OPT)
			{
				GetOPTExchanges(ref result, tradeFor, account);
			}

			return result;
		}

		private void GetCSExchanges(ref List<string> result, string tradeFor, string account)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.tradeFor == tradeFor && trader.CSAccounts.Count > 0)
				{
					foreach (AccountMap acct in trader.CSAccounts.Values)
					{
						if (acct.account == account && acct.Destinations.Count > 0)
						{
							GetExchanges(ref result, acct.Destinations);
						}
					}
				}
			}
		}

		private void GetFUTExchanges(ref List<string> result, string tradeFor, string account)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.tradeFor == tradeFor && trader.FUTAccounts.Count > 0)
				{
					foreach (AccountMap acct in trader.FUTAccounts.Values)
					{
						if (acct.account == account && acct.Destinations.Count > 0)
						{
							GetExchanges(ref result, acct.Destinations);
						}
					}
				}
			}
		}

		private void GetOPTExchanges(ref List<string> result, string tradeFor, string account)
		{
			foreach (TraderMap trader in GLOBAL.HRDS.UserProfiles.Values)
			{
				if (trader.tradeFor == tradeFor && trader.OPTAccounts.Count > 0)
				{
					foreach (AccountMap acct in trader.OPTAccounts.Values)
					{
						if (acct.account == account && acct.Destinations.Count > 0)
						{
							GetExchanges(ref result, acct.Destinations);
						}
					}
				}
			}
		}

		private void GetExchanges(ref List<string> result, Dictionary<int, DestinationMap> dests)
		{
			foreach (DestinationMap dest in dests.Values)
			{
				if (!result.Contains(dest.shortName))
				{
					result.Add(dest.shortName);
				}
			}
		}

		#endregion
	}
}
