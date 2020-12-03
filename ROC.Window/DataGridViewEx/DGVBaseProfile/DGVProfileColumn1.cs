using System;
using System.Collections.Generic;
using System.Text;
using System.Drawing;
using DictionaryEx;

namespace DataGridViewEx
{
	[Serializable]
	public class DGVProfileColumn : MutiTypedUIDictionary
	{
		public bool IsFration
		{
			get
			{
				if (Bools.ContainsKey(DGVColumnProfileFieldID.ColumnIsFraction))
				{
					return Bools[DGVColumnProfileFieldID.ColumnIsFraction];
				}
				else
				{
					return false;
				}
			}
		}

		public bool IsNotified
		{
			get
			{
				if (Bools.ContainsKey(DGVColumnProfileFieldID.ColumnIsNotified))
				{
					return Bools[DGVColumnProfileFieldID.ColumnIsNotified];
				}
				else
				{
					return false;
				}
			}
		}

		public Color DefaultForeColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.ColumnForeColor))
				{
					return Colors[DGVColumnProfileFieldID.ColumnForeColor];
				}
				else
				{
					return Color.White;
				}
			}
		}

		public Color DefaultBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.ColumnBackColor))
				{
					return Colors[DGVColumnProfileFieldID.ColumnBackColor];
				}
				else
				{
					return Color.Black;
				}
			}
		}

		public Font DefaultFont
		{
			get
			{
				if (Fonts.ContainsKey(DGVColumnProfileFieldID.ColumnFont))
				{
					return Fonts[DGVColumnProfileFieldID.ColumnFont];
				}
				else
				{
					return new Font("Tahoma", 8, FontStyle.Regular, GraphicsUnit.Point);
				}
			}
		}

		public Color HeaderForeColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.ColumnHeaderForeColor))
				{
					return Colors[DGVColumnProfileFieldID.ColumnHeaderForeColor];
				}
				else
				{
					return Color.Black;
				}
			}
		}

		public Color HeaderBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.ColumnHeaderBackColor))
				{
					return Colors[DGVColumnProfileFieldID.ColumnHeaderBackColor];
				}
				else
				{
					return Color.Gray;
				}
			}
		}

		public Font HeaderFont
		{
			get
			{
				if (Fonts.ContainsKey(DGVColumnProfileFieldID.ColumnHeaderFont))
				{
					return Fonts[DGVColumnProfileFieldID.ColumnHeaderFont];
				}
				else
				{
					return new Font("Tahoma", 8, FontStyle.Regular, GraphicsUnit.Point);
				}
			}
		}

		public int DisplayIndex
		{
			get
			{
				if (Longs.ContainsKey(DGVColumnProfileFieldID.ColumnDisplayIndex))
				{
					return (int)Longs[DGVColumnProfileFieldID.ColumnDisplayIndex];
				}
				else
				{
					return 0;
				}
			}
		}

		public string Name
		{
			get
			{
				if (Strings.ContainsKey(DGVColumnProfileFieldID.ColumnName))
				{
					return Strings[DGVColumnProfileFieldID.ColumnName];
				}
				else
				{
					return "";
				}
			}
		}

		public bool Visiable
		{
			get
			{
				if (Bools.ContainsKey(DGVColumnProfileFieldID.ColumnVisiable))
				{
					return Bools[DGVColumnProfileFieldID.ColumnVisiable];
				}
				else
				{
					return true;
				}
			}
		}

		public int Width
		{
			get
			{
				if (Longs.ContainsKey(DGVColumnProfileFieldID.ColumnWidth))
				{
					return (int)Longs[DGVColumnProfileFieldID.ColumnWidth];
				}
				else
				{
					return 25;
				}
			}
		}

		public Color SelectedBackColor
		{
			get
			{
				if (Colors.ContainsKey(DGVColumnProfileFieldID.ColumnSelectedBackColor))
				{
					return Colors[DGVColumnProfileFieldID.ColumnSelectedBackColor];
				}
				else
				{
					return Color.Blue;
				}
			}
	} 
	}
}
