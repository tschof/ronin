using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;

using DataGridViewEx;

namespace FormEx
{
	[System.ComponentModel.DesignerCategory("Form")]
	public abstract class VistaWindowBase : Form
	{
		public enum ROCWindowTypes
		{
			None,
			Login,
			Alert,
			Main,
			WatchList,
			PlotList,
			StockTicket,
			OptionTicket,
			FutureTicket,
			QuickTradeTicket,
			Orders,
			Positions,
		}

		public enum AnimationTypes
		{
			Fade,
			Slide,
		}

		#region - Events -

		[Description("Event raised when the value of the Caption property is changed."), Category("Property Changed")]
		public event EventHandler CaptionChanged;

		[Description("Event raised when the value of the ROCWindowType property is changed."), Category("Property Changed")]
		public event EventHandler ROCWindowTypeChanged;

		[Description("Event raised when the value of the Animate property is changed."), Category("Property Changed")]
		public event EventHandler AnimateChanged;

		[Description("Event raised when the value of the AnimationDuration property is changed."), Category("Property Changed")]
		public event EventHandler AnimationDurationChanged;

		[Description("Event raised when the value of the AnimationType property is changed."), Category("Property Changed")]
		public event EventHandler AnimationTypeChanged;

		[Description("Event raised when the value of the AnimationSlideType property is changed."), Category("Property Changed")]
		public event EventHandler AnimationSlideTypeChanged;

		#endregion

		#region - CaptionBar Events -

		[Description("Event raised when the Refresh Button is clicked"), Category("Mouse")]
		public event EventHandler RefreshClicked;

		[Description("Event raised when the Fit Button is clicked"), Category("Mouse")]
		public event EventHandler FitClicked;

		[Description("Event raised when the Help Button is clicked"), Category("Mouse")]
		public event EventHandler HelpClicked;

		#endregion

		private DockWindow _dockWindow;
		private VistaWindowCaptionBar _captionBar;

		private string _caption;
		private ROCWindowTypes _ROCWindowType = ROCWindowTypes.None;

		private bool _animate = true;
		private int _animationDuration = 150;
		private AnimationTypes _animationType = AnimationTypes.Fade;
		private AnimatedWindowHelper.SlideTypes _animationSlideType = AnimatedWindowHelper.SlideTypes.CENTER;

		public VistaWindowBase()
		{
			SetStyle(ControlStyles.OptimizedDoubleBuffer | ControlStyles.SupportsTransparentBackColor, true);

			if (Text == null || Text == "")
			{
				_caption = Name;
			}
			else
			{
				_caption = Text;
			}

			ControlBox = false;
		}

		protected override void OnLoad(EventArgs e)
		{
			if (Animate)
			{
				switch (AnimationType)
				{
					case AnimationTypes.Fade:
						AnimatedWindowHelper.FadeIn(Handle, AnimationDuration);
						break;
					case AnimationTypes.Slide:
						AnimatedWindowHelper.SlidIn(Handle, AnimationDuration, AnimationSlideType);
						break;
				}
				Refresh();
			}
			_dockWindow = new DockWindow(this);
			base.OnLoad(e);
		}

		protected override void OnClosing(CancelEventArgs e)
		{
			if (Animate)
			{
				switch (AnimationType)
				{
					case AnimationTypes.Fade:
						AnimatedWindowHelper.FadeOut(Handle, _animationDuration);
						break;
					case AnimationTypes.Slide:
						AnimatedWindowHelper.SlideOut(Handle, AnimationDuration, AnimationSlideType);
						break;
				}
			}
			base.OnClosing(e);
		}

		protected override void OnActivated(EventArgs e)
		{
			SetCaptionBar(Color.DarkSlateBlue);
			base.OnActivated(e);
		}

		protected override void OnDeactivate(EventArgs e)
		{
			SetCaptionBar(Color.Black);
			base.OnDeactivate(e);
		}

		private void GetCaptionBar()
		{
			if (_captionBar == null)
			{
				if (this.Controls.ContainsKey("CaptionBar"))
				{
					_captionBar = (VistaWindowCaptionBar)this.Controls["CaptionBar"];
				}
			}
		}

		public void SetCaptionBar()
		{
			GetCaptionBar();
			if (_captionBar != null)
			{
				_captionBar.lblCaption.Text = _caption;
			}
		}

		private void SetCaptionBar(Color backColor)
		{
			GetCaptionBar();
			if (_captionBar != null)
			{
				_captionBar.lblCaption.BackColor = backColor;
			}
		}

		protected virtual void OnCaptionChanged(EventArgs e)
		{
			if (CaptionChanged != null)
			{
				CaptionChanged(this, e);
			}
		}

