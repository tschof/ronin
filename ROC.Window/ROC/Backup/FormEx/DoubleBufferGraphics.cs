using System;
using System.Drawing;

namespace FormEx
{
	public class DoubleBufferGraphics
	{
		private Graphics graphics;
		private Bitmap memoryBitmap;
		private int width;
		private int height;

		public DoubleBufferGraphics()
		{
			width = 0;
			height = 0;
		}

		public bool CreateDoubleBuffer(Graphics g, int width, int height)
		{

			if (memoryBitmap != null)
			{
				memoryBitmap.Dispose();
				memoryBitmap = null;
			}

			if (graphics != null)
			{
				graphics.Dispose();
				graphics = null;
			}

			if (width == 0 || height == 0)
				return false;


			if ((width != this.width) || (height != this.height))
			{
				this.width = width;
				this.height = height;

				memoryBitmap = new Bitmap(width, height);
				graphics = Graphics.FromImage(memoryBitmap);
			}

			return true;
		}

		public void Render(Graphics g)
		{
			if (memoryBitmap != null)
				g.DrawImage(memoryBitmap, new Rectangle(0, 0, width, height), 0, 0, width, height, GraphicsUnit.Pixel);
		}

		public bool CanDoubleBuffer()
		{
			return graphics != null;
		}

		public Graphics g
		{
			get
			{
				return graphics;
			}
		}

	}
}
