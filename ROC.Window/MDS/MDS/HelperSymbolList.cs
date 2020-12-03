using System;
using System.Collections.Generic;
using System.Text;
using LBMEx;
using MarketDataEx;

namespace MDS
{
	public static class HelperSymbolList
	{
		public static Dictionary<string, SubscriptionFields> GetSubscriptions(string filterString)
		{
			Dictionary<string, SubscriptionFields> result = new Dictionary<string, SubscriptionFields>();

			if (filterString != "")
			{
				//Key = Symbol + Source
				Dictionary<string, SubscriptionFields> localCopy = new Dictionary<string,SubscriptionFields>();
				
				lock (GLOBAL.HLBM.Subscriptions)
				{
					localCopy = new Dictionary<string, SubscriptionFields>(GLOBAL.HLBM.Subscriptions);
				}

				string[] keys = new string[localCopy.Keys.Count];
				localCopy.Keys.CopyTo(keys, 0);
				keys = getKeys(keys, filterString.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries));

				foreach (string key in keys)
				{
					if (localCopy.ContainsKey(key))
					{
						result.Add(key, localCopy[key]);
					}
				}
			}
			else
			{
				lock (GLOBAL.HLBM.Subscriptions)
				{
					result = new Dictionary<string, SubscriptionFields>(GLOBAL.HLBM.Subscriptions);
				}
			}

			return result;
		}

		public static List<string> GetCreatedSubscriptions(string filterString)
		{
			List<string> result = new List<string>();

			if (filterString != "")
			{
				List<string> localCopy = new List<string>();

				lock (GLOBAL.HLBM.MamaSubscriptionsCreated)
				{
					localCopy = new List<string>(GLOBAL.HLBM.MamaSubscriptionsCreated);
				}

				string[] keys = getKeys(localCopy.ToArray(), filterString.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries));

				foreach (string key in keys)
				{
					if (localCopy.Contains(key))
					{
						result.Add(key);
					}
				}
			}
			else
			{
				lock (GLOBAL.HLBM.MamaSubscriptionsCreated)
				{
					result = new List<string>(GLOBAL.HLBM.MamaSubscriptionsCreated);
				}
			}

			return result;
		}

		public static Dictionary<string, MDServerToClient> GetCurrentMarketData(string filterString)
		{
			Dictionary<string, MDServerToClient> result = new Dictionary<string, MDServerToClient>();

			if (filterString != "")
			{
				Dictionary<string, MDServerToClient> localCopy = new Dictionary<string, MDServerToClient>();

				lock (GLOBAL.HLBM.LatestData)
				{
					localCopy = new Dictionary<string, MDServerToClient>(GLOBAL.HLBM.LatestData);
				}

				string[] keys = new string[localCopy.Keys.Count];
				localCopy.Keys.CopyTo(keys, 0);
				keys = getKeys(keys, filterString.Split(new string[] { "," }, StringSplitOptions.RemoveEmptyEntries));

				foreach (string key in keys)
				{
					if (localCopy.ContainsKey(key))
					{
						result.Add(key, localCopy[key]);
					}
				}
			}
			else
			{
				lock (GLOBAL.HLBM.LatestData)
				{
					result = new Dictionary<string, MDServerToClient>(GLOBAL.HLBM.LatestData);
				}
			}

			return result;
		}

		private static string[] getKeys(string[] keys, string[] filters)
		{
			List<string> result = new List<string>();

			foreach (string filter in filters)
			{
				foreach (string key in keys)
				{
					if (filter.Length > key.Length)
					{
						if (filter.Substring(0, key.Length) == key)
						{
							if (!result.Contains(key))
							{
								result.Add(key);
							}
						}
					}
					else if (key.Length > filter.Length)
					{
						if (key.ToUpper().Substring(0, filter.Length) == filter)
						{
							if (!result.Contains(key))
							{
								result.Add(key);
							}
						}
					}
					else
					{
						if (key.ToUpper() == filter)
						{
							if (!result.Contains(key))
							{
								result.Add(key);
							}
						}
					}
				}
			}

			return result.ToArray();
		}
	}
}
