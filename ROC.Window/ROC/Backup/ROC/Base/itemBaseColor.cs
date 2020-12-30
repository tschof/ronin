using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using ColorDialogEx;

namespace ROC
{
	public partial class itemBaseColor : UserControl
	{
		private ColorWheelDialog _dialog;
		private ColorWheelDialog dialog
		{
			get
			{
				if (_dialog == null)
				{
					_dialog = new ColorWheelDialog();
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
			ItemColor = dialog.Color;
		}

		public itemBaseColor()
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

		public Color ItemColor
		{
			get
			{
				return lblColor.BackColor;
			}
			set
			{
				lblColor.BackColor = value;
			}
		}

		private void lblColor_Click(object sender, EventArgs e)
		{
			dialog.Color = ItemColor;
			dialog.ShowDialog();
		}
	}
}
