using System;
using MarketData;

using Price = Common.Price;

namespace ROC
{
	public class BookSupport : BaseTicket //, BookISupport
	{
		private const int STOCK_PRICE_DIGITS = 2;
		private const int FUTURE_PRICE_DIGITS = 7;

		internal bool IsStock => ((CurrentSecInfo != null) && (CurrentSecInfo.SecType == CSVEx.CSVFieldIDs.SecurityTypes.Equity));
		internal bool IsFuture => ((CurrentSecInfo != null) && (CurrentSecInfo.SecType == CSVEx.CSVFieldIDs.SecurityTypes.Future));

		#region - BookISupport Members -

		public Book UpdateMarketData(Market deltas) // Market
		{
			return UpdateMarketData(deltas, null, null, null, null);
		}

		public Book UpdateMarketData(Market deltas, Price? askPrice, Price? bidPrice, Price? highPrice, Price? lowPrice)
		{
			// Market deltas

			if (CurrentSecInfo == null)
				return null;

			Book result = new Book();
			Book found;

			if (deltas.TryGet(CurrentSecInfo.MDSymbol, out found))
			{
				result.Merge(found);
				if (IsStock)
				{
					makeStockBook(ref result, askPrice, bidPrice, highPrice, lowPrice);
					return result;
				}
			}

			if (IsFuture)
			{
				string symbolDetailBook = "b" + CurrentSecInfo.MDSymbol;
				if (deltas.TryGet(symbolDetailBook, out found))
					result.Merge(found);

				result.RoundPrices(FUTURE_PRICE_DIGITS, askPrice, bidPrice, highPrice, lowPrice);
				result.SortLevels(BookDepthLimit, FUTURE_PRICE_DIGITS);

				return result;
			}

			return null;
		}

		private void makeStockBook(ref Book delta, Price? askPrice, Price? bidPrice, Price? highPrice, Price? lowPrice)
		{
			delta.RoundPrices(STOCK_PRICE_DIGITS, askPrice, bidPrice, highPrice, lowPrice);

			double deltaPrice;
			long deltaSize;

			if (delta.TryGetField(Book.FieldEnum.AskPrice, out deltaPrice) &&
				delta.TryGetField(Book.FieldEnum.AskSize, out deltaSize))
			{
				double makePrice = Math.Round(deltaPrice, 2);
				long makeSize = deltaSize * 100;
				delta.SetAsk(makePrice, makeSize);
				delta.SetField(Book.FieldEnum.AskSize, makeSize, true);
				delta.SetField(Book.FieldEnum.AskPrice, makePrice, true);
			}

			if (delta.TryGetField(Book.FieldEnum.BidPrice, out deltaPrice) &&
				delta.TryGetField(Book.FieldEnum.BidSize, out deltaSize)) 
			{
				double makePrice = Math.Round(deltaPrice, 2);
				long makeSize = deltaSize * 100;
				delta.SetBid(makePrice, makeSize);
				delta.SetField(Book.FieldEnum.BidSize, makeSize, true);
				delta.SetField(Book.FieldEnum.BidPrice, makePrice, true);
			}
		}

		#endregion

		#region - Price Display -

		internal string DefaultPriceFormat = "F";

		internal string GetDoubleFormat(double value)
		{
			double fromatedDouble;
			string formatedValue = "";

			if (IsFuture)
			{
				bool normal = true;
				bool done = false;
				double tickSize = CurrentSecInfo.TickSize;

				double floorDouble;
				double remainderDouble;
				int decimalPlace;

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
										formatedValue = string.Concat(formatedValue, "00");
										break;
									case 1:
										formatedValue = string.Concat(formatedValue, "0");
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
					double.TryParse(parts[0], out price);
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
								double.TryParse(parts[1], out tempPrice);
							}
							else if (parts[1].Length == 3 && parts[1].Substring(0, 1) == "0")
							{
								parts[1] = parts[1].Substring(1, 2);
								if (double.TryParse(parts[1], out tempPrice))
								{
									tempPrice = tempPrice / 10;
								}

							}
							else if (parts[1].Length >= 3 && parts[1].Substring(2, 1) == "0")
							{
								parts[1] = parts[1].Substring(0, 2);
								double.TryParse(parts[1], out tempPrice);
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
							double.TryParse("." + parts[1].TrimEnd(new char[] { '0' }), out tempPrice);
							price = price + tempPrice;
							break;
					}
				}
			}
			else
			{
				double.TryParse(value, out price);
			}

			return price;
		}

		#endregion

		#region - Price Check -

		// 1/128 in srlabs are chooped off at 6 decimal places.
		internal void SRLabsPriceCheck(Book delta)
		{
			double ticksize = CurrentSecInfo.TickSize;
			if (ticksize == Common.Price.EIGHTH_TICK)
				delta.SnapTo(ticksize, 0.0000005);
		}

		#endregion
	}
}
