using System;
using System.Collections.Generic;
using System.Text;
using SRLabEx;
using MarketDataEx;

namespace SRLabMDS
{
	public static class HelperSymbolList
	{
		public static Dictionary<string, SubscriptionFields> GetSubscriptions(string filterString)
		{
			Dictionary<string, SubscriptionFields> result = new Dictionary<string, SubscriptionFields>();

			if (filterString != "")
			{
				//Key = Symbol + Source
				Dictionary<string, SubscriptionFields> localCopy = new Dictionary<string, SubscriptionFields>();

				lock (GLOBAL.HSRLab.Subscriptions)
				{
					localCopy = new Dictionary<string, SubscriptionFields>(GLOBAL.HSRLab.Subscriptions);
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
				lock (GLOBAL.HSRLab.Subscriptions)
				{
					result = new Dictionary<string, SubscriptionFields>(GLOBAL.HSRLab.Subscriptions);
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

				lock (GLOBAL.HSRLab.MamaSubscriptionsCreated)
				{
					localCopy = new List<string>(GLOBAL.HSRLab.MamaSubscriptionsCreated);
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
				lock (GLOBAL.HSRLab.MamaSubscriptionsCreated)
				{
					result = new List<string>(GLOBAL.HSRLab.MamaSubscriptionsCreated);
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

				lock (GLOBAL.HSRLab.LatestData)
				{
					localCopy = new Dictionary<string, MDServerToClient>(GLOBAL.HSRLab.LatestData);
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
				lock (GLOBAL.HSRLab.LatestData)
				{
					result = new Dictionary<string, MDServerToClient>(GLOBAL.HSRLab.LatestData);
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
					if (key.ToUpper().Contains(filter))
					{
						if (!result.Contains(key))
						{
							result.Add(key);
						}
					}
				}
			}

			return result.ToArray();
		}
	}
}
