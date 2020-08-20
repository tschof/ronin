using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.Collections.Generic;
using TposWrapper;
using System.Collections;

/// <summary>
/// Summary description for RdsGlobal
/// </summary>
public static class RdsGlobal
{
	private static bool _inIDE = !System.Diagnostics.Debugger.IsAttached;
	
	public static bool InIDE
	{
		get
		{
			return _inIDE;
		}
	}

	private static Dictionary<int, string> _futureDateCodeByMonth;
	public static Dictionary<int, string> FutureDateCodeByMonth
	{
		get
		{
			if (_futureDateCodeByMonth == null)
			{
				_futureDateCodeByMonth = new Dictionary<int, string>();
				_futureDateCodeByMonth.Add(1, "F");
				_futureDateCodeByMonth.Add(2, "G");
				_futureDateCodeByMonth.Add(3, "H");
				_futureDateCodeByMonth.Add(4, "J");
				_futureDateCodeByMonth.Add(5, "K");
				_futureDateCodeByMonth.Add(6, "M");
				_futureDateCodeByMonth.Add(7, "N");
				_futureDateCodeByMonth.Add(8, "Q");
				_futureDateCodeByMonth.Add(9, "U");
				_futureDateCodeByMonth.Add(10, "V");
				_futureDateCodeByMonth.Add(11, "X");
				_futureDateCodeByMonth.Add(12, "Z");
			}
			return _futureDateCodeByMonth;
		}
	}

	private static Dictionary<string, int> _futureDateCodeByCode;
	public static Dictionary<string, int> FutureDateCodeByCode
	{
		get
		{
			if (_futureDateCodeByCode == null)
			{
				_futureDateCodeByCode = new Dictionary<string, int>();
				_futureDateCodeByCode.Add("F", 1);
				_futureDateCodeByCode.Add("G", 2);
				_futureDateCodeByCode.Add("H", 3);
				_futureDateCodeByCode.Add("J", 4);
				_futureDateCodeByCode.Add("K", 5);
				_futureDateCodeByCode.Add("M", 6);
				_futureDateCodeByCode.Add("N", 7);
				_futureDateCodeByCode.Add("Q", 8);
				_futureDateCodeByCode.Add("U", 9);
				_futureDateCodeByCode.Add("V", 10);
				_futureDateCodeByCode.Add("X", 11);
				_futureDateCodeByCode.Add("Z", 12);
			}
			return _futureDateCodeByCode;
		}
	}

	private static List<string> _monthCode;
	public static List<string> MonthCode
	{
		get
		{
			if (_monthCode == null)
			{
				_monthCode = new List<string>();
				_monthCode.Add("F");
				_monthCode.Add("G");
				_monthCode.Add("H");
				_monthCode.Add("J");
				_monthCode.Add("K");
				_monthCode.Add("M");
				_monthCode.Add("N");
				_monthCode.Add("Q");
				_monthCode.Add("U");
				_monthCode.Add("V");
				_monthCode.Add("X");
				_monthCode.Add("Z");
			}
			return _monthCode;
		}
	}

	public static bool IsFutureBySymbol(string symbol)
	{
		string[] tArray = symbol.Split(new char[] { '.' });
		symbol = tArray[0];

		if (symbol.Length <= 0)
		{
			return false;
		}

		int year = 0;
		if (Int32.TryParse(symbol.Substring(symbol.Length - 1, 1), out year))
		{
			if (MonthCode.Contains(symbol.Substring(symbol.Length - 2, 1)))
			{
				return true;
			}
		}

		return false;
	}

	public static string ConvertToFutureDateCode(DateTime expDate)
	{
		if (FutureDateCodeByMonth.ContainsKey(expDate.Month))
		{
			return string.Concat(new object[] { FutureDateCodeByMonth[expDate.Month], expDate.Year.ToString().Substring(3, 1) });
		}

		return "";
	}

	public static string GetMappedTickerFromRoc(string ticker)
	{
		string tickerBase = "";
		string futureCode = "";

		if (ticker.Length >= 4 && RdsGlobal.IsFutureBySymbol(ticker))
		{
			tickerBase = ticker.Substring(0, ticker.Length - 2);
			futureCode = ticker.Replace(tickerBase, "");

			DataRow[] rows = TposMap.SymbolMap.Select("ROC = '" + tickerBase + "'");
			if (rows.Length > 0)
			{
				return rows[0]["TPOS"].ToString() + futureCode;
			}
		}

		return ticker;
	}

