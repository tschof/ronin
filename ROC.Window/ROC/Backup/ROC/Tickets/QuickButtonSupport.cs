using System;
using System.Collections.Generic;
using System.Text;
using System.ComponentModel;
using System.Windows.Forms;
using LabelEx;
using System.Drawing;

namespace ROC
{
	public delegate void QuickButtonClickedEventHandler(object sender, QuickButtonClickedEventArgs e);

	public class QuickButtonSupport
	{
		#region - Event -

		public event QuickButtonClickedEventHandler QuickButtonClicked;
		private void OnQuickButtonClicked(QuickButtonClickedEventArgs e)
		{
			if (QuickButtonClicked != null)
			{
				QuickButtonClicked(this, e);
			}
		}

		#endregion

		#region - Local Variables -

		private string _lastLoadedSymbol = "";

		private frmStockTicket _stockTicket;
		private frmFutureTicket _futureTicket;
		private frmQuickTicket _quickTicket;
		private frmOptionTicket _optionTicket;

		private TicketTypes _ticketType = TicketTypes.None;

		private Panel panelCommandsEx
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.panelCommandsEx;
					case TicketTypes.Future:
						return _futureTicket.panelCommandsEx;
					case TicketTypes.Quick:
						return _quickTicket.panelCommandsEx;
					case TicketTypes.Option:
						return _optionTicket.panelCommandsEx;
					case TicketTypes.AutoSpread:
					default:
						return null;
				}
			}
		}

		#endregion

		#region - Constructor -

		public QuickButtonSupport(frmStockTicket stockTicket)
		{
			_stockTicket = stockTicket;
			_ticketType = TicketTypes.Stock;
		}

		public QuickButtonSupport(frmFutureTicket futureTicket)
		{
			_futureTicket = futureTicket;
			_ticketType = TicketTypes.Future;
		}

		public QuickButtonSupport(frmQuickTicket quickTicket)
		{
			_quickTicket = quickTicket;
			_ticketType = TicketTypes.Quick;
		}

		public QuickButtonSupport(frmOptionTicket optionTicket)
		{
			_optionTicket = optionTicket;
			_ticketType = TicketTypes.Option;
		}

		#endregion

		public void UILoadSymbolQuickButtonDefaults(List<QuickButtonSettingData> datas)
		{
			UILoadSymbolQuickButtonDefaults(datas, true);
		}
		public void UILoadSymbolQuickButtonDefaults(List<QuickButtonSettingData> datas, bool reload)
		{
			if (datas.Count > 0)
			{
				if (_lastLoadedSymbol == datas[0].symbol && !reload)
				{
					// Don't reload the Quick Button
					return;
				}
				_lastLoadedSymbol = datas[0].symbol;
				ClearSymbolQuickButtons();

				int count = 0;
				foreach (QuickButtonSettingData data in datas)
				{
					if (data.active)
					{
						count++;
						LabelVista lable = SetQuickButtonByData(count, data);
					}
				}

				if (count > 0)
				{
					panelCommandsEx.Visible = true;
					return;
				}
			}

			panelCommandsEx.Visible = false;
		}

		private void ClearSymbolQuickButtons()
		{
			try
			{
				foreach (Control obj in panelCommandsEx.Controls)
				{
					if (obj is LabelVista)
					{
						((LabelVista)obj).Click -= new EventHandler(QuickButtons_Click);
					}
				}

				panelCommandsEx.Controls.Clear();
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		private LabelVista SetQuickButtonByData(int count, QuickButtonSettingData data)
		{
			LabelVista lable = new LabelVista();

			lable.Font = new Font(panelCommandsEx.Font.FontFamily, panelCommandsEx.Font.Size, FontStyle.Bold);
			lable.Padding = new Padding(0, 0, 0, 0);
			lable.ApplyShine = true;
			lable.Animate = true;
			lable.OuterBorderColor = Color.White;
			lable.AutoSize = true;
			lable.MinimumSize = new Size(20, 20);
			lable.TextAlign = ContentAlignment.MiddleCenter;

			lable.ForeColor = Color.Black;
			switch (data.side)
			{
				case QuickButtonSides.Buy:
					lable.BackColor = Color.LimeGreen;
					break;
				case QuickButtonSides.Sell:
					lable.BackColor = Color.Red;
					break;
				case QuickButtonSides.Short:
					lable.BackColor = Color.Orchid;
					break;
				case QuickButtonSides.BuyOpen:
					lable.BackColor = Color.LimeGreen;
					break;
				case QuickButtonSides.BuyClose:
					lable.BackColor = Color.CornflowerBlue;
					break;
				case QuickButtonSides.SellOpen:
					lable.BackColor = Color.Orchid;
					break;
				case QuickButtonSides.SellClose:
					lable.BackColor = Color.Red;
					break;
			}

			lable.Name = "QB" + count.ToString();
			lable.DefaultText = data.displayName;
			lable.Tag = data;

			panelCommandsEx.Controls.Add(lable);
			lable.BringToFront();
			lable.Show();

			lable.Dock = DockStyle.Left;
			lable.Click += new EventHandler(QuickButtons_Click);
			return lable;
		}

		private void QuickButtons_Click(object sender, EventArgs e)
		{
			if (sender is LabelVista)
			{
				if (((LabelVista)sender).Tag is QuickButtonSettingData)
				{
					OnQuickButtonClicked(new QuickButtonClickedEventArgs(((QuickButtonSettingData)((LabelVista)sender).Tag)));
				}
			}
		}
	}

	public class QuickButtonClickedEventArgs : EventArgs
	{
		public QuickButtonClickedEventArgs(QuickButtonSettingData data)
		{
			_data = data;
		}
		
		private QuickButtonSettingData _data;
		public QuickButtonSettingData Data
		{
			get
			{
				return _data;
			}
		}
	}
}
