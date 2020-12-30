using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;

using FormEx;

namespace ROC
{
	[System.ComponentModel.DesignerCategory("Form")]
	public partial class frmWait : VistaWindowBorderless
	{
		public frmWait()
		{
			InitializeComponent();
		}

		public string Message
		{
			get
			{
				return lblCaption.Text;
			}
			set
			{
				lblCaption.Text = value;
			}
		}
	}
}