	public static string GetMappedTickerFromTpos(string ticker)
	{
		string tickerBase = "";
		string futureCode = "";

		if (ticker.Length >= 4 && RdsGlobal.IsFutureBySymbol(ticker))
		{
			tickerBase = ticker.Substring(0, ticker.Length - 2);
			futureCode = ticker.Replace(tickerBase, "");

			DataRow[] rows = TposMap.SymbolMap.Select("TPOS = '" + tickerBase + "'");
			if (rows.Length > 0)
			{
				return rows[0]["ROC"].ToString() + futureCode;
			}
		}

		return ticker;
	}

	public static double GetMappedMultTicker(string ticker)
	{
		string tickerBase = "";
		double mult = 1.0;

		if (ticker.Length >= 4 && RdsGlobal.IsFutureBySymbol(ticker))
		{
			tickerBase = ticker.Substring(0, ticker.Length - 2);

			DataRow[] rows = TposMap.SymbolMultMap.Select("ROC = '" + tickerBase + "'");
			if (rows.Length > 0)
			{
				if (rows[0]["MULT"] != null && rows[0]["MULT"].ToString() != "")
				{
					Double.TryParse(rows[0]["MULT"].ToString(), out mult);
				}
			}
		}

		return mult;
	}

	public static ArrayList ConvertToRocPositions(ArrayList positions)
	{
		TposPosition position;
		DateTime expDate = DateTime.Now;

		foreach (object posObj in positions)
		{
			if (posObj != null)
			{
				position = (TposPosition)posObj;

				// Conver to ROC Symbol
				DataRow[] rows = TposMap.SymbolMap.Select("TPOS = '" + position.m_Symbol + "'");
				if (rows.Length > 0)
				{
					position.m_Symbol = rows[0]["ROC"].ToString();
				}

				if (position.m_SecType == "F")
				{
					// Add the Date Code
					if (DateTime.TryParse(position.m_ExpDate, out expDate))
					{
						position.m_Symbol = position.m_Symbol + RdsGlobal.ConvertToFutureDateCode(expDate);
					}
				}

				position.m_UndSymbol = GetMappedTickerFromTpos(position.m_UndSymbol);
				
				if (position.m_Symbol.Contains(@"/P"))
				{
					position.m_Symbol = position.m_Symbol.Replace(@"/P", @"/PR");
				}
			}
		}

		return positions;
	}

	public static ArrayList ConvertToRocTrades(ArrayList trades)
	{
		TposTrade trade;
		DateTime expDate = DateTime.Now;

		DataRow[] rows = new DataRow[0];
		double mult = 1;

		foreach (object tradeObj in trades)
		{
			if (tradeObj != null)
			{
				trade = (TposTrade)tradeObj;

				if (trade.m_SecType == "F")
				{
					// Convert to ROC symbol
					rows = TposMap.SymbolMap.Select("TPOS = '" + trade.m_Symbol + "'");
					if (rows.Length > 0)
					{
						trade.m_Symbol = rows[0]["ROC"].ToString();
					}

					// Convert to ROC Price
					rows = TposMap.SymbolMultMap.Select("ROC = '" + trade.m_Symbol + "'");
					if (rows.Length > 0)
					{
						if (rows[0]["MULT"] != null && rows[0]["MULT"].ToString() != "")
						{
							if (Double.TryParse(rows[0]["MULT"].ToString(), out mult))
							{
								trade.m_Price = trade.m_Price / mult;
							}
						}
					}

					// Add the Date Code
					if (DateTime.TryParse(trade.m_ExpDate, out expDate))
					{
						trade.m_Symbol = trade.m_Symbol + RdsGlobal.ConvertToFutureDateCode(expDate);
					}
				}

				if (trade.m_Symbol.Contains(@"/P"))
				{
					trade.m_Symbol = trade.m_Symbol.Replace(@"/P", @"/PR");
				}
			}
		}

		return trades;
	}
}
