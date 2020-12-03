using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms.VisualStyles;
using System.Collections.Generic;
using System.Drawing.Drawing2D;

using ContextMenuEx;
using System.Diagnostics;
using ShapeEx;

namespace ButtonEx
{
	[System.ComponentModel.DesignerCategory("code"), DefaultEvent("Click")]
	public class VistaButton : Button
	{
		#region - Event Headers -

		[Description("Event raised when the value of the corner radius property is changed."), Category("Property Changed")]
		public event EventHandler RadiusChanged;

		[Description("Event raised when the value of the InnerBorderColor property is changed."), Category("Property Changed")]
		public event EventHandler InnerBorderColorChanged;

		[Description("Event raised when the value of the OuterBorderColor property is changed."), Category("Property Changed")]
		public event EventHandler OuterBorderColorChanged;

		[Description("Event raised when the value of the ShineColor property is changed."), Category("Property Changed")]
		public event EventHandler ShineColorChanged;

		[Description("Event raised when the value of the GlowColor property is changed."), Category("Property Changed")]
		public event EventHandler GlowColorChanged;

		[Description("Event raised when the value of the ButtonType property is changed."), Category("Property Changed")]
		public event EventHandler ButtonTypeChanged;

		[Description("Event raised when the value of the SplitSize property is changed."), Category("Property Changed")]
		public event EventHandler SplitSizeChanged;

		[Description("Event raised when the value of the SplitStyle property is changed."), Category("Property Changed")]
		public event EventHandler SplitStyleChanged;

		[Description("Event raised when the value of the ShowImage property is changed."), Category("Property Changed")]
		public event EventHandler ShowImageChanged;

		[Description("Event raised when the value of the ShowText property is changed."), Category("Property Changed")]
		public event EventHandler ShowTextChanged;

		[Description("Event raised when split button is clicked"), Category("Action")]
		public event EventHandler SplitClick;

		[Description("Event raised when split button is clicked"), Category("Action")]
		public event EventHandler MouseSplitClick;

		[Description("Event raised when split button is entered"), Category("Mouse")]
		public event EventHandler MouseSplitEnter;

		[Description("Event raised when split button is up"), Category("Mouse")]
		public event EventHandler MouseSplitUp;

		[Description("Event raised when split button is down"), Category("Mouse")]
		public event EventHandler MouseSplitDown;

		public new event PaintEventHandler Paint;

		#endregion

		#region - Type Headers -

		public enum ROCButtonTypes
		{
			Normal,
			Buy,
			Sell,
			FilterOut,
			FilterIn,
		}

		public enum VistaButtonTypes
		{
			Normal,
			Split
		}

		public enum SplitStyles
		{
			Bottom,
			Right,
		}

		private sealed class Frames
		{
			public const int Disabled = 0;
			public const int Pressed = 1;
			public const int Normal = 2;
			public const int Animated = 3;
		}

		private struct Areas
		{
			public Rectangle Client;
			public Rectangle Outter;
			public Rectangle Inner;
			public Rectangle Shine;
		}

		private struct States
		{
			public bool isHovered;
			public bool isMouseDown;
			public bool isKeyDown;
			public bool isFocused;
			public bool isFocusedByKey;
			public bool isPressed
			{
				get
				{
					return isKeyDown || (isMouseDown && isHovered);
				}
			}
		}

		#endregion

		#region - Button States -

		private States _buttonStates;
		private States _splitStates;

		private Point _mousePoint;

		#endregion

		#region - Button Values -

		private bool _fadeOnFocus;
		private int _radius;
		private Color _backColor;
		private Color _outerBorderColor;
		private Color _innerBorderColor;
		private Color _shineColor;
		private Color _glowColor;
		private Button _imageButton;

		private VistaButtonTypes _vistaButtonType;
		private SplitStyles _splitStyle;
		private int _userSplitSize;
		private int _splitSize;
		private int _splitSizeDefault;
		private int _splitArrowOffset;
		private int _splitArrowOffsetDefault;

		private Areas _buttonAreas;
		private Areas _splitAreas;

		private bool _showImage;
		private bool _showText;

		private ROCButtonTypes _rocButtonType;

		#endregion

		#region - Animation Values -

		private const int _animationLength = 300;
		private const int _framesCount = 10;

		#region - Button -
		private bool _isButtonAnimating
		{
			get
			{
				return _buttonDirection != 0;
			}
		}
		private int _buttonCurrentFrame;
		private int _buttonDirection;
		private Timer _buttonTimer;
		#endregion

		#region - Split -
		private bool _isSplitAnimating
		{
			get
			{
				return _splitDirection != 0;
			}
		}
		private int _splitCurrentFrame;
		private int _splitDirection;
		private Timer _splitTimer;
		#endregion

		#endregion

		#region - Frame Values -

		private List<Image> _frames;
		private List<Image> _framesSplit;

		#endregion

		#region - Menu Values -

		private ContextMenuStrip _contextMenu;

		#endregion

		#region - Constructor -

