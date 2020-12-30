namespace MarketData.Ronin
{	
	[System.Serializable]
	internal class MDClientToServer
	{
		private DictionaryEx.MultiTypedDictionary _data = new DictionaryEx.MultiTypedDictionary();

		internal MDClientToServer()
		{
			Initialize("H");
		}
		internal MDClientToServer(string username, string password)
		{
			Initialize("L", username, password);
		}
		internal MDClientToServer(string symbol, bool reload)
		{
			Initialize("U", symbol, "", "", "", reload);
		}
		internal MDClientToServer(string msgType, string symbol, string source, bool reload)
		{
			Initialize(msgType, symbol, source, "", CSVEx.CSVFieldIDs.SecurityTypes.Equity, reload);
		}
		internal MDClientToServer(string msgType, string symbol, string source, string secType, bool reload)
		{
			Initialize(msgType, symbol, source, "", secType, reload);
		}
		internal MDClientToServer(string msgType, string symbol, string source, string subscriptionType, string secType, bool reload)
		{
			Initialize(msgType, symbol, source, subscriptionType, secType, reload);
		}

		private void Initialize(string msgType)
		{
			_data.Set(MDInputFieldIDs.MsgType, msgType);
		}
		private void Initialize(string msgType, string username, string password)
		{
			_data.Set(MDInputFieldIDs.MsgType, msgType);
			_data.Set(MDInputFieldIDs.Username, username);
			_data.Set(MDInputFieldIDs.Password, password);
		}
		private void Initialize(string msgType, string symbol, string source, string subscriptionType, string secType, bool reload)
		{
			_data.Set(MDInputFieldIDs.MsgType, msgType);
			_data.Set(MDInputFieldIDs.MDSymbol, symbol);
			_data.Set(MDInputFieldIDs.MDDataSource, source);
			_data.Set(MDInputFieldIDs.MDSubscriptionType, subscriptionType);
			_data.Set(MDInputFieldIDs.SecType, secType);
			_data.Set(MDInputFieldIDs.Reload, reload);
		}

		internal string BaseSymbol
		{
			get
			{
				if (_data.TryGet(MDInputFieldIDs.MDSymbol, out string symbol)) {
					int where = symbol.IndexOf(',');
					return (where < 0) ? symbol : symbol.Substring(0, where);
				}
				return "";
			}
		}

		internal string MDSymbol
		{
			get =>_data.TryGet(MDInputFieldIDs.MDSymbol, out string value) ? value : "";
			set => _data.Set(MDInputFieldIDs.MDSymbol, value);
		}

		internal string MDDataSource
		{
			get => _data.TryGet(MDInputFieldIDs.MDDataSource, out string value) ? value : "";
			set => _data.Set(MDInputFieldIDs.MDDataSource, value);
		}

		internal string MDKey
		{
			get
			{
				if (_data.TryGet(MDInputFieldIDs.MDSymbol, out string symbol)) {
					if (_data.TryGet(MDInputFieldIDs.MDDataSource, out string source)) {
						return $"{symbol},{source}";
					}
				}
				return "";
			}
		}

		internal string MDBaseKey
		{
			get
			{
				string symbol = BaseSymbol;
				if ((symbol != "") && _data.TryGet(MDInputFieldIDs.MDDataSource, out string source)) {
					return $"{BaseSymbol},{source}";
				}
				return "";
			}
		}

		internal string MsgType
		{
			get => _data.TryGet(MDInputFieldIDs.MsgType, out string value) ? value : "";
			set => _data.Set(MDInputFieldIDs.MsgType, value);
		}

		internal string UserName
		{
			get => _data.TryGet(MDInputFieldIDs.Username, out string value) ? value : "";
			set => _data.Set(MDInputFieldIDs.Username, value);
		}

		internal string Password
		{
			get => _data.TryGet(MDInputFieldIDs.Password, out string value) ? value : "";
			set => _data.Set(MDInputFieldIDs.Password, value);
		}

		internal string SubscriptionType
		{
			get => _data.TryGet(MDInputFieldIDs.MDSubscriptionType, out string value) ? value : "";
			set => _data.Set(MDInputFieldIDs.MDSubscriptionType, value);
		}

		internal string SecType
		{
			get => _data.TryGet(MDInputFieldIDs.SecType, out string value) ? value : "";
			set => _data.Set(MDInputFieldIDs.SecType, value);
		}

		internal bool Reload
		{
			get => _data.TryGet(MDInputFieldIDs.Reload, out bool value) ? value : false;
			set => _data.Set(MDInputFieldIDs.Reload, value);
		}

		internal long FirstTime
		{
			get => _data.TryGet(MDInputFieldIDs.FirstTime, out long value) ? value : 0;
			set => _data.Set(MDInputFieldIDs.FirstTime, value);
		}
	}
}
