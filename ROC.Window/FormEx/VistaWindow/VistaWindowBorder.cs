using System;
using System.ComponentModel;

namespace FormEx
{
	[System.ComponentModel.DesignerCategory("Code")]
	public class VistaWindowBorder : VistaWindowBase
	{
		public VistaWindowBorder()
			: base()
		{
		}

		protected override void OnTextChanged(EventArgs e)
		{
			Caption = Text;
			Text = "";

			SetCaptionBar();
			base.OnTextChanged(e);
		}

		[Browsable(false)]
		[DesignerSerializationVisibility(DesignerSerializationVisibility.Hidden)]
		public new string Text
		{
			get
			{
				return Caption;
			}
			set
			{
				Caption = value;
			}
		}
	}
}
