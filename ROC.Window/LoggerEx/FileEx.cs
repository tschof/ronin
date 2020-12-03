using System;
using System.IO;

namespace LoggerEx
{
	public class FileEx
	{
		public FileEx()
		{
		}

		public void Export(string directory, string data)
		{
			Export(
				directory,
				String.Concat(new object[] { directory, @"\", "Default.profile" }), 
				data);
		}
		public void Export(string directory, string fileName, string data)
		{
			CheckDirectory(directory);

			if (File.Exists(fileName))
			{
				File.Delete(fileName);
			}

			StreamWriter sw = new StreamWriter(fileName);
			sw.Write(data);
			sw.Close();
		}

		public string Import(string directory)
		{
			return Import(directory, 
				String.Concat(new object[] { directory, @"\", "Default.profile" }));
		}
		public string Import(string directory, string fileName)
		{
			CheckDirectory(directory);

			if (File.Exists(fileName))
			{
				// Load
				StreamReader sr = new StreamReader(fileName);
				return sr.ReadToEnd();
			}
			return "";
		}

		private void CheckDirectory(string directory)
		{
			if (!Directory.Exists(directory))
			{
				Directory.CreateDirectory(directory);
			}
		}
	}
}
