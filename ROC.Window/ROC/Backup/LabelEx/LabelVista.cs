using System;
using System.Drawing;
using System.Windows.Forms;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Collections.Generic;
using ShapeEx;

namespace LabelEx
{
	[System.ComponentModel.DesignerCategory("code")]
	public class LabelVista : LabelBase
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

		[Description("Event raised when the shine is applied or removed."), Category("Property Changed")]
		public event EventHandler ApplyShineChanged;

		[Description("Event raised when the value of the Shape property is changed."), Category("Property Changed")]
		public event EventHandler ShapeChanged;

		[Description("Event raised when the value of the Blink property is changed."), Category("Property Changed")]
		public event EventHandler BlinkChanged;

		[Description("Event raised when the value of the BlinkInnerBorderColor property is changed."), Category("Property Changed")]
		public event EventHandler BlinkInnerBorderColorChanged;

		[Description("Event raised when the value of the BlinkOuterBorderColor property is changed."), Category("Property Changed")]
		public event EventHandler BlinkOuterBorderColorChanged;

		[Description("Event raised when the value of the BlinkBackColorChanged property is changed."), Category("Property Changed")]
		public event EventHandler BlinkBackColorChanged;

		[Description("Event raised when the value of the BlinkForeColorChanged property is changed."), Category("Property Changed")]
		public event EventHandler BlinkForeColorChanged;

		[Description("Event raised when the value of the BlinkIntervalChanged value is changed."), Category("Property Changed")]
		public event EventHandler BlinkIntervalChanged;

		public new event PaintEventHandler Paint;

		#endregion

		#region - Type Headers -

		private sealed class Frames
		{
			public const int Disabled = 0;
			public const int Pressed = 1;
			public const int Normal = 2;
			public const int Blink = 3;
			public const int Animated = 4;
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

		public enum Shapes
		{
			RoundRectangle,
			LeftHalfPill,
			RightHalfPill,
			Pill,
		}

		#endregion

		#region - Label States -

		private States _labelStates;

		private Point _mousePoint;

		#endregion

		#region - Label Values -

		private bool _blink;
		private bool _animate;
		private bool _applyShine;
		private bool _fadeOnFocus;

		private int _radius;
		private Color _backColor;
		private Color _outerBorderColor;
		private Color _innerBorderColor;
		private Color _shineColor;
		private Color _glowColor;
		private Label _imageLabel;

		private Color _blinkBackColor;
		private Color _blinkForeColor;
		private Color _blinkOuterBorderColor;
		private Color _blinkInnerBorderColor;

		private Shapes _shape;

		private Areas _labelAreas;

		#endregion

		#region - Frame Values -

		private List<Image> _frames;

		#endregion

		#region - Animation Values -

		private const int _animationLength = 300;
		private const int _framesCount = 10;

		#region - Label -
		private bool _isAnimating
		{
			get
			{
				return _fadeDirection != 0;
			}
		}
		private int _currentFrame;
		private int _fadeDirection;
		private Timer _fadeTimer;

		private int _currentBlinkFrame;
		private Color _currentBlinkForeColor;
		private Timer _blinkTimer;
		#endregion

		#endregion

		#region - Blink Values -

		private int _blinkInterval;
		private bool _blinkOnceOnly;
		private bool _blinkOnChange;

		#endregion

		#region - Constructor -

