using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.IO;

namespace MDS
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

		private static int count = 0;
		public static void SetSubscriptionCache(DataTable table)
		{
			count = count + 1;
			SetSubscriptionCache(table, String.Format("SubscriptionCache_{0:G}{1:G3}{2:G9}.xml", DateTime.Today.Year, DateTime.Today.DayOfYear, count));
		}
		public static void SetSubscriptionCache(DataTable table, string xml)
		{
			try
			{
				string path = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
				path = path + Configuration.Path.Default.CachePath.Replace(@"..", "");

				if (!Directory.Exists(path))
				{
					Directory.CreateDirectory(path);
				}

				if (File.Exists(path + xml))
				{
					File.Delete(path + xml);
				}

				table.WriteXml(path + xml);
				table.Clear();
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}
	}
}
