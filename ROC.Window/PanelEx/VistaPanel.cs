using System;
using System.Windows.Forms;
using System.ComponentModel;
using System.Collections.Generic;
using System.Drawing;
using System.Drawing.Drawing2D;

using ShapeEx;

namespace PanelEx
{
	[System.ComponentModel.DesignerCategory("code")]
	public class VistaPanel : Panel
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

		public new event PaintEventHandler Paint;

		#endregion

		#region - Type Headers -

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

		public enum Shapes
		{
			RoundRectangle,
			LeftHalfPill,
			RightHalfPill,
			Pill,
		}

		#endregion

		#region - Panel States -

		private States _panelStates;

		private Point _mousePoint;

		#endregion

		#region - Panel Values -

		private bool _animate;
		private bool _animateOnClick;
		private bool _applyShine;
		private bool _fadeOnFocus;

		private int _radius;
		private Color _backColor;
		private Color _outerBorderColor;
		private Color _innerBorderColor;
		private Color _shineColor;
		private Color _glowColor;

		private Shapes _shape;

		private Areas _panelAreas;

		#endregion

		#region - Frame Values -

		private List<Image> _frames;

		#endregion

		#region - Animation Values -

		private const int _animationLength = 300;
		private const int _framesCount = 10;

		#region - Panel -
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
		#endregion

		#endregion

		#region - Constructor -

		public VistaPanel()
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
			_animateOnClick = false;
			_shape = Shapes.RoundRectangle;

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

			// Get the working areas of the panel
			CreatePanel(_radius);

			if (_frames == null)
			{
				_frames = new List<Image>();
			}

			#region - State Frames -
			// FRAME_DISABLED
			_frames.Add(CreateBackgroundFrame(_panelAreas, false, false, false, false, false, 0));
			// FRAME_PRESSED
			_frames.Add(CreateBackgroundFrame(_panelAreas, false, true, true, false, true, 0));
			// FRAME_NORMAL
			_frames.Add(CreateBackgroundFrame(_panelAreas, false, false, false, false, true, 0));
			#endregion

			#region - Animation Frames -
			if (!withAnimationFrames)
			{
				return;
			}

			for (int i = 0; i < _framesCount; i++)
			{
				// Animated Frames 3 to 13
				_frames.Add(CreateBackgroundFrame(_panelAreas, false, false, true, true, true, (float)i / (_framesCount - 1F)));
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

		private void CreatePanel(float r)
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

			_panelAreas.Client = rect;
			#endregion

			#region - outter area white border -
			rect.Width--;
			rect.Height--;

			_panelAreas.Outter = rect;
			#endregion

			#region - shine and inner area -
			rect.X++;
			rect.Y++;
			rect.Width -= 2;
			rect.Height -= 2;
			Rectangle rectShine = rect;
			rectShine.Height >>= 1;
			Rectangle rectSplitShine = Rectangle.Empty;

			_panelAreas.Inner = rect;

			_panelAreas.Shine = rectShine;
			#endregion
		}

		#endregion

		#region - Draw Methods -

