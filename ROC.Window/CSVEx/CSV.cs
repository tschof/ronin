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

		public bool TryGetValue(int key, out string value)
		{
			if (_data.TryGet(key, out value))
				return true;

			if (tryGetField(key, out value))
			{
				_data.Set(key, value);
				return true;
			}
			
			return false;
		}

		public bool TryGetValue(int key, out long value)
		{
			if (_data.TryGet(key, out value))
				return true;

			if (tryGetField(key, out string field)) {
				if (long.TryParse(field, out long converted)) {
					_data.Set(key, converted);
					value = converted;
					return true;
				} else {
					Debug.Assert(false, key.ToString() + "|" + field);
				}
			}

			value = 0;
			return false;
		}

		public bool TryGetValue(int key, out double value)
		{
			if (_data.TryGet(key, out value))
				return true;

			if (tryGetField(key, out string field)) {
				bool isPasswordText = false;
				if (double.TryParse(field, out double converted)) {
					_data.Set(key, converted);
					value = converted;
					return true;
				} else if (key == CSVFieldIDs.Price) {
					string command = Command;
					isPasswordText = (command == CSVFieldIDs.MessageTypes.LoggedIn) || (command == CSVFieldIDs.MessageTypes.LoginFailed);
					// Special Case for Loggin mssages L, K, this is the password field
				}

				if (!isPasswordText)
					Debug.Assert(false, string.Concat(key.ToString(), "|", _fields[key]));
			}

			return false;
		}

		public bool TryGetValue(int key, out DateTime value)
		{
			if (_data.TryGet(key, out double dval)) {
				value = DateTime.FromOADate(dval);
				return true;
			}

			if (tryGetField(key, out string dateText)) {
				if ((dateText != null) && (dateText != "00000000")) {
					if (TimeFormats.TryParse(dateText, out value) || DateTime.TryParse(dateText, out value)) {
						value = value.ToLocalTime();
						_data.Set(key, value.ToOADate());
						return true;
					} else {
						Debug.Assert(false, key.ToString() + "|" + dateText);
					}
				}
			}

			value = default;
			return false;
		}

		private string getAsString(int key)
		{
			return TryGetValue(key, out string value) ? value : "";
		}

		private long getAsLong(int key)
		{
			if (TryGetValue(key, out long value))
				return value;
			return (key == CSVFieldIDs.TIF) ? -1 : 0;
		}

		private double getAsDouble(int key)
		{
			return TryGetValue(key, out double value) ? value : 0;
		}

		private DateTime getAsDateTime(int key)
		{
			return TryGetValue(key, out DateTime value) ? value : DateTime.Now;
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

		/* Use TryGetField instead.
		public bool Contains(int key)
		{
			if (TryGetField(key, out string _))
				return true;
			return _hasBackFills && _data.Has(key);
		}
		*/

		#region - Decoded _fields By Name -

		public string Command
		{
			get
			{
				return getAsString(CSVFieldIDs.Command);
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
				return (getAsDouble(CSVFieldIDs.AveragePrice));
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
				return getAsString(CSVFieldIDs.CallPut);
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
				return getAsString(CSVFieldIDs.ClearingAcct);
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
				return getAsString(CSVFieldIDs.ClearingID);
			}
		}

		public long CumShares
		{
			get
			{
				return getAsLong(CSVFieldIDs.CumShares);
			}
		}

		public long ExchangeID
		{
			get
			{
				return getAsLong(CSVFieldIDs.ExchangeID);
			}
		}

		public long AlgoExchangeID
		{
			get
			{
				return getAsLong(CSVFieldIDs.AlgoExchangeID);
			}
		}

		public DateTime EffectiveTime
		{
			get
			{
				return getAsDateTime(CSVFieldIDs.EffectiveTime);
			}
		}

		public string ExpDate
		{
			get
			{
				return getAsString(CSVFieldIDs.ExpDate);
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
				return getAsString(CSVFieldIDs.Firm);
			}
		}

		public string Instructions
		{
			get
			{
				return getAsString(CSVFieldIDs.Instructions);
			}
		}

		public string ExecutionInstruction
		{
			get
			{
				return getAsString(CSVFieldIDs.ExecutionInstruction);
			}
		}

		public string ProgramTrade
		{
			get
			{
				return getAsString(CSVFieldIDs.ProgramTrade);
			}
		}

		public long LeaveShares
		{
			get
			{
				return getAsLong(CSVFieldIDs.LeaveShares);
			}
		}

		public string LocalAcct
		{
			get
			{
				return getAsString(CSVFieldIDs.LocalAcct);
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
				return getAsLong(CSVFieldIDs.Floor);
			}
		}

		public string OmExecTag
		{
			get
			{
				return getAsString(CSVFieldIDs.OmExecTag);
			}
		}

		public string OmTag
		{
			get
			{
				return getAsString(CSVFieldIDs.OmTag);
			}
		}

		public DateTime OmTime
		{
			get
			{
				return getAsDateTime(CSVFieldIDs.OmTime);
			}
		}

		public DateTime ExecutionTime
		{
			get
			{
				return getAsDateTime(CSVFieldIDs.ExecutionTime);
			}
		}

		public string OpenClose
		{
			get
			{
				return getAsString(CSVFieldIDs.OpenClose);
			}
		}

		public DateTime OrderExpirationDateTime
		{
			get
			{
				return getAsDateTime(CSVFieldIDs.OrderExpirationDateTime);
			}
		}

		public long Type
		{
			get
			{
				return getAsLong(CSVFieldIDs.Type);
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
				return getAsLong(CSVFieldIDs.AlgoType);
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
				return getAsLong(CSVFieldIDs.LastShares);
			}
		}

		public long OriginalShares
		{
			get
			{
				return getAsLong(CSVFieldIDs.OriginalShares);
			}
		}

		public string Owner
		{
			get
			{
				return getAsString(CSVFieldIDs.Owner);
			}
		}

		public double Price
		{
			get
			{
				return (getAsDouble(CSVFieldIDs.Price));
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
				return (getAsDouble(CSVFieldIDs.OriginalPrice));
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
				return (getAsDouble(CSVFieldIDs.ExecPrice));
			}
		}

		public long Shares
		{
			get
			{
				return (getAsLong(CSVFieldIDs.Shares));
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
				return getAsString(CSVFieldIDs.SecType);
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
				return (getAsLong(CSVFieldIDs.Side));
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
				return (getAsLong(CSVFieldIDs.Status));
			}
		}

		public double StopPrice
		{
			get
			{
				return (getAsDouble(CSVFieldIDs.StopPrice));
			}
		}

		public double PegOffsetPrice
		{
			get
			{
				return (getAsDouble(CSVFieldIDs.PegOffsetPrice));
			}
		}

		public double StrikePrice
		{
			get
			{
				return (getAsDouble(CSVFieldIDs.StrikePrice));
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
				return getAsString(CSVFieldIDs.Symbol);
			}
		}

		public string Tag
		{
			get
			{
				return getAsString(CSVFieldIDs.Tag);
			}
		}

		public string Text
		{
			get
			{
				return getAsString(CSVFieldIDs.Text);
			}
		}

		public long TIF
		{
			get
			{
				return (getAsLong(CSVFieldIDs.TIF));
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
				return getAsString(CSVFieldIDs.TradeFor);
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
				return getAsString(CSVFieldIDs.Trader);
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
				return getAsString(CSVFieldIDs.Underlying);
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
				return (getAsString(CSVFieldIDs.ParentTag));
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
				return (getAsLong(CSVFieldIDs.CplxOrderType));
			}
		}

		public string ClientEcho
		{
			get
			{
				return (getAsString(CSVFieldIDs.ClientEcho));
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
				return getAsString(CSVFieldIDs.SecurityDefinition);
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
				return getAsDateTime(CSVFieldIDs.EndTime);
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
				return getAsString(CSVFieldIDs.DisplayInstruction);
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
				return getAsString(CSVFieldIDs.CMTAAccount);
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
				return getAsString(CSVFieldIDs.MaturityDay);
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
