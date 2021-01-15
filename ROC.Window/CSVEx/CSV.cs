using System;
using System.Collections.Generic;
using System.Diagnostics;

namespace CSVEx
{
	public class CSV
	{
		private const int MAX_FIELD_COUNT = ushort.MaxValue;
		private const string MESSAGE_SEPARATOR = "!#!";

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

		// _fields set by Properties, missing from or overriding _fields values.
		private class CacheEntry
		{
			internal readonly int Key;
			internal object Value;
			internal CacheEntry(int key, object value) { Key = key; Value = value; }

		}

		private List<CacheEntry> _cache = new List<CacheEntry>();
		private string[] _fields = null;
		private string _children = null;
		private string _encoded = null;
		private int _maxCacheKey = -1;

		public CSV()
		{
		}

		public CSV(int capacity)
		{
			if (capacity < MAX_FIELD_COUNT)
				_fields = new string[capacity];
		}

		public CSV(string message)
		{
			// Decode the CSV message collection.
			if (!string.IsNullOrEmpty(message) && (message != "\n")) {
				// Child CSVs may be included in the received message but are
				// never accessed, we so don't bother parsing them.  Just save
				// them for when we reserialize this CSV message.
				string[] messages = message.Split(new string[] { MESSAGE_SEPARATOR }, 2, StringSplitOptions.RemoveEmptyEntries);

				int messageCount = (messages == null) ? 0 : messages.GetLength(0);
				if (messageCount > 0) {
					_fields = messages[0].Split(new char[] { ',' }, StringSplitOptions.None);
					if (messageCount > 1)
						_children = messages[1]; // Save for Encode().
				}
			}
		}

		#region - Get -

		private delegate bool TryConvertFromText<T>(string text, out T value);

		private bool convertDate(string text, out DateTime when)
		{
			if ((text != "00000000") && DateTime.TryParse(text, out when)) {
				when = when.ToLocalTime();
				return true;
			}
			when = default;
			return false;
		}

		private bool tryGetValue<T>(int key, out T value, out string csvFieldText, TryConvertFromText<T> xform)
		{
			CacheEntry found = _cache.Find(n => n.Key == key);

			// First check the cache.
			if ((found != null) && (found.Value is T v)) {
				value = v;
				csvFieldText = null;
				return true;
			}

			// Then check the csv fields.
			if ((_fields != null) && (key >= 0) && (key < _fields.Length)) {
				csvFieldText = _fields[key];
				if (!string.IsNullOrEmpty(csvFieldText) && xform(csvFieldText, out value)) {
					// Cache the transformed value, but also keep the CSV field.
					_cache.Add(new CacheEntry(key, value));
					return true;
				}
			}

			// Not found or could not convert from string.
			csvFieldText = null;
			value = default;
			return false;
		}

		public bool TryGetValue(int key, out string value)
		{
			CacheEntry found = _cache.Find(n => n.Key == key);

			if (found != null) {
				value = (found.Value is string s) ? s : found.Value.ToString();
				return true;
			}

			if ((_fields != null) && (key >= 0) && (key < _fields.Length)) {
				value = _fields[key];
				return !string.IsNullOrEmpty(value);
			}

			value = null;
			return false;
		}
		public bool TryGetValue(int key, out int value) => tryGetValue(key, out value, out _, int.TryParse);
		public bool TryGetValue(int key, out long value) => tryGetValue(key, out value, out _, long.TryParse);
		public bool TryGetValue(int key, out double value)
		{
			if (!tryGetValue(key, out value, out string text, double.TryParse) && (key == CSVFieldIDs.Price)) {
				if (!string.IsNullOrEmpty(text)) {
					// Special Case for Loggin mssages L, K, this is the password field
					string command = Command;
					bool isPasswordText = (command == CSVFieldIDs.MessageTypes.LoggedIn) || (command == CSVFieldIDs.MessageTypes.LoginFailed);
					Debug.Assert(!isPasswordText, string.Concat(key.ToString(), "|", text));
				}
			}
			return false;
		}
		public bool TryGetValue(int key, out DateTime value)
		{
			if (!tryGetValue(key, out value, out string csvFieldText, convertDate)) {
				Debug.Assert(string.IsNullOrEmpty(csvFieldText), key.ToString() + "|" + csvFieldText);
				return false;
			}
			return true;
		}

		#endregion

