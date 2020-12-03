using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data;

namespace ROC
{
	public class DropDownShortLenders
	{
		#region - Local Variable -

		private frmStockTicket _stockTicket;
		private frmFutureTicket _futureTicket;
		private frmQuickTicket _quickTicket;
		private frmOptionTicket _optionTicket;

		private TicketTypes _ticketType = TicketTypes.None;

		private ComboBox cboShortLenders
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.cboShortLenders;
					case TicketTypes.Future:
						return null;
					case TicketTypes.Quick:
						return null;
					case TicketTypes.Option:
						return null;
					default:
						return null;
				}
			}
		}

		private DataTable shortLenders
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.ShortLenders;
					case TicketTypes.Future:
						return null;
					case TicketTypes.Quick:
						return null;
					case TicketTypes.Option:
						return null;
					default:
						return null;
				}
			}
		}

		#endregion

		#region - Constructor -

		public DropDownShortLenders(frmStockTicket stockTicket)
		{
			_stockTicket = stockTicket;
			_ticketType = TicketTypes.Stock;
		}

		public DropDownShortLenders(frmFutureTicket futureTicket)
		{
			_futureTicket = futureTicket;
			_ticketType = TicketTypes.Future;
		}

		public DropDownShortLenders(frmQuickTicket quickTicket)
		{
			_quickTicket = quickTicket;
			_ticketType = TicketTypes.Quick;
		}

		public DropDownShortLenders(frmOptionTicket optionTicket)
		{
			_optionTicket = optionTicket;
			_ticketType = TicketTypes.Option;
		}

		#endregion

		#region - Short Lenders Selection -

		// This would reset the combox after every Enter and Delete key stroke
		private bool _newShortLender = true;

		public string CurrentShortLender
		{
			get
			{
				return cboShortLenders.Text;
			}
			set
			{
				cboShortLenders.Text = value;
			}
		}

		public void ShortLendersKeyUp(KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Enter:
					if (cboShortLenders.Text != "")
					{
						AddShortLender(cboShortLenders.Text.ToUpper());
					}
					_newShortLender = true;
					break;
				default:
					_newShortLender = false;
					break;
			}
		}

		public void AddShortLender(string shortLender)
		{
			if (shortLenders.DefaultView.FindRows(shortLender).Length == 0)
			{
				shortLenders.Rows.Add(shortLender);
			}
			else
			{
				cboShortLenders.Text = shortLender;
			}

			CurrentShortLender = shortLender;
		}

		public void ShortLendersKeyDown(KeyEventArgs e)
		{
			switch (e.KeyCode)
			{
				case Keys.Delete:
					DeleteShortLender(cboShortLenders.Text.ToUpper());
					break;
				default:
					if (_newShortLender)
					{
						// Set the Text to null
						cboShortLenders.Text = "";
					}
					break;
			}
		}

		public void DeleteShortLender(string shortLender)
		{
			DataRowView[] rows = shortLenders.DefaultView.FindRows(shortLender);

			foreach (DataRowView row in rows)
			{
				shortLenders.Rows.Remove(row.Row);
			}
		}

		#endregion
	}
}
