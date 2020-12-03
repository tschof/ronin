using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.IO;
using System.Diagnostics;

namespace ROC
{
	public static class HelperXml
	{
		//public static void Save(DataTable table, string path, string filename)
		//{
		//    path = GetPath(path);

		//    if (!Directory.Exists(path))
		//    {
		//        Directory.CreateDirectory(path);
		//    }

		//    filename = path + filename;
		//    if (File.Exists(filename))
		//    {
		//        // Make backup then delete the file
		//        string backupFilename = filename + ".bak";
		//        if (File.Exists(backupFilename))
		//        {
		//            File.Delete(backupFilename);
		//        }
		//        File.Copy(filename, backupFilename);
		//        File.Delete(filename);
		//    }

		//    try
		//    {
		//        table.WriteXml(filename);
		//    }
		//    catch (Exception ex)
		//    {
		//        GLOBAL.HROC.AddToException(ex);
		//    }
		//}

		//public static DataTable Load(DataTable table, string path, string filename)
		//{
		//    path = GetPath(path);
		//    filename = path + filename;

		//    if (Directory.Exists(path) && File.Exists(filename))
		//    {
		//        try
		//        {
		//            table.Rows.Clear();
		//            table.ReadXml(filename);
		//        }
		//        catch (Exception ex)
		//        {
		//            GLOBAL.HROC.AddToException(ex);
		//        }
		//    }

		//    return table;
		//}
		//public static DataTable Load(DataTable table, string filename)
		//{
		//    string path = Path.GetDirectoryName(filename);

		//    if (Directory.Exists(path) && File.Exists(filename))
		//    {
		//        try
		//        {
		//            table.Rows.Clear();
		//            table.ReadXml(filename);
		//        }
		//        catch (Exception ex)
		//        {
		//            GLOBAL.HROC.AddToException(ex);
		//        }
		//    }

		//    return table;
		//}

		//private static string GetPath(string path)
		//{
		//    string appPath = Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
		//    appPath = appPath + path.Replace(@"..", "");

		//    return appPath;
		//}
	}
}
