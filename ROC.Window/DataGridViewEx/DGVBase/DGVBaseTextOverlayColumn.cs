using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using DataGridViewEx.Properties;
using System.Drawing.Imaging;

namespace DataGridViewEx
{
	public class DGVBaseTextOverlayColumn : DGVBaseTextColumn
	{
		[Browsable(false)]
		public Image NotifyImage
		{
			get
			{
				return Resources.Failed;
			}
		}

		private Image _notifyImageOpac = null;
		[Browsable(false)]
		public Image NotifyImageOpac
		{
			get
			{
				if (_notifyImageOpac == null)
				{
					Bitmap bmpPic = new Bitmap(NotifyImage.Width, NotifyImage.Height);
					Graphics gfxPic = Graphics.FromImage(bmpPic);
					ColorMatrix cmxPic = new ColorMatrix();
					cmxPic.Matrix33 = 30;

					ImageAttributes iaPic = new ImageAttributes();
					iaPic.SetColorMatrix(cmxPic, ColorMatrixFlag.Default, ColorAdjustType.Bitmap);
					gfxPic.DrawImage(NotifyImage, new Rectangle(0, 0, bmpPic.Width, bmpPic.Height), 0, 0, NotifyImage.Width, NotifyImage.Height, GraphicsUnit.Pixel, iaPic);
					gfxPic.Dispose();

					_notifyImageOpac = bmpPic;
				}
				return _notifyImageOpac;
			}
		}

		public DGVBaseTextOverlayColumn()
		{
			base.CellTemplate = new DGVBaseTextOverlayCell();
		}
	}
}