		public VistaButton()
		{
			base.BackColor = Color.Transparent;

			_showImage = true;
			_showText = true;

			_radius = 4;
			_backColor = Color.Black;
			ForeColor = Color.White;
			_outerBorderColor = Color.White;
			_innerBorderColor = Color.Black;
			_shineColor = Color.White;
			_glowColor = Color.FromArgb(141, 189, 255);

			_vistaButtonType = VistaButtonTypes.Normal;
			_splitStyle = SplitStyles.Right;
			_splitSizeDefault = 35;
			_splitArrowOffsetDefault = 4;

			SetStyle(
				ControlStyles.AllPaintingInWmPaint |
				ControlStyles.CacheText |
				ControlStyles.OptimizedDoubleBuffer |
				ControlStyles.ResizeRedraw |
				ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.Opaque, false);

			_buttonTimer = new Timer();
			_buttonTimer.Interval = _animationLength / _framesCount;
			_buttonTimer.Tick += new EventHandler(buttonTimer_Tick);

			_splitTimer = new Timer();
			_splitTimer.Interval = _animationLength / _framesCount;
			_splitTimer.Tick += new EventHandler(splitTimer_Tick);

			_mousePoint = new Point(-1, -1);
		}

		#endregion

		#region - Frame Methods -

		private void CreateFrames()
		{
			CreateFrames(false);
		}

		private void CreateFrames(bool withAnimationFrames)
		{
			DestroyFrames();
			if (!IsHandleCreated)
			{
				return;
			}

			// Get the working areas of the button
			CreateButton(_radius);

			if (_frames == null)
			{
				_frames = new List<Image>();
			}

			#region - State Frames -
			// FRAME_DISABLED
			_frames.Add(CreateBackgroundFrame(_buttonAreas, false, false, false, false, false, 0));
			// FRAME_PRESSED
			_frames.Add(CreateBackgroundFrame(_buttonAreas, false, true, true, false, true, 0));
			// FRAME_NORMAL
			_frames.Add(CreateBackgroundFrame(_buttonAreas, false, false, false, false, true, 0));
			if (_vistaButtonType == VistaButtonTypes.Split)
			{
				if (_framesSplit == null)
				{
					_framesSplit = new List<Image>();
				}

				//// FRAME_DISABLED
				_framesSplit.Add(CreateBackgroundFrame(_splitAreas, true, false, false, false, false, 0));
				// FRAME_PRESSED
				_framesSplit.Add(CreateBackgroundFrame(_splitAreas, true, true, true, false, true, 0));
				// FRAME_NORMAL
				_framesSplit.Add(CreateBackgroundFrame(_splitAreas, true, false, false, false, true, 0));
			}
			#endregion

			#region - Animation Frames -
			if (!withAnimationFrames)
			{
				return;
			}

			for (int i = 0; i < _framesCount; i++)
			{
				// Animated Frames 3 to 13
				_frames.Add(CreateBackgroundFrame(_buttonAreas, false, false, true, true, true, (float)i / (_framesCount - 1F)));
			}
			if (_vistaButtonType == VistaButtonTypes.Split)
			{
				for (int i = 0; i < _framesCount; i++)
				{
					// Animated Frames 3 to 13
					_framesSplit.Add(CreateBackgroundFrame(_splitAreas, true, false, true, true, true, (float)i / (_framesCount - 1F)));
				}
			}
			#endregion
		}

		private void DestroyFrames()
		{
			if (_frames != null)
			{
				while (_frames.Count > 0)
				{
					_frames[_frames.Count - 1].Dispose();
					_frames.RemoveAt(_frames.Count - 1);
				}
			}
			if (_framesSplit != null)
			{
				while (_framesSplit.Count > 0)
				{
					_framesSplit[_framesSplit.Count - 1].Dispose();
					_framesSplit.RemoveAt(_framesSplit.Count - 1);
				}
			}
		}

		#endregion

		#region - Area Methods -

		private void CreateButton(float r)
		{
			Rectangle rect = ClientRectangle;
			Rectangle rectSplit = Rectangle.Empty;

			#region - client area -
			if (rect.Width <= 0)
			{
				rect.Width = 1;
			}
			if (rect.Height <= 0)
			{
				rect.Height = 1;
			}

			if (_vistaButtonType == VistaButtonTypes.Split)
			{
				CreateSplitButton(ref rect, ref rectSplit, r);
			}

			_buttonAreas.Client = rect;
			_splitAreas.Client = rectSplit;
			#endregion

			#region - outter area white border -
			rect.Width--;
			rect.Height--;
			if (_vistaButtonType == VistaButtonTypes.Split)
			{
				rectSplit.X = 0;
				rectSplit.Y = 0;
				rectSplit.Width--;
				rectSplit.Height--;
			}

			_buttonAreas.Outter = rect;
			_splitAreas.Outter = rectSplit;
			#endregion

			#region - shine and inner area -
			rect.X++;
			rect.Y++;
			rect.Width -= 2;
			rect.Height -= 2;
			Rectangle rectShine = rect;
			rectShine.Height >>= 1;
			Rectangle rectSplitShine = Rectangle.Empty;
			if (_vistaButtonType == VistaButtonTypes.Split)
			{
				rectSplit.X++;
				rectSplit.Y++;
				rectSplit.Width -= 2;
				rectSplit.Height -= 2;
				rectSplitShine = rectSplit;
				rectSplitShine.Height >>= 1;
			}

			_buttonAreas.Inner = rect;
			_splitAreas.Inner = rectSplit;

			_buttonAreas.Shine = rectShine;
			_splitAreas.Shine = rectSplitShine;
			#endregion
		}

