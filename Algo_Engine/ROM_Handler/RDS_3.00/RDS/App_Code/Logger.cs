using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;
using System.IO;
using System.Timers;

/// <summary>
/// Summary description for Logger
/// </summary>
public static class Logger
{
	private static int _debugLevel = Int32.Parse(ConfigurationManager.AppSettings["logLevel"]);
	private static Timer _refreshTimer;
	private static TimeSpan _duration;

	private static string _version;
	private static string _filePath;
	private static TextWriter _out;

	public static void Logit(string from, Exception ex)
	{
		Logger.Logit(String.Concat(new object[] { from, ex.Message }));
		Logger.Logit(String.Concat(new object[] { from, ex.StackTrace.ToString() }));
	}
	public static void Logit(string msg)
	{
		Logit(0, msg);
	}
	public static void Logit(int mode, string msg)
	{
		if (_out != null && mode < _debugLevel)
		{
			_out.WriteLine(String.Concat(DateTime.Now.ToString("MM/dd/yyyy hh:mm:ss.ffff tt"), " - T_", System.Threading.Thread.CurrentThread.ManagedThreadId, ", DBG: ", msg));
			_out.Flush();
		}
	}

	public static void Start()
	{
		if (_version != "")
		{
			Start(_version);
		}
		else
		{
			throw new Exception("Missing RDS Version Number, Can Not Start Log");
		}
	}
	public static void Start(string version)
	{
		_version = version;

		DateTime startTime = DateTime.Now;
		DateTime restartTime = DateTime.Today.AddDays(1);
		TimeSpan diffTime = restartTime.Subtract(startTime);

		if (_refreshTimer == null)
		{
			_refreshTimer = new Timer();
			_refreshTimer.Interval = diffTime.TotalMilliseconds;
			_refreshTimer.Elapsed += new ElapsedEventHandler(_refreshTimer_Elapsed);
			_refreshTimer.Start();
		}

		_filePath = ConfigurationManager.AppSettings["logFilePath"];
		if (!Directory.Exists(_filePath))
		{
			Directory.CreateDirectory(_filePath);
		}

		_filePath = _filePath + String.Format(@"\RDS_v{0:G7}_{1:G}{2:G3}.log", _version, DateTime.Today.Year, DateTime.Today.DayOfYear);

		if (!File.Exists(_filePath))
		{
			File.AppendAllText(_filePath, "");
		}

		_out = new StreamWriter(_filePath, true);
		_out.WriteLine("***************************************************");
		_out.WriteLine();
		_out.WriteLine(String.Concat("RDS Started At : ", DateTime.Now.ToLongDateString()));
		_out.WriteLine();
		_out.WriteLine("***************************************************");
		_out.Flush();

		RemoveOldFiles();
	}

	public static void Stop()
	{
		if (_out != null)
		{
			_out.Flush();
			_out.Dispose();
			_out = null;
		}

		if (_refreshTimer != null)
		{
			_refreshTimer.Stop();
			_refreshTimer.Elapsed -= new ElapsedEventHandler(_refreshTimer_Elapsed); 
			_refreshTimer.Dispose();
			_refreshTimer = null;
		}
	}

	public static void RemoveOldFiles()
	{
		int iTemp = Convert.ToInt32(ConfigurationManager.AppSettings["logFileDuration"]);
		_duration = new TimeSpan(iTemp, 0, 0, 0);

		string path = ConfigurationManager.AppSettings["logFilePath"];
		if (Directory.Exists(path))
		{
			string[] files = Directory.GetFiles(path);

			TimeSpan diffTime;
			foreach (string file in files)
			{
				diffTime = DateTime.Now.Subtract(File.GetCreationTime(file));
				if (diffTime > _duration)
				{
					File.Delete(file);
					Logit(String.Concat("Log File Deleted - ", file));
				}
			}
		}
	}

	private static void _refreshTimer_Elapsed(object sender, ElapsedEventArgs e)
	{
		Stop();
		Start();
	}
}
