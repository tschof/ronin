using System;
using System.Collections.Generic;
using System.Diagnostics;
using DictionaryEx;

namespace CSVEx
{
	public class CSV
	{
		private const int MAX_FIELD_COUNT = ushort.MaxValue;

		private readonly MultiTypedDictionary _data = new MultiTypedDictionary();

		private static class TimeFormats
		{
			private static string[] _formats = new string[] {
				"HHmmss",
				"yyyyMMdd-HH:mm:ss",
				"yyyyMMdd",
				"yyyyMMdd-HH:mm:ss.fff"
			};

			internal static bool TryParse(string text, out DateTime value)
			{
				return DateTime.TryParseExact(text, _formats, System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out value);
			}
		}

		private List<CSV> _children;
		public List<CSV> Children {
			get {
				if (_children == null) {
					_children = new List<CSV>();
				}
				return _children;
			}
		}

		public string Message { get; private set; } = "";
		public string EncodeMessage { get; private set; }

		private string[] _fields;
		public string[] Fields => _fields;

		private bool _hasBackFills = false;

		public CSV()
		{
			_fields = new string[0];
		}

		public CSV(int capacity)
		{
			_fields = new string[capacity];
		}

		public CSV(string message)
		{
			_fields = new string[0];
			Message = message;
			Decode();
		}

		private bool tryGetField(int key, out string value)
		{
			if ((_fields != null) && (key < _fields.Length)) {
				value = _fields[key];
				return !string.IsNullOrEmpty(value);
			}
			value = null;
			return false;
		}

		#region - Decode -

		public void Decode()
		{
			if (Message != null && Message != "\n" && Message != "")
			{
				Decode(Message);
			}
		}
		private void Decode(string message)
		{
			// Split For Children
			string[] childrenMsg = Message.Split(new string[] { "!#!" }, StringSplitOptions.None);

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
				_fields = Message.Split(new char[] { ',' }, StringSplitOptions.None);
			}
		}

		#endregion

		#region - Get -

		private string GetAsString(int key)
		{
			string value;

			if (_data.TryGet(key, out value))
				return value;

			if (tryGetField(key, out value))
			{
				_data.Set(key, value);
				return value;
			}
			
			return "";
		}

		private long GetAsLong(int key)
		{
			if (_data.TryGet(key, out long value))
				return value;

			if (tryGetField(key, out string field))
			{
				if (long.TryParse(field, out long converted))
				{
					_data.Set(key, converted);
					return converted;
				}
				else
				{
					Debug.Assert(false, key.ToString() + "|" + field);
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
			if (_data.TryGet(key, out double value))
				return value;

			if (tryGetField(key, out string field)) {
				if (double.TryParse(field, out double converted)) {
					_data.Set(key, converted);
					return converted;
				} else if ((key == CSVFieldIDs.Price) &&
					(Command == CSVFieldIDs.MessageTypes.LoggedIn || Command == CSVFieldIDs.MessageTypes.LoginFailed)) {
					// Special Case for Loggin mssages L, K, this is the password field
				} else {
					Debug.Assert(false, string.Concat(key.ToString(), "|", _fields[key]));
				}
			}

			return 0;
		}

		private DateTime GetAsDateTime(int key)
		{
			if (_data.TryGet(key, out double value))
				return DateTime.FromOADate(value);
			
			if (tryGetField(key, out string dateText))
			{
				if ((dateText != null) && (dateText != "00000000"))
				{
					DateTime converted;
					if (TimeFormats.TryParse(dateText, out converted) || DateTime.TryParse(dateText, out converted))
					{
						converted = converted.ToLocalTime();
						_data.Set(key, converted.ToOADate());
						return converted;
					}
					else
					{
						Debug.Assert(false, key.ToString() + "|" + dateText);
					}
				}
			}

			return DateTime.Now;
		}

		#endregion

		#region - Encode -

		public void Encode()
		{
			EncodeMessage = "";
			if (_fields != null && _fields.Length > 0)
			{
				Encode(_fields);
			}
		}
		private void Encode(string[] input_fields)
		{
			SetAt(CSVFieldIDs.CurrentTimeStamp, DateTime.Now.ToString("HHmmss"));

			EncodeMessage = string.Join(",", input_fields);

			// If have child convert them to string too
			foreach (CSV child in Children)
			{
				child.Encode();
				EncodeMessage = "!#!" + EncodeMessage + child.EncodeMessage;
				EncodeMessage.Replace('\n', ' ');
			}

			if (Children.Count != 0)
			{
				EncodeMessage += "!#!";
			}

			EncodeMessage += '\n';
		}

		#endregion

		#region - Set -

		// Expands the array as necessary and inserts the value.
		private void insert(int key, string value)
		{
			if (key < MAX_FIELD_COUNT) {
				if (_fields.Length < (key + 1))
					System.Array.Resize(ref _fields, key + 1);
				_fields[key] = value;
			}
		}

		public void SetAt(int key, double val)
		{
			insert(key, Math.Round(val, 7).ToString());
		}

		public void SetAt(int key, DateTime val)
		{
			insert(key, val.ToString("yyyyMMdd-HH:mm:ss"));
		}

		public void SetAt<T>(int key, T val)
		{
			insert(key, val.ToString());
		}

		#endregion

		public bool Contains(int key)
		{
			if (tryGetField(key, out string _))
				return true;
			return _hasBackFills && _data.Has(key);
		}

		#region - Decoded _fields By Name -

		public string Command
		{
			get
			{
				return GetAsString(CSVFieldIDs.Command);
			}
			set
			{
				_hasBackFills = true;
				_data.Set(CSVFieldIDs.Command, value);
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
				_data.Set(CSVFieldIDs.AveragePrice, value);
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
				_data.Set(CSVFieldIDs.CallPut, value);
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
				_data.Set(CSVFieldIDs.ClearingAcct, value);
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
				_data.Set(CSVFieldIDs.ExpDate, value);
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
				_data.Set(CSVFieldIDs.LocalAcct, value);
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
				_data.Set(CSVFieldIDs.Type, value);
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
				_data.Set(CSVFieldIDs.AlgoType, value);
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
				_data.Set(CSVFieldIDs.Price, value);
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
				_data.Set(CSVFieldIDs.OriginalPrice, value);
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
				_data.Set(CSVFieldIDs.Shares, value);
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
				_data.Set(CSVFieldIDs.SecType, value);
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
				_data.Set(CSVFieldIDs.Side, value);
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
				_data.Set(CSVFieldIDs.StrikePrice, value);
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
				_data.Set(CSVFieldIDs.TIF, value);
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
				_data.Set(CSVFieldIDs.TradeFor, value);
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
				_data.Set(CSVFieldIDs.Trader, value);
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
				_data.Set(CSVFieldIDs.Underlying, value);
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
				_data.Set(CSVFieldIDs.ParentTag, value);
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
				_data.Set(CSVFieldIDs.ClientEcho, value);
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
					case CSVFieldIDs.CplxOrderTypes.Container:
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
					case CSVFieldIDs.CplxOrderTypes.Container:
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
				_data.Set(CSVFieldIDs.SecurityDefinition, value);
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
				_data.Set(CSVFieldIDs.EndTime, value.ToOADate());
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
				_data.Set(CSVFieldIDs.DisplayInstruction, value);
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
				_data.Set(CSVFieldIDs.CMTAAccount, value);
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
				_data.Set(CSVFieldIDs.MaturityDay, value);
			}
		}

		#endregion
	}
}