		private void CreateSplitButton(ref Rectangle rect, ref Rectangle splitRect, float r)
		{
			switch (_splitStyle)
			{
				case SplitStyles.Bottom:
					if (_userSplitSize == 0)
					{
						if (rect.Height / 3 < _splitSizeDefault)
						{
							_splitSize = rect.Height / 3;
							if (_splitSize < 1)
							{
								_splitSize = 1;
							}
						}
						else if (rect.Height / 3 >= _splitSizeDefault)
						{
							_splitSize = _splitSizeDefault;
						}
					}
					else
					{
						_splitSize = _userSplitSize;
					}

					rect.Height -= _splitSize;
					if (rect.Height <= 0)
					{
						rect.Height = 1;
					}

					splitRect = new Rectangle(rect.X, rect.Bottom, rect.Width, _splitSize);
					break;
				case SplitStyles.Right:
				default:
					if (_userSplitSize == 0)
					{
						if (rect.Width / 4 < _splitSizeDefault)
						{
							_splitSize = rect.Width / 4;
							if (_splitSize < 1)
							{
								_splitSize = 1;
							}
						}
						else if (rect.Width / 4 >= _splitSizeDefault)
						{
							_splitSize = _splitSizeDefault;
						}
					}
					else
					{
						_splitSize = _userSplitSize;
					}

					rect.Width -= _splitSize;
					if (rect.Width <= 0)
					{
						rect.Width = 1;
					}

					splitRect = new Rectangle(rect.Right, rect.Y, _splitSize, rect.Height);
					break;
			}
		}

		#endregion

		#region - Draw Methods -

		private void DrawButtonBackgroundFromBuffer(Graphics graphics)
		{
			int frame;

			if (!Enabled)
			{
				frame = Frames.Disabled;
			}
			else if (_buttonStates.isPressed)
			{
				frame = Frames.Pressed;
			}
			else if (!_isButtonAnimating && _buttonCurrentFrame == 0)
			{
				frame = Frames.Normal;
			}
			else
			{
				if (!hasButtonAnimationFrames)
				{
					CreateFrames(true);
				}
				frame = Frames.Animated + _buttonCurrentFrame;
			}

			if (_frames == null || _frames.Count == 0)
			{
				CreateFrames();
			}

			graphics.DrawImage(_frames[frame], _buttonAreas.Client);
		}

		private void DrawSplitBackgroundFromBuffer(Graphics graphics)
		{
			int frameSplit;

			if (!Enabled)
			{
				frameSplit = Frames.Disabled;
			}
			else if (_splitStates.isPressed)
			{
				frameSplit = Frames.Pressed;
			}
			else if (!_isSplitAnimating && _splitCurrentFrame == 0)
			{
				frameSplit = Frames.Normal;
			}
			else
			{
				if (!hasButtonAnimationFrames)
				{
					CreateFrames(true);
				}
				frameSplit = Frames.Animated + _splitCurrentFrame;
			}

			if (_framesSplit == null || _framesSplit.Count == 0)
			{
				CreateFrames();
			}

			graphics.DrawImage(_framesSplit[frameSplit], _splitAreas.Client);
		}

