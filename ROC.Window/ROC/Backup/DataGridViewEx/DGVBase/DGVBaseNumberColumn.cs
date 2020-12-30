using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseNumberColumn : DataGridViewColumn
	{
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

		[DefaultValue(false), Category("Appearance"), Description("Display is converted to fraction if ticksize is avaliable")]
		private bool _isFraction = false;
		public bool IsFraction
		{
			get
			{
				return _isFraction;
			}
			set
			{
				if (_isFraction != value)
				{
					_isFraction = value;
				}
			}
		}

		[DefaultValue(false), Category("Appearance"), Description("Display is converted to fraction as 1/64 if ticksize is avaliable")]
		private bool _is64Th = false;
		public bool Is64Th
		{
			get
			{
				return _is64Th;
			}
			set
			{
				if (_is64Th != value)
				{
					_is64Th = value;
				}
			}
		}

		[DefaultValue(false), Category("Appearance"), Description("Display check againist the 64Th list before displaying")]
		private bool _checkFor64Th = false;
		public bool CheckFor64Th
		{
			get
			{
				return _checkFor64Th;
			}
			set
			{
				if (_checkFor64Th != value)
				{
					_checkFor64Th = value;
				}
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

		[DefaultValue(false), Category("Appearance"), Description("Display number as AvgPrice")]
		public bool IsAvgPrice
		{
			get
			{
				if (Name.Contains("Avg"))
				{
					return true;
				}
				return false;
			}
		}

		[DefaultValue(false), Category("Appearance"), Description("Shown with %")]
		private bool _isPercent = false;
		public bool IsPercent
		{
			get
			{
				return _isPercent;
			}
			set
			{
				if (_isPercent != value)
				{
					_isPercent = value;
				}
			}
		}

		[DefaultValue(false), Category("Appearance"), Description("Allow User Change Resolutions")]
		private bool _allowResolutionChange = false;
		public bool AllowResolutionChange
		{
			get
			{
				return _allowResolutionChange;
			}
			set
			{
				_allowResolutionChange = value;
			}
		}

		public DGVBaseNumberColumn()
		{
			base.CellTemplate = new DGVBaseNumberCell();
		}
	}
}
