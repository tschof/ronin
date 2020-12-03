using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

using FontDialogEx;

namespace ROC
{
	public partial class itemFont : UserControl
	{
		private VistaFontDialog _dialog;
		private VistaFontDialog dialog
		{
			get
			{
				if (_dialog == null)
				{
					_dialog = new VistaFontDialog();
					_dialog.PropertyChanged += new PropertyChangedEventHandler(dialog_PropertyChanged);
				}
				return _dialog;
			}
			set
			{
				_dialog = value;
			}
		}

		private void dialog_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			ItemFont = dialog.NewFont;
		}

		public itemFont()
		{
			InitializeComponent();
		}

		public string ItemName
		{
			get
			{
				return lblName.Text;
			}
			set
			{
				lblName.Text = value;
			}
		}

		public Font ItemFont
		{
			get
			{
				return lblFont.Font;
			}
			set
			{
				lblFont.Font = value;
			}
		}

		private void lblFont_Click(object sender, EventArgs e)
		{
			dialog.CurrentFont = ItemFont;
			dialog.ShowDialog();
		}
	}
}
