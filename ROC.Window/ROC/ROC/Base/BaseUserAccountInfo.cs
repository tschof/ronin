using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;

namespace ROC
{
	public partial class BaseUserAccountInfo : UserControl
	{
		#region - Local Variable -

		private object _form;

		private HelperTraderAccount _traderInfo;
		private HelperTraderAccount traderInfo
		{
			get
			{
				if (_traderInfo == null)
				{
					if (_form is frmAutoSpreadTicket)
					{
						_traderInfo = new HelperTraderAccount((frmAutoSpreadTicket)_form);
					}
					else
					{
						_traderInfo = new HelperTraderAccount();
					}
				}
				return _traderInfo;
			}
		}

		private List<string> _tradeFors;
		private List<string> _accounts;
		private List<string> _exchanges;

		#endregion

		#region - Properties -

		private bool _showExchange = false;
		[DefaultValue(typeof(bool), "False")]
		public bool ShowExchange
		{
			get
			{
				return _showExchange;
			}
			set
			{
				_showExchange = value;
			}
		}

		#endregion

		#region - Construction -

		public BaseUserAccountInfo()
		{
			InitializeComponent();
		}

		#endregion

		#region - Base Event -

		private void BaseUserAccountInfo_Load(object sender, EventArgs e)
		{
			lblExchange.Visible = ShowExchange;
			cmdExchange.Visible = ShowExchange;

			_form = ((BaseUserAccountInfo)sender).Parent;

			LoadTradeFor();
		}

		#endregion

		#region - Trader Account Info -

		private void LoadTradeFor()
		{
			_tradeFors = traderInfo.GetTradeFors();
		}

		#endregion
	}
}
