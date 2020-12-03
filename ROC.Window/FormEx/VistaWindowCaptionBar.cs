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
	public partial class VistaWindowCaptionBar : UserControl
	{
		private VistaWindowBase _form = null;

		private bool _enableCaptionEdit = false;

		private bool _showClose = true;
		private bool _showMaximize = true;
		private bool _showMinimize = true;
		private bool _showHelp = false;

		private bool _showRefresh = false;
		private bool _showFit = false;
		private bool _showIcon = true;

		private sealed class CaptionStateIcons
		{
			public const int NORMAL = 0;
			public const int DISABLED = 1;
			public const int PRESSED = 2;
			public const int HOT = 3;
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
			}

			cmdMaximize.Visible = _showMaximize;
			cmdMinimize.Visible = _showMaximize;
			cmdHelp.Visible = _showHelp;

			cmdRefresh.Visible = _showRefresh;
			cmdFit.Visible = _showFit;
			cmdIcon.Visible = _showIcon;

			lblCaption.Text = _form.Caption;
		}

		private void Form_CaptionChanged(object sender, EventArgs e)
		{
			lblCaption.Text = _form.Caption;
			Invalidate();
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

					cmdRefresh.Visible = _showFit;
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

					cmdIcon.Visible = _showIcon;
					Invalidate();
				}
			}
		}

		private void Caption_MouseHover(object sender, EventArgs e)
		{
			if (_form != null)
			{
				_form.Activate();
			}
		}

		private void Button_MouseHover(object sender, EventArgs e)
		{
			setImage(sender, CaptionStateIcons.HOT);

			Caption_MouseHover(null, EventArgs.Empty);
		}

		private void Button_MouseDown(object sender, MouseEventArgs e)
		{
			setImage(sender, CaptionStateIcons.PRESSED);
		}

		private void Button_MouseUp(object sender, MouseEventArgs e)
		{
			setImage(sender, CaptionStateIcons.HOT);
		}

		private void Button_MouseEnter(object sender, EventArgs e)
		{
			setImage(sender, CaptionStateIcons.HOT);
		}

		private void Button_MouseLeave(object sender, EventArgs e)
		{
			setImage(sender, CaptionStateIcons.NORMAL);
		}

		private void Button_MouseMove(object sender, MouseEventArgs e)
		{
			setImage(sender, CaptionStateIcons.HOT);
		}

		private void setImage(object sender, int state)
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
								_form.Close();
								break;
							case "cmdMaximize":
								setWindowState(button);
								break;
							case "cmdMinimize":
								_form.WindowState = FormWindowState.Minimized;
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

		private void setWindowState(VistaButton button)
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

		private void lblCaption_MouseDown(object sender, MouseEventArgs e)
		{
			if (_form != null)
			{
				if (e.Clicks <= 1)
				{
					Point pt = new Point(e.X, e.Y);
					pt = ((VistaLabel)sender).PointToScreen(pt);
					switch (_form.FormBorderStyle)
					{
						case FormBorderStyle.Fixed3D:
						case FormBorderStyle.FixedDialog:
						case FormBorderStyle.Sizable:
							pt = _form.PointToClient(pt);
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

		private void lblCaption_Resize(object sender, EventArgs e)
		{
			txtCaption.Width = lblCaption.Width;
			txtCaption.Left = lblCaption.Left;
		}



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
	}
}
