using System;
using System.ComponentModel;
using System.Windows.Forms;
using ButtonEx;
using System.Drawing;
using LabelEx;
using System.Diagnostics;

namespace FormEx
{
	[ToolboxItem(true)]
	public partial class VistaWindowCaptionBar : UserControl, IDisposable
	{
		private VistaWindowBase _form = null;

		private bool _enableCaptionEdit = false;

		private bool _showClose = true;
		private bool _showMaximize = true;
		private bool _showMinimize = true;
		private bool _showHelp = false;

		private bool _showDisplay = false;
		private bool _showRefresh = false;
		private bool _showFit = false;
		private bool _showIcon = true;
		private bool _showPin = true;
		private bool _showShowAll = false;

		private bool _confirmBeforeClose = false;

		private sealed class CaptionStateIcons
		{
			public const int NORMAL = 0;
			public const int DISABLED = 1;
			public const int PRESSED = 2;
			public const int HOT = 3;
		}

		private sealed class PinIcons
		{
			public const int Pinned = 9;
			public const int UnPinned = 10;
		}

		public VistaWindowCaptionBar()
		{
			InitializeComponent();
		}

		private void VistaWindowCaptionBar_Load(object sender, EventArgs e)
		{
			VistaWindowCaptionBar control = (VistaWindowCaptionBar)sender;
			_form = (VistaWindowBase)control.Parent;

			if (_form != null)
			{
				_form.CaptionChanged += new EventHandler(Form_CaptionChanged);
				_form.TopMostChanged += new EventHandler(Form_TopMostChanged);
			}

			cmdMaximize.Visible = _showMaximize;
			cmdMinimize.Visible = _showMaximize;
			cmdHelp.Visible = _showHelp;

			cmdDisplay.Visible = _showDisplay;
			cmdRefresh.Visible = _showRefresh;
			cmdFit.Visible = _showFit;
		
			lblIcon.Visible = _showIcon;
			lblPin.Visible = _showPin;
			lblShowAll.Visible = _showShowAll;

			lblCaption.Text = _form.Caption;

			Form_TopMostChanged(null, EventArgs.Empty);
		}

		private void Form_CaptionChanged(object sender, EventArgs e)
		{
			lblCaption.Text = _form.Caption;
			Invalidate();
		}

