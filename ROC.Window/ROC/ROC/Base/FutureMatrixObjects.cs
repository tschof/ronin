using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	public partial class FutureMatrixObjects : UserControl
	{
		private Color _backColor;
		private DateTime _expiration;

		#region - Properties -

		private string _symbolDetail = "";
		internal string SymbolDetail
		{
			get
			{
				return _symbolDetail;
			}
		}

		private BaseSecurityInfo _secInfo;
		internal BaseSecurityInfo SecInfo
		{
			get
			{
				if (_secInfo == null)
				{
					_secInfo = new BaseSecurityInfo();
				}
				return _secInfo;
			}
			set
			{
				_secInfo = value;
				if (_secInfo != null && _secInfo.MDSymbol != "")
				{
					dspBidPrice.TickSize = SecInfo.TickSize;
					dspAskPrice.TickSize = SecInfo.TickSize;

					dspBidPrice.MaxDecimal = 7;
					dspAskPrice.MaxDecimal = 7;

					Enabled = true;
				}
				else
				{
					Enabled = false;
				}
			}
		}

		internal double BidPrice
		{
			set
			{
				dspBidPrice.Value = value;
			}
		}

		internal long BidSize
		{
			set
			{
				dspBidSize.Value = value;
			}
		}

		internal double AskPrice
		{
			set
			{
				dspAskPrice.Value = value;
			}
		}

		internal long AskSize
		{
			set
			{
				dspAskSize.Value = value;
			}
		}

		#endregion

		#region - Constructor -

		public FutureMatrixObjects(string symbolDetail, Color backColor, DateTime expiration)
		{
			InitializeComponent();

			Initialize(symbolDetail, backColor, expiration);
		}

		private void Initialize(string symbolDetail, Color backColor, DateTime expiration)
		{
			_symbolDetail = symbolDetail;
			_backColor = backColor;
			_expiration = expiration;
		}

		#endregion

		#region - Object Event -

		protected override void OnLoad(EventArgs e)
		{
			if (!_symbolDetail.Contains("-"))
			{
				dspSymbol.TextAlign = ContentAlignment.MiddleCenter;
				dspSymbol.BackColor = _backColor;
				dspSymbol.ForeColor = Color.Gold;
				dspSymbol.Text = string.Format("({0}) {1}", _expiration.ToString("MMM yy"), _symbolDetail);
			}
			else
			{
				dspSymbol.Text = _symbolDetail;
			}

			base.OnLoad(e);
		}

		private void dspSymbol_MouseDoubleClick(object sender, MouseEventArgs e)
		{
			if (_secInfo.SecType != "" && _symbolDetail != "")
			{
				GLOBAL.HWindows.OpenWindow(_secInfo.SecType, _symbolDetail);
			}
		}

		#endregion

		private void dspBidPrice_MouseEnter(object sender, EventArgs e)
		{
			BackColor = Color.Gold;
		}

		private void dspBidPrice_MouseLeave(object sender, EventArgs e)
		{
			BackColor = Color.Black;
		}
	}
}
