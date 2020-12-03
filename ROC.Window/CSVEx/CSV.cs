using System;
using System.Collections.Generic;
using System.Diagnostics;

using ArrayEx;
using DictionaryEx;
using System.Runtime.Serialization;

namespace CSVEx
{
	public class CSV : MutiTypedDictionary
	{
		private string[] _timeFormats = new string[0];
		private string[] timeFormats
		{
			get
			{
				if (_timeFormats.Length == 0)
				{
					_timeFormats = new string[4];
					_timeFormats[0] = "HHmmss";
					_timeFormats[1] = "yyyyMMdd-HH:mm:ss";
					_timeFormats[2] = "yyyyMMdd";
					//_timeFormats[3] = "yyyyMM";
					_timeFormats[3] = "yyyyMMdd-HH:mm:ss.fff";
				}
				return _timeFormats;
			}
		}

		private List<CSV> _children;
		public List<CSV> Children
		{
			get
			{
				if (_children == null)
				{
					_children = new List<CSV>();
				}
				return _children;
			}
		}

		private string _message;
		public string Message
		{
			get
			{
				return _message;
			}
		}

		private string _encodeMessage;
		public string EncodeMessage
		{
			get
			{
				return _encodeMessage;
			}
		}

		//private int _fieldLength = 0;
		private string[] _fields = new string[0];
		public string[] Fields
		{
			get
			{
				return _fields;
			}
			set
			{
				_fields = value;
			}
		}

		public CSV(SerializationInfo info, StreamingContext ctxt)
			: base(info, ctxt)
		{
		}

		private bool _hasBackFills = false;

		public CSV()
		{
			Initialize();
		}
		private void Initialize()
		{
			_message = "";
		}

		public CSV(string message)
		{
			_message = message;
			Decode();
		}

		#region - Decode -

		public void Decode()
		{
			if (_message != null && _message != "\n" && _message != "")
			{
				Decode(_message);
			}
		}
		private void Decode(string message)
		{
			// Split For Children
			string[] childrenMsg = _message.Split(new string[] { "!#!" }, StringSplitOptions.None);

			if (childrenMsg.Length > 1)
			{
				foreach (string childMsg in childrenMsg)
				{
					CSV childCSV = new CSV(childMsg);
					Children.Add(childCSV);
				}
			}
			else
			{
				// No Children
				_fields = _message.Split(new char[] { ',' }, StringSplitOptions.None);

				//if (Fields.Length > 0)
				//{
				//    _fieldLength = Fields.Length - 1;
				//    for (int i = 0; i < _fieldLength; i++)
				//    {
				//        FilterAndUpdateByFieldID(i);
				//    }
				//}
			}
		}

		#region - Not Used -

		//private void FilterAndUpdateByFieldID(int id)
		//{
		//    if (Fields.Length > id)
		//    {
		//        switch (Fields[id])
		//        {
		//            case "":
		//            case "\n":
		//                break;
		//            default:
		//                UpdateByFieldID(id);
		//                break;
		//        }
		//    }
		//}
		//private void UpdateByFieldID(int id)
		//{
		//    switch (id)
		//    {
		//        case CSVFieldIDs.AveragePrice:
		//            UpdateToDouble(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.CallPut:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ClearingAcct:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ClearingID:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Command:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.CumShares:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.CurrentTimeStamp:
		//            UpdateToDateTime(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.EffectiveTime:
		//            UpdateToDateTime(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ExchangeID:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.AlgoExchangeID:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ExpDate:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Firm:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Floor:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Instructions:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ExecutionInstruction:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.LeaveShares:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.LocalAcct:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.OmExecTag:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.OmTag:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ExecutionTime:
		//            UpdateToDateTime(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.OmTime:
		//            UpdateToDateTime(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.OpenClose:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.OrderExpirationDateTime:
		//            UpdateToDateTime(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.LastShares:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.OriginalShares:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Owner:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Price:
		//            UpdateToDouble(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ExecPrice:
		//            UpdateToDouble(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.SecType:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Shares:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Side:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Status:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.StopPrice:
		//            UpdateToDouble(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.PegOffsetPrice:
		//            UpdateToDouble(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.StrikePrice:
		//            UpdateToDouble(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Symbol:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Tag:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Text:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.TIF:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.TradeFor:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Trader:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Type:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.AlgoType:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.Underlying:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ParentTag:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.CplxOrderType:
		//            UpdateToLong(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ClientEcho:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.ProgramTrade:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.EndTime:
		//            UpdateToDateTime(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.DisplayInstruction:
		//            Update(id, Fields[id]);
		//            break;
		//        case CSVFieldIDs.MaturityDay:
		//            Update(id, Fields[id]);
		//            break;
		//        default:
		//            Update(id, Fields[id]);
		//            break;
		//    }
		//}

