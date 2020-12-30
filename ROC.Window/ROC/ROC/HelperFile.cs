using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Data;

namespace ROC
{
	public static class HelperFile
	{
		public static void Save(string data, string path, string filename)
		{
			try
			{
				File.WriteAllText(GetSaveFileName(path, filename), data);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public static void Save(DataTable data, string path, string filename)
		{
			try
			{
				data.WriteXml(GetSaveFileName(path, filename));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public static string Load(string folder, string filename)
		{
			string data = "";
			string path = Path.Combine(Common.Application.GetApplicationFolder(folder), filename);

			if (File.Exists(path))
			{
				try
				{
					data = File.ReadAllText(path);
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}

			return data;
		}

		public static bool Load(DataTable data, string folder, string filename)
		{
			string path = Path.Combine(Common.Application.GetApplicationFolder(folder), filename);

			if (File.Exists(path))
			{
				try
				{
					data.Rows.Clear();
					data.ReadXml(path);
					return true;
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}

			return false;
		}

		private static string GetSaveFileName(string path, string filename)
		{
			path = Common.Application.GetApplicationFolder(path);
			if (!Directory.Exists(path))
				Directory.CreateDirectory(path);

			return MakeFile(Path.Combine(path, filename));
		}

		private static string MakeFile(string filename)
		{
			if (File.Exists(filename))
			{
				// Make backup then delete the file
				string backupFilename = filename + ".bak";
				if (File.Exists(backupFilename))
				{
					File.Delete(backupFilename);
				}
				File.Copy(filename, backupFilename);
				File.Delete(filename);
			}

			return filename;
		}
	}
}
