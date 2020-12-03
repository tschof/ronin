using System;
using System.Collections.Generic;
using System.Text;
using MarketDataEx;
using ArrayEx;

namespace ROC
{
	public class BookSupport : BaseTicket, BookISupport
	{
		internal bool IsStock
		{
			get
			{
				if (CurrentSecInfo != null &&
					CurrentSecInfo.SecType == CSVEx.CSVFieldIDs.SecutrityTypes.Equity)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		internal bool IsFuture
		{
			get
			{
				if (CurrentSecInfo != null &&
					CurrentSecInfo.SecType == CSVEx.CSVFieldIDs.SecutrityTypes.Future)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		#region - BookISupport Members -

		public MarketDataEx.MDServerToClient UpdateMarketData(Dictionary<string, MarketDataEx.MDServerToClient> deltas)
		{
			return UpdateMarketData(deltas, null, null, null, null);
		}

		public MarketDataEx.MDServerToClient UpdateMarketData(Dictionary<string, MarketDataEx.MDServerToClient> deltas, double? askPrice, double? bidPrice, double? highPrice, double? lowPrice)
		{
			MDServerToClient result = new MDServerToClient();

			if (CurrentSecInfo != null)
			{
				if (deltas.ContainsKey(CurrentSecInfo.MDSymbol))
				{
					result.Update(deltas[CurrentSecInfo.MDSymbol]);

					if (IsStock)
					{
						MakeBook(ref result, askPrice, bidPrice, highPrice, lowPrice);
						return result;
					}
				}

				if (IsFuture)
				{
					string symbolDetailBook = "b" + CurrentSecInfo.MDSymbol;
					if (deltas.ContainsKey(symbolDetailBook))
					{
						result.Update(deltas[symbolDetailBook]);
					}

					CheckBook(ref result, askPrice, bidPrice, highPrice, lowPrice);
					//CheckPrices(ref result, askPrice, bidPrice, highPrice, lowPrice);
					//CheckBook(ref result);
					return result;
				}
			}
			return null;
		}

		private void MakeBook(ref MDServerToClient delta, Nullable<double> askPrice, Nullable<double> bidPrice, Nullable<double> highPrice, Nullable<double> lowPrice)
		{
			double price = 0;
			long size = 0;

			CheckPrices(ref delta, askPrice, bidPrice, highPrice, lowPrice);

			if (delta.AskPrice != null && delta.AskSize != null)
			{
				size = (long)delta.AskSize * 100;
				price = Math.Round((double)delta.AskPrice, 2);

				delta.Update(WombatFieldIDs.AskSize, size);
				if (delta.AskBooks.ContainsKey(price))
				{
					delta.AskBooks[price] = size;
				}
				else
				{
					delta.AskBooks.Add(price, size);
				}
			}

			if (delta.BidPrice != null && delta.BidSize != null)
			{
				size = (long)delta.BidSize * 100;
				price = Math.Round((double)delta.BidPrice, 2);

				delta.Update(WombatFieldIDs.BidSize, size);
				if (delta.BidBooks.ContainsKey(price))
				{
					delta.BidBooks[price] = size;
				}
				else
				{
					delta.BidBooks.Add(price, size);
				}
			}
		}

		private void CheckBook(ref MDServerToClient delta)
		{
			delta.AskBooks = CheckBook("Ask", delta.AskBooks);
			delta.BidBooks = CheckBook("Bid", delta.BidBooks);
		}

		// Used to build Books by (Ask and Bid)
		private void CheckBook(ref MDServerToClient delta, Nullable<double> askPrice, Nullable<double> bidPrice, Nullable<double> highPrice, Nullable<double> lowPrice)
		{
			CheckPrices(ref delta, askPrice, bidPrice, highPrice, lowPrice);

			if (delta.AskBooks.Count > BookDepthLimit && delta.AskPrice != null)
			{
				delta.AskBooks = CheckBook("Ask", delta.AskBooks, Math.Round((double)delta.AskPrice, 7));
			}

			if (delta.BidBooks.Count > BookDepthLimit && delta.BidPrice != null)
			{
				delta.BidBooks = CheckBook("Bid", delta.BidBooks, Math.Round((double)delta.BidPrice, 7));
			}

			// To prevent cross books check againist BidPrice for AskBook
			//if (delta.BidPrice != null && CurrentSecInfo != null)
			//{
			//    if ((double)delta.BidPrice == 0)
			//    {
			//        delta.AskBooks = CheckBook("Ask", delta.AskBooks, 0);
			//    }
			//    else
			//    {
			//        delta.AskBooks = CheckBook("Ask", delta.AskBooks, Math.Round((double)delta.BidPrice + CurrentSecInfo.TickSize, 7));
			//    }
			//}
			//else
			//{
			//    delta.AskBooks.Clear();
			//}

			// To prevent cross books check againist AskPrice for BidBook
			//if (delta.AskPrice != null && CurrentSecInfo != null)
			//{
			//    if ((double)delta.BidPrice == 0)
			//    {
			//        delta.BidBooks = CheckBook("Bid", delta.BidBooks, 0);
			//    }
			//    else
			//    {
			//        delta.BidBooks = CheckBook("Bid", delta.BidBooks, Math.Round((double)delta.AskPrice - CurrentSecInfo.TickSize, 7));
			//    }
			//}
			//else
			//{
			//    delta.BidBooks.Clear();
			//}
		}
		private Dictionary<double, long> CheckBook(string side, Dictionary<double, long> book, double limitPrice)
		{
			Dictionary<double, long> result = new Dictionary<double, long>();

			try
			{
				if (book.Count > 0)
				{
					List<double> prices = ArrayHelper.ConvertToList(book);
					switch (side)
					{
						case "Bid":
							prices.Sort();
							prices.Reverse();
							break;
						case "Ask":
							prices.Sort();
							break;
					}

					if (limitPrice == 0)
					{
						// Make the top of the book to be the limit price
						limitPrice = Math.Round(prices[0], 7);
					}

					double currentPrice = 0;
					foreach (double price in prices)
					{
						if (result.Count > BookDepthLimit)
						{
							// End of the book limit
							break;
						}

						currentPrice = Math.Round(price, 7);

						switch (side)
						{
							case "Bid":
								// Bid
								if (limitPrice != 0 && currentPrice <= limitPrice && !result.ContainsKey(currentPrice))
								{
									result.Add(currentPrice, book[price]);
								}
								break;
							case "Ask":
								// Ask
								if (limitPrice != 0 && currentPrice >= limitPrice && !result.ContainsKey(currentPrice))
								{
									result.Add(currentPrice, book[price]);
								}
								break;
						}
					}
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}

			return result;
		}
		private Dictionary<double, long> CheckBook(string side, Dictionary<double, long> book)
		{
			Dictionary<double, long> result = new Dictionary<double, long>();

			if (book.Count > 0)
			{
				List<double> prices = ArrayHelper.ConvertToList(book);
				switch (side)
				{
					case "Bid":
						prices.Sort();
						prices.Reverse();
						break;
					case "Ask":
						prices.Sort();
						break;
				}

				double currentPrice = 0;
				foreach (double price in prices)
				{
					if (result.Count > BookDepthLimit)
					{
						// End of the book limit
						break;
					}

					currentPrice = Math.Round(price, 7);
					result.Add(currentPrice, book[price]);
				}
			}

			return result;
		}

		private void CheckPrices(ref MDServerToClient delta, Nullable<double> askPrice, Nullable<double> bidPrice, Nullable<double> highPrice, Nullable<double> lowPrice)
		{
			if (delta.AskPrice == null && askPrice != null)
			{
				if (IsStock)
				{
					delta.Update(WombatFieldIDs.AskPrice, Math.Round((double)askPrice, 2));
				}
				else
				{
					delta.Update(WombatFieldIDs.AskPrice, Math.Round((double)askPrice, 7));
				}
			}
			if (delta.BidPrice == null && bidPrice != null)
			{
				if (IsStock)
				{
					delta.Update(WombatFieldIDs.BidPrice, Math.Round((double)bidPrice, 2));
				}
				else
				{
					delta.Update(WombatFieldIDs.BidPrice, Math.Round((double)bidPrice, 7));
				}
			}

			if (IsStock && delta.TradePrice != null)
			{
				if (IsStock)
				{
					delta.Update(WombatFieldIDs.TradePrice, Math.Round((double)delta.TradePrice, 2));
				}
				else
				{
					delta.Update(WombatFieldIDs.TradePrice, Math.Round((double)delta.TradePrice, 7));
				}
			}

			if (highPrice != null && lowPrice != null)
			{
				if (IsStock)
				{
					CheckHighLow(ref delta, Math.Round((double)highPrice, 2), Math.Round((double)lowPrice, 2));
				}
				else
				{
					CheckHighLow(ref delta, Math.Round((double)highPrice, 7), Math.Round((double)lowPrice, 7));
				}
			}
		}

		private void CheckHighLow(ref MDServerToClient delta, double highPrice, double lowPrice)
		{
			if (delta.HighPrice == null && delta.TradePrice != null)
			{
				if (highPrice == 0 || (double)delta.TradePrice > highPrice)
				{
					// The traded price should already rounded for stock tick table
					delta.Update(WombatFieldIDs.HighPrice, (double)delta.TradePrice);
				}
			}

			if (delta.LowPrice == null && delta.TradePrice != null)
			{
				if (lowPrice == 0 || (double)delta.TradePrice < lowPrice)
				{
					// The traded price should already rounded for stock tick table
					delta.Update(WombatFieldIDs.LowPrice, (double)delta.TradePrice);
				}
			}
		}

		#endregion

		#region - Price Display -

		private string _defaultPriceFormat = "F";
		internal string DefaultPriceFormat
		{
			get
			{
				return _defaultPriceFormat;
			}
			set
			{
				_defaultPriceFormat = value;
			}
		}

		internal string GetDoubleFormat(double value)
		{
			double fromatedDouble = 0;
			string formatedValue = "";

			if (IsFuture)
			{
				bool normal = true;
				bool done = false;
				double tickSize = CurrentSecInfo.TickSize;

				double floorDouble = 0;
				double remainderDouble = 0;
				int decimalPlace = -1;

				#region - Fractional Tick Size -

				switch (tickSize.ToString())
				{
					case "0.5":
						break;
					case "0.125":			// 1/8
					case "0.0625":			// 1/16
					case "0.03125":			// 1/32
					case "0.015625":		// 1/64
					case "0.0078125":		// 1/128
						floorDouble = Math.Floor(Math.Abs(value));
						remainderDouble = Math.Abs(value) - floorDouble;
						remainderDouble = remainderDouble / 0.03125;
						formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
						normal = false;
						break;
					case "0.0025":			// 1/400
						floorDouble = Math.Floor(Math.Abs(value) * 100);
						remainderDouble = (Math.Abs(value) * 100) - floorDouble;
						remainderDouble = remainderDouble - Math.Floor(remainderDouble);
						floorDouble = (floorDouble / 100);
						switch (remainderDouble.ToString("F2"))
						{
							case "0.25":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 2);
								break;
							case "0.50":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 4);
								break;
							case "0.75":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 6);
								break;
							default:
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 0);
								break;
						}
						formatedValue = formatedValue.Replace(".", "'");
						normal = false;
						break;
					default:
						break;
				}

				#endregion

				#region - Normal Tick Size -

				if (normal)
				{
					fromatedDouble = Math.Round(value, CurrentSecInfo.DecimalPlaces);
					//_fromatedDouble = _fromatedDouble * DisplayFactor;
					switch (CurrentSecInfo.DecimalPlaces)
					{
						case 0:
						case 1:
						case 2:
						case 3:
						case 4:
							if (DefaultPriceFormat == "F")
							{
								formatedValue = fromatedDouble.ToString("F" + CurrentSecInfo.DecimalPlaces.ToString());
							}
							else
							{
								formatedValue = fromatedDouble.ToString("N" + CurrentSecInfo.DecimalPlaces.ToString());
							}
							break;
						default:
							if (tickSize >= 1)
							{
								if (DefaultPriceFormat == "F")
								{
									formatedValue = fromatedDouble.ToString("F0");
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N0");
								}
								done = true;
							}
							else if (tickSize == 0.5 || tickSize == 0.1)
							{
								if (DefaultPriceFormat == "F")
								{
									formatedValue = fromatedDouble.ToString("F1");
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N1");
								}
								done = true;
							}

							if (!done)
							{
								if (DefaultPriceFormat == "F")
								{
									formatedValue = fromatedDouble.ToString("F7").TrimEnd(new char[] { '0' });
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N7").TrimEnd(new char[] { '0' });
								}
								decimalPlace = formatedValue.IndexOf('.');
								decimalPlace = (formatedValue.Substring(decimalPlace + 1)).Length;

								switch (decimalPlace)
								{
									case 0:
										formatedValue = String.Concat(new object[] { formatedValue, "00" });
										break;
									case 1:
										formatedValue = String.Concat(new object[] { formatedValue, "0" });
										break;
									default:
										break;
								}
							}
							break;
					}
				}

				#endregion
			}
			else
			{
				fromatedDouble = value;
				if (DefaultPriceFormat == "F")
				{
					formatedValue = fromatedDouble.ToString("F2");
				}
				else
				{
					formatedValue = fromatedDouble.ToString("N2");
				}
			}

			return formatedValue;
		}

		internal double GetPriceFromUserInput(string value)
		{
			double price = 0;

			if (value.Contains("'"))
			{
				double tickSize = CurrentSecInfo.TickSize;
				string[] parts = value.Split(new string[] { "'" }, StringSplitOptions.None);
				double tempPrice = 0;

				if (parts.Length == 2)
				{
					Double.TryParse(parts[0], out price);
					// Fraction Part
					switch (tickSize.ToString())
					{
						case "0.125":			// 1/8
						case "0.0625":			// 1/16
						case "0.03125":			// 1/32
						case "0.015625":		// 1/64
						case "0.0078125":		// 1/128
							if (parts[1].Length < 2)
							{
								parts[1] = parts[1] + "0";
								Double.TryParse(parts[1], out tempPrice);
							}
							else if (parts[1].Length == 3 && parts[1].Substring(0, 1) == "0")
							{
								parts[1] = parts[1].Substring(1, 2);
								if (Double.TryParse(parts[1], out tempPrice))
								{
									tempPrice = tempPrice / 10;
								}

							}
							else if (parts[1].Length >= 3 && parts[1].Substring(2, 1) == "0")
							{
								parts[1] = parts[1].Substring(0, 2);
								Double.TryParse(parts[1], out tempPrice);
							}
							if (tempPrice > 32)
							{
								price = 0;
							}
							else
							{
								tempPrice = tempPrice * 0.03125;
								price = price + tempPrice;
							}
							break;
						case "0.0025":			// 1/400
							if (parts[1].Length >= 3)
							{
								switch (parts[1].Substring(2, 1))
								{
									case "2":
										parts[1] = parts[1].Substring(0, 2) + "25";
										break;
									case "4":
										parts[1] = parts[1].Substring(0, 2) + "50";
										break;
									case "6":
										parts[1] = parts[1].Substring(0, 2) + "75";
										break;
									case "0":
									default:
										parts[1] = parts[1].Substring(0, 2) + "00";
										break;
								}
							}
							Double.TryParse("." + parts[1].TrimEnd(new char[] { '0' }), out tempPrice);
							price = price + tempPrice;
							break;
					}
				}
			}
			else
			{
				Double.TryParse(value, out price);
			}

			return price;
		}

		#endregion

		#region - Price Check -

		// 1/128 in srlabs are chooped off at 6 decimal places.
		internal void SRLabsPriceCheck(MDServerToClient delta)
		{
			double ticksize = CurrentSecInfo.TickSize;
			if (ticksize == 0.0078125)
			{
				if (delta.AskPrice != null && (double)delta.AskPrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.AskPrice, (double)delta.AskPrice + 0.0000005);
				}
				if (delta.BidPrice != null && (double)delta.BidPrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.BidPrice, (double)delta.BidPrice + 0.0000005);
				}
				if (delta.TradePrice != null && (double)delta.TradePrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.TradePrice, (double)delta.TradePrice + 0.0000005);
				}
				if (delta.HighPrice != null && (double)delta.HighPrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.HighPrice, (double)delta.HighPrice + 0.0000005);
				}
				if (delta.LowPrice != null && (double)delta.LowPrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.LowPrice, (double)delta.LowPrice + 0.0000005);
				}
				if (delta.ClosePrice != null && (double)delta.ClosePrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.ClosePrice, (double)delta.ClosePrice + 0.0000005);
				}
				if (delta.PrevClosePrice != null && (double)delta.PrevClosePrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.PrevClosePrice, (double)delta.PrevClosePrice + 0.0000005);
				}
				if (delta.SettlePrice != null && (double)delta.SettlePrice % ticksize != 0)
				{
					delta.Update(WombatFieldIDs.SettlePrice, (double)delta.SettlePrice + 0.0000005);
				}

				if (delta.AskBooks.Count > 0)
				{
					Dictionary<double, long> book = new Dictionary<double, long>();
					foreach (double key in delta.AskBooks.Keys)
					{
						if (key % ticksize == 0)
						{
							if (!book.ContainsKey(key))
							{
								book.Add(key, delta.AskBooks[key]);
							}
							else
							{
								book[key] = delta.AskBooks[key];
							}
						}
						else
						{
							double newKey = key + 0.0000005;
							if (!book.ContainsKey(newKey))
							{
								book.Add(newKey, delta.AskBooks[key]);
							}
							else
							{
								book[newKey] = delta.AskBooks[key];
							}
						}
					}

					delta.AskBooks = new Dictionary<double, long>(book);
				}

				if (delta.BidBooks.Count > 0)
				{
					Dictionary<double, long> book = new Dictionary<double, long>();
					foreach (double key in delta.BidBooks.Keys)
					{
						if (key % ticksize == 0)
						{
							if (!book.ContainsKey(key))
							{
								book.Add(key, delta.BidBooks[key]);
							}
							else
							{
								book[key] = delta.BidBooks[key];
							}
						}
						else
						{
							double newKey = key + 0.0000005;
							if (!book.ContainsKey(newKey))
							{
								book.Add(newKey, delta.BidBooks[key]);
							}
							else
							{
								book[newKey] = delta.BidBooks[key];
							}
						}
					}

					delta.BidBooks = new Dictionary<double, long>(book);
				}
			}
		}

		#endregion
	}
}
