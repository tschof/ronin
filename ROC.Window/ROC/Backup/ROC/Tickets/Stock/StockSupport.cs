using System;
using System.Collections.Generic;
using System.Text;

namespace ROC
{
	public class StockSupport : BaseTicket
	{
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
			double fromatedDouble = value;
			string formatedValue = "";

			if (DefaultPriceFormat == "F")
			{
				formatedValue = fromatedDouble.ToString("F2");
			}
			else
			{
				formatedValue = fromatedDouble.ToString("N2");
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
	}
}
