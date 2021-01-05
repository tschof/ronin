using System;
using System.Collections.Generic;
using System.Text;

namespace NumberEx
{
	public class FractionFixed
	{
		private double _tickSize = 0.01;
		public double TickSize
		{
			get
			{
				return _tickSize;
			}
			set
			{
				_tickSize = value;
			}
		}

		private double _displayFactor = 1;
		public double DisplayFactor
		{
			get
			{
				return _displayFactor;
			}
			set
			{
				_displayFactor = value;
			}
		}

		private int _maxDecimalPlace = 7;
		public int MaxDecimalPlace
		{
			get
			{
				return _maxDecimalPlace;
			}
			set
			{
				if (_maxDecimalPlace != value)
				{
					_maxDecimalPlace = value;
				}
			}
		}

		private Fraction _rawFractionVal;
		public Fraction RawFractionVal
		{
			get
			{
				return _rawFractionVal;
			}
		}

		public FractionFixed()
		{
		}
		public FractionFixed(double? tickSize, double? displayFactor)
		{
			if (tickSize.HasValue)
				_tickSize = tickSize.Value;
			if (displayFactor.HasValue)
				_displayFactor = displayFactor.Value;
		}

		public string ToString(decimal val)
		{
			return ToString((double)val);
		}

		public string ToString(double val)
		{
			string result;

			bool isNeg = false;
			val = Math.Round(val, MaxDecimalPlace);

			if (val != 0)
			{
				if (val < 0)
				{
					isNeg = true;
					val = Math.Abs(val);
				}

				double leftSide = Math.Floor(val);
				double rightSide = 0;
				if (val >= leftSide)
				{
					rightSide = val - leftSide;
				}
				else
				{
					System.Diagnostics.Debug.Assert(!System.Diagnostics.Debugger.IsAttached, "val is greater then leftside");
				}
				_rawFractionVal = Math.Round(rightSide, MaxDecimalPlace);

				switch (_rawFractionVal.Denominator)
				{
					case 400:
						result = GetFormated400BasedDisplay(leftSide, rightSide);
						break;
					case 128:
						result = GetFormated32BasedDisplay(leftSide, _rawFractionVal.Numerator, 4);
						break;
					case 64:
						result = GetFormated32BasedDisplay(leftSide, _rawFractionVal.Numerator, 2);
						break;
					case 32:
						result = GetFormated32BasedDisplay(leftSide, _rawFractionVal.Numerator, 1);
						break;
					default:
						val = Math.Round(val, 7);
						result = val.ToString();
						break;
				}
			}
			else
			{
				result = "0.00";
			}

			if (isNeg)
			{
				return string.Concat("-", result);
			}
			else
			{
				return result;
			}
		}

		private string GetFormated32BasedDisplay(double leftSide, long numerator, double div)
		{
			double subNumerator = numerator / div;

			return string.Concat(
				leftSide.ToString("N0"),
				"'",
				subNumerator.ToString("F2").Replace(".", ""));
		}

		private string GetFormated400BasedDisplay(double leftSide, double rightSide)
		{
			rightSide = Math.Round(rightSide * 100, 2);

			double subLeftSide = Math.Floor(rightSide);
			double subRightSide = 0;
			if (rightSide > subLeftSide)
			{
				subRightSide = Math.Round(rightSide - subLeftSide, 2);
			}

			string result = subLeftSide.ToString("00");

			if (subRightSide == 0.25)
			{
				result = result + "2";
			}
			else if (subRightSide == 0.5)
			{
				result = result + "4";
			}
			else if (subRightSide == 0.75)
			{
				result = result + "6";
			}
			else
			{
				result = result + "0";
			}

			return string.Concat(
				leftSide.ToString("N0"),
				"'",
				result);
		}
	}
}
