using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.IO;

namespace SRLabMDS
{
	public static class HelperSubscriptionCache
	{
		private static DataTable _cacheTable;
		public static DataTable CacheTable
		{
			get
			{
				if (_cacheTable == null)
				{
					_cacheTable = new DataTable("Subscription");
					_cacheTable.Columns.Add("MsgType");
					_cacheTable.Columns.Add("Symbol");
					_cacheTable.Columns.Add("Source");
					_cacheTable.Columns.Add("SubscriptionType");
					_cacheTable.Columns.Add("SecType");
					_cacheTable.Columns.Add("Reload");
				}
				return _cacheTable;
			}
		}

		public static void GetSubscriptionCache()
		{
			GetSubscriptionCache("SubscriptionCache.xml");
		}
		public static void GetSubscriptionCache(string xml)
		{
			try
			{
				string path = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
				path = path + Configuration.Path.Default.CachePath.Replace(@"..", "");

				if (Directory.Exists(path) && File.Exists(path + xml))
				{
					CacheTable.Rows.Clear();
					CacheTable.ReadXml(path + xml);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}
	}
}
