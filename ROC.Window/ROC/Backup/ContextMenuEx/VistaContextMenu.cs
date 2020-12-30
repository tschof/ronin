using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;
using System.Collections.Generic;
using System.Drawing.Drawing2D;

namespace ContextMenuEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class VistaContextMenu : ContextMenuStrip
	{
		#region - ContextMenu Values -

		private int radius;
		private Color _itemBackColor;
		private Color _itemForeColor;
		private Color _outerBorderColor;
		private Color _innerBorderColor;
		private Color _shineColor;
		private Color _glowColor;

		#endregion

		#region - Local Variable -

		private Timer _iniInactiveTimer;

		#endregion

		#region - Constructor -

		public VistaContextMenu()
		{
			Initialize(4, Color.Black, Color.White, Color.White, Color.Black, Color.White, Color.FromArgb(255, 141, 189, 255));
		}

		public VistaContextMenu(ToolStripItem[] toolStripItems)
		{
			Initialize(4, Color.Black, Color.White, Color.White, Color.Black, Color.White, Color.FromArgb(255, 141, 189, 255));
			Items.AddRange(toolStripItems);
		}

		#endregion

		#region - Event Override -

		protected override void OnMouseHover(EventArgs e)
		{
			StopTimer();
			base.OnMouseHover(e);
		}

		#endregion

		#region - Public Function -

		public void ShowAgain()
		{
			StopTimer();
			StartTimer();
		}

		#endregion

		#region - Local Functions -

		private void StartTimer()
		{
			if (_iniInactiveTimer == null)
			{
				_iniInactiveTimer = new Timer();
				_iniInactiveTimer.Interval = 1000;
				_iniInactiveTimer.Tick += new EventHandler(IniInactiveTimer_Tick);
				_iniInactiveTimer.Enabled = true;
			}
		}

		private void StopTimer()
		{
			if (_iniInactiveTimer != null)
			{
				_iniInactiveTimer.Stop();
				_iniInactiveTimer.Tick -= new EventHandler(IniInactiveTimer_Tick);
				_iniInactiveTimer.Dispose();
				_iniInactiveTimer = null;
			}
		}

		private void IniInactiveTimer_Tick(object sender, EventArgs e)
		{
			StopTimer();
			Close();
		}

		#endregion

		#region - Constructor Method -

		private void Initialize(int r, Color back, Color fore, Color outer, Color inner, Color shine, Color glow)
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw | ControlStyles.SupportsTransparentBackColor | ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.Opaque, false);

			radius = r;
			_itemBackColor = back;
			_itemForeColor = fore;
			_outerBorderColor = outer;
			_innerBorderColor = inner;
			_shineColor = shine;
			_glowColor = glow;

			Renderer = new VistaContextMenuRenderer(radius, _itemBackColor, _itemForeColor, _outerBorderColor, _innerBorderColor, _shineColor, _glowColor);
		}

		#endregion

		#region - Properties -

		[DefaultValue(4), Category("Appearance"), Description("Indicates whether the button should fade in and fade out when it is getting and loosing the focus.")]
		public int Radius
		{
			get
			{
				return radius;
			}
			set
			{
				if (radius != value)
				{
					radius = value;
				}
			}
		}

		[DefaultValue(typeof(Color), "Black"), Category("Appearance"), Description("The back color of the control.")]
		public Color ItemBackColor
		{
			get
			{
				return _itemBackColor;
			}
			set
			{
				if (!_itemBackColor.Equals(value))
				{
					_itemBackColor = value;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(Color), "White"), Category("Appearance"), Description("The fore color of the control.")]
		public Color ItemForeColor
		{
			get
			{
				return _itemForeColor;
			}
			set
			{
				if (!_itemForeColor.Equals(value))
				{
					_itemForeColor = value;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(Color), "White"), Category("Appearance"), Description("The outter border color of the control.")]
		public Color OuterBorderColor
		{
			get
			{
				return _outerBorderColor;
			}
			set
			{
				if (!_outerBorderColor.Equals(value))
				{
					_outerBorderColor = value;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(Color), "Black"), Category("Appearance"), Description("The inner border color of the control.")]
		public Color InnerBorderColor
		{
			get
			{
				return _innerBorderColor;
			}
			set
			{
				if (!_innerBorderColor.Equals(value))
				{
					_innerBorderColor = value;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(Color), "White"), Category("Appearance"), Description("The shine color of the control.")]
		public Color ShineColor
		{
			get
			{
				return _shineColor;
			}
			set
			{
				if (!_shineColor.Equals(value))
				{
					_shineColor = value;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(Color), "255,141,189,255"), Category("Appearance"), Description("The glow color of the control.")]
		public Color GlowColor
		{
			get
			{
				return _glowColor;
			}
			set
			{
				if (!_glowColor.Equals(value))
				{
					_glowColor = value;
					Invalidate();
				}
			}
		}

		#endregion
	}
}
