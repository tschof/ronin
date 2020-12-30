using System;
using System.Collections.Generic;
using System.Data;

using DictionaryEx;

namespace ROC
{
	[Serializable]
	internal class ROCTicketProfile : MultiTypedUIDictionary, IDisposable
	{
		#region - IDisposable Members -

		public void Dispose()
		{
			SymbolDetails.Clear();
			ShortLenders.Clear();
			Instructions.Clear();
			base.Clear();
		}

		#endregion

		private List<string> _symbolDetails;
		internal List<string> SymbolDetails
		{
			get
			{
				if (_symbolDetails == null)
				{
					_symbolDetails = new List<string>();
				}
				return _symbolDetails;
			}
			set
			{
				_symbolDetails = value;
			}
		}

		private List<string> _shortLenders;
		internal List<string> ShortLenders
		{
			get
			{
				if (_shortLenders == null)
				{
					_shortLenders = new List<string>();
				}
				return _shortLenders;
			}
			set
			{
				_shortLenders = value;
			}
		}

		private List<string> _instructions;
		internal List<string> Instructions
		{
			get
			{
				if (_instructions == null)
				{
					_instructions = new List<string>();
				}
				return _instructions;
			}
			set
			{
				_instructions = value;
			}
		}

		internal ROCTicketProfile()
		{
		}

		#region - Stock Ticket -

