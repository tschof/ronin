using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.IO;
using System.Windows.Forms;

namespace ROC
{
	public static class HelperResubscribe
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

		public static void LoadSubscriptionCache()
		{
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.DefaultExt = "xml";
			ofd.Filter = "Subscription Cache File (*.xml)|*.xml";
			ofd.AddExtension = true;
			ofd.RestoreDirectory = true;
			ofd.Title = "Open Subscription Cache From";

			string path = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
			path = path + Configuration.Path.Default.SubscriptionCachePath.Replace(@"..", "");

			ofd.InitialDirectory = path;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				LoadSubscriptionCache(System.IO.Path.GetDirectoryName(ofd.FileName) + @"\", System.IO.Path.GetFileName(ofd.FileName));
			}
		}
		private static void LoadSubscriptionCache(string path, string file)
		{
			try
			{
				if (Directory.Exists(path) && File.Exists(path + file))
				{
					CacheTable.Rows.Clear();
					CacheTable.ReadXml(path + file);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}
	}
}
