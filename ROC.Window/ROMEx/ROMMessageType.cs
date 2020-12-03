using System;
using System.Collections.Generic;
using System.Text;

namespace ROMEx
{
	//public sealed class ROMMessageType
	//{
	//    public const string HeartBeat = "Q";
	//    public const string LoggedIn = "L";
	//    public const string LoginFailed = "K";
	//    public const string Alert = "@";
	//    public const string InvalidField = "I";
	//    public const string WorkingOrder = "E";
	//    public const string OrderStatus = "S";
	//}

	//public sealed class ROMCommandTypes
	//{
	//    public const string Login = "L";
	//    public const string EnterOrder = "E";
	//    public const string CancelSingle = "C";
	//    public const string CancelMultiple = "c";
	//    public const string Replace = "R";
	//}

	//public sealed class ROMCancelChoices
	//{
	//    public const int KeepOrdersAlive = 0;
	//    public const int CancelAllLiveOrderOnDisconnection = 1;
	//    public const int CancelAllLiveOrderExceptGTD_GTC = 2;

	//    private static Dictionary<int, string> _descriptions;
	//    public static Dictionary<int, string> Descriptions
	//    {
	//        get
	//        {
	//            if (_descriptions == null)
	//            {
	//                _descriptions = new Dictionary<int,string>();
	//                _descriptions.Add(KeepOrdersAlive, "Keep Orders Alive");
	//                _descriptions.Add(CancelAllLiveOrderOnDisconnection, "Cancel All Live Order On Disconnection");
	//                _descriptions.Add(CancelAllLiveOrderExceptGTD_GTC, "Cancel All Live Order Except GTD & GTC");
	//            }

	//            return _descriptions;
	//        }
	//    }
	//}

	//public sealed class ROMOrderSides
	//{
	//    public const int Buy = 1;
	//    public const int Sell = 2;
	//    public const int Short = 5;
	//    public const int SSE = 6;

	//    private static Dictionary<int, string> _descriptions;
	//    public static Dictionary<int, string> Descriptions
	//    {
	//        get
	//        {
	//            if (_descriptions == null)
	//            {
	//                _descriptions = new Dictionary<int,string>();
	//                _descriptions.Add(Buy, "Buy");
	//                _descriptions.Add(Sell, "Sell");
	//                _descriptions.Add(Short, "Sell Short");
	//                _descriptions.Add(SSE, "SSE");
	//            }

	//            return _descriptions;
	//        }
	//    }
	//}

	//public sealed class ROMOrderTypes
	//{
	//    public const int Market = 1;
	//    public const int Limit = 2;
	//    public const int Stop = 3;
	//    public const int StopLimit = 4;
	//    public const int MarketOnClose = 5;
	//    public const int WithOrWithout = 6;
	//    public const int LimitOrBetter = 7;
	//    public const int LimitWithOrWithout = 8;
	//    public const int OnBasis = 9;
	//    public const int OnClose = 10;
	//    public const int LimitOnClose = 11;
	//    public const int MarketAtOpen = 12;
	//    public const int VWAP = 14;

	//    // K = Marketable Limit order on CME/Globex

	//    private static Dictionary<int, string> _descriptions;
	//    public static Dictionary<int, string> Descriptions
	//    {
	//        get
	//        {
	//            if (_descriptions == null)
	//            {
	//                _descriptions = new Dictionary<int, string>();
	//                _descriptions.Add(Market, "Market");
	//                _descriptions.Add(Limit, "Limit");
	//                _descriptions.Add(Stop, "Stop");
	//                _descriptions.Add(StopLimit, "StopLimit");
	//                _descriptions.Add(MarketOnClose, "MarketOnClose");
	//                _descriptions.Add(WithOrWithout, "WithOrWithout");
	//                _descriptions.Add(LimitOrBetter, "LimitOrBetter");
	//                _descriptions.Add(LimitWithOrWithout, "LimitWithOrWithout");
	//                _descriptions.Add(OnBasis, "OnBasis");
	//                _descriptions.Add(OnClose, "OnClose");
	//                _descriptions.Add(LimitOnClose, "LimitOnClose");
	//                _descriptions.Add(MarketAtOpen, "MarketAtOpen");
	//                _descriptions.Add(VWAP, "VWAP");
	//            }

	//            return _descriptions;
	//        }
	//    }
	//}

	//public sealed class ROMTIFCodes
	//{
	//    public const int Day = 0;
	//    public const int GTC = 1;		// Good Till Cancel
	//    public const int OPG = 2;
	//    public const int IOC = 3;		// Immediate or Cancel
	//    public const int FOK = 4;		// Fill or Kill
	//    public const int GTX = 5;
	//    public const int GTD = 6;		// Good Till Date
	//    public const int OCL = 7;		// On Close

	//    private static Dictionary<int, string> _descriptions;
	//    public static Dictionary<int, string> Descriptions
	//    {
	//        get
	//        {
	//            if (_descriptions == null)
	//            {
	//                _descriptions = new Dictionary<int, string>();
	//                _descriptions.Add(Day, "Day");
	//                _descriptions.Add(GTC, "GTC");
	//                _descriptions.Add(OPG, "On Open");
	//                _descriptions.Add(IOC, "IOC");
	//                _descriptions.Add(FOK, "FOK");
	//                _descriptions.Add(GTX, "GTX");
	//                _descriptions.Add(GTD, "GTD");
	//                _descriptions.Add(OCL, "On Close");
	//            }

	//            return _descriptions;
	//        }
	//    }
	//}
}
