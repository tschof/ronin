using System;
using System.Windows.Forms;
using System.Drawing;

namespace DataGridViewEx
{
	public class DGVBaseSideColumn : DataGridViewColumn
	{
		private Color _sellColor = Color.DarkRed;
		public Color SellColor
		{
			get
			{
				return _sellColor;
			}
			set
			{
				_sellColor = value;
			}
		}

		private Color _buyColor = Color.DarkGreen;
		public Color BuyColor
		{
			get
			{
				return _buyColor;
			}
			set
			{
				_buyColor = value;
			}
		}

		private Color _sellShortColor = Color.DarkOrange;
		public Color SellShortColor
		{
			get
			{
				return _sellShortColor;
			}
			set
			{
				_sellShortColor = value;
			}
		}

		private Color _sseColor = Color.DarkSlateBlue;
		public Color SSEColor
		{
			get
			{
				return _sseColor;
			}
			set
			{
				_sseColor = value;
			}
		}

		public DGVBaseSideColumn()
		{
			base.CellTemplate = new DGVBaseSideCell();
		}
	}
}
