using System;
using System.Collections.Generic;
using System.Text;
using CSVEx;
using System.Data;

namespace ROMEx
{
	public class RomBasicOrder
	{
		public RomBasicOrder()
		{
		}

		public RomBasicOrder(RomBasicOrder order)
		{
			trader = order.trader;
			side = order.side;
			symbol = order.symbol;
			mdSymbol = order.mdSymbol;
			shares = order.shares;
			price = order.price;
			orderType = order.orderType;
			duration = order.duration;
			capacity = order.capacity;
			clearingFirmID = order.clearingFirmID;
			clearingAcctID = order.clearingAcctID;
			exchangeID = order.exchangeID;
			homeExchange = order.homeExchange;
			status = order.status;
			instructions = order.instructions;
			stopPrice = order.stopPrice;
			discretionary = order.discretionary;
			localAcctAcrn = order.localAcctAcrn;
			firmArc = order.firmArc;
			execInstructions = order.execInstructions;
			tradeFor = order.tradeFor;
			shortLender = order.shortLender;
			show = order.show;
			minQty = order.minQty;
			secType = order.secType;
			algoType = order.algoType;
			algoExchangeID = order.algoExchangeID;
			occAcct = order.occAcct;
			cmtaFirmID = order.cmtaFirmID;
			multiplier = order.multiplier;

			expDate = order.expDate;
			maturityDay = order.maturityDay;
			underlying = order.underlying;
			strike = order.strike;
			callPut = order.callPut;
			openClose = order.openClose;

			parentTag = order.parentTag;
			cplxOrderType = order.cplxOrderType;
			clientEcho = order.clientEcho;

			orderID = "";

			startTime = order.startTime;
			endTime = order.endTime;

			programTrade = order.programTrade;
			pegPrice = order.pegPrice;
			displayInstruction = order.displayInstruction;
		}

		public string trader = "";
		public string side = "";
		public string symbol = "";
		public string mdSymbol = "";
		public string shares = "";
		public string price = "";
		public string orderType = "";
		public string duration = "";
		public string capacity = "";
		public string clearingFirmID = "";  // clearingID
		public string clearingAcctID = "";
		public string exchangeID = "";
		public string homeExchange = "";
		public string status = "0";
		public string instructions = "";
		public string stopPrice = "";
		public string discretionary = "N";
		public string localAcctAcrn = "";
		public string firmArc = "";
		public string execInstructions = "";
		public string tradeFor = "";
		public string shortLender = "";
		public string show = "";
		public string minQty = "";
		public string secType = "E";
		public string algoType = "";
		public string algoExchangeID = "";
		public string occAcct = "";
		public string cmtaFirmID = "";
		public string multiplier = "";

		public string expDate = "";
		public string maturityDay = "";
		public string underlying = "";
		public string strike = "";
		public string callPut = "";
		public string openClose = "";

		public string parentTag = "";
		public string cplxOrderType = "";
		public string clientEcho = "";

		public string orderID = "";

		public string startTime = "";
		public string endTime = "";

		public string programTrade = "";
		public string pegPrice = "";
		public string displayInstruction = "";
	}

	public class RomBasicReplace
	{
		public RomBasicReplace()
		{
		}

		public RomBasicReplace(RomBasicReplace order)
		{
			trader = order.trader;
			side = order.side;
			symbol = order.symbol;
			newShares = order.newShares;
			newPrice = order.newPrice;

			duration = order.duration;
			newStopPrice = order.newStopPrice;

			//orgShares = order.orgShares;
			//cumShares = order.cumShares;
			//leaveShares = order.leaveShares;

			//execInstructions = order.execInstructions;
			//shortLender = order.shortLender;

			orderID = order.orderID;
			tradeFor = order.tradeFor;
		}

		public string trader = "";
		public string side = "";
		public string symbol = "";
		public string newShares = "";
		public string newPrice = "";
		
		public string duration = "";
		public string newStopPrice = "";

		//public string orgShares = "";
		//public string cumShares = "";
		//public string leaveShares = "";

		//public string execInstructions = "";
		//public string shortLender = "";

		public string orderID = "";
		public string tradeFor = "";

		public string newPagPrice = "";
	}

	public class RomBasicCancel
	{
		public string side = "";
		public string mdSymbol = "";
		public string shares = "";
		public string price = "";
		public string orderID = "";
	}

	public class ROMMessageMaker
	{
		//private bool _simulatorFlag = false;
		//public bool SimulatorFlag
		//{
		//    set
		//    {
		//        _simulatorFlag = value;
		//    }
		//}

