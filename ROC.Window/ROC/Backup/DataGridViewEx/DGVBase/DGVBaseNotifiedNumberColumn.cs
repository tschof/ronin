using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVBaseNotifiedNumberColumn : DGVBaseNumberColumn
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

		#region - Notification -

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

		private float _upBorderWidth = 1.5f;
		public float UpBorderWidth
		{
			get
			{
				return _upBorderWidth;
			}
			set
			{
				_upBorderWidth = value;
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

		private float _downBorderWidth = 1.5f;
		public float DownBorderWidth
		{
			get
			{
				return _downBorderWidth;
			}
			set
			{
				_downBorderWidth = value;
			}
		}

		private bool _isNotified = false;
		public bool IsNotified
		{
			get
			{
				return _isNotified;
			}
			set
			{
				if (_isNotified != value)
				{
					_isNotified = value;
				}
			}
		}

		private int _notifyDuration = 3000;
		public int NotifyDuration
		{
			get
			{
				return _notifyDuration;
			}
			set
			{
				if (_notifyDuration != value)
				{
					_notifyDuration = value;
				}
			}
		}

		private NotifyTypes _notifyType = NotifyTypes.Border;
		public NotifyTypes NotifyType
		{
			get
			{
				return _notifyType;
			}
			set
			{
				if (_notifyType != value)
				{
					_notifyType = value;
				}
			}
		}

		#endregion

		#region - Show Bar -

		private bool _showBar = false;
		public bool ShowBar
		{
			get
			{
				return _showBar;
			}
			set
			{
				_showBar = value;
			}
		}

		private long _showBarMax = 1;
		public long ShowBarMax
		{
			get
			{
				return _showBarMax;
			}
			set
			{
				_showBarMax = value;
			}
		}

		#endregion

		private bool _isTradePrice = false;
		public bool IsTradePrice
		{
			get
			{
				return _isTradePrice;
			}
			set
			{
				_isTradePrice = value;
			}
		}

		public DGVBaseNotifiedNumberColumn()
		{
			base.CellTemplate = new DGVBaseNotifiedNumberCell();
		}
	}
}
