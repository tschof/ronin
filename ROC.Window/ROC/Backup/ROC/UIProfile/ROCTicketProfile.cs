using System;
using System.Collections.Generic;
using System.Text;

using DictionaryEx;
using System.Data;
using System.Drawing;

namespace ROC
{
	[Serializable]
	public class ROCTicketProfile : MutiTypedUIDictionary, IDisposable
	{
		#region - IDisposable Members -

		public void Dispose()
		{
			SymbolDetails.Clear();
			ShortLenders.Clear();
			Instructions.Clear();

			Strings.Clear();
			Doubles.Clear();
			Longs.Clear();
			DateTimes.Clear();
			Bools.Clear();
			Colors.Clear();
			Fonts.Clear();
		}

		#endregion

		private List<string> _symbolDetails;
		public List<string> SymbolDetails
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
		public List<string> ShortLenders
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
		public List<string> Instructions
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

		public ROCTicketProfile()
		{
		}

		#region - Stock Ticket -

		public ROCTicketProfile(frmStockTicket ticket)
		{
			Export(ticket);
		}
		public ROCTicketProfile(frmStockTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		public ROCTicketProfile(frmStockTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		public void Export(frmStockTicket ticket)
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

			Update(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);
			Update(ROCTicketProfileFieldID.CurrentShortLender, ticket.CurrentShortLender);
			Update(ROCTicketProfileFieldID.CurrentInstruction, ticket.CurrentInstruction);

			Update(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Update(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Update(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);

			Update(ROCTicketProfileFieldID.IsLevel2, ticket.IsLevel2);
		}

		public void Import(frmStockTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				ticket.IsLoadingValue = true;

				if (clone)
				{
					if (prof.CurrentSymbolDetail != "")
					{
						ticket.CurrentSymbolDetail = prof.CurrentSymbolDetail;
					}
					if (prof.CurrentTradeFor != "")
					{
						ticket.CurrentTradeFor = prof.CurrentTradeFor;
					}
					if (prof.CurrentAccount != "")
					{
						ticket.CurrentAccount = prof.CurrentAccount;
					}
					if (prof.CurrentExchange != "")
					{
						ticket.CurrentExchange = prof.CurrentExchange;
					}
				}

				ticket.CurrentShortLender = prof.CurrentShortLender;
				ticket.CurrentInstruction = prof.CurrentInstruction;
				ticket.IsLevel2 = prof.IsLevel2;

				ticket.IsLoadingValue = false;
			}
		}

		#endregion

		#region - Future Ticket -

		public ROCTicketProfile(frmFutureTicket ticket)
		{
			Export(ticket);
		}
		public ROCTicketProfile(frmFutureTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		public ROCTicketProfile(frmFutureTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		public void Export(frmFutureTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			Update(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);

			Update(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Update(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Update(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);
		}

		public void Import(frmFutureTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				if (clone)
				{
					if (prof.CurrentSymbolDetail != "")
					{
						ticket.CurrentSymbolDetail = prof.CurrentSymbolDetail;
					}
					if (prof.CurrentTradeFor != "")
					{
						ticket.CurrentTradeFor = prof.CurrentTradeFor;
					}
					if (prof.CurrentAccount != "")
					{
						ticket.CurrentAccount = prof.CurrentAccount;
					}
					if (prof.CurrentExchange != "")
					{
						ticket.CurrentExchange = prof.CurrentExchange;
					}
				}
			}

			prof.Dispose();
		}

		#endregion

		#region - Quick Ticket -

		public ROCTicketProfile(frmQuickTicket ticket)
		{
			Export(ticket);
		}
		public ROCTicketProfile(frmQuickTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		public ROCTicketProfile(frmQuickTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		public void Export(frmQuickTicket ticket)
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

			Update(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);

			Update(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Update(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Update(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);

			Update(ROCTicketProfileFieldID.Opacity, ticket.TicketOpacity); 
			Update(ROCTicketProfileFieldID.BackColor, ticket.TicketBackColor);

			Update(ROCTicketProfileFieldID.ShowBidPrice, ticket.ShowBidPrice);
			Update(ROCTicketProfileFieldID.ShowBidSize, ticket.ShowBidSize);
			Update(ROCTicketProfileFieldID.ShowAskPrice, ticket.ShowAskPrice);
			Update(ROCTicketProfileFieldID.ShowAskSize, ticket.ShowAskSize);
			Update(ROCTicketProfileFieldID.ShowNetChange, ticket.ShowNetChange);
			Update(ROCTicketProfileFieldID.ShowPctChange, ticket.ShowPctChange);
			Update(ROCTicketProfileFieldID.ShowTotalVolume, ticket.ShowTotalVolume);
			Update(ROCTicketProfileFieldID.ShowTradedVolume, ticket.ShowTradedVolume);
			Update(ROCTicketProfileFieldID.ShowLowPrice, ticket.ShowLowPrice);
			Update(ROCTicketProfileFieldID.ShowHighPrice, ticket.ShowHighPrice);
			Update(ROCTicketProfileFieldID.ShowExchange, ticket.ShowExchange);

			Update(ROCTicketProfileFieldID.ShowTicker, ticket.ShowTicker);

			Update(ROCTicketProfileFieldID.ShowQty, ticket.ShowQty);
			Update(ROCTicketProfileFieldID.ShowOrder, ticket.ShowOrder);
			Update(ROCTicketProfileFieldID.ShowLimitPrice, ticket.ShowLimitPrice);
			Update(ROCTicketProfileFieldID.ShowStopPrice, ticket.ShowStopPrice);
			Update(ROCTicketProfileFieldID.ShowDuration, ticket.ShowDuration);
			Update(ROCTicketProfileFieldID.ShowDuration, ticket.ShowDuration);

			Update(ROCTicketProfileFieldID.ShowOrderInfo, ticket.ShowOrderInfo);

			Update(ROCTicketProfileFieldID.ShowCommand, ticket.ShowCommand);

			Update(ROCTicketProfileFieldID.ShowIncrement, ticket.ShowIncrement);

			Update(ROCTicketProfileFieldID.ShowSelectedAccountOnly, ticket.ShowSelectedAccountOnly);
		}

		public void Import(frmQuickTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				if (clone)
				{
					if (prof.CurrentSymbolDetail != "")
					{
						ticket.CurrentSymbolDetail = prof.CurrentSymbolDetail;
					}
					if (prof.CurrentTradeFor != "")
					{
						ticket.CurrentTradeFor = prof.CurrentTradeFor;
					}
					if (prof.CurrentAccount != "")
					{
						ticket.CurrentAccount = prof.CurrentAccount;
					}
					if (prof.CurrentExchange != "")
					{
						ticket.CurrentExchange = prof.CurrentExchange;
					}
				}

				ticket.TicketOpacity = prof.TicketOpacity;
				ticket.TicketBackColor = prof.TicketBackColor;

				ticket.ShowBidPrice = prof.ShowBidPrice;
				ticket.ShowBidSize = prof.ShowBidSize;
				ticket.ShowAskPrice = prof.ShowAskPrice;
				ticket.ShowAskSize = prof.ShowAskSize;
				ticket.ShowNetChange = prof.ShowNetChange;
				ticket.ShowPctChange = prof.ShowPctChange;
				ticket.ShowTotalVolume = prof.ShowTotalVolume;
				ticket.ShowTradedVolume = prof.ShowTradedVolume;
				ticket.ShowLowPrice = prof.ShowLowPrice;
				ticket.ShowHighPrice = prof.ShowHighPrice;
				ticket.ShowPrevClosePrice = prof.ShowPrevClosePrice;

				ticket.ShowTicker = prof.ShowTicker;

				ticket.ShowQty = prof.ShowQty;
				ticket.ShowOrder = prof.ShowOrder;
				ticket.ShowLimitPrice = prof.ShowLimitPrice;
				ticket.ShowStopPrice = prof.ShowStopPrice;
				ticket.ShowDuration = prof.ShowDuration;
				ticket.ShowExchange = prof.ShowExchange;

				ticket.ShowOrderInfo = prof.ShowOrderInfo;

				ticket.ShowCommand = prof.ShowCommand;

				ticket.ShowIncrement = prof.ShowIncrement;

				ticket.ShowSelectedAccountOnly = prof.ShowSelectedAccountOnly;
			}

			prof.Dispose();
		}

		#endregion

		#region - Option Ticket -

		public ROCTicketProfile(frmOptionTicket ticket)
		{
			Export(ticket);
		}
		public ROCTicketProfile(frmOptionTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		public ROCTicketProfile(frmOptionTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		public void Export(frmOptionTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			Update(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);

			Update(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Update(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Update(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);
			
			Update(ROCTicketProfileFieldID.StrikesToShow, ticket.StrikesToShow);
			Update(ROCTicketProfileFieldID.ExpirationsToShow, ticket.ExpirationsToShow);
			Update(ROCTicketProfileFieldID.ShowMonthlyExpirationsOnly, ticket.ShowMonthlyExpirationsOnly);
		}

		public void Import(frmOptionTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				if (clone)
				{
					if (prof.CurrentSymbolDetail != "")
					{
						ticket.CurrentSymbolDetail = prof.CurrentSymbolDetail;
					}
					if (prof.CurrentTradeFor != "")
					{
						ticket.CurrentTradeFor = prof.CurrentTradeFor;
					}
					if (prof.CurrentAccount != "")
					{
						ticket.CurrentAccount = prof.CurrentAccount;
					}
					if (prof.CurrentExchange != "")
					{
						ticket.CurrentExchange = prof.CurrentExchange;
					}
				}

				ticket.StrikesToShow = prof.StrikesToShow;
				ticket.ExpirationsToShow = prof.ExpirationsToShow;
				ticket.ShowMonthlyExpirationsOnly = prof.ShowMonthlyExpirationsOnly;
			}

			prof.Dispose();
		}

		#endregion

		#region - Auto Spread Ticket -

		public ROCTicketProfile(frmAutoSpreadTicket ticket)
		{
			Export(ticket);
		}
		public ROCTicketProfile(frmAutoSpreadTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		public ROCTicketProfile(frmAutoSpreadTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		public void Export(frmAutoSpreadTicket ticket)
		{
			Update(ROCTicketProfileFieldID.CurrentTradeFor, ticket.CurrentTradeFor);
			Update(ROCTicketProfileFieldID.CurrentAccount, ticket.CurrentAccount);
			Update(ROCTicketProfileFieldID.CurrentExchange, ticket.CurrentExchange);
		}

		public void Import(frmAutoSpreadTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				ticket.CurrentTradeFor = prof.CurrentTradeFor;
				ticket.CurrentAccount = prof.CurrentAccount;
				ticket.CurrentExchange = prof.CurrentExchange;
			}

			prof.Dispose();
		}

		#endregion

		#region - Future Matrix Ticket -

		public ROCTicketProfile(frmFutureMatrixTicket ticket)
		{
			Export(ticket);
		}
		public ROCTicketProfile(frmFutureMatrixTicket ticket, ROCTicketProfile prof)
		{
			Import(ticket, prof, true);
		}
		public ROCTicketProfile(frmFutureMatrixTicket ticket, ROCTicketProfile prof, bool clone)
		{
			Import(ticket, prof, clone);
		}

		public void Export(frmFutureMatrixTicket ticket)
		{
			foreach (DataRow row in ticket.CurrentSymbolDetails.Rows)
			{
				if (row["SymbolDetail"] != DBNull.Value && row["SymbolDetail"].ToString() != "")
				{
					SymbolDetails.Add(row["SymbolDetail"].ToString());
				}
			}

			Update(ROCTicketProfileFieldID.CurrentSymbolDetail, ticket.CurrentSymbolDetail);
		}

		public void Import(frmFutureMatrixTicket ticket, ROCTicketProfile prof, bool clone)
		{
			lock (prof)
			{
				if (clone)
				{
					if (prof.CurrentSymbolDetail != "")
					{
						ticket.CurrentSymbolDetail = prof.CurrentSymbolDetail;
					}
				}
			}

			prof.Dispose();
		}

		#endregion

		#region - Ticket -

		public string CurrentSymbolDetail
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.CurrentSymbolDetail))
				{
					return Strings[ROCTicketProfileFieldID.CurrentSymbolDetail];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.CurrentSymbolDetail, value);
			}
		}

		public string CurrentShortLender
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.CurrentShortLender))
				{
					return Strings[ROCTicketProfileFieldID.CurrentShortLender];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.CurrentShortLender, value);
			}
		}

		public string CurrentInstruction
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.CurrentInstruction))
				{
					return Strings[ROCTicketProfileFieldID.CurrentInstruction];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.CurrentInstruction, value);
			}
		}