		private void DrawForegroundFromButton(PaintEventArgs pevent)
		{
			try
			{
				if (_imageButton == null || _imageButton.Image != Image)
				{
					_imageButton = new Button();
					_imageButton.Parent = new imageButtonControl();
					_imageButton.SuspendLayout();
					_imageButton.BackColor = Color.Transparent;
					_imageButton.FlatAppearance.BorderSize = 0;
					_imageButton.FlatStyle = FlatStyle.Flat;
				}
				else
				{
					_imageButton.SuspendLayout();
				}
				_imageButton.AutoEllipsis = AutoEllipsis;
				if (Enabled)
				{
					_imageButton.ForeColor = ForeColor;
				}
				else
				{
					_imageButton.ForeColor = Color.FromArgb((3 * ForeColor.R + _backColor.R) >> 2,
						(3 * ForeColor.G + _backColor.G) >> 2,
						(3 * ForeColor.B + _backColor.B) >> 2);
				}
				_imageButton.Font = Font;
				_imageButton.RightToLeft = RightToLeft;
				if (_imageButton.Image != null && _imageButton.Image != Image)
				{
					_imageButton.Image.Dispose();
				}
				if (_showImage && Image != null)
				{
					_imageButton.Image = Image;
					if (!Enabled)
					{
						Size size = Image.Size;
						float[][] newColorMatrix = new float[5][];
						newColorMatrix[0] = new float[] { 0.2125f, 0.2125f, 0.2125f, 0f, 0f };
						newColorMatrix[1] = new float[] { 0.2577f, 0.2577f, 0.2577f, 0f, 0f };
						newColorMatrix[2] = new float[] { 0.0361f, 0.0361f, 0.0361f, 0f, 0f };
						float[] arr = new float[5];
						arr[3] = 1f;
						newColorMatrix[3] = arr;
						newColorMatrix[4] = new float[] { 0.38f, 0.38f, 0.38f, 0f, 1f };
						System.Drawing.Imaging.ColorMatrix matrix = new System.Drawing.Imaging.ColorMatrix(newColorMatrix);
						System.Drawing.Imaging.ImageAttributes disabledImageAttr = new System.Drawing.Imaging.ImageAttributes();
						disabledImageAttr.ClearColorKey();
						disabledImageAttr.SetColorMatrix(matrix);
						_imageButton.Image = new Bitmap(Image.Width, Image.Height);
						using (Graphics gr = Graphics.FromImage(_imageButton.Image))
						{
							gr.DrawImage(Image, new Rectangle(0, 0, size.Width, size.Height), 0, 0, size.Width, size.Height, GraphicsUnit.Pixel, disabledImageAttr);
						}
					}
				}
				_imageButton.ImageAlign = ImageAlign;
				_imageButton.ImageIndex = ImageIndex;
				_imageButton.ImageKey = ImageKey;
				_imageButton.ImageList = ImageList;
				_imageButton.Padding = Padding;
				_imageButton.Size = _buttonAreas.Client.Size;
				if (_showText)
				{
					_imageButton.Text = Text;
					_imageButton.TextAlign = TextAlign;
					_imageButton.TextImageRelation = TextImageRelation;
					_imageButton.UseCompatibleTextRendering = UseCompatibleTextRendering;
				}
				_imageButton.UseMnemonic = UseMnemonic;
				_imageButton.ResumeLayout();
				InvokePaint(_imageButton, pevent);
			}
			catch (Exception ex)
			{
				Debug.Assert(!Debugger.IsAttached, ex.Message, ex.StackTrace);
			}
		}

		private void DrawButtonForeground(Graphics g)
		{
			if (Focused && ShowFocusCues)
			{
				Rectangle rect = _buttonAreas.Client;
				rect.Inflate(-4, -4);
				ControlPaint.DrawFocusRectangle(g, rect);
			}
		}

		private Image CreateBackgroundFrame(Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, float glowOpacity)
		{
			Image img = new Bitmap(areas.Client.Width, areas.Client.Height);
			using (Graphics g = Graphics.FromImage(img))
			{
				g.Clear(Color.Transparent);
				DrawButtonBackground(g, areas, isSplit, pressed, hovered, animating, enabled, _outerBorderColor, _backColor, _glowColor, _shineColor, _innerBorderColor, glowOpacity);
			}

			return img;
		}

		private void DrawButtonBackground(Graphics g, Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, Color outerBorderColor, Color backColor, Color glowColor, Color shineColor, Color innerBorderColor, float glowOpacity)
		{
			SmoothingMode sm = g.SmoothingMode;
			InterpolationMode im = g.InterpolationMode;

			g.SmoothingMode = SmoothingMode.AntiAlias;
			g.InterpolationMode = InterpolationMode.HighQualityBicubic;

			DrawWhiteBorder(g, areas.Outter, outerBorderColor);
			DrawContent(g, areas.Inner, backColor, pressed);
			if ((hovered || animating) && !pressed)
			{
				DrawGlow(g, areas.Inner, glowColor, glowOpacity);
			}
			DrawShine(g, areas.Shine, shineColor, pressed, enabled);
			DrawBlackBorder(g, areas.Inner, innerBorderColor);
			if (isSplit)
			{
				DrawSplitArrow(g, areas.Outter);
			}

			g.SmoothingMode = sm;
			g.InterpolationMode = im;
		}

		private void DrawWhiteBorder(Graphics g, Rectangle rect, Color outerBorderColor)
		{
			using (GraphicsPath bw = ShapeHelper.GetRoundRect(rect, _radius))
			{
				using (Pen p = new Pen(outerBorderColor))
				{
					g.DrawPath(p, bw);
				}
			}
		}

		private void DrawContent(Graphics g, Rectangle rect, Color backColor, bool pressed)
		{
			using (GraphicsPath bb = ShapeHelper.GetRoundRect(rect, _radius - 2))
			{
				int opacity = pressed ? 0xcc : 0x7f;
				using (Brush br = new SolidBrush(Color.FromArgb(opacity, backColor)))
				{
					g.FillPath(br, bb);
				}
			}
		}

		private void DrawGlow(Graphics g, Rectangle rect, Color glowColor, float glowOpacity)
		{
			using (GraphicsPath clip = ShapeHelper.GetRoundRect(rect, _radius - 2))
			{
				g.SetClip(clip, CombineMode.Intersect);
				using (GraphicsPath brad = ShapeHelper.GetBottomRadialPath(rect))
				{
					using (PathGradientBrush pgr = new PathGradientBrush(brad))
					{
						unchecked
						{
							int opacity = (int)(0xB2 * glowOpacity + .5f);
							RectangleF bounds = brad.GetBounds();
							pgr.CenterPoint = new PointF((bounds.Left + bounds.Right) / 2f, (bounds.Top + bounds.Bottom) / 2f);
							pgr.CenterColor = Color.FromArgb(opacity, glowColor);
							pgr.SurroundColors = new Color[] { Color.FromArgb(0, glowColor) };
						}
						g.FillPath(pgr, brad);
					}
				}
				g.ResetClip();
			}
		}

