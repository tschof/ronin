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
	public partial class menuBaseFilter : UserControl, INotifyPropertyChanged
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

		private Dictionary<string, FilterItem> _items;
		public Dictionary<string, FilterItem> Items
		{
			get
			{
				return _items;
			}
		}

		private int _width = 0;
		private int _height = 0;

		private bool _vertical = false;

		public menuBaseFilter(Dictionary<string, FilterItem> items)
		{
			Initialize(items, false);
		}
		public menuBaseFilter(Dictionary<string, FilterItem> items, bool vertical)
		{
			Initialize(items, vertical);
		}

		private void Initialize(Dictionary<string, FilterItem> items, bool vertical)
		{
			InitializeComponent();

			_vertical = vertical;
			_items = items;

			foreach (FilterItem item in _items.Values)
			{
				item.PropertyChanged += new PropertyChangedEventHandler(item_PropertyChanged);
				AddLabel(item);
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

				if (Height < 88)
				{
					dspFilterMenu.DefaultText = "";
				}
				else
				{
					dspFilterMenu.DefaultText = "Selection...";
				}
			}
			else
			{
				Height = _height + 4;
				Width = _width + 4;
			}
		}

		private void AddLabel(FilterItem item)
		{
			LabelVista lable = new LabelVista();

			lable.Font = new Font(lable.Font.FontFamily, 9.75f);
			lable.Padding = new Padding(0, 2, 0, 2);
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

			lable.Name = item.Name;
			lable.DefaultText = item.Name;
			if (item.Hide)
			{
				lable.ForeColor = Color.White;
			}
			else
			{
				lable.ForeColor = Color.GreenYellow;
			}

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
				string key = ((LabelVista)sender).Name;

				if (_items.ContainsKey(key))
				{
					_items[key].Hide = !_items[key].Hide;
					UpdateColor((Label)sender, _items[key].Hide);
				}
			}
		}

		private void item_PropertyChanged(object sender, PropertyChangedEventArgs e)
		{
			onPropertyChanged(new PropertyChangedEventArgs(e.PropertyName));
		}

		private void UpdateColor(Label lbl, bool hideVal)
		{
			if (hideVal)
			{
				lbl.ForeColor = Color.White;
			}
			else
			{
				lbl.ForeColor = Color.GreenYellow;
			}
		}
	}

	public class FilterItem : INotifyPropertyChanged
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

		private string _name = "";
		public string Name
		{
			get
			{
				return _name;
			}
		}

		private bool _hide = false;
		public bool Hide
		{
			get
			{
				return _hide;
			}
			set
			{
				if (_hide != value)
				{
					_hide = value;
					onPropertyChanged(new PropertyChangedEventArgs(_name));
				}
			}
		}

		public FilterItem(string name, bool hide)
		{
			_name = name;
			_hide = hide;
		}
	}
}