		public string Encode()
		{
			// Rebuild the encoded CSV if any fields have changed.
			if (_encoded == null) {
				int maxFieldKey = (_fields == null) ? -1 : _fields.Length - 1;
				string message = null;

				if ((maxFieldKey >= 0) || (_maxCacheKey >= 0)) {
					int maxKey = System.Math.Max(maxFieldKey, _maxCacheKey);
					string[] values = new string[maxKey + 1];

					// Add original values.
					for (int i = 0; i <= maxFieldKey; ++i)
						values[i] = _fields[i];

					// Add cached values over the originals.
					foreach (CacheEntry entry in _cache) {
						switch (entry.Value) {
							case double dval:
								values[entry.Key] = Math.Round(dval, 7).ToString();
								break;
							case DateTime wval:
								values[entry.Key] = wval.ToString("yyyyMMdd-HH:mm:ss");
								break;
							case string sval:
								values[entry.Key] = sval;
								break;
							default:
								values[entry.Key] = entry.Value.ToString();
								break;
						}
					}

					// Add timestamp.
					if (CSVFieldIDs.CurrentTimeStamp <= maxKey)
						values[CSVFieldIDs.CurrentTimeStamp] = DateTime.Now.ToString("HHmmss");

					// Encode.
					message = string.Join(",", values);
				}

				// Append child CSVs if applicable and assign.
				if (string.IsNullOrEmpty(_children)) {
					_encoded = message ?? ""; // Empty string, not null, so we don't rebuild unnecessarily.
				} else if (message == null) {
					_encoded = MESSAGE_SEPARATOR + _children;
				} else {
					// Oddly, the parent also gets the messages separator prefix.
					_encoded = MESSAGE_SEPARATOR + message + MESSAGE_SEPARATOR + _children;
				}
			}

			return _encoded;
		}

		#region - Set -

		public void SetAt<T>(int key, T value)
		{
			// Add values to the cache, not the original csv.  The cache is
			// merged with the original csv during encoding.
			CacheEntry found = _cache.Find(n => n.Key == key);
			if (found == null)
				_cache.Add(new CacheEntry(key, value));
			else
				found.Value = value;

			// Remember the largest key for building the encoded CSV message.
			if (key > _maxCacheKey)
				_maxCacheKey = key;

			// Force rebuild of encoded CSV message.
			_encoded = null;
		}

		#endregion

		#region - Decoded _fields By Name -