		private void Form_TopMostChanged(object sender, EventArgs e)
		{
			if (_form != null)
			{
				if (_form.TopMost)
				{
					lblPin.ImageIndex = PinIcons.Pinned;
				}
				else
				{
					lblPin.ImageIndex = PinIcons.UnPinned;
				}
			}
		}

		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("Allow user to confirm before close")]
		public bool ConfirmBeforeClose
		{
			get
			{
				return _confirmBeforeClose;
			}
			set
			{
				if (_confirmBeforeClose != value)
				{
					_confirmBeforeClose = value;
				}
			}
		}

		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("Allow user to edit Caption Bar or not.")]
		public bool EnableCaptionEdit
		{
			get
			{
				return _enableCaptionEdit;
			}
			set
			{
				if (_enableCaptionEdit != value)
				{
					_enableCaptionEdit = value;
				}
			}
		}

		[DefaultValue(typeof(bool), "true"), Category("Appearance"), Description("Show Close Button or Not.")]
		public bool ShowClose
		{
			get
			{
				return _showClose;
			}
			set
			{
				if (_showClose != value)
				{
					_showClose = value;

					cmdClose.Visible = _showClose;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "true"), Category("Appearance"), Description("Show Maximize Button or Not.")]
		public bool ShowMaximize
		{
			get
			{
				return _showMaximize;
			}
			set
			{
				if (_showMaximize != value)
				{
					_showMaximize = value;

					cmdMaximize.Visible = _showMaximize;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "true"), Category("Appearance"), Description("Show Minimize Button or Not.")]
		public bool ShowMinimize
		{
			get
			{
				return _showMinimize;
			}
			set
			{
				if (_showMinimize != value)
				{
					_showMinimize = value;

					cmdMinimize.Visible = _showMinimize;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("Show Help Button or Not.")]
		public bool ShowHelp
		{
			get
			{
				return _showHelp;
			}
			set
			{
				if (_showHelp != value)
				{
					_showHelp = value;

					cmdHelp.Visible = _showHelp;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("Show Display Button or Not.")]
		public bool ShowDisplay
		{
			get
			{
				return _showDisplay;
			}
			set
			{
				if (_showDisplay != value)
				{
					_showDisplay = value;

					cmdDisplay.Visible = _showDisplay;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("Show Refresh Button or Not.")]
		public bool ShowRefresh
		{
			get
			{
				return _showRefresh;
			}
			set
			{
				if (_showRefresh != value)
				{
					_showRefresh = value;

					cmdRefresh.Visible = _showRefresh;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("Show Fit Button or Not.")]
		public bool ShowFit
		{
			get
			{
				return _showFit;
			}
			set
			{
				if (_showFit != value)
				{
					_showFit = value;

					cmdFit.Visible = _showFit;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "true"), Category("Appearance"), Description("Show Icon Button or Not.")]
		public bool ShowIcon
		{
			get
			{
				return _showIcon;
			}
			set
			{
				if (_showIcon != value)
				{
					_showIcon = value;

					lblIcon.Visible = _showIcon;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "true"), Category("Appearance"), Description("Show Icon Button or Not.")]
		public bool ShowPin
		{
			get
			{
				return _showPin;
			}
			set
			{
				if (_showPin != value)
				{
					_showPin = value;

					lblPin.Visible = _showPin;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(bool), "false"), Category("Appearance"), Description("Show ShowShowAll Button or Not.")]
		public bool ShowShowAll
		{
			get
			{
				return _showShowAll;
			}
			set
			{
				if (_showShowAll != value)
				{
					_showShowAll = value;

					lblShowAll.Visible = _showShowAll;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(string), ""), Category("Appearance"), Description("Show Icon Button Text")]
		public string IconText
		{
			get
			{
				return lblIcon.Text;
			}
			set
			{
				if (lblIcon.Text != value)
				{
					lblIcon.Text = value;
					Invalidate();
				}
			}
		}

		[DefaultValue(typeof(int), "0"), Category("Appearance"), Description("Show Icon From IconList.")]
		public int IconIndex
		{
			get
			{
				return lblIcon.ImageIndex;
			}
			set
			{
				if (lblIcon.ImageIndex != value && value < imageListIcons.Images.Count)
				{
					lblIcon.ImageIndex = value;
					Invalidate();
				}
			}
		}

		#region - Window Action -

		private void Button_MouseHover(object sender, EventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);

			lblCaption_MouseHover(null, EventArgs.Empty);
		}

		private void Button_MouseDown(object sender, MouseEventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.PRESSED);
		}

		private void Button_MouseUp(object sender, MouseEventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);
		}

		private void Button_MouseEnter(object sender, EventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);
		}

		private void Button_MouseLeave(object sender, EventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.NORMAL);
		}

		private void Button_MouseMove(object sender, MouseEventArgs e)
		{
			Button_Action(sender, CaptionStateIcons.HOT);
		}

		private void Button_Action(object sender, int state)
		{
			VistaButton button = (VistaButton)sender;
			if (button.ImageList != null && button.ImageList.Images.Count > state)
			{
				button.ImageIndex = state;
			}

			if (_form != null)
			{
				switch (state)
				{
					case CaptionStateIcons.PRESSED:
						switch (button.Name)
						{
							case "cmdClose":
								if (_confirmBeforeClose)
								{
									if (MessageBox.Show("Close this window?", "Alert", MessageBoxButtons.YesNo) == DialogResult.Yes)
									{
										_form.Close();
									}
								}
								else
								{
									_form.Close();
								}
								break;
							case "cmdMaximize":
								SetWindowState(button);
								break;
							case "cmdMinimize":
								if (_form.ShowInTaskbar)
								{
									_form.WindowState = FormWindowState.Minimized;
								}
								else
								{
									_form.Visible = false;
								}
								break;
							case "cmdHelp":
								break;
							case "cmdIcon":
								break;
							default:
								break;
						}
						break;
					default:
						break;
				}
			}
		}

		private void SetWindowState(VistaButton button)
		{
			switch (_form.WindowState)
			{
				case FormWindowState.Maximized:
					_form.WindowState = FormWindowState.Normal;
					button.ImageList = this.imageListMaximize;
					button.ImageIndex = CaptionStateIcons.NORMAL;
					break;
				default:
					_form.WindowState = FormWindowState.Maximized;
					button.ImageList = this.imageListRestore;
					button.ImageIndex = CaptionStateIcons.NORMAL;
					break;
			}
		}

		#endregion

		#region - Caption Bar Edit -

		private void lblCaption_MouseDown(object sender, MouseEventArgs e)
		{
			if (_form != null)
			{
				if (e.Clicks <= 1)
				{
					Point pt = new Point(Cursor.Position.X, Cursor.Position.Y);
					switch (_form.FormBorderStyle)
					{
						case FormBorderStyle.Fixed3D:
						case FormBorderStyle.FixedDialog:
						case FormBorderStyle.Sizable:
							pt = _form.PointToClient(pt);
							pt.X = pt.X + 4;
							pt.Y = pt.Y + 4;
							//pt = _form.PointToClient(pt);
							break;
						case FormBorderStyle.None:
							break;
					}
					IntPtr lParam = DockWindowHelper.Bit.MakeLParam(pt.X, pt.Y);

					DockWindowHelper.ReleaseCapture();
					DockWindowHelper.SendMessage(_form.Handle, DockWindowHelper.WM.WM_NCLBUTTONDOWN, DockWindowHelper.WM.WM_HTCAPTION, (int)lParam);
				}
				else
				{
					if (_enableCaptionEdit)
					{
						lblCaption.Visible = false;
						txtCaption.Width = lblCaption.Width;
						txtCaption.Text = _form.Caption;
						txtCaption.Visible = true;

						_form.ActiveControl = txtCaption;
					}
				}
			}
		}

		private void lblCaption_MouseHover(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.Activate();
			}
		}

		private void txtCaption_KeyPress(object sender, KeyPressEventArgs e)
		{
			if (_form != null)
			{
				switch ((Keys)e.KeyChar)
				{
					case Keys.Escape:
						lblCaption.Visible = true;
						txtCaption.Visible = false;
						break;
					case Keys.Enter:
						_form.Caption = txtCaption.Text;

						lblCaption.Visible = true;
						txtCaption.Visible = false;
						break;
					default:
						break;
				}
			}
		}

		private void txtCaption_DoubleClick(object sender, EventArgs e)
		{
			if (!lblCaption.Visible)
			{
				lblCaption.Visible = true;
				txtCaption.Visible = false;
			}
		}

		#endregion

		#region - Commands -

		private void cmdFit_Click(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.OnFitClick(sender, e);
			}
		}

		private void cmdHelp_Click(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.OnHelpClick(sender, e);
			}
		}

		private void cmdRefresh_Click(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.OnRefreshClick(sender, e);
			}
		}

		private void cmdDisplay_Click(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.OnDisplayClick(sender, e);
			}
		}

		private void lblIcon_Click(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.OnIconClick(sender, e);
			}
		}

		private void lblPin_Click(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.OnTop = !_form.OnTop;
				_form.OnPinClick(sender, e);
			}
		}

		private void lblShowAll_Click(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.OnShowAllClick(sender, e);
			}
		}

		#endregion

		#region IDisposable Members

		void IDisposable.Dispose()
		{
			_form = null;
		}

		#endregion
	}
}