		private void DrawShine(Graphics g, Rectangle rect, Color shineColor, bool pressed, bool enabled)
		{
			if (rect.Width > 0 && rect.Height > 0)
			{
				rect.Height += 2;
				using (GraphicsPath bh = ShapeHelper.GetTopRoundRect(rect, _radius - 2))
				{
					int opacity = 0x99;
					if (pressed | !enabled)
					{
						opacity = (int)(.4f * opacity + .5f);
					}
					using (LinearGradientBrush br = new LinearGradientBrush(rect, Color.FromArgb(opacity, shineColor), Color.FromArgb(opacity / 3, shineColor), LinearGradientMode.Vertical))
					{
						g.FillPath(br, bh);
					}
				}
				rect.Height -= 2;
			}
		}

		private void DrawBlackBorder(Graphics g, Rectangle rect, Color innerBorderColor)
		{
			using (GraphicsPath bb = ShapeHelper.GetRoundRect(rect, _radius - 1))
			{
				using (Pen p = new Pen(innerBorderColor))
				{
					g.DrawPath(p, bb);
				}
			}
		}

		private void DrawSplitArrow(Graphics g, Rectangle rect)
		{
			// Get the center of the split rect first
			Point center = new Point(rect.X + rect.Width / 2, rect.Y + rect.Height / 2);

			Point[] points = new Point[3];

			// Get the arrow offset from center
			if (rect.Width < rect.Height)
			{
				if (rect.Width / 4 < _splitArrowOffsetDefault)
				{
					_splitArrowOffset = rect.Width / 4;
				}
				else
				{
					_splitArrowOffset = _splitArrowOffsetDefault;
				}
			}
			else
			{
				if (rect.Height / 4 < _splitArrowOffsetDefault)
				{
					_splitArrowOffset = rect.Height / 4;
				}
				else
				{
					_splitArrowOffset = _splitArrowOffsetDefault;
				}
			}

			points[0] = new Point(center.X - _splitArrowOffset, center.Y - _splitArrowOffset);
			points[1] = new Point(center.X + _splitArrowOffset, center.Y - _splitArrowOffset);
			points[2] = new Point(center.X, center.Y + _splitArrowOffset);

			using (Brush b = new SolidBrush(ForeColor))
			{
				g.FillPolygon(b, points);
			}
		}

		#endregion

		#region - Control Events -

		protected override void OnPaint(PaintEventArgs e)
		{
			DrawButtonBackgroundFromBuffer(e.Graphics);
			if (_vistaButtonType == VistaButtonTypes.Split)
			{
				DrawSplitBackgroundFromBuffer(e.Graphics);
			}
			DrawForegroundFromButton(e);
			DrawButtonForeground(e.Graphics);

			if (Paint != null)
			{
				Paint(this, e);
			}
		}

		protected override void OnSizeChanged(EventArgs e)
		{
			CreateFrames();
			base.OnSizeChanged(e);
		}

		protected virtual void OnRadiusChanged(EventArgs e)
		{
			if (RadiusChanged != null)
			{
				RadiusChanged(this, e);
			}
		}

		protected virtual void OnInnerBorderColorChanged(EventArgs e)
		{
			if (InnerBorderColorChanged != null)
			{
				InnerBorderColorChanged(this, e);
			}
		}

		protected virtual void OnOuterBorderColorChanged(EventArgs e)
		{
			if (OuterBorderColorChanged != null)
			{
				OuterBorderColorChanged(this, e);
			}
		}

		protected virtual void OnShineColorChanged(EventArgs e)
		{
			if (ShineColorChanged != null)
			{
				ShineColorChanged(this, e);
			}
		}

		protected virtual void OnGlowColorChanged(EventArgs e)
		{
			if (GlowColorChanged != null)
			{
				GlowColorChanged(this, e);
			}
		}

		protected virtual void OnButtonExTypeChanged(EventArgs e)
		{
			if (ButtonTypeChanged != null)
			{
				ButtonTypeChanged(this, e);
			}
		}

		protected virtual void OnSplitSizeChanged(EventArgs e)
		{
			if (SplitSizeChanged != null)
			{
				SplitSizeChanged(this, e);
			}
		}

		protected virtual void OnSplitStyleChanged(EventArgs e)
		{
			if (SplitStyleChanged != null)
			{
				SplitStyleChanged(this, e);
			}
		}

		protected virtual void OnShowImageChanged(EventArgs e)
		{
			if (ShowImageChanged != null)
			{
				ShowImageChanged(this, e);
			}
		}

		protected virtual void OnShowTextChanged(EventArgs e)
		{
			if (ShowTextChanged != null)
			{
				ShowTextChanged(this, e);
			}
		}

		protected virtual void OnSplitClick(EventArgs e)
		{
			if (SplitClick != null)
			{
				SplitClick(this, e);
			}
		}

