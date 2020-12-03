using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace ShapeEx
{
	public sealed class ShapeHelper
	{
		#region - Round Rect -

		public static GraphicsPath GetRoundRect(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X, y = rect.Y, w = rect.Width, h = rect.Height;

			if (w < h)
			{
				if ((w / 3) < r)
				{
					r = w / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}
			else
			{
				if ((h / 3) < r)
				{
					r = h / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}

			p.StartFigure();
			p.AddArc(x, y, r, r, 180, 90);
			p.AddArc(x + w - r, y, r, r, 270, 90);
			p.AddArc(x + w - r, y + h - r, r, r, 0, 90);
			p.AddArc(x, y + h - r, r, r, 90, 90);
			p.CloseFigure();

			return p;
		}

		public static GraphicsPath GetTopRoundRect(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X;
			float y = rect.Y;
			float w = rect.Width;
			float h = rect.Height;
			int d = (int)r << 1;

			p.StartFigure();
			p.AddArc(x, y, d, d, 180, 90);				// topleft
			p.AddLine(x + r, y, x + w - r, y);			// top
			p.AddArc(x + w - d, y, d, d, 270, 90);		// topright
			p.AddLine(x + w, y + r, x + w, y + h);		// right
			p.AddLine(x + w, y + h, x, y + h);			// bottom
			p.AddLine(x, y + h, x, y + r);				// left
			p.CloseFigure();

			return p;
		}

		#endregion

		#region - Left Half Pill -

		public static GraphicsPath GetLeftHalfPill(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X, y = rect.Y, w = rect.Width, h = rect.Height;

			if (w < h)
			{
				if ((w / 3) < r)
				{
					r = w / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}
			else
			{
				if ((h / 3) < r)
				{
					r = h / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}

			p.StartFigure();
			p.AddArc(x, y, h, h, 180, 90);
			p.AddArc(x + w - r, y, r, r, 270, 90);
			p.AddArc(x + w - r, y + h - r, r, r, 0, 90);
			p.AddArc(x, y, h, h, 90, 90);
			p.CloseFigure();

			return p;
		}

		public static GraphicsPath GetTopLeftHalfPill(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X;
			float y = rect.Y;
			float w = rect.Width;
			float h = rect.Height;
			int d = (int)r << 1;

			p.StartFigure();
			p.AddArc(x, y, h * 2, h * 2, 180, 90);		// topleft
			p.AddLine(x + r, y, x + w - r, y);			// top
			p.AddArc(x + w - d, y, d, d, 270, 90);		// topright
			p.AddLine(x + w, y + r, x + w, y + h);		// right
			p.AddLine(x + w, y + h, x, y + h);			// bottom
			p.AddLine(x, y + h, x, y + r);				// left
			p.CloseFigure();

			return p;
		}

		#endregion

		#region - Right Half Pill -

		public static GraphicsPath GetRightHalfPill(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X, y = rect.Y, w = rect.Width, h = rect.Height;

			if (w < h)
			{
				if ((w / 3) < r)
				{
					r = w / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}
			else
			{
				if ((h / 3) < r)
				{
					r = h / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}

			p.StartFigure();
			p.AddArc(x, y, r, r, 180, 90);
			p.AddArc(x + w - h, y, h, h, 270, 90);
			p.AddArc(x + w - h, y, h, h, 0, 90);
			p.AddArc(x, y + h - r, r, r, 90, 90);
			p.CloseFigure();

			return p;
		}

		public static GraphicsPath GetTopRightHalfPill(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X;
			float y = rect.Y;
			float w = rect.Width;
			float h = rect.Height;
			int d = (int)r << 1;

			p.StartFigure();
			p.AddArc(x, y, r, r, 180, 90);							// topleft
			p.AddLine(x + r, y, x + w, y);							// top
			p.AddArc(x + w - (h * 2), y, h * 2, h * 2, 270, 90);	// topright
			p.AddLine(x + w, y + r, x + w, y + h);					// right
			p.AddLine(x + w, y + h, x, y + h);						// bottom
			p.AddLine(x, y + h, x, y + r);							// left
			p.CloseFigure();

			return p;
		}

		#endregion

		#region - Full Pill -

		public static GraphicsPath GetFullPill(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X, y = rect.Y, w = rect.Width, h = rect.Height;

			if (w < h)
			{
				if ((w / 3) < r)
				{
					r = w / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}
			else
			{
				if ((h / 3) < r)
				{
					r = h / 3;
					if (r <= 0)
					{
						r = 1;
					}
				}
			}

			p.StartFigure();
			p.AddArc(x, y, h, h, 180, 90);
			p.AddArc(x + w - h, y, h, h, 270, 90);
			p.AddArc(x + w - h, y, h, h, 0, 90);
			p.AddArc(x, y, h, h, 90, 90);
			p.CloseFigure();

			return p;
		}

		public static GraphicsPath GetTopFullPill(RectangleF rect, float r)
		{
			GraphicsPath p = new GraphicsPath();
			float x = rect.X;
			float y = rect.Y;
			float w = rect.Width;
			float h = rect.Height;
			int d = (int)r << 1;

			p.StartFigure();
			p.AddArc(x, y, h * 2, h * 2, 180, 90);							// topleft
			p.AddLine(x + r, y, x + w, y);							// top
			p.AddArc(x + w - (h * 2), y, h * 2, h * 2, 270, 90);	// topright
			p.AddLine(x + w, y + r, x + w, y + h);					// right
			p.AddLine(x + w, y + h, x, y + h);						// bottom
			p.AddLine(x, y + h, x, y + r);							// left
			p.CloseFigure();

			return p;
		}

		#endregion

		#region - Glow Paths -

		public static GraphicsPath GetBottomRadialPath(RectangleF rect)
		{
			GraphicsPath p = new GraphicsPath();

			rect.X -= rect.Width * .35f;
			rect.Y -= rect.Height * .15f;
			rect.Width *= 1.7f;
			rect.Height *= 2.3f;

			p.AddEllipse(rect);
			p.CloseFigure();

			return p;
		}

		public static GraphicsPath GetLeftRadialPath(RectangleF rect)
		{
			GraphicsPath p = new GraphicsPath();

			rect.X -= rect.Width * .75f;
			rect.Y -= rect.Height * .15f;
			rect.Width *= 1.7f;
			rect.Height *= 1.25f;

			p.AddEllipse(rect);
			p.CloseFigure();

			return p;
		}

		public static GraphicsPath GetRightRadialPath(RectangleF rect)
		{
			GraphicsPath p = new GraphicsPath();

			rect.X -= rect.Width * .25f;
			rect.Y -= rect.Height * .15f;
			rect.Width *= 1.7f;
			rect.Height *= 1.25f;

			p.AddEllipse(rect);
			p.CloseFigure();

			return p;
		}

		#endregion
	}
}
