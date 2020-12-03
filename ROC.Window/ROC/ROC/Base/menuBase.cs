using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

using LabelEx;

namespace ROC
{
	[ToolboxItem(false)]
	public partial class menuBase : UserControl, INotifyPropertyChanged
	{
		#region - INotifyPropertyChanged Members -

		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual void onPropertyChanged(PropertyChangedEventArgs e)
		{
			if (PropertyChanged != null)
			{
				PropertyChanged(this, e);
			}
		}

		#endregion

		private Dictionary<string, string> _items;
		private int _width = 0;

		public menuBase(Dictionary<string, string> items)
		{
			InitializeComponent();
			_items = items;
			foreach (string item in _items.Values)
			{
				AddLabel(item);
			}

			Width = _width;
		}

		private void AddLabel(string item)
		{
			LabelVista lable = new LabelVista();

			lable.Font = new Font(lable.Font.FontFamily, 9.75f);
			lable.Padding = new Padding(0, 2, 0, 2);
			lable.ApplyShine = true;
			lable.Animate = true;
			lable.OuterBorderColor = Color.Black;

			lable.Dock = DockStyle.Left;
			lable.AutoSize = true;

			lable.Name = item;
			lable.DefaultText = item;

			Controls.Add(lable);
			lable.BringToFront();
			lable.Show();

			_width = _width + lable.Width;

			lable.Click += new EventHandler(lable_Click);
		}

		private void lable_Click(object sender, EventArgs e)
		{
			if (sender is LabelVista)
			{
				// Get the sender name
				onPropertyChanged(new PropertyChangedEventArgs(((LabelVista)sender).Name));
			}
		}
	}
}
