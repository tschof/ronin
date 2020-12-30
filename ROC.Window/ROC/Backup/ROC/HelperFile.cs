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
			Save(data, path, filename, true);
		}
		public static void Save(string data, string path, string filename, bool useLocalPath)
		{
			try
			{
				File.WriteAllText(GetSaveFileName(path, filename, useLocalPath), data);
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public static void Save(DataTable data, string path, string filename)
		{
			Save(data, path, filename, true);
		}
		public static void Save(DataTable data, string path, string filename, bool useLocalPath)
		{
			try
			{
				data.WriteXml(GetSaveFileName(path, filename, useLocalPath));
			}
			catch (Exception ex)
			{
				GLOBAL.HROC.AddToException(ex);
			}
		}

		public static string Load(string path, string filename)
		{
			return Load(path, filename, true);
		}
		public static string Load(string path, string filename, bool useLocalPath)
		{
			string data = "";

			if (useLocalPath)
			{
				path = GetPath(path);
			}

			if (filename.Substring(0, 2) != "\\")
			{
				filename = "\\" + filename;
			}
			filename = path + filename;

			if (HasFile(path, filename))
			{
				try
				{
					data = File.ReadAllText(filename);
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}

			return data;
		}

		public static DataTable Load(DataTable data, string path, string filename)
		{
			return Load(data, path, filename, true);
		}
		public static DataTable Load(DataTable data, string path, string filename, bool useLocalPath)
		{
			if (useLocalPath)
			{
				path = GetPath(path);
			}

			filename = path + filename;

			if (HasFile(path, filename))
			{
				try
				{
					data.Rows.Clear();
					data.ReadXml(filename);
				}
				catch (Exception ex)
				{
					GLOBAL.HROC.AddToException(ex);
				}
			}

			return data;
		}

		private static string GetPath(string path)
		{
			string appPath = Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
			appPath = appPath + path.Replace(@"..", "");

			return appPath;
		}

		private static string GetSaveFileName(string path, string filename, bool useLocalPath)
		{
			if (useLocalPath)
			{
				path = GetPath(path);
			}

			MakeDirectory(path);

			return MakeFile(path + filename);
		}

		private static void MakeDirectory(string path)
		{
			if (!Directory.Exists(path))
			{
				Directory.CreateDirectory(path);
			}
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

		private static bool HasFile(string path, string filename)
		{
			if (Directory.Exists(path) && File.Exists(filename))
			{
				return true;
			}
			return false;
		}
	}
}
