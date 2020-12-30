using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;

using DataGridViewEx;
using System.Diagnostics;

namespace FormEx
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class VistaWindowBase : Form
	{
		public enum ROCWindowTypes
		{
			None,
			Login,
			Alert,
			Wait,
			Main,
			WatchList,
			PlotList,
			StockTicket,
			OptionTicket,
			FutureTicket,
			QuickTradeTicket,
			Orders,
			Trades,
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

		[Description("Event raised when the value of the TopMost property is changed."), Category("Property Changed")]
		public event EventHandler TopMostChanged;

		[Description("Event raised when the value of the Extended property is changed."), Category("Property Changed")]
		public event EventHandler ExtendedChanged;

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

		[Description("Event raised when the UI Timer Ticked."), Category("Property Changed")]
		public event EventHandler UITimerTicked;

		[Description("Event raised when the Default UI Timer Interval Changed"), Category("Property Changed")]
		public event EventHandler DefaultUITimerIntervalChanged;

		#endregion

		#region - CaptionBar Events -

		[Description("Event raised when the Refresh Button is clicked"), Category("Mouse")]
		public event EventHandler RefreshClicked;

		[Description("Event raised when the Fit Button is clicked"), Category("Mouse")]
		public event EventHandler FitClicked;

		[Description("Event raised when the Display Button is clicked"), Category("Mouse")]
		public event EventHandler DisplayClicked;

		[Description("Event raised when the Help Button is clicked"), Category("Mouse")]
		public event EventHandler HelpClicked;

		[Description("Event raised when the Icon Label is clicked"), Category("Mouse")]
		public event EventHandler IconClicked;

		[Description("Event raised when the Pin Label is clicked"), Category("Mouse")]
		public event EventHandler PinClicked;

		[Description("Event raised when the ShowAll Label is clicked"), Category("Mouse")]
		public event EventHandler ShowAllClicked;

		#endregion

		#region - Local Variables -

		private DockWindow _dockWindow;
		private VistaWindowCaptionBar _captionBar;

		private string _caption;
		private ROCWindowTypes _ROCWindowType = ROCWindowTypes.None;

		private bool _animate = true;
		private int _animationDuration = 150;
		private AnimationTypes _animationType = AnimationTypes.Fade;
		private AnimatedWindowHelper.SlideTypes _animationSlideType = AnimatedWindowHelper.SlideTypes.CENTER;

		private int _defaultUITimerInterval = 150;
		private Timer _uiTimer;

		#endregion

		public VistaWindowBase()
		{			
			SetStyle(
				ControlStyles.ContainerControl |
				ControlStyles.CacheText | 
				ControlStyles.UserPaint | 
				ControlStyles.OptimizedDoubleBuffer, true);

			//	ControlStyles.SupportsTransparentBackColor

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

		#region - Override Events -

		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				StopTimer();

				_dockWindow = null;

				if (_captionBar != null)
				{
					_captionBar.Dispose();
				}
				_captionBar = null;
			}
			base.Dispose(disposing);
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

		protected override void OnLocationChanged(EventArgs e)
		{
			//if (Top + Height < 0)
			//{
			//    Top = 0;
			//}
			//if (Left + Width < 0)
			//{
			//    Left = 0;
			//}
			base.OnLocationChanged(e);
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

		#endregion

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

		#region - OnEvents -

		protected virtual void OnCaptionChanged(EventArgs e)
		{
			if (CaptionChanged != null)
			{
				CaptionChanged(this, e);
			}
		}

		protected virtual void OnTopMostChanged(EventArgs e)
		{
			if (TopMostChanged != null)
			{
				TopMostChanged(this, e);
			}
		}

		protected virtual void OnExtendedChanged(EventArgs e)
		{
			if (ExtendedChanged != null)
			{
				ExtendedChanged(this, e);
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

		protected virtual void OnUITimerTicked(EventArgs e)
		{
			if (UITimerTicked != null)
			{
				UITimerTicked(this, e);
			}
		}

		protected virtual void OnDefaultUITimerIntervalChanged(EventArgs e)
		{
			if (DefaultUITimerIntervalChanged != null)
			{
				DefaultUITimerIntervalChanged(this, e);
			}
		}

		#endregion

		#region - CaptionBar OnEvents -

		public virtual void OnIconClick(object sender, EventArgs e)
		{
			if (IconClicked != null)
			{
				IconClicked(sender, e);
			}
		}

		public virtual void OnRefreshClick(object sender, EventArgs e)
		{
			if (RefreshClicked != null)
			{
				RefreshClicked(sender, e);
			}
		}

		public virtual void OnDisplayClick(object sender, EventArgs e)
		{
			if (DisplayClicked != null)
			{
				DisplayClicked(sender, e);
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

		public virtual void OnPinClick(object sender, EventArgs e)
		{
			if (PinClicked != null)
			{
				PinClicked(sender, e);
			}
		}

		public virtual void OnShowAllClick(object sender, EventArgs e)
		{
			if (ShowAllClicked != null)
			{
				ShowAllClicked(sender, e);
			}
		}

		#endregion

		#region - Properties -

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

		[Category("Appearance")]
		public bool OnTop
		{
			get
			{
				return TopMost;
			}
			set
			{
				if (TopMost != value)
				{
					TopMost = value;
					OnTopMostChanged(EventArgs.Empty);
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

		[DefaultValue(150), Category("Appearance")]
		public int DefaultUITimerInterval
		{
			get
			{
				return _defaultUITimerInterval;
			}
			set
			{
				if (_defaultUITimerInterval != value)
				{
					_defaultUITimerInterval = value;
					OnDefaultUITimerIntervalChanged(EventArgs.Empty);
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

		#endregion

		#region - Timer -

		public void StartTimer()
		{
			StartTimer(_defaultUITimerInterval);
		}
		public void StartTimer(int interval)
		{
			if (_uiTimer == null)
			{
				_uiTimer = new Timer();
				_uiTimer.Tick += new EventHandler(uiTimer_Tick);
				if (interval < 25)
				{
					interval = 25;
				}
				_uiTimer.Interval = interval;
				_uiTimer.Enabled = true;
			}
		}

		public void StopTimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Stop();
				_uiTimer.Tick -= new EventHandler(uiTimer_Tick);
			}

			_uiTimer = null;
		}

		public void PauseTimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Stop();
			}
		}

		public void ResumeTimer()
		{
			if (_uiTimer != null)
			{
				_uiTimer.Start();
			}
		}

		private void uiTimer_Tick(object sender, EventArgs e)
		{
			PauseTimer();

			OnUITimerTicked(EventArgs.Empty);

			ResumeTimer();
		}

		#endregion

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

		public void ResizeToFit(SplitContainer split)
		{
			int panel1Width = 0;
			int panel2Width = 0;

			DataGridView grid1 = FindGrid1(split);
			DataGridView grid2 = FindGrid2(split);

			if (grid1 != null)
			{
				foreach (DataGridViewColumn col in grid1.Columns)
				{
					if (col.Visible)
					{
						panel1Width = panel1Width + col.Width;
					}
				}

				foreach (DataGridViewColumn col in grid2.Columns)
				{
					if (col.Visible)
					{
						panel2Width = panel2Width + col.Width;
					}
				}

				//Control 1 = Vertical Scroll Bar
				if (grid1.Controls[1].Visible)
				{
					split.SplitterDistance = panel1Width + grid1.Controls[1].Width;
				}
				else
				{
					split.SplitterDistance = panel1Width;
				}

				if (grid2.Controls[1].Visible)
				{
					Width = split.SplitterDistance + split.SplitterWidth + panel2Width + grid2.Controls[1].Width;
				}
				else
				{
					Width = split.SplitterDistance + split.SplitterWidth + panel2Width;
				}

				//Control 0 = Horizontal Scroll Bar
				if (WindowState == FormWindowState.Normal)
				{
					if (split.Panel1.MaximumSize.Width != 0)
					{
						int fitCount = 0;
						while (fitCount < 30 && grid1.Controls[0].Visible && split.SplitterDistance < split.Panel1.MaximumSize.Width)
						{
							split.SplitterDistance += 5;
							fitCount++;
						}
					}
					else
					{
						int fitCount = 0;
						while (grid1.Controls[0].Visible)
						{
							split.SplitterDistance += 5;
							if (fitCount > 30 || split.SplitterDistance > ((ScrollBar)grid1.Controls[0]).Maximum + 30)
							{
								break;
							}
							fitCount++;
						}
					}

					split.FixedPanel = FixedPanel.Panel1;

					if (split.Panel2.MaximumSize.Width != 0)
					{
						int fitCount = 0;
						while (fitCount < 30 && grid2.Controls[0].Visible && split.Width - split.SplitterDistance - split.SplitterWidth < split.Panel2.MaximumSize.Width)
						{
							Width += 5;
							fitCount++;
						}
					}
					else
					{
						int fitCount = 0;
						while (grid2.Controls[0].Visible)
						{
							Width += 5;
							if (fitCount > 30 || split.Width - split.SplitterDistance - split.SplitterWidth > ((ScrollBar)grid2.Controls[0]).Maximum + 30)
							{
								break;
							}
							fitCount++;
						}
					}

					split.FixedPanel = FixedPanel.None;
				}
				else if (WindowState == FormWindowState.Maximized)
				{
					split.SplitterDistance = (split.Width - split.SplitterWidth) / 2;
				}
			}
		}

		private DataGridView FindGrid1(SplitContainer split)
		{
			foreach (Control ctl in split.Panel1.Controls)
			{
				if (ctl is DataGridView)
				{
					return (DataGridView)ctl;
				}
			}

			return null;
		}

		private DataGridView FindGrid2(SplitContainer split)
		{
			foreach (Control ctl in split.Panel2.Controls)
			{
				if (ctl is DataGridView)
				{
					return (DataGridView)ctl;
				}
			}

			return null;
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
			if (WindowState == FormWindowState.Normal)
			{
				if (MaximumSize.Width != 0)
				{
					int fitCount = 0;
					while (fitCount < 30 && grid.Controls[0].Visible && Width < MaximumSize.Width)
					{
						Width += 5;
						fitCount++;
					}
				}
				else
				{
					int fitCount = 0;
					while (grid.Controls[0].Visible)
					{
						Width += 5;
						if (fitCount > 30 || Width > ((ScrollBar)grid.Controls[0]).Maximum + 30)
						{
							return;
						}
						fitCount++;
					}
				}
			}
		}

		public void ActivateGrid(DataGridView grid)
		{
			Activate();
			grid.Select();
		}

		#endregion
	}
}