		//private void UpdateToLong(int key, string val)
		//{
		//    Int64 tL = 0;
		//    if (val != null)
		//    {
		//        if (Int64.TryParse(val, out tL))
		//        {
		//            Update(key, tL);
		//        }
		//        else
		//        {
		//            Debug.Assert(false, String.Concat(new object[] { key.ToString(), "|", val }));
		//        }
		//    }
		//}
		//private void UpdateToDouble(int key, string val)
		//{
		//    double tD;
		//    if (val != null)
		//    {
		//        if (Double.TryParse(val, out tD))
		//        {
		//            Update(key, tD);
		//        }
		//        else
		//        {
		//            if ((Command == CSVFieldIDs.MessageTypes.LoggedIn || Command == CSVFieldIDs.MessageTypes.LoginFailed) &&
		//                key == CSVFieldIDs.Price)
		//            {
		//                // Special Case for Loggin mssages L, K, this is the password field
		//            }
		//            else
		//            {
		//                Debug.Assert(false, String.Concat(new object[] { key.ToString(), "|", val }));
		//            }
		//        }
		//    }
		//}

		//private void UpdateToDateTime(int key, string val)
		//{
		//    DateTime tDT;

		//    if (val != "00000000")
		//    {
		//        bool tOk = DateTime.TryParseExact(val, timeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
		//        if (!tOk)
		//        {
		//            tOk = DateTime.TryParse(val, out tDT);
		//        }

		//        if (tOk)
		//        {
		//            tDT = tDT.ToLocalTime();
		//            Update(key, tDT.ToOADate());
		//        }
		//        else
		//        {
		//            Debug.Assert(false, String.Concat(new object[] { key.ToString(), "|", val }));
		//        }
		//    }
		//}

		#endregion

		#endregion

		#region - Get -

		private string GetAsString(int key)
		{
			//if (!Strings.ContainsKey(key))
			//{
			//    FilterAndUpdateByFieldID(key);
			//}

			if (Strings.ContainsKey(key))
			{
				return Strings[key];
			}
			else if (Contains(key))
			{
				Update(key, Fields[key]);
				return Fields[key];
			}
			
			return "";
		}
		private long GetAsLong(int key)
		{
			//if (!Longs.ContainsKey(key))
			//{
			//    FilterAndUpdateByFieldID(key);
			//}

			if (Longs.ContainsKey(key))
			{
				return Longs[key];
			}
			else if (Contains(key))
			{
				Int64 tL = 0;
				if (Fields[key] != null)
				{
					if (Int64.TryParse(Fields[key], out tL))
					{
						Update(key, tL);
						return tL;
					}
					else
					{
						Debug.Assert(false, String.Concat(new object[] { key.ToString(), "|", Fields[key] }));
					}
				}
			}
			switch (key)
			{
				case CSVFieldIDs.TIF:
					return -1;
				default:
					return 0;
			}
		}
		private double GetAsDouble(int key)
		{
			//if (!Doubles.ContainsKey(key))
			//{
			//    FilterAndUpdateByFieldID(key);
			//}

			if (Doubles.ContainsKey(key))
			{
				return Doubles[key];
			}
			else if (Contains(key))
			{
				double tD;
				if (Fields[key] != null)
				{
					if (Double.TryParse(Fields[key], out tD))
					{
						Update(key, tD);
						return tD;
					}
					else
					{
						if ((Command == CSVFieldIDs.MessageTypes.LoggedIn || Command == CSVFieldIDs.MessageTypes.LoginFailed) && 
							key == CSVFieldIDs.Price)
						{
							// Special Case for Loggin mssages L, K, this is the password field
						}
						else
						{
							Debug.Assert(false, String.Concat(new object[] { key.ToString(), "|", Fields[key] }));
						}
					}
				}
			}

			return 0;
		}
		private DateTime GetAsDateTime(int key)
		{
			//if (!Doubles.ContainsKey(key))
			//{
			//    FilterAndUpdateByFieldID(key);
			//}

			if (Doubles.ContainsKey(key))
			{
				return DateTime.FromOADate(Doubles[key]);
			}
			else if (Contains(key))
			{
				DateTime tDT;

				if (Fields[key] != null && Fields[key] != "00000000")
				{
					bool tOk = DateTime.TryParseExact(Fields[key], timeFormats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
					if (!tOk)
					{
						tOk = DateTime.TryParse(Fields[key], out tDT);
					}

					if (tOk)
					{
						tDT = tDT.ToLocalTime();
						Update(key, tDT.ToOADate());
						return tDT;
					}
					else
					{
						Debug.Assert(false, String.Concat(new object[] { key.ToString(), "|", Fields[key] }));
					}
				}
			}

			return DateTime.Now;
		}

		#endregion

		#region - Encode -

		public void Encode()
		{
			_encodeMessage = "";
			if (Fields != null && Fields.Length > 0)
			{
				Encode(Fields);
			}
		}
		private void Encode(string[] inputFields)
		{
			SetAt(CSVFieldIDs.CurrentTimeStamp, DateTime.Now.ToString("HHmmss"));

			_encodeMessage = String.Join(",", inputFields);

			// If have child convert them to string too
			foreach (CSV child in Children)
			{
				child.Encode();
				_encodeMessage = String.Concat(new object[] { "!#!", _encodeMessage, child.EncodeMessage });
				_encodeMessage.Replace('\n', ' ');
			}

			if (Children.Count != 0)
			{
				_encodeMessage += "!#!";
			}

			_encodeMessage += '\n';
		}

		#endregion

		#region - Set -

		public void SetAt(int key, double val)
		{
			val = Math.Round(val, 7);
			Fields = (string[])ArrayHelper.Update(Fields, key, val.ToString());
		}
		public void SetAt(int key, decimal val)
		{
			Fields = (string[])ArrayHelper.Update(Fields, key, val.ToString());
		}
		public void SetAt(int key, long val)
		{
			Fields = (string[])ArrayHelper.Update(Fields, key, val.ToString());
		}
		public void SetAt(int key, int val)
		{
			Fields = (string[])ArrayHelper.Update(Fields, key, val.ToString());
		}
		public void SetAt(int key, string val)
		{
			Fields = (string[])ArrayHelper.Update(Fields, key, val);
		}
		public void SetAt(int key, DateTime val)
		{
			Fields = (string[])ArrayHelper.Update(Fields, key, val.ToString("yyyyMMdd-HH:mm:ss"));
		}

		#endregion

		public bool Contains(int key)
		{
			if (_hasBackFills)
			{
				if ((key < Fields.Length && Fields[key] != "") ||
					Strings.ContainsKey(key) ||
					Doubles.ContainsKey(key) ||
					Longs.ContainsKey(key))
				{
					return true;
				}
			}
			else
			{
				// No Bakc Fills then only check the Field Array
				if (key < Fields.Length && Fields[key] != "")
				{
					return true;
				}
			}

			//if (Fields.Length >= key + 1)
			//{
			//    if (GetAsString(key) != "")
			//    {
			//        return true;
			//    }
			//}

			return false;
		}

		#region - Decoded Fields By Name -

		public string Command
		{
			get
			{
				return GetAsString(CSVFieldIDs.Command);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.Command, value);
			}
		}

