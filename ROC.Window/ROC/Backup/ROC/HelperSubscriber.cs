using System;
using System.Diagnostics;
using CSVEx;
using MarketDataEx;

namespace ROC
{
	public static class HelperSubscriber
	{
		private static bool _useGroupSubscription = Configuration.ROC.Default.UseGroupSubscription;

		public static void ReSubscribe(string symbol, string source, string subscriptionType, string secType)
		{
			foreach (HelperMDS mds in GLOBAL.HMDSs)
			{
				mds.ReSubscribe(symbol, source, subscriptionType, secType);
			}
		}

		public static void Subscribe(string symbol, string source, string secType)
		{
			if (GLOBAL.HRDS.CheckSource(source))
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					switch (secType)
					{
						case CSVFieldIDs.SecutrityTypes.Equity:
							if (_useGroupSubscription)
							{
								// Group subscription have the base symbol,
								mds.Subscribe(symbol, source, "L2", secType);
							}
							else
							{
								mds.Subscribe(symbol, source, "", secType);
							}
							break;
						case CSVFieldIDs.SecutrityTypes.Option:
							if (source.Contains(OptionDataSource.OPRANBBO) && Configuration.Override.Default.UseOverridNBBO)
							{
								source = Configuration.Override.Default.OverrideNBBO;
							}
							mds.Subscribe(symbol, source, "", secType);
							break;
						default:
							mds.Subscribe(symbol, source, "", secType);
							break;
					}
				}
			}
		}

		public static void SubscribeL2(string symbol, string source, string secType)
		{
			if (GLOBAL.HRDS.CheckSource(source))
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					if (_useGroupSubscription)
					{
						mds.Subscribe(symbol, source, "L2", secType);
					}
					else
					{
						switch (source.ToUpper())
						{
							case "CTA":
								foreach (string key in L2PartipcantCode.IDsCTA.Keys)
								{
									mds.Subscribe(string.Concat(new object[] { symbol, ".", key }), source, "", secType);
								}
								break;
							case "NASDAQ":
								foreach (string key in L2PartipcantCode.IDsNASDAQ.Keys)
								{
									mds.Subscribe(string.Concat(new object[] { symbol, ".", key }), source, "", secType);
								}
								break;
						}
					}
				}
			}
		}

		public static void SubscribeBook(string symbol, string source, string secType)
		{
			if (GLOBAL.HRDS.CheckSource(source))
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					mds.Subscribe("b" + symbol, source, "BOOK", secType);
				}
			}
		}

		public static void SubscribeSSF(string symbol, string source, string secType)
		{
			if (GLOBAL.HRDS.CheckSource(source))
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					mds.Subscribe(symbol, source, "", secType);
				}
			}
		}

		// OPRA AND OPRANBBO
		public static void SubscribeOption(string symbol)
		{
			SubscribeOption(symbol, "", true);
		}
		public static void SubscribeOption(string symbol, bool withNBBO)
		{
			SubscribeOption(symbol, "", withNBBO);
		}
		public static void SubscribeOption(string symbol, string symbolDetail, bool withNBBO)
		{
			if (GLOBAL.HRDS.CheckSource("OPRA"))
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					if (symbolDetail != "" && GLOBAL.OptionToExchangeMaps.ContainsKey(symbolDetail))
					{
						foreach (string key in OptionExchangeCode.IDs.Keys)
						{
							if (GLOBAL.OptionToExchangeMaps[symbolDetail].Contains(key))
							{
								mds.Subscribe(string.Concat(new object[] { symbol, ".", key }), OptionDataSource.OPRA, "", CSVFieldIDs.SecutrityTypes.Option);
							}
						}
					}
					else
					{
						foreach (string key in OptionExchangeCode.IDs.Keys)
						{
							mds.Subscribe(string.Concat(new object[] { symbol, ".", key }), OptionDataSource.OPRA, "", CSVFieldIDs.SecutrityTypes.Option);
						}
					}

					if (withNBBO)
					{
						SubScribeOptionNBBO(symbol);
					}
				}
			}
		}

		public static void SubscribeOptionNBBO(string symbol, string source)
		{
			switch (source)
			{
				case "CME":
				case "ICE":
					SubscribeOptionFuture(symbol, source);
					break;
				default:
					SubScribeOptionNBBO(symbol);
					break;
			}
		}
		private static void SubScribeOptionNBBO(string symbol)
		{
			if (GLOBAL.HRDS.CheckSource("OPRANBBO"))
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					if (Configuration.Override.Default.UseOverridNBBO)
					{
						mds.Subscribe(symbol, Configuration.Override.Default.OverrideNBBO, "", CSVFieldIDs.SecutrityTypes.Option);
					}
					else
					{
						mds.Subscribe(symbol, OptionDataSource.OPRANBBO, "", CSVFieldIDs.SecutrityTypes.Option);
					}
				}
			}
		}
		private static void SubscribeOptionFuture(string symbol, string source)
		{
			if (GLOBAL.HRDS.CheckSource(source))
			{
				foreach (HelperMDS mds in GLOBAL.HMDSs)
				{
					mds.Subscribe(symbol, source, "", CSVFieldIDs.SecutrityTypes.OptionFuture);
				}
			}
		}
		
		public static string WombatTypeBySymbolDetail(string symbolDetail)
		{
			if (GLOBAL.HRDS.SymbolDetailToRocSymbolMap.ContainsKey(symbolDetail))
			{
				return WombatTypeBySymbol(GLOBAL.HRDS.SymbolDetailToRocSymbolMap[symbolDetail]);
			}
			else
			{
				return WombatTypeBySymbol(symbolDetail);
			}
		}

		public static string WombatTypeBySymbol(string symbol)
		{
			string secType = "";

			if (GLOBAL.HRDS.SymbolSecurityInfos.ContainsKey(symbol))
			{
				secType = GLOBAL.HRDS.SymbolSecurityInfos[symbol].SecType;
			}
			else if (GLOBAL.HRDS.SymbolToOptionInfoMap.ContainsKey(symbol))
			{
				secType = GLOBAL.HRDS.SymbolToOptionInfoMap[symbol].MDSymbol;
			}

			return secType;
		}

		public static string WombatSymbolBySymbolDetail(string symbolDetail)
		{
			if (GLOBAL.HRDS.SymbolDetailToRocSymbolMap.ContainsKey(symbolDetail))
			{
				return WombatSymbolBySymbol(GLOBAL.HRDS.SymbolDetailToRocSymbolMap[symbolDetail]);
			}
			else
			{
				return WombatSymbolBySymbol(symbolDetail);
			}
		}

		public static string WombatSymbolBySymbol(string symbol)
		{
			string wombatSymbol = "";

			if (GLOBAL.HRDS.SymbolSecurityInfos.ContainsKey(symbol))
			{
				wombatSymbol = GLOBAL.HRDS.SymbolSecurityInfos[symbol].MDSymbol;
			}
			else if (GLOBAL.HRDS.SymbolToOptionInfoMap.ContainsKey(symbol))
			{
				wombatSymbol = GLOBAL.HRDS.SymbolToOptionInfoMap[symbol].MDSymbol;
			}

			if (wombatSymbol != "" && wombatSymbol.Contains("/P"))
			{
				wombatSymbol = wombatSymbol.Replace("/P", "p");
			}
			else if (wombatSymbol != "" && wombatSymbol.Contains("/RT"))
			{
				wombatSymbol = wombatSymbol.Replace("/RT", "r");
			}
			else if (wombatSymbol != "" && wombatSymbol.Contains("/RTWI"))
			{
				wombatSymbol = wombatSymbol.Replace("/RTWI", "rw");
			}
			return wombatSymbol;
		}

		public static string WombatSourceBySymbolDetail(string symbolDetail)
		{
			if (GLOBAL.HRDS.SymbolDetailToRocSymbolMap.ContainsKey(symbolDetail))
			{
				return WombatSourceBySymbol(GLOBAL.HRDS.SymbolDetailToRocSymbolMap[symbolDetail]);
			}
			else
			{
				return WombatSourceBySymbol(symbolDetail);
			}
		}

		public static string WombatSourceBySymbol(string symbol)
		{
			if (GLOBAL.HRDS.SymbolSecurityInfos.ContainsKey(symbol))
			{
				return GLOBAL.HRDS.SymbolSecurityInfos[symbol].MDSource;
			}
			else if (GLOBAL.HRDS.SymbolToOptionInfoMap.ContainsKey(symbol))
			{
				return GLOBAL.HRDS.SymbolToOptionInfoMap[symbol].MDSource;
			}
			else
			{
				return "";
			}
		}

		// Back from Wombat
		public static string ROCSymbolByIssueSymbol(string issueSymbol)
		{
			if (issueSymbol.Contains("p"))
			{
				issueSymbol = issueSymbol.Replace("p", "/PR");
			}
			else if (issueSymbol.Contains("r"))
			{
				issueSymbol = issueSymbol.Replace("r", "/RT");
			}
			else if (issueSymbol.Contains("rw"))
			{
				issueSymbol = issueSymbol.Replace("rw", "/RTWI");
			}

			return issueSymbol;
		}
	}
}
