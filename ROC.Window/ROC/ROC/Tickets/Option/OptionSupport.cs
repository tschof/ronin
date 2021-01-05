using System;
using System.Collections.Generic;

using Common;

namespace ROC
{
	public class OptionSupport : BaseTicket
	{
		internal bool IsStock
		{
			get
			{
				if (CurrentSecInfo != null &&
					CurrentSecInfo.SecType == CSVEx.CSVFieldIDs.SecurityTypes.Equity)
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
					CurrentSecInfo.SecType == CSVEx.CSVFieldIDs.SecurityTypes.Future)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}

		private List<string> _gettingOptionChainSymbols;
		internal List<string> GettingOptionChainSymbols
		{
			get
			{
				if (_gettingOptionChainSymbols == null)
				{
					_gettingOptionChainSymbols = new List<string>();
				}
				return _gettingOptionChainSymbols;
			}
			set
			{
				_gettingOptionChainSymbols = value;
			}
		}

		#region - Plus one month check -

		private int _monthToAdd = 0;
		internal int MonthToAdd
		{
			get
			{
				return _monthToAdd;
			}
			set
			{
				_monthToAdd = value;
			}
		}

		internal void PlusOneCheck()
		{
			if (CurrentSecInfo.MDSource == "CME" && MonthToAdd == 0)
			{
				switch (CurrentSecInfo.SecType)
				{
					case CSVEx.CSVFieldIDs.SecurityTypes.Future:
						if (HelperOption.PlusOne.Contains(CurrentSecInfo.Underlying))
						{
							MonthToAdd = 1;
						}
						break;
				}
			}
		}

		internal string ConvertToDisplayDate(DateTime date)
		{
			string result = "";

			if (MonthToAdd > 0)
			{
				DateTime tDT = date.AddMonths(MonthToAdd);
				result = String.Format("{0} ({1})", new object[] { tDT.ToString("yy MMM").ToUpper(), date.ToString("yy MMM dd").ToUpper() });
			}
			else
			{
				result = String.Format("{0}", new object[] { date.ToString("yy MMM dd").ToUpper() });
			}

			return result;
		}

		#endregion

		#region - Display as 64th check -

		private bool _is64Th = false;
		internal bool Is64Th
		{
			get
			{
				return _is64Th;
			}
			set
			{
				_is64Th = value;
			}
		}

		internal void Show64ThCheck()
		{
			if (CurrentSecInfo.MDSource == "CME")
			{
				switch (CurrentSecInfo.SecType)
				{
					case CSVEx.CSVFieldIDs.SecurityTypes.Future:
						if (SettingData.Utility.TryGetBaseSymbol(CurrentSecInfo.Underlying, out string symbol))
						{
							symbol = CurrentSecInfo.Underlying;
						}
						if (HelperOption.ShowAs64TH.Contains(symbol))
						{
							Is64Th = true;
						}
						break;
				}
			}
		}

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
						if (Is64Th)
						{
							floorDouble = Math.Floor(Math.Abs(value));
							remainderDouble = Math.Abs(value) - floorDouble;
							remainderDouble = remainderDouble / 0.015625;
							formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
							normal = false;
						}
						else
						{
							floorDouble = Math.Floor(Math.Abs(value));
							remainderDouble = Math.Abs(value) - floorDouble;
							remainderDouble = remainderDouble / 0.03125;
							formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
							normal = false;
						}
						break;
					case "0.0025":			// 1/400
					case "0.00125":			// 1/800
						floorDouble = Math.Floor(Math.Abs(value) * 100);
						remainderDouble = (Math.Abs(value) * 100) - floorDouble;
						remainderDouble = remainderDouble - Math.Floor(remainderDouble);
						floorDouble = (floorDouble / 100);
						switch (remainderDouble.ToString("F3"))
						{
							case "0.125":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 1);
								break;
							case "0.250":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 2);
								break;
							case "0.375":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 3);
								break;
							case "0.500":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 4);
								break;
							case "0.625":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 5);
								break;
							case "0.750":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 6);
								break;
							case "0.875":
								formatedValue = string.Format("{0}{1}", floorDouble.ToString("F2"), 7);
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
							}
							else if (parts[1].Length >= 3 && parts[1].Substring(2, 1) == "0")
							{
								parts[1] = parts[1].Substring(0, 2);
							}
							else if (parts[1].Length >= 3 && parts[1].Substring(2, 1) == "5")
							{
								parts[1] = parts[1].Substring(0, 2) + ".5";
							}
							if (Is64Th)
							{
								double.TryParse(parts[1], out tempPrice);
								if (tempPrice > 64)
								{
									price = 0;
								}
								else
								{
									tempPrice = tempPrice * 0.015625;
									price = price + tempPrice;
								}

							}
							else
							{
								double.TryParse(parts[1], out tempPrice);
								if (tempPrice > 32)
								{
									price = 0;
								}
								else
								{
									tempPrice = tempPrice * 0.03125;
									price = price + tempPrice;
								}
							}
							break;
						case "0.0025":			// 1/400
						case "0.00125":			// 1/800
							if (parts[1].Length >= 3)
							{
								switch (parts[1].Substring(0, 1))
								{
									case "1":
										parts[1] = parts[1].Substring(0, 2) + "125";
										break;
									case "2":
										parts[1] = parts[1].Substring(0, 2) + "25";
										break;
									case "3":
										parts[1] = parts[1].Substring(0, 2) + "375";
										break;
									case "4":
										parts[1] = parts[1].Substring(0, 2) + "50";
										break;
									case "5":
										parts[1] = parts[1].Substring(0, 2) + "625";
										break;
									case "6":
										parts[1] = parts[1].Substring(0, 2) + "75";
										break;
									case "7":
										parts[1] = parts[1].Substring(0, 2) + "875";
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
	}
}
