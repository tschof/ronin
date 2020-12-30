using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace ColorDialogEx
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class ColorWheelDialog : Form, INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, e);
			}
		}

		#endregion

		private enum ChangeStyle
		{
			MouseMove,
			RGB,
			HSV,
			None
		}

		private ChangeStyle changeType;
		private Point selectedPoint;

		private Rectangle _rectSelectedColor;
		private Rectangle _rectBrightness;
		private Rectangle _rectColorWheel;

		private ColorWheel _colorWheel;
		private ColorHandler.RGB _rgb;
		private ColorHandler.HSV _hsv;
		private bool _isInUpdate;

		public ColorWheelDialog()
		{
			SetStyle(ControlStyles.AllPaintingInWmPaint | ControlStyles.OptimizedDoubleBuffer | ControlStyles.ResizeRedraw | ControlStyles.SupportsTransparentBackColor | ControlStyles.UserPaint, true);

			InitializeComponent();

			#region - HSV Events -

			nudHue.TextChanged += new EventHandler(HandleTextChange);
			nudHue.ValueChanged += new EventHandler(HandleHSVChange);

			nudSaturation.TextChanged += new EventHandler(HandleTextChange);
			nudSaturation.ValueChanged += new EventHandler(HandleHSVChange);

			nudBrightness.TextChanged += new EventHandler(HandleTextChange);
			nudBrightness.ValueChanged += new EventHandler(HandleHSVChange);

			#endregion

			#region - RBG Events -

			nudRed.TextChanged += new EventHandler(HandleTextChange);
			nudRed.ValueChanged += new EventHandler(HandleRGBChange);

			nudBlue.TextChanged += new EventHandler(HandleTextChange);
			nudBlue.ValueChanged += new EventHandler(HandleRGBChange);

			nudGreen.TextChanged += new EventHandler(HandleTextChange);
			nudGreen.ValueChanged += new EventHandler(HandleRGBChange);

			#endregion

			#region - Mouse Events -

			panelColorWheel.MouseUp += new MouseEventHandler(HandleMouseUp);
			MouseUp += new MouseEventHandler(HandleMouseUp);
			MouseDown += new MouseEventHandler(HandleMouseMove);
			MouseMove += new MouseEventHandler(HandleMouseMove);

			#endregion

			#region - Form Events -

			Paint += new PaintEventHandler(HandlePaint);
			Load += new EventHandler(HandleLoad);

			#endregion

			changeType = ChangeStyle.None;
			_isInUpdate = false;

			panelSelectedColor.Visible = false;
			panelBrightness.Visible = false;
			panelColorWheel.Visible = false;

			_rectSelectedColor = new Rectangle(panelSelectedColor.Location, panelSelectedColor.Size);
			_rectBrightness = new Rectangle(panelBrightness.Location, panelBrightness.Size);
			_rectColorWheel = new Rectangle(panelColorWheel.Location, panelColorWheel.Size);

			_colorWheel = new ColorWheel(_rectColorWheel, _rectBrightness, _rectSelectedColor);
			_colorWheel.ColorChanged += new ColorWheel.ColorChangedEventHandler(HandleColorWheelChanged);
		}

		private void HandleLoad(object sender, EventArgs e)
		{
			SetRGB(_rgb);
			SetHSV(_hsv);
		}

		private void HandleColorWheelChanged(object sender, ColorChangedEventArgs e)
		{
			SetRGB(e.RGB);
			SetHSV(e.HSV);
		}

		private void HandlePaint(object sender, PaintEventArgs e)
		{
			switch (changeType)
			{
				case ChangeStyle.HSV:
					_colorWheel.Draw(e.Graphics, _hsv);
					break;
				case ChangeStyle.MouseMove:
				case ChangeStyle.None:
					_colorWheel.Draw(e.Graphics, selectedPoint);
					break;
				case ChangeStyle.RGB:
					_colorWheel.Draw(e.Graphics, _rgb);
					break;
			}
		}

		private void HandleMouseMove(object sender, MouseEventArgs e)
		{
			if (e.Button == MouseButtons.Left)
			{
				changeType = ChangeStyle.MouseMove;
				selectedPoint = new Point(e.X, e.Y);
				this.Invalidate();
			}
		}

		private void HandleMouseUp(object sender, MouseEventArgs e)
		{
			_colorWheel.SetMouseUp();
			changeType = ChangeStyle.None;
		}

		private void HandleRGBChange(object sender, EventArgs e)
		{
			if (!_isInUpdate)
			{
				changeType = ChangeStyle.RGB;
				_rgb = new ColorHandler.RGB((int)nudRed.Value, (int)nudGreen.Value, (int)nudBlue.Value);
				SetHSV(ColorHandler.RGBtoHSV(_rgb));
				this.Invalidate();
			}
		}

		private void HandleHSVChange(object sender, EventArgs e)
		{
			if (!_isInUpdate)
			{
				changeType = ChangeStyle.HSV;
				_hsv = new ColorHandler.HSV((int)nudHue.Value, (int)nudSaturation.Value, (int)nudBrightness.Value);
				SetRGB(ColorHandler.HSVtoRGB(_hsv));
				this.Invalidate();
			}
		}

		private void HandleTextChange(object sender, EventArgs e)
		{
			Decimal x = ((NumericUpDown)sender).Value;
		}

		private void SetRGB(ColorHandler.RGB RGB)
		{
			_isInUpdate = true;
			RefreshValue(nudRed, RGB.Red);
			RefreshValue(nudBlue, RGB.Blue);
			RefreshValue(nudGreen, RGB.Green);
			_isInUpdate = false;
		}

		private void SetHSV(ColorHandler.HSV HSV)
		{
			_isInUpdate = true;
			RefreshValue(nudHue, HSV.Hue);
			RefreshValue(nudSaturation, HSV.Saturation);
			RefreshValue(nudBrightness, HSV.value);
			_isInUpdate = false;
		}

		private void SetFromKnownColor(Color color)
		{
			if (!_isInUpdate)
			{
				SetColor(color);

				//changeType = ChangeStyle.RGB;
				//_rgb = new ColorHandler.RGB(
				//    Convert.ToInt32(color.R),
				//    Convert.ToInt32(color.B),
				//    Convert.ToInt32(color.G));
				//_colorWheel.Draw(CreateGraphics(), _rgb);
				//SetRGB(_rgb);

				//_hsv = new ColorHandler.HSV(
				//    Convert.ToInt32(color.GetHue()),
				//    Convert.ToInt32(color.GetSaturation()),
				//    Convert.ToInt32(color.GetBrightness()));
				//_colorWheel.Draw(CreateGraphics(), _hsv);
				//SetHSV(_hsv);

				panelPreDefiniedColor.BackColor = color;

				this.Invalidate();
			}
		}

		private void SetColor(Color color)
		{
			changeType = ChangeStyle.RGB;
			_rgb = new ColorHandler.RGB(color.R, color.G, color.B);
			_hsv = ColorHandler.RGBtoHSV(_rgb); 
		}

		private void RefreshValue(NumericUpDown nud, int value)
		{
			if (nud.Value != value && value <= 255)
			{
				nud.Value = value;
				nud.Refresh();
			}
		}

		public Color Color
		{
			get
			{
				if (_colorWheel != null)
				{
					return _colorWheel.Color;
				}
				else
				{
					return Color.White;
				}
			}
			set
			{
				SetColor(value);
			}
		}

		private void cmdCancel_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void cmdApply_Click(object sender, EventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs("Color"));
			Close();
		}

		private void KnownColor_Click(object sender, EventArgs e)
		{
			SetFromKnownColor(((Panel)sender).BackColor);
		}
	}
}
