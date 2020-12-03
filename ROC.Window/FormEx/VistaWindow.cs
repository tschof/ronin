using System;
using System.ComponentModel;

namespace FormEx
{
	[System.ComponentModel.DesignerCategory("Form")]
	public class VistaWindow : VistaWindowBase
	{
		public VistaWindow()
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