		internal ROCTicketProfile(frmStockTicket ticket)
		{
			Export(ticket);
		}
		internal ROCTicketProfile(frmStockTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		internal ROCTicketProfile(frmStockTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		internal void Export(frmStockTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			foreach (DataRow row in ticket.ShortLenders.Rows)
			{
				if (row["ShortLender"] != DBNull.Value && row["ShortLender"].ToString() != "")
				{
					ShortLenders.Add(row["ShortLender"].ToString());
				}
			}

			foreach (DataRow row in ticket.Instructions.Rows)
			{
				if (row["Instruction"] != DBNull.Value && row["Instruction"].ToString() != "")
				{
					Instructions.Add(row["Instruction"].ToString());
				}
			}

			Set(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);
			Set(ROCTicketProfileFieldID.CurrentShortLender, ticket.CurrentShortLender);
			Set(ROCTicketProfileFieldID.CurrentInstruction, ticket.CurrentInstruction);

			Set(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Set(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Set(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);

			Set(ROCTicketProfileFieldID.IsLevel2, ticket.IsLevel2);
		}

		internal void Import(frmStockTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof) {
				ticket.IsLoadingValue = true;
				string sval;

				if (clone) {
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentSymbolDetail, out sval))
						ticket.CurrentSymbolDetail = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentTradeFor, out sval))
						ticket.CurrentTradeFor = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentAccount, out sval))
						ticket.CurrentAccount = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentExchange, out sval))
						ticket.CurrentExchange = sval;
				}

				if (prof.TryGet(ROCTicketProfileFieldID.CurrentShortLender, out sval))
					ticket.CurrentShortLender = sval;
				if (prof.TryGet(ROCTicketProfileFieldID.CurrentInstruction, out sval))
					ticket.CurrentInstruction = sval;
				if (prof.TryGet(ROCTicketProfileFieldID.IsLevel2, out bool bval))
					ticket.IsLevel2 = bval;
				ticket.IsLoadingValue = false;
			}
		}

		#endregion

		#region - Future Ticket -

		internal ROCTicketProfile(frmFutureTicket ticket)
		{
			Export(ticket);
		}
		internal ROCTicketProfile(frmFutureTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		internal ROCTicketProfile(frmFutureTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		internal void Export(frmFutureTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			Set(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);
			Set(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Set(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Set(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);
		}

		internal void Import(frmFutureTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				if (clone)
				{
					string sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentSymbolDetail, out sval))
						ticket.CurrentSymbolDetail = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentTradeFor, out sval))
						ticket.CurrentTradeFor = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentAccount, out sval))
						ticket.CurrentAccount = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentExchange, out sval))
						ticket.CurrentExchange = sval;
				}
			}

			prof.Dispose();
		}

		#endregion

		#region - Quick Ticket -

		internal ROCTicketProfile(frmQuickTicket ticket)
		{
			Export(ticket);
		}
		internal ROCTicketProfile(frmQuickTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		internal ROCTicketProfile(frmQuickTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		internal void Export(frmQuickTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			foreach (DataRow row in ticket.ShortLenders.Rows)
			{
				if (row["ShortLender"] != DBNull.Value && row["ShortLender"].ToString() != "")
				{
					ShortLenders.Add(row["ShortLender"].ToString());
				}
			}

			Set(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);
			Set(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Set(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Set(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);
			Set(ROCTicketProfileFieldID.Opacity, ticket.TicketOpacity); 
			Set(ROCTicketProfileFieldID.BackColor, ticket.TicketBackColor);
			Set(ROCTicketProfileFieldID.ShowBidPrice, ticket.ShowBidPrice);
			Set(ROCTicketProfileFieldID.ShowBidSize, ticket.ShowBidSize);
			Set(ROCTicketProfileFieldID.ShowAskPrice, ticket.ShowAskPrice);
			Set(ROCTicketProfileFieldID.ShowAskSize, ticket.ShowAskSize);
			Set(ROCTicketProfileFieldID.ShowNetChange, ticket.ShowNetChange);
			Set(ROCTicketProfileFieldID.ShowPctChange, ticket.ShowPctChange);
			Set(ROCTicketProfileFieldID.ShowTotalVolume, ticket.ShowTotalVolume);
			Set(ROCTicketProfileFieldID.ShowTradedVolume, ticket.ShowTradedVolume);
			Set(ROCTicketProfileFieldID.ShowLowPrice, ticket.ShowLowPrice);
			Set(ROCTicketProfileFieldID.ShowHighPrice, ticket.ShowHighPrice);
			Set(ROCTicketProfileFieldID.ShowExchange, ticket.ShowExchange);
			Set(ROCTicketProfileFieldID.ShowTicker, ticket.ShowTicker);
			Set(ROCTicketProfileFieldID.ShowQty, ticket.ShowQty);
			Set(ROCTicketProfileFieldID.ShowOrder, ticket.ShowOrder);
			Set(ROCTicketProfileFieldID.ShowLimitPrice, ticket.ShowLimitPrice);
			Set(ROCTicketProfileFieldID.ShowStopPrice, ticket.ShowStopPrice);
			Set(ROCTicketProfileFieldID.ShowDuration, ticket.ShowDuration);
			Set(ROCTicketProfileFieldID.ShowDuration, ticket.ShowDuration);
			Set(ROCTicketProfileFieldID.ShowOrderInfo, ticket.ShowOrderInfo);
			Set(ROCTicketProfileFieldID.ShowCommand, ticket.ShowCommand);
			Set(ROCTicketProfileFieldID.ShowIncrement, ticket.ShowIncrement);
			Set(ROCTicketProfileFieldID.ShowSelectedAccountOnly, ticket.ShowSelectedAccountOnly);
		}

		internal void Import(frmQuickTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				if (clone)
				{
					string sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentSymbolDetail, out sval))
						ticket.CurrentSymbolDetail = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentTradeFor, out sval))
						ticket.CurrentTradeFor = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentAccount, out sval))
						ticket.CurrentAccount = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentExchange, out sval))
						ticket.CurrentExchange = sval;
				}

				bool bval;
				if (prof.TryGet(ROCTicketProfileFieldID.Opacity, out double opacity))
					ticket.TicketOpacity = opacity;
				if (prof.TryGet(ROCTicketProfileFieldID.BackColor, out System.Drawing.Color color))
					ticket.TicketBackColor = color;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowBidPrice, out bval))
					ticket.ShowBidPrice = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowBidSize, out bval))
					ticket.ShowBidSize = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowAskPrice, out bval))
					ticket.ShowAskPrice = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowAskSize, out bval))
					ticket.ShowAskSize = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowNetChange, out bval))
					ticket.ShowNetChange = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowPctChange, out bval))
					ticket.ShowPctChange = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowTotalVolume, out bval))
					ticket.ShowTotalVolume = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowTradedVolume, out bval))
					ticket.ShowTradedVolume = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowLowPrice, out bval))
					ticket.ShowLowPrice = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowHighPrice, out bval))
					ticket.ShowHighPrice = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowPrevClosePrice, out bval))
					ticket.ShowPrevClosePrice = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowTicker, out bval))
					ticket.ShowTicker = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowQty, out bval))
					ticket.ShowQty = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowOrder, out bval))
					ticket.ShowOrder = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowLimitPrice, out bval))
					ticket.ShowLimitPrice = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowStopPrice, out bval))
					ticket.ShowStopPrice = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowDuration, out bval))
					ticket.ShowDuration = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowExchange, out bval))
					ticket.ShowExchange = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowOrderInfo, out bval))
					ticket.ShowOrderInfo = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowCommand, out bval))
					ticket.ShowCommand = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowIncrement, out bval))
					ticket.ShowIncrement = bval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowSelectedAccountOnly, out bval))
					ticket.ShowSelectedAccountOnly = bval;
			}

			prof.Dispose();
		}

		#endregion

		#region - Option Ticket -

		internal ROCTicketProfile(frmOptionTicket ticket)
		{
			Export(ticket);
		}
		internal ROCTicketProfile(frmOptionTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		internal ROCTicketProfile(frmOptionTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		internal void Export(frmOptionTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			Set(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);

			Set(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Set(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Set(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);
			
			Set(ROCTicketProfileFieldID.StrikesToShow, ticket.StrikesToShow);
			Set(ROCTicketProfileFieldID.ExpirationsToShow, ticket.ExpirationsToShow);
			Set(ROCTicketProfileFieldID.ShowMonthlyExpirationsOnly, ticket.ShowMonthlyExpirationsOnly);
		}

		internal void Import(frmOptionTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				string sval;

				if (clone)
				{
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentSymbolDetail, out sval))
						ticket.CurrentSymbolDetail = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentTradeFor, out sval))
						ticket.CurrentTradeFor = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentAccount, out sval))
						ticket.CurrentAccount = sval;
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentExchange, out sval))
						ticket.CurrentExchange = sval;
				}

				if (prof.TryGet(ROCTicketProfileFieldID.StrikesToShow, out sval))
					ticket.StrikesToShow = sval;
				if (prof.TryGet(ROCTicketProfileFieldID.ExpirationsToShow, out sval))
					ticket.ExpirationsToShow = sval;
				if (prof.TryGet(ROCTicketProfileFieldID.ShowMonthlyExpirationsOnly, out bool bval))
					ticket.ShowMonthlyExpirationsOnly = bval;
			}

			prof.Dispose();
		}

		#endregion

		#region - Auto Spread Ticket -

		internal ROCTicketProfile(frmAutoSpreadTicket ticket)
		{
			Export(ticket);
		}
		internal ROCTicketProfile(frmAutoSpreadTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		internal ROCTicketProfile(frmAutoSpreadTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		internal void Export(frmAutoSpreadTicket ticket)
		{
			Set(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Set(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Set(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);
		}

		internal void Import(frmAutoSpreadTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				string sval;
				if (prof.TryGet(ROCTicketProfileFieldID.CurrentTradeFor, out sval))
					ticket.CurrentTradeFor = sval;
				if (prof.TryGet(ROCTicketProfileFieldID.CurrentAccount, out sval))
					ticket.CurrentAccount = sval;
				if (prof.TryGet(ROCTicketProfileFieldID.CurrentExchange, out sval))
					ticket.CurrentExchange = sval;
			}

			prof.Dispose();
		}

		#endregion

		#region - Future Matrix Ticket -

		internal ROCTicketProfile(frmFutureMatrixTicket ticket)
		{
			Export(ticket);
		}
		internal ROCTicketProfile(frmFutureMatrixTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		internal ROCTicketProfile(frmFutureMatrixTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		internal void Export(frmFutureMatrixTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			Set(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);
		}

		internal void Import(frmFutureMatrixTicket ticket, ROCTicketProfile prof, bool clone)
		{
			if (clone) {
				lock (prof)
				{
					if (prof.TryGet(ROCTicketProfileFieldID.CurrentSymbolDetail, out string sval))
						ticket.CurrentSymbolDetail = sval;
				}
			}

			prof.Dispose();
		}

		#endregion
	}
}
