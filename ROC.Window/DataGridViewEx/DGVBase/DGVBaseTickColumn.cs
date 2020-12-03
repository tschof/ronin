using System;
using System.Windows.Forms;
using System.Drawing;
using DataGridViewEx.Properties;

namespace DataGridViewEx
{
	public class DGVBaseTickColumn : DataGridViewColumn
	{
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
				_notifyType = value;
			}
		}

		public Image DefaultImage
		{
			get
			{
				return Resources.Blank;
			}
		}

		public Image UpImage
		{
			get
			{
				return Resources.UpGreen;
			}
		}

		public Image DownImage
		{
			get
			{
				return Resources.DownRed;
			}
		}

		public DGVBaseTickColumn()
		{
			base.CellTemplate = new DGVBaseTickCell();
		}
	}
}
