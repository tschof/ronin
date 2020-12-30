using System;
using System.Windows.Forms;
using System.Drawing;
using System.ComponentModel;
using DataGridViewEx.Properties;

namespace DataGridViewEx
{
	public class DGVBaseQuickNumberColumn : DGVBaseNumberColumn
	{
		private bool _showBar = false;
		public bool ShowBar
		{
			get
			{
				return _showBar;
			}
			set
			{
				_showBar = value;
			}
		}

		private long _showBarMax = 1;
		public long ShowBarMax
		{
			get
			{
				return _showBarMax;
			}
			set
			{
				_showBarMax = value;
			}
		}

		private bool _isTradePrice = false;
		public bool IsTradePrice
		{
			get
			{
				return _isTradePrice;
			}
			set
			{
				_isTradePrice = value;
			}
		}

		private bool _flagStopOpen = false;
		public bool FlagStopOpen
		{
			get
			{
				return _flagStopOpen;
			}
			set
			{
				_flagStopOpen = value;
			}
		}

		public Image StopImage
		{
			get
			{
				return Resources.Stop;
			}
		}

		private Color _stopForeColor = Color.Fuchsia;
		public Color StopForeColor
		{
			get
			{
				return _stopForeColor;
			}
			set
			{
				_stopForeColor = value;
			}
		}

		public DGVBaseQuickNumberColumn()
		{
			base.CellTemplate = new DGVBaseQuickNumberCell();
		}
	}
}
