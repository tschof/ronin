using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;

namespace LabelEx
{
	[System.ComponentModel.DesignerCategory("code")]
	public class LabelBase : Label
	{
		private char[] _trimArray = new char[] { '0' };

		private int _decimalPlace = -1;

		private string _formatedValue = "";

		private double _floorDouble = 0;
		private double _remainderDouble = 0;

		[DefaultValue(""), Category("Appearance"), Description("Default Text")]
		private string _defaultText = "";
		public string DefaultText
		{
			get
			{
				return _defaultText;
			}
			set
			{
				_defaultText = value;
				Text = _defaultText;
				Invalidate();
			}
		}

		[DefaultValue(0.01), Category("Appearance"), Description("Tick size for the value, only applicable when this value is a number")]
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

		[DefaultValue(1), Category("Appearance"), Description("Display factor for the value, only applicable when this value is a number")]
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

		[DefaultValue(false), Category("Appearance"), Description("Display forecolor is auto set based on label value Positive = Green Negative = Red")]
		private bool _autoColor = false;
		public bool AutoColor
		{
			get
			{
				return _autoColor;
			}
			set
			{
				_autoColor = value;
			}
		}

		[DefaultValue("White"), Category("Appearance"), Description("Default auto color")]
		private Color _defaultColor = Color.White;
		public Color DefaultColor
		{
			get
			{
				return _defaultColor;
			}
			set
			{
				_defaultColor = value;
			}
		}

		[DefaultValue("LimeGreen"), Category("Appearance"), Description("Default auto positive color")]
		private Color _upColor = Color.LimeGreen;
		public Color UpColor
		{
			get
			{
				return _upColor;
			}
			set
			{
				_upColor = value;
			}
		}

		[DefaultValue("Red"), Category("Appearance"), Description("Default auto negative color")]
		private Color _downColor = Color.Red;
		public Color DownColor
		{
			get
			{
				return _downColor;
			}
			set
			{
				_downColor = value;
			}
		}

		[DefaultValue(false), Category("Appearance"), Description("Display number as currency")]
		private bool _isCurrency = false;
		public bool IsCurrency
		{
			get
			{
				return _isCurrency;
			}
			set
			{
				_isCurrency = value;
			}
		}

		[DefaultValue(false), Category("Appearance"), Description("Display number as fraction")]
		private bool _isFraction = false;
		public bool IsFraction
		{
			get
			{
				return _isFraction;
			}
			set
			{
				_isFraction = value;
			}
		}

		[DefaultValue(false), Category("Appearance"), Description("Display number as percent")]
		private bool _isPercent = false;
		public bool IsPercent
		{
			get
			{
				return _isPercent;
			}
			set
			{
				_isPercent = value;
			}
		}

		[DefaultValue(7), Category("Appearance"), Description("Display max decimal places")]
		private int _maxDecimal = 7;
		public int MaxDecimal
		{
			get
			{
				return _maxDecimal;
			}
			set
			{
				_maxDecimal = value;
			}
		}

		public LabelBase()
		{
			SetStyle(
				ControlStyles.AllPaintingInWmPaint |
				ControlStyles.CacheText |
				ControlStyles.OptimizedDoubleBuffer |
				ControlStyles.ResizeRedraw |
				ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.Opaque, false);
		}

		[Browsable(false), Category("Appearance")]
		private object _value = "";
		public object Value
		{
			get
			{
				return _value;
			}
			set
			{
				if (value != null && value.ToString() != "")
				{
					if (_value != value)
					{
						_value = value;

						switch (value.GetType().Name)
						{
							case "Decimal":
							case "Double":
								_formatedValue = GetDoubleFormat(Convert.ToDouble(value));
								break;
							case "Int":
							case "Int64":
							case "Long":
								_formatedValue = GetLongFormat(Convert.ToInt64(value));
								break;
							case "String":
							default:
								_formatedValue = value.ToString();
								break;
						}

						if (IsPercent)
						{
							Text = _formatedValue + "%";
						}
						else
						{
							Text = _formatedValue;
						}
					}
				}

				if (Text == "")
				{
					Text = DefaultText;
				}
			}
		}

		[Browsable(false), Category("Appearance")]
		private bool _is64Th = false;
		public bool Is64Th
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

