using System;
using System.ComponentModel;
using System.Windows.Forms;
using System.Drawing;

using LabelEx;

namespace FormEx
{
	[System.ComponentModel.DesignerCategory("code")]
	public class VistaWindowBorderless : VistaWindowBase
	{
		private FormBorderStyle _formBorderStyle = FormBorderStyle.None;

		public VistaWindowBorderless()
			: base()
		{
			base.FormBorderStyle = FormBorderStyle;
		}

		[Browsable(false)]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public new FormBorderStyle FormBorderStyle
		{
			get
			{
				return _formBorderStyle;
			}
			set
			{
				_formBorderStyle = FormBorderStyle.None;
			}
		}

		public void MoveByCaptionLable(object sender, MouseEventArgs e)
		{
			Point pt = new Point(e.X, e.Y);

			//pt = PointToScreen(pt);
			pt = ((LabelVista)sender).PointToScreen(pt);
			//pt = PointToClient(pt);

			IntPtr lParam = DockWindowHelper.Bit.MakeLParam(pt.X, pt.Y);

			DockWindowHelper.ReleaseCapture();
			DockWindowHelper.SendMessage(Handle, DockWindowHelper.WM.WM_NCLBUTTONDOWN, DockWindowHelper.WM.WM_HTCAPTION, (int)lParam);
		}
	}
}
