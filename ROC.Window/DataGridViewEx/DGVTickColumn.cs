using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;

namespace DataGridViewEx
{
	public class DGVTickColumn : DataGridViewColumn
	{
		public DGVTickColumn()
		{
			base.CellTemplate = new DGVTickCell();
		}

		[Description("Event raised Notify is enabled or disabled."), Category("Property Changed")]
		public event EventHandler IsNotifyChanged;

		[Description("Event raised Notify Duration is changed."), Category("Property Changed")]
		public event EventHandler NotifyDurationChanged;

		[Description("Event raised Notify Type is changed."), Category("Property Changed")]
		public event EventHandler NotifyTypeChanged;

		[Description("Event raised Notify Positive Color is changed."), Category("Property Changed")]
		public event EventHandler NotifyPositiveColorChanged;

		[Description("Event raised Notify Positive Border Width is changed."), Category("Property Changed")]
		public event EventHandler NotifyPositiveBorderWidthChanged;

		[Description("Event raised Notify Negative Color is changed."), Category("Property Changed")]
		public event EventHandler NotifyNegativeColorChanged;

		[Description("Event raised Notify Negative Border Width is changed."), Category("Property Changed")]
		public event EventHandler NotifyNegativeBorderWidthChanged;

		protected virtual void OnIsNotifyChanged(EventArgs e)
		{
			if (IsNotifyChanged != null)
			{
				IsNotifyChanged(this, e);
			}
		}

		protected virtual void OnNotifyDurationChanged(EventArgs e)
		{
			if (NotifyDurationChanged != null)
			{
				NotifyDurationChanged(this, e);
			}
		}

		protected virtual void OnNotifyTypeChanged(EventArgs e)
		{
			if (NotifyTypeChanged != null)
			{
				NotifyTypeChanged(this, e);
			}
		}

		protected virtual void OnNotifyPositiveColorChanged(EventArgs e)
		{
			if (NotifyPositiveColorChanged != null)
			{
				NotifyPositiveColorChanged(this, e);
			}
		}

		protected virtual void OnNotifyPositiveBorderWidthChanged(EventArgs e)
		{
			if (NotifyPositiveBorderWidthChanged != null)
			{
				NotifyPositiveBorderWidthChanged(this, e);
			}
		}

		protected virtual void OnNotifyNegativeColorChanged(EventArgs e)
		{
			if (NotifyNegativeColorChanged != null)
			{
				NotifyNegativeColorChanged(this, e);
			}
		}

		protected virtual void OnNotifyNegativeBorderWidthChanged(EventArgs e)
		{
			if (NotifyNegativeBorderWidthChanged != null)
			{
				NotifyNegativeBorderWidthChanged(this, e);
			}
		}

		private bool _isNotified = false;
		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("The column will notify on cell value change")]
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
					OnIsNotifyChanged(EventArgs.Empty);
				}
			}
		}

		private int _notifyDuration = 3000;
		[DefaultValue(typeof(int), "3000"), Category("Appearance"), Description("The animation duration")]
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
					OnNotifyDurationChanged(EventArgs.Empty);
				}
			}
		}

		private NotifyTypes _notifyType = NotifyTypes.Border;
		[DefaultValue(typeof(DataGridViewEx.NotifyTypes), "Border"), Category("Appearance"), Description("The notify type, border, background or foreground")]
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
					OnNotifyTypeChanged(EventArgs.Empty);
				}
			}
		}

		private Color _notifyPositiveColor = Color.LimeGreen;
		[DefaultValue(typeof(Color), "LimeGreen"), Category("Appearance"), Description("The notify color when the change is positive")]
		public Color NotifyPositiveColor
		{
			get
			{
				return _notifyPositiveColor;
			}
			set
			{
				if (_notifyPositiveColor != value)
				{
					_notifyPositiveColor = value;
					NotifyPositivePen = new Pen(_notifyPositiveColor, NotifyPositiveBorderWidth);
					OnNotifyPositiveColorChanged(EventArgs.Empty);
				}
			}
		}

		private float _notifyPositiveBorderWidth = 1.5f;
		[DefaultValue(typeof(float), "1.5"), Category("Appearance"), Description("The notify border width when the change is positive")]
		public float NotifyPositiveBorderWidth
		{
			get
			{
				return _notifyPositiveBorderWidth;
			}
			set
			{
				if (_notifyPositiveBorderWidth != value)
				{
					_notifyPositiveBorderWidth = value;
					NotifyPositivePen = new Pen(NotifyPositiveColor, _notifyPositiveBorderWidth);
					OnNotifyPositiveBorderWidthChanged(EventArgs.Empty);
				}
			}
		}

		private Pen _notifyPositivePen;
		internal Pen NotifyPositivePen
		{
			get
			{
				if (_notifyPositivePen == null)
				{
					_notifyPositivePen = new Pen(NotifyPositiveColor, NotifyPositiveBorderWidth);
				}
				return _notifyPositivePen;
			}
			set
			{
				_notifyPositivePen = value;
			}
		}

		private Color _notifyNegativeColor = Color.Red;
		[DefaultValue(typeof(Color), "Red"), Category("Appearance"), Description("The notify color when the change is negative")]
		public Color NotifyNegativeColor
		{
			get
			{
				return _notifyNegativeColor;
			}
			set
			{
				if (_notifyNegativeColor != value)
				{
					_notifyNegativeColor = value;
					NotifyNegativePen = new Pen(_notifyNegativeColor, NotifyNegativeBorderWidth);
					OnNotifyNegativeColorChanged(EventArgs.Empty);
				}
			}
		}

		private float _notifyNegativeBorderWidth = 1.5f;
		[DefaultValue(typeof(float), "1.5"), Category("Appearance"), Description("The notify border width when the change is negative")]
		public float NotifyNegativeBorderWidth
		{
			get
			{
				return _notifyNegativeBorderWidth;
			}
			set
			{
				if (_notifyNegativeBorderWidth != value)
				{
					_notifyNegativeBorderWidth = value;
					NotifyNegativePen = new Pen(NotifyNegativeColor, _notifyNegativeBorderWidth);
					OnNotifyNegativeBorderWidthChanged(EventArgs.Empty);
				}
			}
		}

		private Pen _notifyNegativePen;
		internal Pen NotifyNegativePen
		{
			get
			{
				if (_notifyNegativePen == null)
				{
					_notifyNegativePen = new Pen(NotifyNegativeColor, NotifyNegativeBorderWidth);
				}
				return _notifyNegativePen;
			}
			set
			{
				_notifyNegativePen = value;
			}
		}
	}
}