		protected virtual void OnROCWindowTypeChanged(EventArgs e)
		{
			if (ROCWindowTypeChanged != null)
			{
				ROCWindowTypeChanged(this, e);
			}
		}

		protected virtual void OnAnimateChanged(EventArgs e)
		{
			if (AnimateChanged != null)
			{
				AnimateChanged(this, e);
			}
		}

		protected virtual void OnAnimationDurationChanged(EventArgs e)
		{
			if (AnimationDurationChanged != null)
			{
				AnimationDurationChanged(this, e);
			}
		}

		protected virtual void OnAnimationTypeChanged(EventArgs e)
		{
			if (AnimationTypeChanged != null)
			{
				AnimationTypeChanged(this, e);
			}
		}

		protected virtual void OnAnimationSlideTypeChanged(EventArgs e)
		{
			if (AnimationSlideTypeChanged != null)
			{
				AnimationSlideTypeChanged(this, e);
			}
		}

		public virtual void OnRefreshClick(object sender, EventArgs e)
		{
			if (RefreshClicked != null)
			{
				RefreshClicked(sender, e);
			}
		}

		public virtual void OnFitClick(object sender, EventArgs e)
		{
			if (FitClicked != null)
			{
				FitClicked(sender, e);
			}
		}

		public virtual void OnHelpClick(object sender, EventArgs e)
		{
			if (HelpClicked != null)
			{
				HelpClicked(sender, e);
			}
		}

		[DefaultValue(typeof(VistaWindowBase.ROCWindowTypes), "None"), Category("Appearance")]
		public ROCWindowTypes ROCWindowType
		{
			get
			{
				return _ROCWindowType;
			}
			set
			{
				if (_ROCWindowType != value)
				{
					_ROCWindowType = value;
					OnROCWindowTypeChanged(EventArgs.Empty);
				}
			}
		}

		[Category("Appearance")]
		public string Caption
		{
			get
			{
				if (_caption == null)
				{
					_caption = Name;
				}
				return _caption;
			}
			set
			{
				if (_caption != value)
				{
					_caption = value;
					SetCaptionBar();
					OnCaptionChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Boolean), "True"), Category("Appearance")]
		public bool Animate
		{
			get
			{
				return _animate;
			}
			set
			{
				if (_animate != value)
				{
					_animate = value;
					OnAnimateChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(150), Category("Appearance")]
		public int AnimationDuration
		{
			get
			{
				return _animationDuration;
			}
			set
			{
				if (_animationDuration != value)
				{
					_animationDuration = value;
					OnAnimationDurationChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(VistaWindowBase.AnimationTypes), "Fade"), Category("Appearance")]
		public AnimationTypes AnimationType
		{
			get
			{
				return _animationType;
			}
			set
			{
				if (_animationType != value)
				{
					_animationType = value;
				}
			}
		}

		[DefaultValue(typeof(AnimatedWindowHelper.SlideTypes), "CENTER"), Category("Appearance")]
		public AnimatedWindowHelper.SlideTypes AnimationSlideType
		{
			get
			{
				return _animationSlideType;
			}
			set
			{
				if (_animationSlideType != value)
				{
					_animationSlideType = value;

				}
			}
		}

		#region - Shared Functions -

		public void SetColumnMode(DataGridView grid, DataGridViewAutoSizeColumnMode mode)
		{
			int oldColWidth = 0;
			foreach (DataGridViewColumn col in grid.Columns)
			{
				switch (mode)
				{
					case DataGridViewAutoSizeColumnMode.NotSet:
					case DataGridViewAutoSizeColumnMode.None:
						oldColWidth = col.Width;
						col.AutoSizeMode = mode;
						col.Width = oldColWidth;
						break;
					default:
						col.AutoSizeMode = mode;
						break;
				}
			}
		}

		public void ResizeToFit(DataGridView grid)
		{
			int totoalWidth = 0;
			foreach (DataGridViewColumn col in grid.Columns)
			{
				if (col.Visible)
				{
					totoalWidth = totoalWidth + col.Width;
				}
			}

			//Control 1 = Vertical Scroll Bar
			if (grid.Controls[1].Visible)
			{
				Width = totoalWidth + grid.Controls[1].Width;
			}
			else
			{
				Width = totoalWidth;
			}

			//Control 0 = Horizontal Scroll Bar
			if (MaximumSize.Width != 0)
			{
				while (grid.Controls[0].Visible && Width < MaximumSize.Width)
				{
					Width += 5;
				}
			}
			else
			{
				while (grid.Controls[0].Visible)
				{
					Width += 5;
				}
			}
		}

		#endregion
	}
}