		public LabelVista()
		{
			base.BackColor = Color.Transparent;

			_radius = 4;
			_backColor = Color.Black;
			ForeColor = Color.White;
			_outerBorderColor = Color.White;
			_innerBorderColor = Color.Black;
			_shineColor = Color.White;
			_glowColor = Color.FromArgb(141, 189, 255);
			_animate = false;
			_shape = Shapes.RoundRectangle;

			_blinkBackColor = Color.Red;
			_blinkForeColor = Color.White;
			_blinkOuterBorderColor = Color.White;
			_blinkInnerBorderColor = Color.Red;

			_blinkInterval = 250;
			_blinkOnceOnly = false;

			SetStyle(
				ControlStyles.AllPaintingInWmPaint |
				ControlStyles.CacheText | 
				ControlStyles.OptimizedDoubleBuffer | 
				ControlStyles.ResizeRedraw | 
				ControlStyles.UserPaint, true);
			SetStyle(ControlStyles.Opaque, false);

			_fadeTimer = new Timer();
			_fadeTimer.Interval = _animationLength / _framesCount;
			_fadeTimer.Tick += new EventHandler(fadeTimer_Tick);

			_blinkTimer = new Timer();
			_blinkTimer.Interval = _blinkInterval;
			_blinkTimer.Tick += new EventHandler(blinkTimer_Tick);

			_currentBlinkFrame = Frames.Normal;
			_currentBlinkForeColor = ForeColor;

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

			// Get the working areas of the label
			CreateLabel(_radius);

			if (_frames == null)
			{
				_frames = new List<Image>();
			}

			#region - State Frames -
			// FRAME_DISABLED
			_frames.Add(CreateBackgroundFrame(_labelAreas, false, false, false, false, false, 0));
			// FRAME_PRESSED
			_frames.Add(CreateBackgroundFrame(_labelAreas, false, true, true, false, true, 0));
			// FRAME_NORMAL
			_frames.Add(CreateBackgroundFrame(_labelAreas, false, false, false, false, true, 0));
			#endregion

			#region - BlinkFrames -
			// FRAME_BLINK
			_frames.Add(CreateBackgroundBlinkFrame(_labelAreas, false, false, false, false, true, 0));
			#endregion

			#region - Animation Frames -
			if (!withAnimationFrames)
			{
				return;
			}

			for (int i = 0; i < _framesCount; i++)
			{
				// Animated Frames 4 to 14
				_frames.Add(CreateBackgroundFrame(_labelAreas, false, false, true, true, true, (float)i / (_framesCount - 1F)));
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
		}

		#endregion

		#region - Area Methods -

		private void CreateLabel(float r)
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

			_labelAreas.Client = rect;
			#endregion

			#region - outter area white border -
			rect.Width--;
			rect.Height--;

			_labelAreas.Outter = rect;
			#endregion

			#region - shine and inner area -
			rect.X++;
			rect.Y++;
			rect.Width -= 2;
			rect.Height -= 2;
			Rectangle rectShine = rect;
			rectShine.Height >>= 1;
			Rectangle rectSplitShine = Rectangle.Empty;

			_labelAreas.Inner = rect;

			_labelAreas.Shine = rectShine;
			#endregion
		}

		#endregion

		#region - Draw Methods -

		private void DrawLabelBackgroundFromBuffer(Graphics graphics)
		{
			int frame;

			if (!Enabled)
			{
				frame = Frames.Disabled;
			}
			else if (_labelStates.isPressed)
			{
				frame = Frames.Pressed;
			}
			else if (!_isAnimating && _currentFrame == 0)
			{
				if (_blink || _blinkOnceOnly)
				{
					frame = _currentBlinkFrame;
				}
				else
				{
					frame = Frames.Normal;
				}
			}
			else
			{
				if (!hasAnimationFrames)
				{
					CreateFrames(true);
				}
				frame = Frames.Animated + _currentFrame;
			}

			if (_frames == null || _frames.Count == 0)
			{
				CreateFrames();
			}

			graphics.DrawImage(_frames[frame], _labelAreas.Client);
		}