		public string CurrentTradeFor
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.CurrentTradeFor))
				{
					return Strings[ROCTicketProfileFieldID.CurrentTradeFor];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.CurrentTradeFor, value);
			}
		}

		public string CurrentAccount
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.CurrentAccount))
				{
					return Strings[ROCTicketProfileFieldID.CurrentAccount];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.CurrentAccount, value);
			}
		}

		public string CurrentExchange
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.CurrentExchange))
				{
					return Strings[ROCTicketProfileFieldID.CurrentExchange];
				}
				else
				{
					return "";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.CurrentExchange, value);
			}
		}

		public bool IsLevel2
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.IsLevel2))
				{
					return Bools[ROCTicketProfileFieldID.IsLevel2];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.IsLevel2, value);
			}
		}

		public string StrikesToShow
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.StrikesToShow))
				{
					return Strings[ROCTicketProfileFieldID.StrikesToShow];
				}
				else
				{
					return "ALL";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.StrikesToShow, value);
			}
		}

		public string ExpirationsToShow
		{
			get
			{
				if (Strings.ContainsKey(ROCTicketProfileFieldID.ExpirationsToShow))
				{
					return Strings[ROCTicketProfileFieldID.ExpirationsToShow];
				}
				else
				{
					return "ALL";
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ExpirationsToShow, value);
			}
		}

		public bool ShowMonthlyExpirationsOnly
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowMonthlyExpirationsOnly))
				{
					return Bools[ROCTicketProfileFieldID.ShowMonthlyExpirationsOnly];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowMonthlyExpirationsOnly, value);
			}
		}

		public double TicketOpacity
		{
			get
			{
				if (Doubles.ContainsKey(ROCTicketProfileFieldID.Opacity))
				{
					return Doubles[ROCTicketProfileFieldID.Opacity];
				}
				else
				{
					return 1;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.Opacity, value);
			}
		}

		public Color TicketBackColor
		{
			get
			{
				if (Colors.ContainsKey(ROCTicketProfileFieldID.BackColor))
				{
					return Colors[ROCTicketProfileFieldID.BackColor];
				}
				else
				{
					return Color.LightSteelBlue;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.BackColor, value);
			}
		}

		public bool ShowBidPrice
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowBidPrice))
				{
					return Bools[ROCTicketProfileFieldID.ShowBidPrice];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowBidPrice, value);
			}
		}

		public bool ShowBidSize
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowBidSize))
				{
					return Bools[ROCTicketProfileFieldID.ShowBidSize];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowBidSize, value);
			}
		}

		public bool ShowAskPrice
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowAskPrice))
				{
					return Bools[ROCTicketProfileFieldID.ShowAskPrice];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowAskPrice, value);
			}
		}

		public bool ShowAskSize
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowAskSize))
				{
					return Bools[ROCTicketProfileFieldID.ShowAskSize];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowAskSize, value);
			}
		}

		public bool ShowNetChange
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowNetChange))
				{
					return Bools[ROCTicketProfileFieldID.ShowNetChange];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowNetChange, value);
			}
		}

		public bool ShowPctChange
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowPctChange))
				{
					return Bools[ROCTicketProfileFieldID.ShowPctChange];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowPctChange, value);
			}
		}

		public bool ShowTotalVolume
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowTotalVolume))
				{
					return Bools[ROCTicketProfileFieldID.ShowTotalVolume];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowTotalVolume, value);
			}
		}

		public bool ShowTradedVolume
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowTradedVolume))
				{
					return Bools[ROCTicketProfileFieldID.ShowTradedVolume];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowTradedVolume, value);
			}
		}

		public bool ShowLowPrice
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowLowPrice))
				{
					return Bools[ROCTicketProfileFieldID.ShowLowPrice];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowLowPrice, value);
			}
		}

		public bool ShowHighPrice
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowHighPrice))
				{
					return Bools[ROCTicketProfileFieldID.ShowHighPrice];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowHighPrice, value);
			}
		}

		public bool ShowPrevClosePrice
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowPrevClosePrice))
				{
					return Bools[ROCTicketProfileFieldID.ShowPrevClosePrice];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowPrevClosePrice, value);
			}
		}

		public bool ShowTicker
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowTicker))
				{
					return Bools[ROCTicketProfileFieldID.ShowTicker];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowTicker, value);
			}
		}

		public bool ShowQty
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowQty))
				{
					return Bools[ROCTicketProfileFieldID.ShowQty];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowQty, value);
			}
		}

		public bool ShowOrder
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowOrder))
				{
					return Bools[ROCTicketProfileFieldID.ShowOrder];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowOrder, value);
			}
		}

		public bool ShowLimitPrice
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowLimitPrice))
				{
					return Bools[ROCTicketProfileFieldID.ShowLimitPrice];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowLimitPrice, value);
			}
		}

		public bool ShowStopPrice
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowStopPrice))
				{
					return Bools[ROCTicketProfileFieldID.ShowStopPrice];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowStopPrice, value);
			}
		}

		public bool ShowDuration
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowDuration))
				{
					return Bools[ROCTicketProfileFieldID.ShowDuration];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowDuration, value);
			}
		}

		public bool ShowExchange
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowExchange))
				{
					return Bools[ROCTicketProfileFieldID.ShowExchange];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowExchange, value);
			}
		}

		public bool ShowOrderInfo
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowOrderInfo))
				{
					return Bools[ROCTicketProfileFieldID.ShowOrderInfo];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowOrderInfo, value);
			}
		}

		public bool ShowCommand
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowCommand))
				{
					return Bools[ROCTicketProfileFieldID.ShowCommand];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowCommand, value);
			}
		}

		public bool ShowIncrement
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowIncrement))
				{
					return Bools[ROCTicketProfileFieldID.ShowIncrement];
				}
				else
				{
					return true;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowIncrement, value);
			}
		}

		public bool ShowSelectedAccountOnly
		{
			get
			{
				if (Bools.ContainsKey(ROCTicketProfileFieldID.ShowSelectedAccountOnly))
				{
					return Bools[ROCTicketProfileFieldID.ShowSelectedAccountOnly];
				}
				else
				{
					return false;
				}
			}
			set
			{
				Update(ROCTicketProfileFieldID.ShowSelectedAccountOnly, value);
			}
		}
		
		#endregion
	}
}
