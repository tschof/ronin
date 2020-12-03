using System;
using System.Collections.Generic;
using DictionaryEx;
using System.Diagnostics;

namespace ROMEx
{
	public class CSV : MutiTypedDictionary
	{
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

		private Dictionary<int, string> _fields;
		public Dictionary<int, string> Fields
		{
			get
			{
				if (_fields == null)
				{
					_fields = new Dictionary<int, string>();
				}
				return _fields;
			}
			set
			{
				_fields = value;
			}
		}

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
				string[] fields = _message.Split(new char[] { ',' }, StringSplitOptions.None);

				if (fields.Length > 0)
				{
					int length = fields.Length - 1;
					for (int i = 0; i < length; i++)
					{
						switch (fields[i])
						{
							case "":
							case "\n":
								break;
							default:
								// Store every thing as string.
								Fields.Add(i, fields[i]);

								switch (i)
								{
									case CSVFieldIDs.AveragePrice:
										UpdateToDouble(i, fields[i]);
										break;
									case CSVFieldIDs.CallPut:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.ClearingAcct:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.ClearingID:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Command:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.CplxOrderType:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.CumShares:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.CurrentTimeStamp:
										UpdateToDateTime(i, fields[i]);
										break;
									case CSVFieldIDs.EffectiveTime:
										UpdateToDateTime(i, fields[i]);
										break;
									case CSVFieldIDs.ExchangeID:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.ExpDate:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Firm:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Floor:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.Instructions:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.LeavShares:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.LocalAcct:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.OmExecTag:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.OmTag:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.ExecutionTime:
										UpdateToDateTime(i, fields[i]);
										break;
									case CSVFieldIDs.OmTime:
										UpdateToDateTime(i, fields[i]);
										break;
									case CSVFieldIDs.OpenClose:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.OrderExpirationDateTime:
										UpdateToDateTime(i, fields[i]);
										break;
									case CSVFieldIDs.OriginalShares:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.Owner:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Price:
										UpdateToDouble(i, fields[i]);
										break;
									case CSVFieldIDs.SecType:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Shares:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.Side:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.Status:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.StopPrice:
										UpdateToDouble(i, fields[i]);
										break;
									case CSVFieldIDs.StrikePrice:
										UpdateToDouble(i, fields[i]);
										break;
									case CSVFieldIDs.Symbol:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Tag:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Text:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.TIF:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.TradeFor:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Trader:
										Update(i, fields[i]);
										break;
									case CSVFieldIDs.Type:
										UpdateToLong(i, fields[i]);
										break;
									case CSVFieldIDs.Underlying:
										Update(i, fields[i]);
										break;
									default:
										Update(i, fields[i]);
										break;
								}

								break;
						} // switch (field[i])
					} // for (int i = 0; i < length; i++)
				} // if (fields.Length > 0)
			} // if (childrenMsg.Length > 1)
		}

		private void UpdateToLong(int key, string val)
		{
			Int64 tL = 0;
			if (Int64.TryParse(val, out tL))
			{
				Update(key, tL);
			}
			else
			{
				Debug.Assert(!Debugger.IsAttached, String.Concat(new object[] { key.ToString(), "|", val }));
			}
		}
		private void UpdateToDouble(int key, string val)
		{
			double tD;
			if (Double.TryParse(val, out tD))
			{
				Update(key, tD);
			}
			else
			{
				Debug.Assert(!Debugger.IsAttached, String.Concat(new object[] { key.ToString(), "|", val }));
			}
		}
		private void UpdateToDateTime(int key, string val)
		{
			DateTime tDT;
			bool tOk = DateTime.TryParseExact(val, "yyyyMMdd-HH:mm:ss", System.Globalization.CultureInfo.CurrentCulture, System.Globalization.DateTimeStyles.None, out tDT);
			if (!tOk)
			{
				tOk = DateTime.TryParse(val, out tDT);
			}

			if (tOk)
			{
				Update(key, tDT);
			}
			else
			{
				Debug.Assert(!Debugger.IsAttached, String.Concat(new object[] { key.ToString(), "|", val }));
			}
		}

		private string GetAsString(int key)
		{
			if (Strings.ContainsKey(key))
			{
				return Strings[key];
			}
			else
			{
				return null;
			}
		}
		private double GetAsDouble(int key, string val)
		{
			double tD = 0;
			if (!Double.TryParse(val, out tD))
			{
				Debug.Assert(!Debugger.IsAttached, String.Concat(new object[] { key.ToString(), "|", val }));
			}
			return tD;
		}
		private long GetAsLong(int key, string val)
		{
			long tL = 0;
			if (!Int64.TryParse(val, out tL))
			{
				Debug.Assert(!Debugger.IsAttached, String.Concat(new object[] { key.ToString(), "|", val }));
			}
			return tL;
		}
		private DateTime GetAsDateTime(int key, string val)
		{
			DateTime tDT = DateTime.Now;
			if (!DateTime.TryParse(val, out tDT))
			{
				Debug.Assert(!Debugger.IsAttached, String.Concat(new object[] { key.ToString(), "|", val }));
			}
			return tDT;
		}

		public void Encode()
		{
			_encodeMessage = "";
			if (Fields != null)
			{
				Encode(Fields);
			}
		}
		private void Encode(Dictionary<int, string> inputFields)
		{
			SetAt(CSVFieldIDs.CurrentTimeStamp, DateTime.Now);

			// Covert to string
			for (int count = 0; count <= CSVFieldIDs.NumFields; count++)
			{
				if (inputFields.ContainsKey(count))
				{
					_encodeMessage = String.Concat(new object[] { _encodeMessage, inputFields[count], "," });
				}
				else
				{
					_encodeMessage = String.Concat(new object[] { _encodeMessage, "," });
				}
			}

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

		public string Get(int key)
		{
			if (Strings.ContainsKey(key))
			{
				return Strings[key];
			}
			else
			{
				return null;
			}
		}
		public Nullable<long> GetAsLong(int key)
		{
			if (Longs.ContainsKey(key))
			{
				return Longs[key];
			}
			else
			{
				return null;
			}
		}
		public Nullable<double> GetAsDouble(int key)
		{
			if (Doubles.ContainsKey(key))
			{
				return Doubles[key];
			}
			else
			{
				return null;
			}
		}
		public Nullable<DateTime> GetAsDateTime(int key)
		{
			if (DateTimes.ContainsKey(key))
			{
				return DateTimes[key];
			}
			else
			{
				return null;
			}
		}

		public void SetAt(int key, double val)
		{
			val = Math.Round(val, 7);
			if (Fields.ContainsKey(key))
			{
				Fields[key] = val.ToString();
			}
			else
			{
				Fields.Add(key, val.ToString());
			}
			Update(key, val);
		}
		public void SetAt(int key, decimal val)
		{
			if (Fields.ContainsKey(key))
			{
				Fields[key] = val.ToString();
			}
			else
			{
				Fields.Add(key, val.ToString());
			}
			Update(key, (double)val);
		}
		public void SetAt(int key, long val)
		{
			if (Fields.ContainsKey(key))
			{
				Fields[key] = val.ToString();
			}
			else
			{
				Fields.Add(key, val.ToString());
			}
			Update(key, val);
		}
		public void SetAt(int key, int val)
		{
			if (Fields.ContainsKey(key))
			{
				Fields[key] = val.ToString();
			}
			else
			{
				Fields.Add(key, val.ToString());
			}
			Update(key, val);
		}
		public void SetAt(int key, string val)
		{
			if (Fields.ContainsKey(key))
			{
				Fields[key] = val;
			}
			else
			{
				Fields.Add(key, val);
			}
			Update(key, val);
		}
		public void SetAt(int key, DateTime val)
		{
			if (Fields.ContainsKey(key))
			{
				Fields[key] = val.ToString("yyyyMMdd-HH:mm:ss");
			}
			else
			{
				Fields.Add(key, val.ToString("yyyyMMdd-HH:mm:ss"));
			}
			Update(key, val);
		}

		#region - Decoded Fields By Name -

		public string Command
		{
			get
			{
				return Strings[CSVFieldIDs.Command];
			}
		}

		public Nullable<double> AveragePrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.AveragePrice, GetAsString(CSVFieldIDs.AveragePrice)));
			}
		}

		public string CallPut
		{
			get
			{
				return GetAsString(CSVFieldIDs.CallPut);
			}
		}

		public string ClearingAcct
		{
			get
			{
				return GetAsString(CSVFieldIDs.ClearingAcct);
			}
		}

		public string ClearingID
		{
			get
			{
				return GetAsString(CSVFieldIDs.ClearingID);
			}
		}

		public Nullable<long> CumShares
		{
			get
			{
				return GetAsLong(CSVFieldIDs.CumShares, GetAsString(CSVFieldIDs.CumShares));
			}
		}

		public Nullable<long> ExchangeID
		{
			get
			{
				return GetAsLong(CSVFieldIDs.ExchangeID, GetAsString(CSVFieldIDs.ExchangeID));
			}
		}

		public Nullable<DateTime> EffectiveTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.EffectiveTime, GetAsString(CSVFieldIDs.EffectiveTime));
			}
		}

		public string ExpDate
		{
			get
			{
				return GetAsString(CSVFieldIDs.ExpDate);
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

		public Nullable<long> LeavShares
		{
			get
			{
				return GetAsLong(CSVFieldIDs.LeavShares, GetAsString(CSVFieldIDs.LeavShares));
			}
		}

		public string LocalAcct
		{
			get
			{
				return GetAsString(CSVFieldIDs.LocalAcct);
			}
		}

		public Nullable<long> Floor
		{
			get
			{
				return GetAsLong(CSVFieldIDs.Floor, GetAsString(CSVFieldIDs.Floor));
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

		public Nullable<DateTime> OmTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.OmTime, GetAsString(CSVFieldIDs.OmTime));
			}
		}

		public string OpenClose
		{
			get
			{
				return GetAsString(CSVFieldIDs.OpenClose);
			}
		}

		public Nullable<DateTime> OrderExpirationDateTime
		{
			get
			{
				return GetAsDateTime(CSVFieldIDs.OrderExpirationDateTime, GetAsString(CSVFieldIDs.OrderExpirationDateTime));
			}
		}

		public Nullable<long> Type
		{
			get
			{
				return GetAsLong(CSVFieldIDs.Type, GetAsString(CSVFieldIDs.Type));
			}
		}

		public Nullable<long> OriginalShares
		{
			get
			{
				return GetAsLong(CSVFieldIDs.OriginalShares, GetAsString(CSVFieldIDs.OriginalShares));
			}
		}

		public string Owner
		{
			get
			{
				return GetAsString(CSVFieldIDs.Owner);
			}
		}

		public Nullable<double> Price
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.Price, GetAsString(CSVFieldIDs.Price)));
			}
		}

		public Nullable<long> Shares
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.Shares, GetAsString(CSVFieldIDs.Shares)));
			}
		}

		public string SecType
		{
			get
			{
				return GetAsString(CSVFieldIDs.SecType);
			}
		}

		public Nullable<long> Side
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.Side, GetAsString(CSVFieldIDs.Side)));
			}
		}

		public Nullable<long> Status
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.Status, GetAsString(CSVFieldIDs.Status)));
			}
		}

		public Nullable<double> StopPrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.StopPrice, GetAsString(CSVFieldIDs.StopPrice)));
			}
		}

		public Nullable<double> StrikePrice
		{
			get
			{
				return (GetAsDouble(CSVFieldIDs.StrikePrice, GetAsString(CSVFieldIDs.StrikePrice)));
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

		public Nullable<long> TIF
		{
			get
			{
				return (GetAsLong(CSVFieldIDs.TIF, GetAsString(CSVFieldIDs.TIF)));
			}
		}

		public string TradeFor
		{

			get
			{
				return GetAsString(CSVFieldIDs.TradeFor);
			}
		}

		public string Trader
		{

			get
			{
				return GetAsString(CSVFieldIDs.Trader);
			}
		}

		public string Underlying
		{

			get
			{
				return GetAsString(CSVFieldIDs.Underlying);
			}
		}

		#endregion
	}
}
