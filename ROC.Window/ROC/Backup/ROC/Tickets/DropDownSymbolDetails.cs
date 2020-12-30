using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Data;

namespace ROC
{
	public class DropDownSymbolDetails
	{
		#region - Local Variable -

		private frmStockTicket _stockTicket;
		private frmFutureTicket _futureTicket;
		private frmQuickTicket _quickTicket;
		private frmOptionTicket _optionTicket;
		private frmFutureMatrixTicket _futureMatrixTicket;

		private TicketTypes _ticketType = TicketTypes.None;

		private ComboBox cboSymbolDetails
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.cboSymbolDetails;
					case TicketTypes.Future:
						return _futureTicket.cboSymbolDetails;
					case TicketTypes.Quick:
						return _quickTicket.cboSymbolDetails;
					case TicketTypes.Option:
						return _optionTicket.cboSymbolDetails;
					case TicketTypes.FutureMatrix:
						return _futureMatrixTicket.cboSymbolDetails;
					case TicketTypes.AutoSpread:
					default:
						return null;
				}
			}
		}

		private DataTable symbolDetails
		{
			get
			{
				switch (_ticketType)
				{
					case TicketTypes.Stock:
						return _stockTicket.CurrentSymbolDetails;
					case TicketTypes.Future:
						return _futureTicket.CurrentSymbolDetails;
					case TicketTypes.Quick:
						return _quickTicket.CurrentSymbolDetails;
					case TicketTypes.Option:
						return _optionTicket.CurrentSymbolDetails;
					case TicketTypes.FutureMatrix:
						return _futureMatrixTicket.CurrentSymbolDetails;
					case TicketTypes.AutoSpread:
					default:
						return null;
				}
			}
		}

		#endregion

		#region - Constructor -

		public DropDownSymbolDetails(frmStockTicket stockTicket)
		{
			_stockTicket = stockTicket;
			_ticketType = TicketTypes.Stock;
		}

		public DropDownSymbolDetails(frmFutureTicket futureTicket)
		{
			_futureTicket = futureTicket;
			_ticketType = TicketTypes.Future;
		}

		public DropDownSymbolDetails(frmQuickTicket quickTicket)
		{
			_quickTicket = quickTicket;
			_ticketType = TicketTypes.Quick;
		}

		public DropDownSymbolDetails(frmOptionTicket optionTicket)
		{
			_optionTicket = optionTicket;
			_ticketType = TicketTypes.Option;
		}

		public DropDownSymbolDetails(frmFutureMatrixTicket futureMatrixTicket)
		{
			_futureMatrixTicket = futureMatrixTicket;
			_ticketType = TicketTypes.FutureMatrix;
		}

		#endregion

		#region - Symbol Details Selection -

		// This would reset the combox after every Enter and Delete key stroke
		private bool _newSymbolDetail = true;
		public bool NewSymbolDetail
		{
			get
			{
				return _newSymbolDetail;
			}
			set
			{
				_newSymbolDetail = value;
			}
		}

		private bool _undoSymbolDetail = false;
		public bool UndoSymbolDetail
		{
			get
			{
				return _undoSymbolDetail;
			}
			set
			{
				_undoSymbolDetail = value;
			}
		}

		// This is used to get Symbol Setting Defaults and Subscription.
		public string CurrentSymbolDetail
		{
			get
			{
				return cboSymbolDetails.Text.ToUpper().Trim();
			}
			set
			{
				if (cboSymbolDetails != null)
				{
					cboSymbolDetails.Text = value.ToUpper();
				}
			}
		}

		public void SymbolDetailsKeyUp(KeyEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.KeyCode)
			{
				case Keys.Enter:
					if (cboSymbolDetails.Text != "")
					{
						AddSymbolDetail(cboSymbolDetails.Text.ToUpper());
					}
					_newSymbolDetail = true;
					break;
				case Keys.Escape:
					_undoSymbolDetail = true;
					break;
				default:
					_newSymbolDetail = false;
					break;
			}
		}

		public void AddSymbolDetail(string symbolDetail)
		{
			if (symbolDetails != null)
			{
				if (symbolDetails.DefaultView.FindRows(symbolDetail).Length == 0)
				{
					symbolDetails.Rows.Add(symbolDetail);
				}

				CurrentSymbolDetail = symbolDetail;
			}
		}

		public void SymbolDetailsKeyDown(KeyEventArgs e)
		{
			GLOBAL.Interrupt = true;
			switch (e.KeyCode)
			{
				case Keys.Delete:
					DeleteSymbolDetail(cboSymbolDetails.Text.ToUpper());
					break;
				default:
					if (_newSymbolDetail)
					{
						// Set the Text to null
						_newSymbolDetail = false;
						CurrentSymbolDetail = "";
					}
					break;
			}

			// Disable Beep
			if (e.KeyCode == Keys.Enter)
			{
				e.SuppressKeyPress = true;
			}
		}

		public void DeleteSymbolDetail(string symbolDetail)
		{
			DataRowView[] rows = symbolDetails.DefaultView.FindRows(symbolDetail);

			foreach (DataRowView row in rows)
			{
				symbolDetails.Rows.Remove(row.Row);
			}

			// Set the Text to null
			CurrentSymbolDetail = "";
		}

		#endregion
	}
}
