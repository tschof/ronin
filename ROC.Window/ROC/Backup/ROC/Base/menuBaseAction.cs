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
	public partial class menuBaseAction : UserControl, INotifyPropertyChanged
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
		private int _height = 0;

		private bool _vertical = false;

		public menuBaseAction(Dictionary<string, string> items)
		{
			Initialize(items, false);
		}
		public menuBaseAction(Dictionary<string, string> items, bool vertical)
		{
			Initialize(items, vertical);
		}

		private void Initialize(Dictionary<string, string> items, bool vertical)
		{
			InitializeComponent();

			_vertical = vertical;
			_items = items;
			
			foreach (string item in _items.Keys)
			{
				AddLabel(item, _items[item]);
			}

			Padding = new Padding(2);

			if (_vertical)
			{
				Height = _height + 4;
				Width = vistaPanelStatus.Width + _width + 4;
				foreach (object obj in Controls)
				{
					if (obj is LabelVista)
					{
						((LabelVista)obj).AutoSize = false;
						((LabelVista)obj).Width = _width;
					}
				}

				if (Height < 85)
				{
					dspFilterMenu.DefaultText = "";
				}
				else
				{
					dspFilterMenu.DefaultText = "Commands...";
				}
			}
			else
			{
				Height = _height + 4;
				Width = _width + 4;
			}
		}

		private void AddLabel(string item, string display)
		{
			LabelVista lable = new LabelVista();

			lable.Font = new Font(lable.Font.FontFamily, 9.75f);
			lable.Padding = new Padding(0, 2, 0, 2);
			lable.ApplyShine = true;
			lable.Animate = true;
			lable.OuterBorderColor = Color.White;

			lable.AutoSize = true;

			if (_vertical)
			{
				vistaPanelStatus.Visible = true;
			}
			else
			{
				vistaPanelStatus.Visible = false;
			}

			lable.Name = item;
			lable.DefaultText = display;

			Controls.Add(lable);
			lable.BringToFront();
			lable.Show();

			if (_vertical)
			{
				lable.Dock = DockStyle.Top;
				_height = _height + lable.Height;
				if (lable.Width > _width)
				{
					_width = lable.Width;
				}
			}
			else
			{
				_width = _width + lable.Width;
				if (lable.Height > _height)
				{
					_height = lable.Height;
				}
				lable.Dock = DockStyle.Left;
			}

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
