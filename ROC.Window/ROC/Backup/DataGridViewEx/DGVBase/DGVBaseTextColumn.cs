using System;
using System.Windows.Forms;

namespace DataGridViewEx
{
	public class DGVBaseTextColumn : DataGridViewColumn
	{
		private int _showLength = 0;
		public int ShowLength
		{
			get
			{
				return _showLength;
			}
			set
			{
				if (_showLength < 0)
				{
					_showLength = 0;
				}
				else
				{
					_showLength = value;
				}
			}
		}


		public DGVBaseTextColumn()
		{
			base.CellTemplate = new DGVBaseTextCell();
		}
	}
}
