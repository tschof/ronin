using System;
using System.Windows.Forms;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.ComponentModel;
using System.Collections.Generic;
using System.Collections;
using ShapeEx;

namespace ContextMenuEx
{
	internal class VistaContextMenuRenderer : ToolStripProfessionalRenderer
	{
		#region - Type Headers -

		public int FRAME_Normal { get { return 0; } }
		public int FRAME_Selected { get { return 1; } }

		public int IMAGE_OFFSET { get { return 26; } }

		private struct Areas
		{
			public Rectangle Client;
			public Rectangle Outter;
			public Rectangle Inner;
			public Rectangle Shine;
		}

		#endregion

		#region - Renderer Values -

		private int _radius;
		private Color _backColor;
		private Color _foreColor;
		private Color _outerBorderColor;
		private Color _innerBorderColor;
		private Color _shineColor;
		private Color _glowColor;

		#endregion

		#region - Area Values -

		private Hashtable _areasByItem;
		private Areas _itemAreas;

		#endregion

		#region - Frame Values -

		private Hashtable _framesByItem;
		private List<Image> _frames;

		#endregion

		#region - Constructor -

		public VistaContextMenuRenderer()
		{
			InitializeHelper(4, Color.Black, Color.White, Color.White, Color.Black, Color.White, Color.FromArgb(255, 141, 189, 255));
		}

		public VistaContextMenuRenderer(int r, Color back, Color fore, Color outer, Color inner, Color shine, Color glow)
		{
			InitializeHelper(r, back, fore, outer, inner, shine, glow);
		}

		#endregion

		#region - Constructor Method -

		private void InitializeHelper(int r, Color back, Color fore, Color outer, Color inner, Color shine, Color glow)
		{
			_radius = r;
			_backColor = back;
			_foreColor = fore;
			_outerBorderColor = outer;
			_innerBorderColor = inner;
			_shineColor = shine;
			_glowColor = glow;

			_areasByItem = new Hashtable();
			_framesByItem = new Hashtable();
		}

		#endregion

		#region - Frame Method -

		private void CreateFrames()
		{
			CreateFrames(false);
		}