		public double AveragePrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.AveragePrice));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.AveragePrice, value);
			}
		}

		public string CallPut
		{
			get
			{
				return GetAsString(CSVFieldIDs.CallPut);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.CallPut, value);
			}
		}

		public string ClearingAcct
		{
			get
			{
				return GetAsString(CSVFieldIDs.ClearingAcct);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.ClearingAcct, value);
			}
		}

		public string ClearingID
		{
			get
			{
				return GetAsString(CSVFieldIDs.ClearingID);
			}
		}

		public long CumShares
		{
			get
			{
				return GetAsLong(CSVFieldIDs.CumShares);
			}
		}

		public long ExchangeID
		{
			get
			{
				return GetAsLong(CSVFieldIDs.ExchangeID);
			}
		}

		public long AlgoExchangeID
		{
			get
			{
				return GetAsLong(CSVFieldIDs.AlgoExchangeID);
			}
		}

		public DateTime EffectiveTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.EffectiveTime);
			}
		}

		public string ExpDate
		{
			get
			{
				return GetAsString(CSVFieldIDs.ExpDate);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.ExpDate, value);
			}
		}

		public string Firm
		{
			get
			{
				return GetAsString(CSVFieldIDs.Firm);
			}
		}

		public string Instructions
		{
			get
			{
				return GetAsString(CSVFieldIDs.Instructions);
			}
		}

		public string ExecutionInstruction
		{
			get
			{
				return GetAsString(CSVFieldIDs.ExecutionInstruction);
			}
		}

		public string ProgramTrade
		{
			get
			{
				return GetAsString(CSVFieldIDs.ProgramTrade);
			}
		}

		public long LeaveShares
		{
			get
			{
				return GetAsLong(CSVFieldIDs.LeaveShares);
			}
		}

		public string LocalAcct
		{
			get
			{
				return GetAsString(CSVFieldIDs.LocalAcct);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.LocalAcct, value);
			}
		}

		public long Floor
		{
			get
			{
				return GetAsLong(CSVFieldIDs.Floor);
			}
		}

		public string OmExecTag
		{
			get
			{
				return GetAsString(CSVFieldIDs.OmExecTag);
			}
		}

		public string OmTag
		{
			get
			{
				return GetAsString(CSVFieldIDs.OmTag);
			}
		}

		public DateTime OmTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.OmTime);
			}
		}

		public DateTime ExecutionTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.ExecutionTime);
			}
		}

		public string OpenClose
		{
			get
			{
				return GetAsString(CSVFieldIDs.OpenClose);
			}
		}

		public DateTime OrderExpirationDateTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.OrderExpirationDateTime);
			}
		}

		public long Type
		{
			get
			{
				return GetAsLong(CSVFieldIDs.Type);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.Type, value);
			}
		}

		public long AlgoType
		{
			get
			{
				return GetAsLong(CSVFieldIDs.AlgoType);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.AlgoType, value);
			}
		}

		public long LastShares
		{
			get
			{
				return GetAsLong(CSVFieldIDs.LastShares);
			}
		}

		public long OriginalShares
		{
			get
			{
				return GetAsLong(CSVFieldIDs.OriginalShares);
			}
		}

		public string Owner
		{
			get
			{
				return GetAsString(CSVFieldIDs.Owner);
			}
		}

		public double Price
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.Price));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.Price, value);
			}
		}

		public double OriginalPrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.OriginalPrice));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.OriginalPrice, value);
			}
		}

		public double ExecPrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.ExecPrice));
			}
		}

		public long Shares
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.Shares));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.Shares, value);
			}
		}

		public string SecType
		{
			get
			{
				return GetAsString(CSVFieldIDs.SecType);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.SecType, value);
			}
		}

		public long Side
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.Side));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.Side, value);
			}
		}

		public long Status
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.Status));
			}
		}

		public double StopPrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.StopPrice));
			}
		}

		public double PegOffsetPrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.PegOffsetPrice));
			}
		}

		public double StrikePrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.StrikePrice));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.StrikePrice, value);
			}
		}

		public string Symbol
		{
			get
			{
				return GetAsString(CSVFieldIDs.Symbol);
			}
		}

		public string Tag
		{
			get
			{
				return GetAsString(CSVFieldIDs.Tag);
			}
		}

		public string Text
		{
			get
			{
				return GetAsString(CSVFieldIDs.Text);
			}
		}

		public long TIF
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.TIF));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.TIF, value);
			}
		}

		public string TradeFor
		{
			get
			{
				return GetAsString(CSVFieldIDs.TradeFor);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.TradeFor, value);
			}
		}

		public string Trader
		{
			get
			{
				return GetAsString(CSVFieldIDs.Trader);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.Trader, value);
			}
		}

		public string Underlying
		{
			get
			{
				return GetAsString(CSVFieldIDs.Underlying);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.Underlying, value);
			}
		}

		public string ParentTag
		{
			get
			{
				return (GetAsString(CSVFieldIDs.ParentTag));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.ParentTag, value);
			}
		}

		public long CplxOrderType
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.CplxOrderType));
			}
		}

		public string ClientEcho
		{
			get
			{
				return (GetAsString(CSVFieldIDs.ClientEcho));
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.ClientEcho, value);
			}
		}

		public bool UpdateOrder
		{
			get
			{
				switch (CplxOrderType)
				{
					/*
					case 6:
						return false;
					case 5:
					default:
						return true;
					*/
					case CSVFieldIDs.CplxOrderTypes.Continer:
						return true;
					case CSVFieldIDs.CplxOrderTypes.Leg:
					default:
						return false;
				}
			}
		}

		public bool UpdateTrade
		{
			get
			{
				switch (CplxOrderType)
				{
					/*
					case 5:
						return false;
					case 6:
					default:
						return true;
					*/
					case CSVFieldIDs.CplxOrderTypes.Leg:
					    return true;
					case CSVFieldIDs.CplxOrderTypes.Continer:
					default:
					    return false;
				}
			}
		}

		public string SecurityDefinition
		{
			get
			{
				_hasBackFills = true;
				return GetAsString(CSVFieldIDs.SecurityDefinition);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.SecurityDefinition, value);
			}
		}

		public DateTime EndTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.EndTime);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.EndTime, value.ToOADate());
			}
		}

		public string DisplayInstruction
		{
			get
			{
				return GetAsString(CSVFieldIDs.DisplayInstruction);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.DisplayInstruction, value);
			}
		}

		public string CMTAAccount
		{
			get
			{
				return GetAsString(CSVFieldIDs.CMTAAccount);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.CMTAAccount, value);
			}
		}

		public string MaturityDay
		{
			get
			{
				return GetAsString(CSVFieldIDs.MaturityDay);
			}
			set
			{
				_hasBackFills = true;
				Update(CSVFieldIDs.MaturityDay, value);
			}
		}

		#endregion
	}
}