		protected virtual void OnMouseSplitClick(EventArgs e)
		{
			if (MouseSplitClick != null)
			{
				MouseSplitClick(this, e);
			}
		}

		protected virtual void OnMouseSplitEnter(EventArgs e)
		{
			if (MouseSplitEnter != null)
			{
				MouseSplitEnter(this, e);
			}
		}

		protected virtual void OnMouseSplitUp(EventArgs e)
		{
			if (MouseSplitUp != null)
			{
				MouseSplitUp(this, e);
			}
		}

		protected virtual void OnMouseSplitDown(EventArgs e)
		{
			if (MouseSplitDown != null)
			{
				MouseSplitDown(this, e);
			}
		}

		#endregion

		#region - Mouse and Keyboard Events -

		protected override void OnClick(EventArgs e)
		{
			if (inSplitArea)
			{
				_splitStates.isKeyDown = _splitStates.isMouseDown = false;
				OnSplitClick(e);
			}
			else
			{
				_buttonStates.isKeyDown = _buttonStates.isMouseDown = false;
				base.OnClick(e);
			}
		}

		protected override void OnEnter(EventArgs e)
		{
			if (inSplitArea)
			{
				_splitStates.isFocused = _splitStates.isFocusedByKey = true;
				OnMouseSplitEnter(e);
				if (_fadeOnFocus)
				{
					splitFadeIn();
				}
			}
			else
			{
				_buttonStates.isFocused = _buttonStates.isFocusedByKey = true;
				base.OnEnter(e);
				if (_fadeOnFocus)
				{
					buttonFadeIn();
				}
			}
		}

		protected override void OnLeave(EventArgs e)
		{
			base.OnLeave(e);

			_splitStates.isFocused = _splitStates.isFocusedByKey = _splitStates.isKeyDown = _splitStates.isMouseDown = false;
			_buttonStates.isFocused = _buttonStates.isFocusedByKey = _buttonStates.isKeyDown = _buttonStates.isMouseDown = false;

			Invalidate();

			if (_fadeOnFocus)
			{
				splitFadeOut();
				buttonFadeOut();
			}
		}

		protected override void OnKeyDown(KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Space)
			{
				_buttonStates.isKeyDown = true;
				Invalidate(_buttonAreas.Client);
			}
			else if (e.KeyCode == Keys.Enter)
			{
				_splitStates.isKeyDown = true;
				Invalidate(_splitAreas.Client);
			}
			base.OnKeyDown(e);
		}

