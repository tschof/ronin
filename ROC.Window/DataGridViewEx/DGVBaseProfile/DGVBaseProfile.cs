using System;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Drawing;
using System.Runtime.Serialization;

using DictionaryEx;
using System.Diagnostics;

namespace DataGridViewEx
{
	[Serializable]
	public class DGVBaseProfile : MutiTypedUIDictionary, IDisposable
	{
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
				ProfileColumns.Add(col.Name, new DGVColumnProfile());

				ProfileColumns[col.Name].ColumnForeColor = col.DefaultCellStyle.ForeColor;
				ProfileColumns[col.Name].ColumnBackColor = col.DefaultCellStyle.BackColor;
				ProfileColumns[col.Name].ColumnFont = col.DefaultCellStyle.Font;

				ProfileColumns[col.Name].HeaderForeColor = col.HeaderCell.Style.ForeColor;
				ProfileColumns[col.Name].HeaderBackColor = col.HeaderCell.Style.BackColor;
				ProfileColumns[col.Name].HeaderFont = col.HeaderCell.Style.Font;

				ProfileColumns[col.Name].ColumnDisplayIndex = col.DisplayIndex;
				ProfileColumns[col.Name].ColumnName = col.Name;
				ProfileColumns[col.Name].ColumnVisiable = col.Visible;
				ProfileColumns[col.Name].ColumnWidth = col.Width;

				ProfileColumns[col.Name].SelectedBackColor = col.DefaultCellStyle.SelectionBackColor;
			}
			else
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
			get
			{
				if (Colors.ContainsKey(DGVProfileFieldID.Grid.ForeColor))
				{
					return Colors[DGVProfileFieldID.Grid.ForeColor];
				}
				else
				{
					return Color.White;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.ForeColor, value);
			}
		}

		public Color GridSelectedBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVProfileFieldID.Grid.SelectedBackColor))
				{
					return Colors[DGVProfileFieldID.Grid.SelectedBackColor];
				}
				else
				{
					return Color.Blue;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.SelectedBackColor, value);
			}
		}

		public Color GridBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVProfileFieldID.Grid.BackColor))
				{
					return Colors[DGVProfileFieldID.Grid.BackColor];
				}
				else
				{
					return Color.Black;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.BackColor, value);
			}
		}

		public Color GridColor
		{
			get
			{
				if (Colors.ContainsKey(DGVProfileFieldID.Grid.LineColor))
				{
					return Colors[DGVProfileFieldID.Grid.LineColor];
				}
				else
				{
					return Color.Black;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.LineColor, value);
			}
		}

		public Font GridFont
		{
			get
			{
				if (Fonts.ContainsKey(DGVProfileFieldID.Grid.Font))
				{
					return Fonts[DGVProfileFieldID.Grid.Font];
				}
				else
				{
					return new Font("Microsoft Sans Serif", 9.00F, System.Drawing.FontStyle.Regular);
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.Font, value);
			}
		}

		public bool GridAutoCenterOnTraded
		{
			get
			{
				if (Bools.ContainsKey(DGVProfileFieldID.Grid.AutoCenterOnTraded))
				{
					return Bools[DGVProfileFieldID.Grid.AutoCenterOnTraded];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.AutoCenterOnTraded, value);
			}
		}

		public long GridAutoCenterRange
		{
			get
			{
				if (Longs.ContainsKey(DGVProfileFieldID.Grid.AutoCenterRange))
				{
					return Longs[DGVProfileFieldID.Grid.AutoCenterRange];
				}
				else
				{
					return 3;
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.AutoCenterRange, value);
			}
		}

		public Color SelectedStopLimitBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVProfileFieldID.Grid.SelectedStopLimitBackColor))
				{
					return Colors[DGVProfileFieldID.Grid.SelectedStopLimitBackColor];
				}
				else
				{
					return Color.FromArgb(34, 34, 34);
				}
			}
			set
			{
				Update(DGVProfileFieldID.Grid.SelectedStopLimitBackColor, value);
			}
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