		private void CreateFrames(bool withAnimationFrames)
		{
			DestroyFrames();

			// FRAME_NORMAL
			_frames.Add(CreateBackgroundFrame(_itemAreas, false, false, false, false, true, 0));

			// FRAME_PRESSED
			_frames.Add(CreateBackgroundFrame(_itemAreas, false, false, true, true, true, 1));
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

		private void CreateItemAreas(Rectangle itemRect, float r)
		{
			Rectangle rect = itemRect;

			#region - client area -
			if (rect.Width <= 0)
			{
				rect.Width = 1;
			}
			if (rect.Height <= 0)
			{
				rect.Height = 1;
			}

			rect.X = IMAGE_OFFSET;
			rect.Width -= IMAGE_OFFSET;

			_itemAreas.Client = rect;
			#endregion

			#region - outter area white border -
			rect.X = 0;

			rect.Width--;
			rect.Height--;

			_itemAreas.Outter = rect;
			#endregion

			#region - shine and inner area -
			rect.X++;
			rect.Y++;
			rect.Width -= 2;
			rect.Height -= 2;
			Rectangle rectShine = rect;
			rectShine.Height >>= 1;

			_itemAreas.Inner = rect;
			_itemAreas.Shine = rectShine;
			#endregion
		}

		#endregion

		#region - Draw Method -

		private Image CreateBackgroundFrame(Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, float glowOpacity)
		{
			Image img = new Bitmap(areas.Client.Width, areas.Client.Height);
			using (Graphics g = Graphics.FromImage(img))
			{
				g.Clear(Color.Transparent);
				DrawItemBackground(g, areas, isSplit, pressed, hovered, animating, enabled, _outerBorderColor, _backColor, _glowColor, _shineColor, _innerBorderColor, glowOpacity);
			}

			return img;
		}

		private void DrawMarginBackground(Graphics g, Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, Color outerBorderColor, Color backColor, Color glowColor, Color shineColor, Color innerBorderColor, float glowOpacity)
		{
			SmoothingMode sm = g.SmoothingMode;
			InterpolationMode im = g.InterpolationMode;

			g.SmoothingMode = SmoothingMode.AntiAlias;
			g.InterpolationMode = InterpolationMode.HighQualityBicubic;

			DrawWhiteBorder(g, areas.Outter);
			DrawContent(g, areas.Inner, pressed);
			if ((hovered || animating) && !pressed)
			{
				DrawGlow(g, areas.Inner, glowOpacity);
			}
			DrawShine(g, areas.Shine, pressed, enabled);
			DrawBlackBorder(g, areas.Inner);

			g.SmoothingMode = sm;
			g.InterpolationMode = im;
		}

		private void DrawItemBackground(Graphics g, Areas areas, bool isSplit, bool pressed, bool hovered, bool animating, bool enabled, Color outerBorderColor, Color backColor, Color glowColor, Color shineColor, Color innerBorderColor, float glowOpacity)
		{
			SmoothingMode sm = g.SmoothingMode;
			InterpolationMode im = g.InterpolationMode;

			g.SmoothingMode = SmoothingMode.AntiAlias;
			g.InterpolationMode = InterpolationMode.HighQualityBicubic;

			DrawWhiteBorder(g, areas.Outter);
			DrawContent(g, areas.Inner, pressed);
			if ((hovered || animating) && !pressed)
			{
				DrawGlow(g, areas.Inner, glowOpacity);
			}
			DrawShine(g, areas.Shine, pressed, enabled);
			DrawBlackBorder(g, areas.Inner);

			g.SmoothingMode = sm;
			g.InterpolationMode = im;
		}

		private void DrawWhiteBorder(Graphics g, Rectangle rect)
		{
			using (GraphicsPath bw = ShapeHelper.GetRoundRect(rect, _radius))
			{
				using (Pen p = new Pen(_outerBorderColor))
				{
					g.DrawPath(p, bw);
				}
			}
		}

		private void DrawContent(Graphics g, Rectangle rect, bool pressed)
		{
			using (GraphicsPath bb = ShapeHelper.GetRoundRect(rect, _radius - 2))
			{
				int opacity = pressed ? 0xcc : 0x7f;
				using (Brush br = new SolidBrush(Color.FromArgb(opacity, _backColor)))
				{
					g.FillPath(br, bb);
				}
			}
		}

		private void DrawGlow(Graphics g, Rectangle rect, float glowOpacity)
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
							pgr.CenterColor = Color.FromArgb(opacity, _glowColor);
							pgr.SurroundColors = new Color[] { Color.FromArgb(0, _glowColor) };
						}
						g.FillPath(pgr, brad);
					}
				}
				g.ResetClip();
			}
		}

		private void DrawShine(Graphics g, Rectangle rect, bool pressed, bool enabled)
		{
			if (rect.Width > 0 && rect.Height > 0)
			{
				rect.Height++;
				using (GraphicsPath bh = ShapeHelper.GetTopRoundRect(rect, _radius - 2))
				{
					rect.Height++;
					int opacity = 0x99;
					if (pressed | !enabled)
					{
						opacity = (int)(.4f * opacity + .5f);
					}
					using (LinearGradientBrush br = new LinearGradientBrush(rect, Color.FromArgb(opacity, _shineColor), Color.FromArgb(opacity / 3, _shineColor), LinearGradientMode.Vertical))
					{
						g.FillPath(br, bh);
					}
				}
				rect.Height -= 2;
			}
		}

		private void DrawBlackBorder(Graphics g, Rectangle rect)
		{
			using (GraphicsPath bb = ShapeHelper.GetRoundRect(rect, _radius - 1))
			{
				using (Pen p = new Pen(_innerBorderColor))
				{
					g.DrawPath(p, bb);
				}
			}
		}

		#endregion

		#region - Control Events -

		protected override void OnRenderMenuItemBackground(ToolStripItemRenderEventArgs e)
		{
			string key = e.Item.Size.ToString();

			#region - Areas -
			if (_areasByItem.ContainsKey(key))
			{
				_itemAreas = (Areas)_areasByItem[key];
			}
			else
			{
				Rectangle rect = new Rectangle(new Point(0, 0), e.Item.Size);
				CreateItemAreas(rect, _radius);

				_areasByItem.Add(key, _itemAreas);
			}
			#endregion

			#region - Frames -
			if (_framesByItem.ContainsKey(key))
			{
				_frames = (List<Image>)_framesByItem[key];
			}
			else
			{
				_frames = new List<Image>();
				CreateFrames();

				_framesByItem.Add(key, _frames);
			}
			#endregion

			int frame;
			if (e.Item.Selected)
			{
				frame = FRAME_Selected;
			}
			else
			{
				frame = FRAME_Normal;
			}

			e.Graphics.DrawImage(_frames[frame], _itemAreas.Client);
		}

		protected override void OnRenderItemText(ToolStripItemTextRenderEventArgs e)
		{
			e.TextColor = _foreColor;
			base.OnRenderItemText(e);
		}

		protected override void OnRenderArrow(ToolStripArrowRenderEventArgs e)
		{
			e.ArrowColor = _foreColor;
			base.OnRenderArrow(e);
		}

		protected override void OnRenderToolStripBackground(ToolStripRenderEventArgs e)
		{
			//base.OnRenderToolStripBackground(e);
		}

		protected override void OnRenderToolStripBorder(ToolStripRenderEventArgs e)
		{
			// base.OnRenderToolStripBorder(e);
		}

		#endregion

		#region - Properties -

		public Color BackColor
		{
			get
			{
				return _backColor;
			}
			set
			{
				_backColor = value;
			}
		}

		public Color ForeColor
		{
			get
			{
				return _foreColor;
			}
			set
			{
				_foreColor = value;
			}
		}

		public Color OuterBorderColor
		{
			get
			{
				return _outerBorderColor;
			}
			set
			{
				_outerBorderColor = value;
			}
		}

		public Color InnerBorderColor
		{
			get
			{
				return _innerBorderColor;
			}
			set
			{
				_innerBorderColor = value;
			}
		}

		public Color ShineColor
		{
			get
			{
				return _shineColor;
			}
			set
			{
				_shineColor = value;
			}
		}

		public Color GlowColor
		{
			get
			{
				return _glowColor;
			}
			set
			{
				_glowColor = value;
			}
		}

		#endregion
	}
}
