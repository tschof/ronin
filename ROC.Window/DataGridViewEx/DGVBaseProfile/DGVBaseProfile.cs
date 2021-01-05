using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;

using System.Diagnostics;

namespace DataGridViewEx
{
	[Serializable]
	public class DGVBaseProfile : IDisposable
	{
		private DictionaryEx.MultiTypedUIDictionary _data = new DictionaryEx.MultiTypedUIDictionary();

		protected void Update(int key, Color value)
		{
			_data.Set(key, value);
		}

		protected void Update(int key, Font value)
		{
			_data.Set(key, value);
		}

		protected void Update<T>(int key, T value)
		{
			_data.Set(key, value);
		}

		protected bool TryGet(int key, out long value)
		{
			return _data.TryGet(key, out value);
		}

		protected bool TryGet(int key, out string value)
		{
			return _data.TryGet(key, out value);
		}

		protected bool TryGet(int key, out bool value)
		{
			return _data.TryGet(key, out value);
		}

		private List<string> _symbolDetailList;
		public List<string> SymbolDetailList
		{
			get
			{
				if (_symbolDetailList == null)
				{
					_symbolDetailList = new List<string>();
				}
				return _symbolDetailList;
			}
			set
			{
				_symbolDetailList = value;
			}
		}

		private List<AutoSpreadSymbolSetting> _autoSpreadSymbolSettingList;
		public List<AutoSpreadSymbolSetting> AutoSpreadSymbolSettingList
		{
			get
			{
				if (_autoSpreadSymbolSettingList == null)
				{
					_autoSpreadSymbolSettingList = new List<AutoSpreadSymbolSetting>();
				}
				return _autoSpreadSymbolSettingList;
			}
			set
			{
				_autoSpreadSymbolSettingList = value;
			}
		}

		private Dictionary<string, DGVColumnProfile> _profileColumns;
		public Dictionary<string, DGVColumnProfile> ProfileColumns
		{
			get
			{
				if (_profileColumns == null)
				{
					_profileColumns = new Dictionary<string, DGVColumnProfile>();
				}
				return _profileColumns;
			}
			set
			{
				_profileColumns = value;
			}
		}

		public DGVBaseProfile()
		{
		}

		public void AddProfile(DataGridViewColumn col)
		{
			if (!ProfileColumns.ContainsKey(col.Name))
			{
				var profile = new DGVColumnProfile();
				ProfileColumns.Add(col.Name, profile);

				profile.ColumnForeColor = col.DefaultCellStyle.ForeColor;
				profile.ColumnBackColor = col.DefaultCellStyle.BackColor;
				profile.ColumnFont = col.DefaultCellStyle.Font;

				profile.HeaderForeColor = col.HeaderCell.Style.ForeColor;
				profile.HeaderBackColor = col.HeaderCell.Style.BackColor;
				profile.HeaderFont = col.HeaderCell.Style.Font;

				profile.ColumnDisplayIndex = col.DisplayIndex;
				profile.ColumnName = col.Name;
				profile.ColumnVisiable = col.Visible;
				profile.ColumnWidth = col.Width;

				profile.SelectedBackColor = col.DefaultCellStyle.SelectionBackColor;
			} else
			{
				Debug.Assert(false, "Duplicated: " + col.Name);
			}
		}

		public void SetProfile(DataGridViewColumn col, DGVColumnProfile val)
		{
			if (col != null)
			{
				col.DefaultCellStyle.SelectionBackColor = ((DGVBase)col.DataGridView).DefaultSelectedBackgroundColor;
				//col.DefaultCellStyle.BackColor = col.DataGridView.BackgroundColor;
				col.DefaultCellStyle.BackColor = val.ColumnBackColor;
				col.DefaultCellStyle.ForeColor = val.ColumnForeColor;
				col.DefaultCellStyle.Font = val.ColumnFont;
				col.DisplayIndex = val.ColumnDisplayIndex;
				col.Visible = val.ColumnVisiable;
				col.Width = val.ColumnWidth;
			}
		}

		#region - Grid -

		public Color GridForeColor
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.ForeColor, out Color value) ? value : Color.White;
			set => _data.Set(DGVProfileFieldID.Grid.ForeColor, value);
		}

		public Color GridSelectedBackColor
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.SelectedBackColor, out Color value) ? value : Color.Blue;
			set => _data.Set(DGVProfileFieldID.Grid.SelectedBackColor, value);
		}

		public Color GridBackColor
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.BackColor, out Color value) ? value : Color.White;
			set => _data.Set(DGVProfileFieldID.Grid.BackColor, value);
		}

		public Color GridColor
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.LineColor, out Color value) ? value : Color.Black;
			set => _data.Set(DGVProfileFieldID.Grid.LineColor, value);
		}

		public Font GridFont
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.Font, out Font value) ? value : new Font("Microsoft Sans Serif", 9.00F, FontStyle.Regular);
			set => _data.Set(DGVProfileFieldID.Grid.Font, value);
		}

		public bool GridAutoCenterOnTraded
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.AutoCenterOnTraded, out bool value) ? value : false;
			set => _data.Set(DGVProfileFieldID.Grid.AutoCenterOnTraded, value);
		}

		public long GridAutoCenterRange
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.AutoCenterRange, out long value) ? value : 3;
			set => _data.Set(DGVProfileFieldID.Grid.AutoCenterRange, value);
		}

		public Color SelectedStopLimitBackColor
		{
			get => _data.TryGet(DGVProfileFieldID.Grid.SelectedStopLimitBackColor, out Color value) ? value : Color.FromArgb(34, 34, 34);
			set => _data.Set(DGVProfileFieldID.Grid.SelectedStopLimitBackColor, value);
		}

		#endregion

		#region - IDisposable Members -

		public void Dispose()
		{
			SymbolDetailList.Clear();
			ProfileColumns.Clear();
		}

		#endregion
	}
}