		public long AlgoExchangeID => TryGetValue(CSVFieldIDs.AlgoExchangeID, out long value) ? value : 0;
		public long AlgoType {
			get => TryGetValue(CSVFieldIDs.AlgoType, out long value) ? value : 0;
			set => SetAt(CSVFieldIDs.AlgoType, value);
		}
		public double AveragePrice {
			get => TryGetValue(CSVFieldIDs.AveragePrice, out double value) ? value : 0;
			set => SetAt(CSVFieldIDs.AveragePrice, value);
		}
		public string CallPut {
			get => TryGetValue(CSVFieldIDs.CallPut, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.CallPut, value);
		}
		public string ClearingAcct
		{
			get => TryGetValue(CSVFieldIDs.ClearingAcct, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.ClearingAcct, value);
		}
		public string ClearingID => TryGetValue(CSVFieldIDs.ClearingID, out string value) ? value : "";
		public string ClientEcho {
			get => TryGetValue(CSVFieldIDs.ClientEcho, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.ClientEcho, value);
		}
		public string CMTAAccount {
			get => TryGetValue(CSVFieldIDs.CMTAAccount, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.CMTAAccount, value);
		}
		public string Command {
			get => TryGetValue(CSVFieldIDs.Command, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.Command, value);
		}
		public long CplxOrderType => TryGetValue(CSVFieldIDs.CplxOrderType, out long value) ? value : 0;
		public long CumShares => TryGetValue(CSVFieldIDs.CumShares, out long value) ? value : 0;
		public string DisplayInstruction {
			get => TryGetValue(CSVFieldIDs.DisplayInstruction, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.DisplayInstruction, value);
		}
		public DateTime EffectiveTime => TryGetValue(CSVFieldIDs.EffectiveTime, out DateTime value) ? value : default;
		public DateTime EndTime {
			get => TryGetValue(CSVFieldIDs.EndTime, out DateTime value) ? value : default;
			set => SetAt(CSVFieldIDs.EndTime, value);
		}
		public long ExchangeID => TryGetValue(CSVFieldIDs.ExchangeID, out long value) ? value : 0;
		public double ExecPrice => TryGetValue(CSVFieldIDs.ExecPrice, out double value) ? value : 0;
		public string ExecutionInstruction => TryGetValue(CSVFieldIDs.ExecutionInstruction, out string value) ? value : "";
		public DateTime ExecutionTime => TryGetValue(CSVFieldIDs.ExecutionTime, out DateTime value) ? value : default;
		public string ExpDate
		{
			get => TryGetValue(CSVFieldIDs.ExpDate, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.ExpDate, value);
		}
		public string Firm => TryGetValue(CSVFieldIDs.Firm, out string value) ? value : "";
		public long Floor => TryGetValue(CSVFieldIDs.Floor, out long value) ? value : 0;
		public string Instructions => TryGetValue(CSVFieldIDs.Instructions, out string value) ? value : "";
		public long LastShares => TryGetValue(CSVFieldIDs.LastShares, out long value) ? value : 0;
		public long LeaveShares => TryGetValue(CSVFieldIDs.LeaveShares, out long value) ? value : 0;
		public string LocalAcct {
			get => TryGetValue(CSVFieldIDs.LocalAcct, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.LocalAcct, value);
		}
		public string MaturityDay {
			get => TryGetValue(CSVFieldIDs.MaturityDay, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.MaturityDay, value);
		}
		public string OmExecTag => TryGetValue(CSVFieldIDs.OmExecTag, out string value) ? value : "";
		public string OmTag => TryGetValue(CSVFieldIDs.OmTag, out string value) ? value : "";
		public DateTime OmTime => TryGetValue(CSVFieldIDs.OmTime, out DateTime value) ? value : default;
		public string OpenClose => TryGetValue(CSVFieldIDs.OpenClose, out string value) ? value : "";
		public DateTime OrderExpirationDateTime => TryGetValue(CSVFieldIDs.OrderExpirationDateTime, out DateTime value) ? value : default;
		public double OriginalPrice {
			get => TryGetValue(CSVFieldIDs.OriginalPrice, out double value) ? value : 0;
			set => SetAt(CSVFieldIDs.OriginalPrice, value);
		}
		public long OriginalShares => TryGetValue(CSVFieldIDs.OriginalShares, out long value) ? value : 0;
		public string Owner => TryGetValue(CSVFieldIDs.Owner, out string value) ? value : "";
		public string ParentTag {
			get => TryGetValue(CSVFieldIDs.ParentTag, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.ParentTag, value);
		}
		public double PegOffsetPrice => TryGetValue(CSVFieldIDs.PegOffsetPrice, out double value) ? value : 0;
		public double Price {
			get => TryGetValue(CSVFieldIDs.Price, out double value) ? value : 0;
			set => SetAt(CSVFieldIDs.Price, value);
		}
		public string ProgramTrade => TryGetValue(CSVFieldIDs.ProgramTrade, out string value) ? value : "";
		public string SecType {
			get => TryGetValue(CSVFieldIDs.SecType, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.SecType, value);
		}
		public string SecurityDefinition {
			get => TryGetValue(CSVFieldIDs.SecurityDefinition, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.SecurityDefinition, value);
		}
		public long Shares
		{
			get => TryGetValue(CSVFieldIDs.Shares, out long value) ? value : 0;
			set => SetAt(CSVFieldIDs.Shares, value);
		}
		public long Side
		{
			get => TryGetValue(CSVFieldIDs.Side, out long value) ? value : 0;
			set => SetAt(CSVFieldIDs.Side, value);
		}
		public long Status => TryGetValue(CSVFieldIDs.Status, out long value) ? value : 0;
		public double StopPrice => TryGetValue(CSVFieldIDs.StopPrice, out double value) ? value : 0;
		public double StrikePrice
		{
			get => TryGetValue(CSVFieldIDs.StrikePrice, out double value) ? value : 0;
			set => SetAt(CSVFieldIDs.StrikePrice, value);
		}
		public string Symbol => TryGetValue(CSVFieldIDs.Symbol, out string value) ? value : "";
		public string Tag => TryGetValue(CSVFieldIDs.Tag, out string value) ? value : "";
		public string Text => TryGetValue(CSVFieldIDs.Text, out string value) ? value : "";
		public long TIF
		{
			get => TryGetValue(CSVFieldIDs.TIF, out long value) ? value : 0;
			set => SetAt(CSVFieldIDs.TIF, value);
		}
		public string TradeFor
		{
			get => TryGetValue(CSVFieldIDs.TradeFor, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.TradeFor, value);
		}
		public string Trader
		{
			get => TryGetValue(CSVFieldIDs.Trader, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.Trader, value);
		}
		public long Type {
			get => TryGetValue(CSVFieldIDs.Type, out long value) ? value : 0;
			set => SetAt(CSVFieldIDs.Type, value);
		}
		public string Underlying
		{
			get => TryGetValue(CSVFieldIDs.Underlying, out string value) ? value : "";
			set => SetAt(CSVFieldIDs.Underlying, value);
		}
		public bool UpdateOrder => CplxOrderType == CSVFieldIDs.CplxOrderTypes.Container;
		public bool UpdateTrade => CplxOrderType == CSVFieldIDs.CplxOrderTypes.Leg;

		#endregion
	}
}