		//private string _baseLoginMsg = "L,{0},{1},0,,,,{2}\n";
		public string GetLogin(string username, string password, bool cancelOnDisconnect, bool skipGTCandGTD)
		{
			CSV csv = new CSV();

			csv.Fields = new string[8];
			csv.SetAt(CSVFieldIDs.CMD.L.Command, CSVFieldIDs.CommandTypes.Login);
			csv.SetAt(CSVFieldIDs.CMD.L.LoginName, username);
			if (cancelOnDisconnect)
			{
				if (skipGTCandGTD)
				{
					csv.SetAt(CSVFieldIDs.CMD.L.CancelChoice, CSVFieldIDs.CancelChoices.CancelAllLiveOrderExceptGTD_GTC);
				}
				else
				{
					csv.SetAt(CSVFieldIDs.CMD.L.CancelChoice, CSVFieldIDs.CancelChoices.CancelAllLiveOrderOnDisconnection);
				}
			}
			else
			{
				csv.SetAt(CSVFieldIDs.CMD.L.CancelChoice, CSVFieldIDs.CancelChoices.KeepOrdersAlive);
			}
			csv.SetAt(CSVFieldIDs.CMD.L.Password, password);
			
			csv.Encode();
			return csv.EncodeMessage;
			//return string.Format(_baseLoginMsg, DateTime.Now.ToString("HHmmss"), username, password);
		}

		public string GetOrder(RomBasicOrder order, ref CSV csv)
		{
			csv = new CSV();

			csv.Fields = new string[78];
			csv.SetAt(CSVFieldIDs.CMD.E.Command, CSVFieldIDs.CommandTypes.EnterOrder);
			csv.SetAt(CSVFieldIDs.CMD.E.Trader, order.trader);
			if (order.orderID == "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.Tag, GetOrderID(order.trader));
			}
			else
			{
				csv.SetAt(CSVFieldIDs.CMD.E.Tag, order.orderID);
			}
			csv.SetAt(CSVFieldIDs.CMD.E.Side, order.side);
			csv.SetAt(CSVFieldIDs.CMD.E.Symbol, order.symbol);
			csv.SetAt(CSVFieldIDs.CMD.E.SecurityDefinition, order.mdSymbol);
			csv.SetAt(CSVFieldIDs.CMD.E.Shares, order.shares);
			csv.SetAt(CSVFieldIDs.CMD.E.Type, order.orderType);
			csv.SetAt(CSVFieldIDs.CMD.E.TIF, order.duration);
			csv.SetAt(CSVFieldIDs.CMD.E.Capacity, order.capacity);
			csv.SetAt(CSVFieldIDs.CMD.E.ClearingID, order.clearingFirmID);
			csv.SetAt(CSVFieldIDs.CMD.E.ClearingAcct, order.clearingAcctID);
			csv.SetAt(CSVFieldIDs.CMD.E.Discretionary, order.discretionary);
			csv.SetAt(CSVFieldIDs.CMD.E.ExchangeID, order.exchangeID);
			csv.SetAt(CSVFieldIDs.CMD.E.CMTAAccount, order.cmtaFirmID);
			csv.SetAt(CSVFieldIDs.CMD.E.LocalAcct, order.localAcctAcrn);
			csv.SetAt(CSVFieldIDs.CMD.E.Firm, order.firmArc);
			csv.SetAt(CSVFieldIDs.CMD.E.TradeFor, order.tradeFor);
			csv.SetAt(CSVFieldIDs.CMD.E.ShortLender, order.shortLender);
			csv.SetAt(CSVFieldIDs.CMD.E.Instructions, order.instructions);
			csv.SetAt(CSVFieldIDs.CMD.E.ExecutionInstruction, order.execInstructions);
			csv.SetAt(CSVFieldIDs.CMD.E.SecType, order.secType);
			csv.SetAt(CSVFieldIDs.CMD.E.Price, order.price);
			csv.SetAt(CSVFieldIDs.CMD.E.StopPrice, order.stopPrice);
			csv.SetAt(CSVFieldIDs.CMD.E.MarketMakerAccount, order.occAcct);
			csv.SetAt(CSVFieldIDs.CMD.E.MarketMakerHomeExchange, order.homeExchange);
			csv.SetAt(CSVFieldIDs.CMD.E.Multiplier, order.multiplier);
			csv.SetAt(CSVFieldIDs.CMD.E.MinQty, order.minQty);

			if (order.algoType != "" || order.algoType == "0")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.AlgoType, order.algoType);
			}

