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
	public partial class menuBaseVertical : UserControl, INotifyPropertyChanged
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
		private int _height = 0;

		public menuBaseVertical(Dictionary<string, string> items, string caption)
		{
			InitializeComponent();

			dspMainMenu.DefaultText = caption;
			
			_items = items;
			foreach (string item in _items.Values)
			{
				AddLabel(item);
			}

			Height = _height;
		}

		private void AddLabel(string item)
		{
			LabelVista lable = new LabelVista();

			lable.Font = new Font(lable.Font.FontFamily, 9.75f);
			lable.Padding = new Padding(0, 2, 0, 2);
			lable.ApplyShine = true;
			lable.Animate = true;
			lable.OuterBorderColor = Color.White;

			lable.Dock = DockStyle.Top;
			lable.AutoSize = true;

			lable.Name = item;
			lable.DefaultText = item;

			Controls.Add(lable);
			lable.BringToFront();
			lable.Show();

			_height = _height + lable.Height;

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
