using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseWatchNumberColumn : DGVBaseNumberColumn
	{
		[DefaultValue(false), Category("Appearance"), Description("Display is converted link")]
		private bool _isUnderLine = false;
		public bool IsUnderLine
		{
			get
			{
				return _isUnderLine;
			}
			set
			{
				if (_isUnderLine != value)
				{
					_isUnderLine = value;
				}
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

		public DGVBaseWatchNumberColumn()
		{
			base.CellTemplate = new DGVBaseWatchNumberCell();
		}
	}
}