		private void DrawForegroundFromLabel(PaintEventArgs pevent)
		{
			if (_imageLabel == null)
			{
				_imageLabel = new Label();
				_imageLabel.Parent = new imageLabelControl();
				_imageLabel.SuspendLayout();
				_imageLabel.BackColor = Color.Transparent;
				_imageLabel.BorderStyle = BorderStyle.None;
				_imageLabel.FlatStyle = FlatStyle.Flat;
			}
			else
			{
				_imageLabel.SuspendLayout();
			}
			_imageLabel.AutoEllipsis = AutoEllipsis;
			if (Enabled)
			{
				if (_blink || _blinkOnceOnly)
				{
					_imageLabel.ForeColor = _currentBlinkForeColor;
				}
				else
				{
					_imageLabel.ForeColor = ForeColor;
				}
			}
			else
			{
				_imageLabel.ForeColor = Color.FromArgb((3 * ForeColor.R + _backColor.R) >> 2,
					(3 * ForeColor.G + _backColor.G) >> 2,
					(3 * ForeColor.B + _backColor.B) >> 2);
			}
			_imageLabel.Font = Font;
			_imageLabel.RightToLeft = RightToLeft;
			if (_imageLabel.Image != null && _imageLabel.Image != Image)
			{
				_imageLabel.Image.Dispose();
			}
			if (Image != null)
			{
				_imageLabel.Image = Image;
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
					_imageLabel.Image = new Bitmap(Image.Width, Image.Height);
					using (Graphics gr = Graphics.FromImage(_imageLabel.Image))
					{
						gr.DrawImage(Image, new Rectangle(0, 0, size.Width, size.Height), 0, 0, size.Width, size.Height, GraphicsUnit.Pixel, disabledImageAttr);
					}
				}
			}
			_imageLabel.ImageAlign = ImageAlign;
			_imageLabel.ImageIndex = ImageIndex;
			_imageLabel.ImageKey = ImageKey;
			_imageLabel.ImageList = ImageList;
			_imageLabel.Padding = Padding;
			_imageLabel.Size = _labelAreas.Client.Size;
			_imageLabel.Text = Text;
			_imageLabel.TextAlign = TextAlign;
			_imageLabel.UseCompatibleTextRendering = UseCompatibleTextRendering;
			_imageLabel.UseMnemonic = UseMnemonic;
			_imageLabel.ResumeLayout();
			InvokePaint(_imageLabel, pevent);
		}

		private Image CreateBackgroundFrame(Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, float glowOpacity)
		{
			Image img = new Bitmap(areas.Client.Width, areas.Client.Height);
			using (Graphics g = Graphics.FromImage(img))
			{
				g.Clear(Color.Transparent);
				DrawBackground(g, areas, isSplit, pressed, hovered, animating, enabled, _outerBorderColor, _backColor, _glowColor, _shineColor, _innerBorderColor, glowOpacity);
			}

			return img;
		}

		private Image CreateBackgroundBlinkFrame(Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, float glowOpacity)
		{
			Image img = new Bitmap(areas.Client.Width, areas.Client.Height);
			using (Graphics g = Graphics.FromImage(img))
			{
				g.Clear(Color.Transparent);
				DrawBackground(g, areas, isSplit, pressed, hovered, animating, enabled, _blinkOuterBorderColor, _blinkBackColor, _glowColor, _shineColor, _blinkInnerBorderColor, glowOpacity);
			}

			return img;
		}

		private void DrawBackground(Graphics g, Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, Color outerBorderColor, Color backColor, Color glowColor, Color shineColor, Color innerBorderColor, float glowOpacity)
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
			if (_applyShine)
			{
				DrawShine(g, areas.Shine, shineColor, pressed, enabled);
			}
			DrawBlackBorder(g, areas.Inner, innerBorderColor);

