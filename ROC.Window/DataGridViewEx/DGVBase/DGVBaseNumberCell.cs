using System;
using System.Drawing;
using System.Diagnostics;
using System.Drawing.Drawing2D;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseNumberCell : DGVBaseCellNoPaint
	{
		private DGVBaseNumberColumn owningColumn
		{
			get
			{
				return (DGVBaseNumberColumn)OwningColumn;
			}
		}

		private string SymbolDetail
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex <= DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("SymbolDetail") && OwningRow.Cells["SymbolDetail"] != null)
					{
						return (string)OwningRow.Cells["SymbolDetail"].Value;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return "";
			}
		}

		private char[] symbolSpliter = new char[] { ' ' };
		private string UnderlyingSymbol
		{
			get
			{
				try
				{
					if (SymbolDetail != "" && SymbolDetail.Contains(" "))
					{
						string[] t = SymbolDetail.Split(symbolSpliter);
						if (t.Length > 0)
						{
							return t[0];
						}
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return "";
			}
		}

		private string BaseSymbol
		{
			get
			{
				try
				{
					if (UnderlyingSymbol != "" && UnderlyingSymbol.Length >= 4)
					{
						int num = 0;
						if (int.TryParse(UnderlyingSymbol.Substring(UnderlyingSymbol.Length - 1, 1), out num))
						{
							switch (UnderlyingSymbol.Substring(UnderlyingSymbol.Length - 2, 1))
							{
								case GLOBAL.MonthCodes.January:
								case GLOBAL.MonthCodes.February:
								case GLOBAL.MonthCodes.March:
								case GLOBAL.MonthCodes.April:
								case GLOBAL.MonthCodes.May:
								case GLOBAL.MonthCodes.June:
								case GLOBAL.MonthCodes.July:
								case GLOBAL.MonthCodes.August:
								case GLOBAL.MonthCodes.September:
								case GLOBAL.MonthCodes.October:
								case GLOBAL.MonthCodes.November:
								case GLOBAL.MonthCodes.December:
									return UnderlyingSymbol.Substring(0, UnderlyingSymbol.Length - 2);
								default:
									break;
							}
						}
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}

				return "";
			}
		}

		public double TickSize
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex <= DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("TickSize") && OwningRow.Cells["TickSize"].Value != null)
					{
						return (double)OwningRow.Cells["TickSize"].Value;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return 0.01;
			}
		}

		public string SecType
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex <= DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("SecType") && OwningRow.Cells["SecType"].Value != null)
					{
						return OwningRow.Cells["SecType"].Value.ToString();
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return "";
			}
		}

		public double DisplayFactor
		{
			get
			{
				try
				{
					if (RowIndex >= 0 && RowIndex < DataGridView.Rows.Count - 1 && DataGridView.Columns.Contains("DisplayFactor") && OwningRow.Cells["DisplayFactor"] != null)
					{
						return (double)OwningRow.Cells["DisplayFactor"].Value;
					}
				}
				catch (Exception ex)
				{
					if (Debugger.IsAttached)
					{
						Debug.Print(ex.Message);
						Debug.Print(ex.StackTrace);
					}
				}
				return 1;
			}
		}

		public override string GetDisplayValue(object value)
		{
			if (value != null && value.ToString() != "")
			{
				switch (ValueType.Name)
				{
					case "Decimal":
					case "Double":
						return GetDoubleFormat(Convert.ToDouble(value));
					case "Int":
					case "Int64":
					case "Long":
						return GetLongFormat(Convert.ToInt64(value));
					case "String":
					default:
						break;
				}
			}

			return "";
		}

		private string GetLongFormat(long value)
		{
			if (owningColumn.DefaultCellStyle.Format == "F")
			{
				return value.ToString("F0");
			}
			else
			{
				return value.ToString("N0");
			}
		}

		private string GetDoubleFormat(double value)
		{
			string secType = SecType;
			double fromatedDouble = 0;
			string formatedValue = "";

			switch (secType)
			{
				case "E":
					#region - Stock -

					if (owningColumn.AllowResolutionChange)
					{
						fromatedDouble = Math.Round(value, owningColumn.MaxDecimal);
						if (owningColumn.DefaultCellStyle.Format == "F")
						{
							formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
						}
						else
						{
							formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
						}
					}
					else
					{
						switch (owningColumn.Name)
						{
							case "Factor":
							case "Ratio":
							case "UserRatio":
								// Speical Case Auto Spread Setting Grid
								fromatedDouble = Math.Round(value, 7);
								if (owningColumn.DefaultCellStyle.Format == "F")
								{
									formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
								}
								break;
							default:
								fromatedDouble = Math.Round(value, 2);
								if (owningColumn.MaxDecimal < 2)
								{
									if (owningColumn.DefaultCellStyle.Format == "F")
									{
										formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
									}
									else
									{
										formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
									}
								}
								else
								{
									if (owningColumn.DefaultCellStyle.Format == "F")
									{
										formatedValue = fromatedDouble.ToString("F2");
									}
									else
									{
										formatedValue = fromatedDouble.ToString("N2");
									}
								}
								break;
						}
					}

					#endregion
					break;
				default:
					bool normal = true;
					bool done = false;
					double tickSize = TickSize;

					double floorDouble = 0;
					double remainderDouble = 0;
					int decimalPlace = -1;

					#region - Convert To Fraction -

					if (owningColumn.IsFraction)
					{
						switch (tickSize.ToString())
						{
							case "0.5":
								break;
							case "0.125":			// 1/8
							case "0.0625":			// 1/16
							case "0.03125":			// 1/32
							case "0.015625":		// 1/64
							case "0.0078125":		// 1/128
								if (owningColumn.Is64Th ||
									(owningColumn.CheckFor64Th && GLOBAL.OptionOnFuture.ShowAs64TH.Contains(BaseSymbol)))
								{
									floorDouble = Math.Floor(Math.Abs(value));
									remainderDouble = Math.Abs(value) - floorDouble;
									if (owningColumn.IsAvgPrice)
									{
										remainderDouble = Math.Round(remainderDouble / 0.015625, 2);
									}
									else
									{
										remainderDouble = remainderDouble / 0.015625;
									}
									formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
									normal = false;
								}
								else
								{
									floorDouble = Math.Floor(Math.Abs(value));
									remainderDouble = Math.Abs(value) - floorDouble;
									if (owningColumn.IsAvgPrice)
									{
										remainderDouble = Math.Round(remainderDouble / 0.03125, 2);
									}
									else
									{
										remainderDouble = remainderDouble / 0.03125;
									}
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
								/*if (owningColumn.IsAvgPrice)
								{
									if (owningColumn.Is64Th ||
										(owningColumn.CheckFor64Th && GLOBAL.OptionOnFuture.ShowAs64TH.Contains(BaseSymbol)))
									{
										// Round to nearest 64Th
										floorDouble = Math.Floor(Math.Abs(value));
										remainderDouble = Math.Abs(value) - floorDouble;
										remainderDouble = Math.Round(remainderDouble / 0.015625, 2);
										formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
										normal = false;
									}
									else
									{
										// Round to nearest 32Th
										floorDouble = Math.Floor(Math.Abs(value));
										remainderDouble = Math.Abs(value) - floorDouble;
										remainderDouble = Math.Round(remainderDouble / 0.03125, 2);
										formatedValue = string.Format("{0}'{1}", floorDouble.ToString("N0"), remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
										normal = false;
									}
								}*/
								break;
						}
					}

					#endregion

					#region - Normal Tick Size -

					if (normal)
					{
						if (owningColumn.IsAvgPrice)
						{
							fromatedDouble = Math.Round(value, 7);
							formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString()).TrimEnd(new char[] { '0' }); ;
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
						else
						{
							fromatedDouble = Math.Round(value, owningColumn.MaxDecimal);
							if (owningColumn.AllowResolutionChange)
							{
								if (owningColumn.DefaultCellStyle.Format == "F")
								{
									formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
								}
								else
								{
									formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
								}
							}
							else
							{
								//_fromatedDouble = _fromatedDouble * DisplayFactor;
								switch (owningColumn.MaxDecimal)
								{
									case 0:
									case 1:
									case 2:
									case 3:
									case 4:
										#region - 4 Decimal -
										if (owningColumn.DefaultCellStyle.Format == "F")
										{
											formatedValue = fromatedDouble.ToString("F" + owningColumn.MaxDecimal.ToString());
										}
										else
										{
											formatedValue = fromatedDouble.ToString("N" + owningColumn.MaxDecimal.ToString());
										}
										break;
										#endregion
									default:
										#region - If isFraction -
										if (owningColumn.IsFraction)
										{
											if (tickSize >= 1)
											{
												if (owningColumn.DefaultCellStyle.Format == "F")
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
												if (owningColumn.DefaultCellStyle.Format == "F")
												{
													formatedValue = fromatedDouble.ToString("F1");
												}
												else
												{
													formatedValue = fromatedDouble.ToString("N1");
												}
												done = true;
											}
										}
										#endregion

										#region - If still not done by fraction conversion -
										if (!done)
										{
											if (owningColumn.DefaultCellStyle.Format == "F")
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
										#endregion
										break;
								}
							}
						}
					}

					#endregion
					break;
			}

			if (formatedValue != "")
			{
				if (owningColumn.IsPercent)
				{
					formatedValue =  string.Concat(formatedValue, "%");
				}
				else if (owningColumn.IsCurrency)
				{
					if (value < 0)
					{
						formatedValue = string.Format("(${0})", formatedValue.Replace("-", ""));
					}
					else
					{
						formatedValue = string.Format("${0}", formatedValue);
					}
				}
			}

			return formatedValue;
		}
	}
}