		protected override void OnKeyUp(KeyEventArgs e)
		{
			if (_buttonStates.isKeyDown && e.KeyCode == Keys.Space)
			{
				_buttonStates.isKeyDown = false;
				Invalidate(_buttonAreas.Client);
			}
			else if (_splitStates.isKeyDown && e.KeyCode == Keys.Enter)
			{
				_splitStates.isKeyDown = false;
				Invalidate(_splitAreas.Client);
			}
			base.OnKeyUp(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if (inSplitArea)
			{
				if (!_splitStates.isMouseDown) // && e.Button == MouseButtons.Left)
				{
					_splitStates.isMouseDown = true;
					_splitStates.isFocusedByKey = false;
					Invalidate();
					OnMouseSplitDown(e);
				}
			}
			else
			{
				if (!_buttonStates.isMouseDown) // && e.Button == MouseButtons.Left)
				{
					_buttonStates.isMouseDown = true;
					_buttonStates.isFocusedByKey = false;
					Invalidate(_buttonAreas.Client);
					base.OnMouseDown(e);
				}
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			if (inSplitArea)
			{
				_splitStates.isMouseDown = false;
				Invalidate(_splitAreas.Client);
				OnMouseSplitUp(e);
			}
			else if (_buttonStates.isMouseDown)
			{
				_buttonStates.isMouseDown = false;
				Invalidate(_buttonAreas.Client);
				base.OnMouseUp(e);
			}
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (e.Button != MouseButtons.None)
			{
				if (inSplitArea)
				{
					if (_splitStates.isHovered)
					{
						_splitStates.isHovered = false;
					}
					else
					{
						_splitStates.isHovered = true;
					}
				}
				else if (inButtonArea)
				{
					if (_buttonStates.isHovered)
					{
						_buttonStates.isHovered = false;
					}
					else
					{
						_buttonStates.isHovered = true;
					}
				}
			}

			if (inSplitArea)
			{
				if (_buttonStates.isHovered)
				{
					_buttonStates.isHovered = false;
					buttonFadeOut();
					Invalidate(_buttonAreas.Client);
				}

				_splitStates.isHovered = true;
				splitFadeIn();
				Invalidate(_splitAreas.Client);
			}
			else if (inButtonArea)
			{
				if (_splitStates.isHovered)
				{
					_splitStates.isHovered = false;
					splitFadeOut();
					Invalidate(_splitAreas.Client);
				}

				_buttonStates.isHovered = true;
				buttonFadeIn();
				Invalidate(_buttonAreas.Client);
			}
		}

		protected override void OnMouseEnter(EventArgs e)
		{
			if (inSplitArea)
			{
				_splitStates.isHovered = true;
				splitFadeIn();
				Invalidate(_splitAreas.Client);
			}
			else if (inButtonArea)
			{
				_buttonStates.isHovered = true;
				buttonFadeIn();
				Invalidate(_buttonAreas.Client);
			}
			base.OnMouseEnter(e);
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			if (_splitStates.isHovered)
			{
				_splitStates.isMouseDown = false;
				_splitStates.isHovered = false;
				splitFadeOut();
				Invalidate(_splitAreas.Client);
			}

			if (_buttonStates.isHovered)
			{
				_buttonStates.isMouseDown = false;
				_buttonStates.isHovered = false;
				buttonFadeOut();
				Invalidate(_buttonAreas.Client);
			}

			base.OnMouseLeave(e);
		}

		#endregion

		#region - Mouse Capture -

		private bool inButtonArea
		{
			get
			{
				bool isIn = false;
				ConvertToLocalMousePoint();
				if (_buttonAreas.Client.Contains(_mousePoint))
				{
					isIn = true;
				}

				return isIn;
			}
		}

		private bool inSplitArea
		{
			get
			{
				bool isIn = false;
				ConvertToLocalMousePoint();
				if (_vistaButtonType == VistaButtonTypes.Split)
				{
					if (_splitAreas.Client.Contains(_mousePoint))
					{
						isIn = true;
					}
				}

				return isIn;
			}
		}

		private void ConvertToLocalMousePoint()
		{
			_mousePoint = PointToClient(Cursor.Position);
		}

		#endregion

		#region - Animation -

		private void buttonFadeIn()
		{
			_buttonDirection = 1;
			_buttonTimer.Enabled = true;
		}

		private void buttonFadeOut()
		{
			_buttonDirection = -1;
			_buttonTimer.Enabled = true;
		}

		private void buttonTimer_Tick(object sender, EventArgs e)
		{
			if (!_buttonTimer.Enabled)
			{
				return;
			}
			Refresh();
			_buttonCurrentFrame += _buttonDirection;
			if (_buttonCurrentFrame == -1)
			{
				_buttonCurrentFrame = 0;
				_buttonTimer.Enabled = false;
				_buttonDirection = 0;
				return;
			}
			if (_buttonCurrentFrame == _framesCount)
			{
				_buttonCurrentFrame = _framesCount - 1;
				_buttonTimer.Enabled = false;
				_buttonDirection = 0;
			}
		}

		private void splitFadeIn()
		{
			_splitDirection = 1;
			_splitTimer.Enabled = true;
		}

		private void splitFadeOut()
		{
			_splitDirection = -1;
			_splitTimer.Enabled = true;
		}

		private void splitTimer_Tick(object sender, EventArgs e)
		{
			if (!_splitTimer.Enabled)
			{
				return;
			}
			Refresh();
			_splitCurrentFrame += _splitDirection;
			if (_splitCurrentFrame == -1)
			{
				_splitCurrentFrame = 0;
				_splitTimer.Enabled = false;
				_splitDirection = 0;
				return;
			}
			if (_splitCurrentFrame == _framesCount)
			{
				_splitCurrentFrame = _framesCount - 1;
				_splitTimer.Enabled = false;
				_splitDirection = 0;
			}
		}

		private bool hasButtonAnimationFrames
		{
			get
			{
				return _frames != null && _frames.Count > Frames.Animated;
			}
		}

		private bool hasSplitAnimationFrames
		{
			get
			{
				return _framesSplit != null && _framesSplit.Count > Frames.Animated;
			}
		}

		#endregion

		#region - Properties -

		public virtual new ContextMenuStrip ContextMenuStrip
		{
			get
			{
				return _contextMenu;
			}
			set
			{
				_contextMenu = value;
			}
		}

		public virtual new Color BackColor
		{
			get
			{
				return _backColor;
			}
			set
			{
				if (!_backColor.Equals(value))
				{
					_backColor = value;
					UseVisualStyleBackColor = false;
					CreateFrames();
					OnBackColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "White")]
		public virtual new Color ForeColor
		{
			get
			{
				return base.ForeColor;
			}
			set
			{
				if (base.ForeColor != value)
				{
					base.ForeColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnForeColorChanged(EventArgs.Empty);
				}
			}
		}

		public override Image BackgroundImage
		{
			get
			{
				return base.BackgroundImage;
			}
			set
			{
				base.BackgroundImage = value;
			}
		}

		public override ImageLayout BackgroundImageLayout
		{
			get
			{
				return base.BackgroundImageLayout;
			}
			set
			{
				base.BackgroundImageLayout = value;
			}
		}

		[DefaultValue(4), Category("Appearance"), Description("Indicates whether the control should fade in and fade out when it is getting and loosing the focus.")]
		public virtual int Radius
		{
			get
			{
				return _radius;
			}
			set
			{
				if (_radius != value && value > 0)
				{
					_radius = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnRadiusChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "Black"), Category("Appearance"), Description("The inner border color of the control.")]
		public virtual Color InnerBorderColor
		{
			get
			{
				return _innerBorderColor;
			}
			set
			{
				if (_innerBorderColor != value)
				{
					_innerBorderColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnInnerBorderColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "White"), Category("Appearance"), Description("The outer border color of the control.")]
		public virtual Color OuterBorderColor
		{
			get
			{
				return _outerBorderColor;
			}
			set
			{
				if (_outerBorderColor != value)
				{
					_outerBorderColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnOuterBorderColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "White"), Category("Appearance"), Description("The shine color of the control.")]
		public virtual Color ShineColor
		{
			get
			{
				return _shineColor;
			}
			set
			{
				if (_shineColor != value)
				{
					_shineColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnShineColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "255,141,189,255"), Category("Appearance"), Description("The glow color of the control.")]
		public virtual Color GlowColor
		{
			get
			{
				return _glowColor;
			}
			set
			{
				if (_glowColor != value)
				{
					_glowColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnGlowColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Boolean), "False"), Category("Appearance"), Description("Indicates whether the control should fade in and fade out when it is getting and loosing the focus.")]
		public virtual bool FadeOnFocus
		{
			get
			{
				return _fadeOnFocus;
			}
			set
			{
				if (_fadeOnFocus != value)
				{
					_fadeOnFocus = value;
					_buttonTimer.Enabled = _fadeOnFocus;
				}
			}
		}

		[DefaultValue(typeof(ButtonEx.VistaButton.VistaButtonTypes), "Normal"), Category("Appearance"), Description("Indicates whether the button is a split or not.")]
		public virtual VistaButtonTypes ButtonType
		{
			get
			{
				return _vistaButtonType;
			}
			set
			{
				if (_vistaButtonType != value)
				{
					_vistaButtonType = value;
				}
				CreateFrames();
				if (IsHandleCreated)
				{
					Invalidate();
				}
				OnButtonExTypeChanged(EventArgs.Empty);
			}
		}

		[DefaultValue(0), Category("Appearance"), Description("Indicates the width of split when SplitStyle is left or right and hight of split when SplitStyle is top or bottom.")]
		public virtual int UserSplitSize
		{
			get
			{
				return _userSplitSize;
			}
			set
			{
				if (_userSplitSize != value)
				{
					_userSplitSize = value;
				}
				CreateFrames();
				if (IsHandleCreated)
				{
					Invalidate();
				}
				OnSplitSizeChanged(EventArgs.Empty);
			}
		}

		[DefaultValue(typeof(ButtonEx.VistaButton.SplitStyles), "Right"), Category("Appearance"), Description("Indicates the split style.")]
		public virtual SplitStyles SplitStyle
		{
			get
			{
				return _splitStyle;
			}
			set
			{
				if (_vistaButtonType == VistaButtonTypes.Split)
				{
					if (_splitStyle != value)
					{
						_splitStyle = value;
					}
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnSplitStyleChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Boolean), "True"), Category("Appearance"), Description("The Image of the control.")]
		public virtual bool ShowImage
		{
			get
			{
				return _showImage;
			}
			set
			{
				if (_showImage != value)
				{
					_showImage = value;
					_imageButton = null;
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnShowImageChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Boolean), "True"), Category("Appearance"), Description("The Text of the control.")]
		public virtual bool ShowText
		{
			get
			{
				return _showText;
			}
			set
			{
				if (_showText != value)
				{
					_showText = value;
					_imageButton = null;
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnShowTextChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(ButtonEx.VistaButton.ROCButtonTypes), "Normal"), Category("Appearance"), Description("Indicates whether the button is a Normal, Buy or Sell type.")]
		public virtual ROCButtonTypes ROCButtonType
		{
			get
			{
				return _rocButtonType;
			}
			set
			{
				if (_rocButtonType != value)
				{
					_rocButtonType = value;
				}
			}
		}

		[Browsable(false)]
		public PushButtonState State
		{
			get
			{
				if (!Enabled)
				{
					return PushButtonState.Disabled;
				}
				if (_buttonStates.isPressed)
				{
					return PushButtonState.Pressed;
				}
				if (_buttonStates.isHovered)
				{
					return PushButtonState.Hot;
				}
				if (_buttonStates.isFocused || IsDefault)
				{
					return PushButtonState.Default;
				}
				return PushButtonState.Normal;
			}
		}

		[Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), EditorBrowsable(EditorBrowsableState.Never)]
		public new FlatButtonAppearance FlatAppearance
		{
			get
			{
				return base.FlatAppearance;
			}
		}

		[Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), EditorBrowsable(EditorBrowsableState.Never)]
		public new FlatStyle FlatStyle
		{
			get
			{
				return base.FlatStyle;
			}
			set
			{
				base.FlatStyle = value;
			}
		}

		[Browsable(false), DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden), EditorBrowsable(EditorBrowsableState.Never)]
		public new bool UseVisualStyleBackColor
		{
			get
			{
				return base.UseVisualStyleBackColor;
			}
			set
			{
				base.UseVisualStyleBackColor = value;
			}
		}

		#endregion

		private class imageButtonControl : Control
		{
			protected override void OnPaintBackground(PaintEventArgs pevent) { }
			protected override void OnPaint(PaintEventArgs e) { }
		}
	}
}
