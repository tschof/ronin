namespace Common
{
	public static class Application
	{
		public static string ApplicationFolder { get; private set; }
		public static string ApplicationName { get; private set; }

		static Application()
		{
			System.Diagnostics.Process current = System.Diagnostics.Process.GetCurrentProcess();
			ApplicationFolder = System.IO.Path.GetDirectoryName(current.MainModule.FileName);
			ApplicationName = current.ProcessName;
		}

		public static string GetApplicationFolder(string path, string defaultFolderName = null)
		{
			if (string.IsNullOrEmpty(path)) {
				// No arg; return a default folder.
				if (string.IsNullOrEmpty(defaultFolderName))
					return ApplicationFolder;
				else
					return System.IO.Path.Combine(ApplicationFolder, defaultFolderName);
			} else if (System.IO.Path.IsPathRooted(path)) {
				// The given path has a root (is not relative).
				return path;
			} else {
				// The arg is a relative path; return a full path relative to the application.
				return System.IO.Path.Combine(ApplicationFolder, path);
			}
		}
	}
}