			if (order.algoExchangeID != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.AlgoExchangeID, order.algoExchangeID);
			}

			if (order.pegPrice != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.PegPriceOffset, order.pegPrice);
			}

			if (order.programTrade != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.ProgramTrade, order.programTrade);
			}

			if (order.startTime != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.BeginTime, order.startTime);
			}

			if (order.endTime != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.EndTime, order.endTime);
			}
			
			if (order.show != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.Floor, order.show);
			}

			if (order.callPut != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.PutCall, order.callPut);
			}

			if (order.openClose != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.OpenClose, order.openClose);
			}

			if (order.strike != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.StrikePrice, order.strike);
			}

			if (order.expDate != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.ExpDate, order.expDate);
			}

			if (order.maturityDay != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.MaturityDay, order.maturityDay);
			}

			if (order.underlying != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.Underlying, order.underlying);
			}

			if (order.parentTag != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.ParentTag, order.parentTag);
			}

			if (order.cplxOrderType != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.CplxOrderType, order.cplxOrderType);
			}

			if (order.clientEcho != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.ClientEcho, order.clientEcho);
			}

			if (order.displayInstruction != "")
			{
				csv.SetAt(CSVFieldIDs.CMD.E.DisplayInstruction, order.displayInstruction);
			}

			//if (_simulatorFlag)
			//{
			//    // TEST ONLY!!!!!!!!!!!
			//    csv.SetAt(CSVFieldIDs.CMD.E.ExecutionInstruction, "X");
			//}

			csv.Encode();
			return csv.EncodeMessage;
		}

		private long _orderIDSeed = 0;
		public string GetOrderID(string trader)
		{
			_orderIDSeed = _orderIDSeed + 1;
			if (_orderIDSeed > 9999)
			{
				_orderIDSeed = 1;
			}

			return string.Concat(trader, DateTime.Now.ToString("MMddHHmmss"), _orderIDSeed);
		}

		public string GetReplace(RomBasicReplace replace)
		{
			CSV csv = new CSV();

			csv.Fields = new string[65];
			csv.SetAt(CSVFieldIDs.CMD.R.Command, CSVFieldIDs.CommandTypes.Replace);
			csv.SetAt(CSVFieldIDs.CMD.R.Trader, replace.trader);
			csv.SetAt(CSVFieldIDs.CMD.R.Tag, replace.orderID);
			csv.SetAt(CSVFieldIDs.CMD.R.Side, replace.side);
			csv.SetAt(CSVFieldIDs.CMD.R.Symbol, replace.symbol);
			csv.SetAt(CSVFieldIDs.CMD.R.TIF, replace.duration);

			// Force the Rom API to think its a fix client
			csv.SetAt(CSVFieldIDs.CMD.R.LeaveShares, "-1");
			csv.SetAt(CSVFieldIDs.CMD.R.NewShares, replace.newShares);
			csv.SetAt(CSVFieldIDs.CMD.R.NewStopPrice, replace.newStopPrice);
			csv.SetAt(CSVFieldIDs.CMD.R.NewPrice, replace.newPrice);
			csv.SetAt(CSVFieldIDs.CMD.R.NewPegPrice, replace.newPagPrice);

			csv.Encode();
			return csv.EncodeMessage;
		}

		public string GetCancel(string trader, RomBasicCancel cancel)
		{
			CSV csv = new CSV();

			csv.Fields = new string[8];
			csv.SetAt(CSVFieldIDs.CMD.C.Command, CSVFieldIDs.CommandTypes.CancelSingle);
			csv.SetAt(CSVFieldIDs.CMD.C.Trader, trader);
			csv.SetAt(CSVFieldIDs.CMD.C.Tag, cancel.orderID);
			csv.SetAt(CSVFieldIDs.CMD.C.Side, cancel.side);
			csv.SetAt(CSVFieldIDs.CMD.C.Symbol, cancel.mdSymbol);
			csv.SetAt(CSVFieldIDs.CMD.C.OriginalShares, cancel.shares);
			csv.SetAt(CSVFieldIDs.CMD.C.Price, cancel.price);

			csv.Encode();
			return csv.EncodeMessage;
		}

		public string GetCancelAll(string trader, bool dayOrderOnly, string exchange)
		{
			CSV csv = new CSV();

			csv.Fields = new string[28];
			csv.SetAt(CSVFieldIDs.CMD.c.Command, CSVFieldIDs.CommandTypes.CancelMultiple);
			csv.SetAt(CSVFieldIDs.CMD.c.Trader, trader);
			if (dayOrderOnly)
			{
				csv.SetAt(CSVFieldIDs.CMD.c.OnlyDayorder, "1");
			}
			else
			{
				csv.SetAt(CSVFieldIDs.CMD.c.OnlyDayorder, "");
			}
			csv.SetAt(CSVFieldIDs.CMD.c.Exchange, exchange);
			csv.SetAt(CSVFieldIDs.CMD.c.LoginName, trader);

			csv.Encode();
			return csv.EncodeMessage;
		}

		public string GetHartbeat()
		{
			return "H,\n";
		}
	}
}
