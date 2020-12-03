using System;
using System.Collections.Generic;
using System.Text;
using MarketDataEx;
using SocketEx;

namespace MDS
{
	public static class HelperUserList
	{
		public static Dictionary<IntPtr, MDClientToServer> GetClientToUserMap()
		{
			Dictionary<IntPtr, MDClientToServer> result = new Dictionary<IntPtr, MDClientToServer>();

			lock (GLOBAL.HMDS.ClientToUserMap)
			{
				if (GLOBAL.HMDS.ClientToUserMap.Count > 0)
				{
					result = new Dictionary<IntPtr, MDClientToServer>(GLOBAL.HMDS.ClientToUserMap);
				}
			}

			return result;
		}

		public static string GetClientIP(IntPtr clientKey)
		{
			string result = "";

			if (GLOBAL.HMDS.Clients != null && clientKey != null)
			{
				lock (GLOBAL.HMDS.Clients)
				{
					if (GLOBAL.HMDS.Clients.Count > 0 && GLOBAL.HMDS.Clients.ContainsKey(clientKey))
					{
						if (GLOBAL.HMDS.Clients[clientKey] != null && GLOBAL.HMDS.Clients[clientKey].ClientSocket != null && GLOBAL.HMDS.Clients[clientKey].ClientSocket.RemoteEndPoint != null)
						{
							result = GLOBAL.HMDS.Clients[clientKey].ClientSocket.RemoteEndPoint.ToString();
						}
					}
				}
			}

			return result;
		}
	}
}