		private string GetDoubleFormat(double value)
		{
			bool normal = true;
			bool done = false;
			double tickSize = TickSize;

			switch (tickSize.ToString())
			{
				case "0.125":			// 1/8
				case "0.0625":			// 1/16
				case "0.03125":			// 1/32
				case "0.015625":		// 1/64
				case "0.0078125":		// 1/128
					if (Is64Th)
					{
						_floorDouble = Math.Floor(Math.Abs(value));
						_remainderDouble = Math.Abs(value) - _floorDouble;
						_remainderDouble = _remainderDouble / 0.015625;
						_formatedValue = string.Format("{0}'{1}", _floorDouble.ToString("N0"), _remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
						normal = false;
					}
					else
					{
						_floorDouble = Math.Floor(Math.Abs(value));
						_remainderDouble = Math.Abs(value) - _floorDouble;
						_remainderDouble = _remainderDouble / 0.03125;
						_formatedValue = string.Format("{0}'{1}", _floorDouble.ToString("N0"), _remainderDouble.ToString("00.00").Replace(".", "").Substring(0, 3));
						normal = false;
					}
					break;
				case "0.0025":			// 1/400
				case "0.00125":			// 1/800
					_floorDouble = Math.Floor(Math.Abs(value) * 100);
					_remainderDouble = (Math.Abs(value) * 100) - _floorDouble;
					_remainderDouble = _remainderDouble - Math.Floor(_remainderDouble);
					_floorDouble = (_floorDouble / 100);
					switch (_remainderDouble.ToString("F3"))
					{
						case "0.125":
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 1);
							break;
						case "0.250":
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 2);
							break;
						case "0.375":
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 3);
							break;
						case "0.500":
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 4);
							break;
						case "0.625":
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 5);
							break;
						case "0.750":
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 6);
							break;
						case "0.875":
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 7);
							break;
						default:
							_formatedValue = string.Format("{0}{1}", _floorDouble.ToString("F2"), 0);
							break;
					}
					_formatedValue = _formatedValue.Replace(".", "'");
					normal = false;
					break;
				default:
					break;
			}

			if (normal)
			{
				value = Math.Round(value, MaxDecimal);
				//value = value * DisplayFactor;
				switch (MaxDecimal)
				{
					case 0:
						_formatedValue = value.ToString("N0");
						break;
					case 1:
						_formatedValue = value.ToString("N1");
						break;
					default:
						if (IsFraction)
						{
							if (tickSize >= 1)
							{
								_formatedValue = value.ToString("N0");
								done = true;
							}
							else if (tickSize == 0.5 || tickSize == 0.1)
							{
								_formatedValue = value.ToString("N1");
								done = true;
							}
						}

						if (!done)
						{
							_formatedValue = value.ToString("N7").TrimEnd(_trimArray);
							_decimalPlace = _formatedValue.IndexOf('.');
							_decimalPlace = (_formatedValue.Substring(_decimalPlace + 1)).Length;

							switch (_decimalPlace)
							{
								case 0:
									_formatedValue = string.Concat(_formatedValue, "00");
									break;
								case 1:
									_formatedValue = string.Concat(_formatedValue, "0");
									break;
								default:
									break;
							}
						}
						break;
				}
			}

			if (AutoColor || IsCurrency)
			{
				if (value < 0)
				{
					if (AutoColor)
					{
						ForeColor = DownColor;
					}
					if (IsCurrency)
					{
						_formatedValue = string.Format("(${0})", _formatedValue.Replace("-", ""));
					}
				}
				else if (value > 0)
				{
					if (AutoColor)
					{
						ForeColor = UpColor;
					}
					if (IsCurrency)
					{
						_formatedValue = string.Format("${0}", _formatedValue);
					}
				}
				else
				{
					if (AutoColor)
					{
						ForeColor = DefaultColor;
					}
					if (IsCurrency)
					{
						_formatedValue = string.Format("${0}", _formatedValue);
					}
				}
			}

			return _formatedValue;
		}
		private string GetLongFormat(long value)
		{
			_formatedValue = value.ToString("N0");

			if (AutoColor || IsCurrency)
			{
				if (value < 0)
				{
					if (AutoColor)
					{
						ForeColor = DownColor;
					}
					if (IsCurrency)
					{
						_formatedValue = string.Format("(${0})", _formatedValue);
					}
				}
				else if (value > 0)
				{
					if (AutoColor)
					{
						ForeColor = UpColor;
					}
					if (IsCurrency)
					{
						_formatedValue = string.Format("${0}", _formatedValue);
					}
				}
				else
				{
					if (AutoColor)
					{
						ForeColor = DefaultColor;
					}
					if (IsCurrency)
					{
						_formatedValue = string.Format("${0}", _formatedValue);
					}
				}
			}

			return _formatedValue;
		}

		#region - ToolTip -

		private ToolTip _toolTip = new ToolTip();
		public void SetToolTip(string tip)
		{
			_toolTip.SetToolTip(this, tip);
		}

		#endregion
	}
}