			g.SmoothingMode = sm;
			g.InterpolationMode = im;
		}

		private void DrawWhiteBorder(Graphics g, Rectangle rect, Color outerBorderColor)
		{
			switch (_shape)
			{
				case Shapes.LeftHalfPill:
					using (GraphicsPath bw = ShapeHelper.GetLeftHalfPill(rect, _radius))
					{
						using (Pen p = new Pen(_outerBorderColor))
						{
							g.DrawPath(p, bw);
						}
					}
					break;
				case Shapes.RightHalfPill:
					using (GraphicsPath bw = ShapeHelper.GetRightHalfPill(rect, _radius))
					{
						using (Pen p = new Pen(outerBorderColor))
						{
							g.DrawPath(p, bw);
						}
					}
					break;
				case Shapes.Pill:
					using (GraphicsPath bw = ShapeHelper.GetFullPill(rect, _radius))
					{
						using (Pen p = new Pen(outerBorderColor))
						{
							g.DrawPath(p, bw);
						}
					}
					break;
				case Shapes.RoundRectangle:
				default:
					using (GraphicsPath bw = ShapeHelper.GetRoundRect(rect, _radius))
					{
						using (Pen p = new Pen(outerBorderColor))
						{
							g.DrawPath(p, bw);
						}
					}
					break;
			}
		}

		private void DrawContent(Graphics g, Rectangle rect, Color backColor, bool pressed)
		{
			switch (_shape)
			{
				case Shapes.LeftHalfPill:
					using (GraphicsPath bb = ShapeHelper.GetLeftHalfPill(rect, _radius - 2))
					{
						int opacity = pressed ? 0xcc : 0x7f;
						using (Brush br = new SolidBrush(Color.FromArgb(opacity, backColor)))
						{
							g.FillPath(br, bb);
						}
					}
					break;
				case Shapes.RightHalfPill:
					using (GraphicsPath bb = ShapeHelper.GetRightHalfPill(rect, _radius - 2))
					{
						int opacity = pressed ? 0xcc : 0x7f;
						using (Brush br = new SolidBrush(Color.FromArgb(opacity, backColor)))
						{
							g.FillPath(br, bb);
						}
					}
					break;
				case Shapes.Pill:
					using (GraphicsPath bb = ShapeHelper.GetFullPill(RectangleF.Inflate(rect, 0F, -1F), _radius - 2))
					{
						int opacity = pressed ? 0xcc : 0x7f;
						using (Brush br = new SolidBrush(Color.FromArgb(opacity, backColor)))
						{
							g.FillPath(br, bb);
						}
					}
					break;
				case Shapes.RoundRectangle:
				default:
					using (GraphicsPath bb = ShapeHelper.GetRoundRect(rect, _radius - 2))
					{
						int opacity = pressed ? 0xcc : 0x7f;
						using (Brush br = new SolidBrush(Color.FromArgb(opacity, backColor)))
						{
							g.FillPath(br, bb);
						}
					}
					break;
			}
		}

		private void DrawGlow(Graphics g, Rectangle rect, Color glowColor, float glowOpacity)
		{
			switch (_shape)
			{
				case Shapes.LeftHalfPill:
					using (GraphicsPath clip = ShapeHelper.GetLeftHalfPill(rect, _radius - 2))
					{
						g.SetClip(clip, CombineMode.Intersect);
						using (GraphicsPath brad = ShapeHelper.GetLeftRadialPath(rect))
						{
							using (PathGradientBrush pgr = new PathGradientBrush(brad))
							{
								unchecked
								{
									int opacity = (int)(0xB2 * glowOpacity + .5f);
									RectangleF bounds = brad.GetBounds();
									pgr.CenterPoint = new PointF((bounds.Left + bounds.Right) * 2f, (bounds.Top + bounds.Bottom) / 2f);
									pgr.CenterColor = Color.FromArgb(opacity, glowColor);
									pgr.SurroundColors = new Color[] { Color.FromArgb(0, glowColor) };
								}
								g.FillPath(pgr, brad);
							}
						}
						g.ResetClip();
					}
					break;
				case Shapes.RightHalfPill:
					using (GraphicsPath clip = ShapeHelper.GetRightHalfPill(rect, _radius - 2))
					{
						g.SetClip(clip, CombineMode.Intersect);
						using (GraphicsPath brad = ShapeHelper.GetRightRadialPath(rect))
						{
							using (PathGradientBrush pgr = new PathGradientBrush(brad))
							{
								unchecked
								{
									int opacity = (int)(0xB2 * glowOpacity + .5f);
									RectangleF bounds = brad.GetBounds();
									pgr.CenterPoint = new PointF((bounds.Left + bounds.Right) * 2f, (bounds.Top + bounds.Bottom) / 2f);
									pgr.CenterColor = Color.FromArgb(opacity, _glowColor);
									pgr.SurroundColors = new Color[] { Color.FromArgb(0, _glowColor) };
								}
								g.FillPath(pgr, brad);
							}
						}
						g.ResetClip();
					}
					break;
				case Shapes.Pill:
					using (GraphicsPath clip = ShapeHelper.GetFullPill(rect, _radius - 2))
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
									pgr.CenterColor = Color.FromArgb(opacity, _glowColor);
									pgr.SurroundColors = new Color[] { Color.FromArgb(0, _glowColor) };
								}
								g.FillPath(pgr, brad);
							}
						}
						g.ResetClip();
					}
					break;
				case Shapes.RoundRectangle:
				default:
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
									pgr.CenterColor = Color.FromArgb(opacity, _glowColor);
									pgr.SurroundColors = new Color[] { Color.FromArgb(0, _glowColor) };
								}
								g.FillPath(pgr, brad);
							}
						}
						g.ResetClip();
					}
					break;
			}
		}

		private void DrawShine(Graphics g, Rectangle rect, Color shineColor, bool pressed, bool enabled)
		{
			if (rect.Width > 0 && rect.Height > 0)
			{
				switch (_shape)
				{
					case Shapes.LeftHalfPill:
						using (GraphicsPath bh = ShapeHelper.GetTopLeftHalfPill(rect, _radius - 2))
						{
							int opacity = 0x99;
							if (pressed | !enabled)
							{
								opacity = (int)(.4f * opacity + .5f);
							}
							using (LinearGradientBrush br = new LinearGradientBrush(rect, Color.FromArgb(opacity, shineColor), Color.FromArgb(opacity / 3, _shineColor), LinearGradientMode.Vertical))
							{
								g.FillPath(br, bh);
							}
						}
						break;
					case Shapes.RightHalfPill:
						using (GraphicsPath bh = ShapeHelper.GetTopRightHalfPill(rect, _radius - 2))
						{
							int opacity = 0x99;
							if (pressed | !enabled)
							{
								opacity = (int)(.4f * opacity + .5f);
							}
							using (LinearGradientBrush br = new LinearGradientBrush(rect, Color.FromArgb(opacity, shineColor), Color.FromArgb(opacity / 3, _shineColor), LinearGradientMode.Vertical))
							{
								g.FillPath(br, bh);
							}
						}
						break;
					case Shapes.Pill:
						using (GraphicsPath bh = ShapeHelper.GetTopFullPill(rect, _radius - 2))
						{
							int opacity = 0x99;
							if (pressed | !enabled)
							{
								opacity = (int)(.4f * opacity + .5f);
							}
							using (LinearGradientBrush br = new LinearGradientBrush(rect, Color.FromArgb(opacity, shineColor), Color.FromArgb(opacity / 3, _shineColor), LinearGradientMode.Vertical))
							{
								g.FillPath(br, bh);
							}
						}
						break;
					case Shapes.RoundRectangle:
					default:
						rect.Height += 2;
						using (GraphicsPath bh = ShapeHelper.GetTopRoundRect(rect, _radius - 2))
						{
							int opacity = 0x99;
							if (pressed | !enabled)
							{
								opacity = (int)(.4f * opacity + .5f);
							}
							using (LinearGradientBrush br = new LinearGradientBrush(rect, Color.FromArgb(opacity, shineColor), Color.FromArgb(opacity / 3, _shineColor), LinearGradientMode.Vertical))
							{
								g.FillPath(br, bh);
							}
						}
						rect.Height -= 2;
						break;
				}
			}
		}

		private void DrawBlackBorder(Graphics g, Rectangle rect, Color innerBorderColor)
		{
			switch (_shape)
			{
				case Shapes.LeftHalfPill:
					using (GraphicsPath bb = ShapeHelper.GetLeftHalfPill(rect, _radius - 1))
					{
						using (Pen p = new Pen(innerBorderColor))
						{
							g.DrawPath(p, bb);
						}
					}
					break;
				case Shapes.RightHalfPill:
					using (GraphicsPath bb = ShapeHelper.GetRightHalfPill(rect, _radius - 1))
					{
						using (Pen p = new Pen(innerBorderColor))
						{
							g.DrawPath(p, bb);
						}
					}
					break;
				case Shapes.Pill:
					using (GraphicsPath bb = ShapeHelper.GetFullPill(RectangleF.Inflate(rect, 0F, -1F), _radius - 1))
					{
						using (Pen p = new Pen(innerBorderColor))
						{
							g.DrawPath(p, bb);
						}
					}
					break;
				case Shapes.RoundRectangle:
				default:
					using (GraphicsPath bb = ShapeHelper.GetRoundRect(rect, _radius - 1))
					{
						using (Pen p = new Pen(innerBorderColor))
						{
							g.DrawPath(p, bb);
						}
					}
					break;
			}
		}

		#endregion

		#region - Control Events -

		protected override void OnPaint(PaintEventArgs e)
		{
			DrawLabelBackgroundFromBuffer(e.Graphics);
			DrawForegroundFromLabel(e);

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

		protected override void OnTextChanged(EventArgs e)
		{
			base.OnTextChanged(e);

			if (_blinkOnChange)
			{
				BlinkOnce();
			}
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

		protected virtual void OnApplyShineChanged(EventArgs e)
		{
			if (ApplyShineChanged != null)
			{
				ApplyShineChanged(this, e);
			}
		}

		protected virtual void OnGlowColorChanged(EventArgs e)
		{
			if (GlowColorChanged != null)
			{
				GlowColorChanged(this, e);
			}
		}

		protected virtual void OnShapeChanged(EventArgs e)
		{
			if (ShapeChanged != null)
			{
				ShapeChanged(this, e);
			}
		}

		protected virtual void OnBlinkChanged(EventArgs e)
		{
			if (BlinkChanged != null)
			{
				BlinkChanged(this, e);
			}
		}

		protected virtual void OnBlinkInnerBorderColorChanged(EventArgs e)
		{
			if (BlinkInnerBorderColorChanged != null)
			{
				BlinkInnerBorderColorChanged(this, e);
			}
		}

		protected virtual void OnBlinkOuterBorderColorChanged(EventArgs e)
		{
			if (BlinkOuterBorderColorChanged != null)
			{
				BlinkOuterBorderColorChanged(this, e);
			}
		}

		protected virtual void OnBlinkBackColorChanged(EventArgs e)
		{
			if (BlinkBackColorChanged != null)
			{
				BlinkBackColorChanged(this, e);
			}
		}

		protected virtual void OnBlinkForeColorChanged(EventArgs e)
		{
			if (BlinkForeColorChanged != null)
			{
				BlinkForeColorChanged(this, e);
			}
		}

		protected virtual void OnBlinkIntervalChanged(EventArgs e)
		{
			if (BlinkIntervalChanged != null)
			{
				BlinkIntervalChanged(this, e);
			}
		}

		#endregion

		#region - Mouse and Keyboard Events -

		protected override void OnClick(EventArgs e)
		{
			_labelStates.isKeyDown = _labelStates.isMouseDown = false;
			base.OnClick(e);
		}

		protected override void OnEnter(EventArgs e)
		{
			_labelStates.isFocused = _labelStates.isFocusedByKey = true;
			base.OnEnter(e);
			if (_fadeOnFocus && _animate)
			{
				fadeIn();
			}
		}

		protected override void OnLeave(EventArgs e)
		{
			base.OnLeave(e);

			_labelStates.isFocused = _labelStates.isFocusedByKey = _labelStates.isKeyDown = _labelStates.isMouseDown = false;

			Invalidate();

			if (_fadeOnFocus && _animate)
			{
				fadeOut();
			}
		}

		protected override void OnKeyDown(KeyEventArgs e)
		{
			if (e.KeyCode == Keys.Enter)
			{
				_labelStates.isKeyDown = true;
				Invalidate(_labelAreas.Client);
			}
			base.OnKeyDown(e);
		}

		protected override void OnKeyUp(KeyEventArgs e)
		{
			if (_labelStates.isKeyDown && e.KeyCode == Keys.Enter)
			{
				_labelStates.isKeyDown = false;
				Invalidate(_labelAreas.Client);
			}
			base.OnKeyUp(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if (!_labelStates.isMouseDown && e.Button == MouseButtons.Left)
			{
				_labelStates.isMouseDown = true;
				_labelStates.isFocusedByKey = false;
				Invalidate(_labelAreas.Client);
				base.OnMouseDown(e);
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			_labelStates.isMouseDown = false;
			Invalidate(_labelAreas.Client);
			base.OnMouseUp(e);
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (e.Button != MouseButtons.None)
			{
				if (_labelStates.isHovered)
				{
					_labelStates.isHovered = false;
				}
				else
				{
					_labelStates.isHovered = true;
				}
			}

			if (_labelStates.isHovered)
			{
				_labelStates.isHovered = false;
				if (_animate)
				{
					fadeOut();
				}
				Invalidate(_labelAreas.Client);
			}

			_labelStates.isHovered = true;
			if (_animate)
			{
				fadeIn();
			}
			Invalidate(_labelAreas.Client);
		}

		protected override void OnMouseEnter(EventArgs e)
		{
			_labelStates.isHovered = true;
			if (_animate)
			{
				fadeIn();
			}
			Invalidate(_labelAreas.Client);
			base.OnMouseEnter(e);
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			if (_labelStates.isHovered)
			{
				_labelStates.isMouseDown = false;
				_labelStates.isHovered = false;
				if (_animate)
				{
					fadeOut();
				}
				Invalidate(_labelAreas.Client);
			}

			base.OnMouseLeave(e);
		}

		#endregion

		#region - Mouse Capture -

		private void ConvertToLocalMousePoint()
		{
			_mousePoint = PointToClient(Cursor.Position);
		}

		#endregion

		#region - Animation -

		private void fadeIn()
		{
			_fadeDirection = 1;
			_fadeTimer.Enabled = true;
		}

		private void fadeOut()
		{
			_fadeDirection = -1;
			_fadeTimer.Enabled = true;
		}

		private void fadeTimer_Tick(object sender, EventArgs e)
		{
			if (!_fadeTimer.Enabled)
			{
				return;
			}
			Refresh();
			_currentFrame += _fadeDirection;
			if (_currentFrame == -1)
			{
				_currentFrame = 0;
				_fadeTimer.Enabled = false;
				_fadeDirection = 0;
				return;
			}
			if (_currentFrame == _framesCount)
			{
				_currentFrame = _framesCount - 1;
				_fadeTimer.Enabled = false;
				_fadeDirection = 0;
			}
		}

		private bool hasAnimationFrames
		{
			get
			{
				return _frames != null && _frames.Count > Frames.Animated;
			}
		}

		#endregion

		#region - Blink -

		private void blinkTimer_Tick(object sender, EventArgs e)
		{
			if (!_blinkTimer.Enabled)
			{
				return;
			}
			if (_currentBlinkFrame == Frames.Normal)
			{
				_currentBlinkFrame = Frames.Blink;
				_currentBlinkForeColor = _blinkForeColor;
			}
			else
			{
				_currentBlinkFrame = Frames.Normal;
				_currentBlinkForeColor = ForeColor;
				if (_blinkOnceOnly)
				{
					_blinkOnceOnly = false;
					_blinkTimer.Enabled = false;
				}
			}

			if (!_blink && !_blinkOnceOnly)
			{
				_blinkTimer.Enabled = false;
			}

			if (_blinkTimer.Interval != _blinkInterval)
			{
				_blinkTimer.Interval = _blinkInterval;
			}

			Refresh();
		}

		public void BlinkOnce()
		{
			BlinkOnce(_blinkInterval);
		}

		public void BlinkOnce(int interval)
		{
			_blinkInterval = interval;
			if (_blinkTimer.Interval != _blinkInterval)
			{
				_blinkTimer.Interval = _blinkInterval;
			}

			_currentBlinkFrame = Frames.Blink;
			_currentBlinkForeColor = _blinkForeColor;

			_blinkTimer.Enabled = true;
			_blinkOnceOnly = true;

			Refresh();
		}

		#endregion

		#region - Properties -

		[DefaultValue(typeof(Color), "Black")]
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
					CreateFrames();
					OnBackColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Boolean), "False")]
		public virtual new bool AutoSize
		{
			get
			{
				return base.AutoSize;
			}
			set
			{
				base.AutoSize = value;
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
				base.ForeColor = value;
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

		[DefaultValue(typeof(Boolean), "False"), Category("Appearance"), Description("Apply shine color to control or not.")]
		public virtual bool ApplyShine
		{
			get
			{
				return _applyShine;
			}
			set
			{
				if (_applyShine != value)
				{
					_applyShine = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnApplyShineChanged(EventArgs.Empty);
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
					_fadeTimer.Enabled = _fadeOnFocus;
				}
			}
		}

		[DefaultValue(typeof(Boolean), "False"), Category("Appearance"), Description("Indicates whether the control should fade in and fade out when mouse over.")]
		public virtual bool Animate
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
				}
			}
		}

		[DefaultValue(typeof(LabelEx.LabelVista.Shapes), "RoundRectangle"), Category("Appearance"), Description("Indicates whether the control shape.")]
		public virtual Shapes Shape
		{
			get
			{
				return _shape;
			}
			set
			{
				if (_shape != value)
				{
					_shape = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnShapeChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Boolean), "False"), Category("Appearance"), Description("Indicates whether the control should flash or not")]
		public virtual bool Blink
		{
			get
			{
				return _blink;
			}
			set
			{
				if (_blink != value)
				{
					_blink = value;
					_blinkTimer.Enabled = _blink;
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnBlinkChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "Red"), Category("Appearance"), Description("The inner border color of the control when blinking.")]
		public virtual Color BlinkInnerBorderColor
		{
			get
			{
				return _blinkInnerBorderColor;
			}
			set
			{
				if (_blinkInnerBorderColor != value)
				{
					_blinkInnerBorderColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnBlinkInnerBorderColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "White"), Category("Appearance"), Description("The outer border color of the control when blinking.")]
		public virtual Color BlinkOuterBorderColor
		{
			get
			{
				return _blinkOuterBorderColor;
			}
			set
			{
				if (_blinkOuterBorderColor != value)
				{
					_blinkOuterBorderColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnBlinkOuterBorderColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "Red"), Category("Appearance"), Description("The back color of the control when blinking.")]
		public virtual Color BlinkBackColor
		{
			get
			{
				return _blinkBackColor;
			}
			set
			{
				if (_blinkBackColor != value)
				{
					_blinkBackColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnBlinkBackColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Color), "White"), Category("Appearance"), Description("The fore color of the control when blinking.")]
		public virtual Color BlinkForeColorColor
		{
			get
			{
				return _blinkForeColor;
			}
			set
			{
				if (_blinkForeColor != value)
				{
					_blinkForeColor = value;
					CreateFrames();
					if (IsHandleCreated)
					{
						Invalidate();
					}
					OnBlinkForeColorChanged(EventArgs.Empty);
				}
			}
		}

		[DefaultValue(typeof(Int32), "250"), Category("Appearance"), Description("The Blink rate")]
		public virtual int BlinkInterval
		{
			get
			{
				return _blinkInterval;
			}
			set
			{
				if (_blinkInterval != value)
				{
					_blinkInterval = value;
				}
				OnBlinkIntervalChanged(EventArgs.Empty);
			}
		}

		[DefaultValue(typeof(Boolean), "False"), Category("Appearance"), Description("Blink on control value change.")]
		public virtual bool BlinkOnValueChange
		{
			get
			{
				return _blinkOnChange;
			}
			set
			{
				if (_blinkOnChange != value)
				{
					_blinkOnChange = value;
				}
			}
		}

		#endregion

		private class imageLabelControl : Control
		{
			protected override void OnPaintBackground(PaintEventArgs pevent) { }
			protected override void OnPaint(PaintEventArgs e) { }
		}
	}
}