		private void DrawPanelBackgroundFromBuffer(Graphics graphics)
		{
			int frame;

			if (!Enabled)
			{
				frame = Frames.Disabled;
			}
			else if (_panelStates.isPressed)
			{
				frame = Frames.Pressed;
			}
			else if (!_isAnimating && _currentFrame == 0)
			{
				frame = Frames.Normal;
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

			graphics.DrawImage(_frames[frame], _panelAreas.Client);
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
						using (Pen p = new Pen(outerBorderColor))
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
									pgr.CenterColor = Color.FromArgb(opacity, glowColor);
									pgr.SurroundColors = new Color[] { Color.FromArgb(0, glowColor) };
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
									pgr.CenterColor = Color.FromArgb(opacity, glowColor);
									pgr.SurroundColors = new Color[] { Color.FromArgb(0, glowColor) };
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
									pgr.CenterColor = Color.FromArgb(opacity, glowColor);
									pgr.SurroundColors = new Color[] { Color.FromArgb(0, glowColor) };
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
			DrawPanelBackgroundFromBuffer(e.Graphics);
			//DrawForegroundFromButton(e);
			//DrawButtonForeground(e.Graphics);

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

		#endregion

		#region - Mouse and Keyboard Events -

		protected override void OnClick(EventArgs e)
		{
			_panelStates.isKeyDown = _panelStates.isMouseDown = false;
			base.OnClick(e);
		}

		protected override void OnEnter(EventArgs e)
		{
			_panelStates.isFocused = _panelStates.isFocusedByKey = true;
			base.OnEnter(e);
			if (_fadeOnFocus && _animate)
			{
				fadeIn();
			}
		}

		protected override void OnLeave(EventArgs e)
		{
			base.OnLeave(e);

			_panelStates.isFocused = _panelStates.isFocusedByKey = _panelStates.isKeyDown = _panelStates.isMouseDown = false;

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
				_panelStates.isKeyDown = true;
				Invalidate(_panelAreas.Client);
			}
			base.OnKeyDown(e);
		}

		protected override void OnKeyUp(KeyEventArgs e)
		{
			if (_panelStates.isKeyDown && e.KeyCode == Keys.Enter)
			{
				_panelStates.isKeyDown = false;
				Invalidate(_panelAreas.Client);
			}
			base.OnKeyUp(e);
		}

		protected override void OnMouseDown(MouseEventArgs e)
		{
			if (!_panelStates.isMouseDown && e.Button == MouseButtons.Left)
			{
				_panelStates.isMouseDown = true;
				_panelStates.isFocusedByKey = false;
				if (AnimateOnClick)
				{
					Invalidate(_panelAreas.Client);
				}
				base.OnMouseDown(e);
			}
		}

		protected override void OnMouseUp(MouseEventArgs e)
		{
			_panelStates.isMouseDown = false;
			if (AnimateOnClick)
			{
				Invalidate(_panelAreas.Client);
			}
			base.OnMouseUp(e);
		}

		protected override void OnMouseMove(MouseEventArgs e)
		{
			base.OnMouseMove(e);

			if (e.Button != MouseButtons.None)
			{
				if (_panelStates.isHovered)
				{
					_panelStates.isHovered = false;
				}
				else
				{
					_panelStates.isHovered = true;
				}
			}

			if (_panelStates.isHovered)
			{
				_panelStates.isHovered = false;
				if (_animate)
				{
					fadeOut();
					Invalidate(_panelAreas.Client);
				}
				if (AnimateOnClick)
				{
					Invalidate(_panelAreas.Client);
				}
			}

			_panelStates.isHovered = true;
			if (_animate)
			{
				fadeIn();
				Invalidate(_panelAreas.Client);
			}
			if (AnimateOnClick)
			{
				Invalidate(_panelAreas.Client);
			}
		}

		protected override void OnMouseEnter(EventArgs e)
		{
			_panelStates.isHovered = true;
			if (_animate)
			{
				fadeIn();
			}
			Invalidate(_panelAreas.Client);
			base.OnMouseEnter(e);
		}

		protected override void OnMouseLeave(EventArgs e)
		{
			if (_panelStates.isHovered)
			{
				_panelStates.isHovered = false;
				if (_animate)
				{
					fadeOut();
				}
				Invalidate(_panelAreas.Client);
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

		#region - Properties -

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
		[DefaultValue(typeof(Boolean), "False"), Category("Appearance"), Description("Indicates whether the control should change state on mouse click.")]
		public virtual bool AnimateOnClick
		{
			get
			{
				return _animateOnClick;
			}
			set
			{
				if (_animateOnClick != value)
				{
					_animateOnClick = value;
				}
			}
		}

		[DefaultValue(typeof(PanelEx.VistaPanel.Shapes), "RoundRectangle"), Category("Appearance"), Description("Indicates whether the control shape.")]
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

		#endregion
	}
}
