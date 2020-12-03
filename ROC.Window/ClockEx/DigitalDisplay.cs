using System;
using System.Drawing;
using System.Drawing.Drawing2D;

namespace ClockEx
{
	// A digit class which can draw a number on a graphics surface
	internal static class DigitalDisplay
	{
		private static float _lineWidth = 20.0F;
		// end point coordinates of lines in the digital display
		private static Point[] _points;

		private static int[] _displayNum = new int[12]{
			63,12,118,94,77,91,123,14,127,95,
			111, // to display 'A'
			103}; // to display 'P'

		private static Pen _pen = new Pen(Color.Red);
		static internal void SetPenColor(Color digitColor)
		{
			_pen.Color = digitColor;

			_pen.StartCap = LineCap.Triangle;
			_pen.EndCap = LineCap.Triangle;

			setDimPen();
		}

		private static Pen _dimPen = null;
		static private void setDimPen()
		{
			if (_dimPen == null)
			{
				_dimPen = (Pen)_pen.Clone();
			}
			_dimPen.Width = _pen.Width;
			_dimPen.Color = Color.FromArgb(
				_pen.Color.R > 0 ? 25 : 0,
				_pen.Color.G > 0 ? 25 : 0,
				_pen.Color.B > 0 ? 25 : 0);
		}

		private static Color _backColor = Color.Black;
		static internal void SetBackColor(Color backColor)
		{
			_backColor = backColor;
		}

		// Draw Number
		static internal Image Draw(Size size, int num)
		{
			calculateAllParameters(size);

			int check;
			Image img = new Bitmap(size.Width, size.Height);
			using (Graphics g = Graphics.FromImage(img))
			{
				g.Clear(_backColor);
				for (int i = 0; i < 7; i++)
				{
					check = (int)System.Math.Pow(2, i);
					if ((check & _displayNum[num]) == 0)
					{
						g.DrawLine(_dimPen, _points[i * 2], _points[i * 2 + 1]);
					}
					else
					{
						g.DrawLine(_pen, _points[i * 2], _points[i * 2 + 1]);
					}
				}
			}

			return img;
		}

		// Draws the complete rectangle in dim shade to give the digital effect :-)
		static internal Image Draw(Size size)
		{
			calculateAllParameters(size);

			Image img = new Bitmap(size.Width, size.Height);
			using (Graphics g = Graphics.FromImage(img))
			{
				g.Clear(_backColor);
				for (int i = 0; i < 7; i++)
				{
					g.DrawLine(_dimPen, _points[i * 2], _points[i * 2 + 1]);
				}
			}

			return img;
		}

		static internal Image Draw(Size size, char ch)
		{
			// 10 and 11 are indices of A and P in the displayNum array
			switch (Char.ToUpper(ch))
			{
				case 'A':
					return Draw(size, 10);
				case 'P':
					return Draw(size, 11);
				default:
					return Draw(size);
			}
		}

		static internal Image Draw(Size size, ColonType type, bool dim)
		{
			calculateAllParameters(size);

			// choose a pen for blinking
			Pen p = (dim) ? _dimPen : _pen;

			// Rectangles in which colons are displayed
			Rectangle colonRect1;
			Rectangle colonRect2;
			Rectangle area = new Rectangle(0, 0, size.Width, size.Height);

			colonRect1 = colonRect2 = area;
			colonRect1.X = colonRect2.X = area.X + (int)((area.Width - _lineWidth) / 2.0);
			colonRect1.Y = area.Y + area.Height / 3;
			colonRect2.Y = area.Y + (area.Height * 2) / 3;
			colonRect1.Width = colonRect1.Height = colonRect2.Width = colonRect2.Height = (int)_lineWidth;

			Image img = new Bitmap(size.Width, size.Height);
			using (Graphics g = Graphics.FromImage(img))
			{
				g.Clear(_backColor);
				switch (type)
				{
					case ColonType.Circular:
						g.DrawEllipse(p, colonRect1);
						g.DrawEllipse(p, colonRect2);
						break;
					case ColonType.Rectangular:
						g.DrawRectangle(p, colonRect1);
						g.DrawRectangle(p, colonRect2);
						break;
				}
			}

			return img;
		}

		static private void calculateAllParameters(Size size)
		{
			Rectangle rect = new Rectangle(0, 0, size.Width, size.Height);

			_points = new Point[14]; // there are 7 lines in a display
			for (int i = 0; i < 14; i++)
			{
				_points[i] = new Point(0, 0);
			}

			_lineWidth = (int)(rect.Width / 5);
			if (_lineWidth < 2)
			{
				_lineWidth = 2;
			}
			if (_lineWidth > 20)
			{
				_lineWidth = 20;
			}
			_pen.Width = _lineWidth;
			setDimPen();

			CalculateLineEnds(rect);
		}

		static private void CalculateLineEnds(Rectangle rect)
		{
			// 0,1,2,9,10,11,12 points share the same left edge X coordinate
			_points[0].X = _points[1].X = _points[2].X = _points[9].X = _points[10].X = _points[11].X = _points[12].X = rect.Left + 2;

			// points 3,4,5,6,7,8,13 the right edge X coordinate
			_points[3].X = _points[4].X = _points[5].X = _points[6].X =	_points[7].X = _points[8].X = _points[13].X = rect.Right - (int)_lineWidth;

			// Points 1,2,3,4 are the top most points
			_points[1].Y = _points[2].Y = _points[3].Y = _points[4].Y = (int)(rect.Top) + 1;

			// Points 0,11,12,13,5,6 are the middle points
			_points[0].Y = _points[11].Y = _points[12].Y = _points[13].Y = _points[5].Y = _points[6].Y = rect.Top + (int)((rect.Height - _lineWidth) / 2.0);
			
			// Points 7,8,9,10 are on the bottom edge
			_points[7].Y = _points[8].Y = _points[9].Y = _points[10].Y = rect.Top + (int)(rect.Height - _lineWidth);

			// now adjust the coordinates that were computed, to get the digital look
			AdjustCoordinates();
		}

		static private void AdjustCoordinates()
		{
			Point swap; // required in case points have to be swapped
			for (int i = 0; i < 7; i++)
			{
				// Always draw from left to right and top to bottom
				// Adjust the end points accordingly
				if (_points[i * 2].X > _points[(i * 2) + 1].X || _points[i * 2].Y > _points[(i * 2) + 1].Y)
				{
					swap = _points[i * 2]; _points[i * 2] = _points[(i * 2) + 1]; _points[(i * 2) + 1] = swap;
				}

				// for horizontal lines adjust the X coord
				if (_points[i * 2].X != _points[(i * 2) + 1].X)
				{
					_points[i * 2].X += (int)(_lineWidth / 1.6);
					_points[(i * 2) + 1].X -= (int)(_lineWidth / 1.6);
				}
				// for vertical lines adjust the y coord
				if (_points[i * 2].Y != _points[(i * 2) + 1].Y)
				{
					_points[i * 2].Y += (int)(_lineWidth / 1.6);
					_points[(i * 2) + 1].Y -= (int)(_lineWidth / 1.6);
				}
			}
		}
	}
}
