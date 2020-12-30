using System;
using System.IO;
using DateTimeEx;

namespace LoggerEx
{
	public enum LogTypes : int
	{
		Benchmark,
		TCP,

		Log,
		Trace,	
		RTE,
		User,
		
		MDS,
		LBM,
		SRLab,
		RDS,
		ROM,

		Orders,
		Positions,
		Trades,
	}

	public class Logger : IDisposable
	{
		private DateTime _startedTime;
		private DateTime _endTime;

		private string _logDirectory;
		private LogTypes _currentType;
		private int _retensionDays;

		private string _filePath = "";
		public string FilePath
		{
			get
			{
				return _filePath;
			}
		}

		private int _fileCount = 0;
		private long _fileSizeCurrent = 0;
		private long _fileSizeLimit = 50;
		private long fileSizeLimit
		{
			get
			{
				return _fileSizeLimit * 1000000;
			}
		}

		private TextWriter _textWriter;

		private DateTimeHP _dtHP;

		private System.Windows.Forms.Timer _rollTimer;

		private object _syncObject = new object();

		#region - Constructor -

		public Logger(LogTypes type)
		{
			initializeLogger("", type, 7, 50);
		}
		public Logger(LogTypes type, int retensionDays)
		{
			initializeLogger("", type, retensionDays, 50);
		}
		public Logger(string path, LogTypes type)
		{
			initializeLogger(path, type, 7, 50);
		}
		public Logger(string path, LogTypes type, long fileSizeLimit)
		{
			initializeLogger(path, type, 7, fileSizeLimit);
		}
		public Logger(string path, LogTypes type, int retensionDays)
		{
			initializeLogger(path, type, retensionDays, 50);
		}
		public Logger(string path, LogTypes type, int retensionDays, long fileSizeLimit)
		{
			initializeLogger(path, type, retensionDays, fileSizeLimit);
		}

		private void initializeLogger(string path, LogTypes type, int retensionDays, long fileSizeLimit)
		{
			_dtHP = new DateTimeHP();

			_fileSizeLimit = fileSizeLimit * 1000000;
			_retensionDays = retensionDays;
			_currentType = type;

			_logDirectory = GetPath(path);

			if (!Directory.Exists(_logDirectory))
			{
				Directory.CreateDirectory(_logDirectory);
			}

			switch (type)
			{
				case LogTypes.Orders:
				case LogTypes.Positions:
				case LogTypes.Trades:
					break;
				default:
					CleanOldLogFiles();
					break;
			}

			_fileCount = 0;
			_fileSizeCurrent = 0;

			StartRollTimer();
		}

		private string GetPath(string path)
		{
			if (path == null || path == "")
			{
				path = @"..\Logs\";
			}

			string appPath = Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
			appPath = appPath + path.Replace(@"..", "");

			return appPath;
		}
		
		private void SetPathByType(int count)
		{
			switch (_currentType)
			{
				case LogTypes.Benchmark:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.bch", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.TCP:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.tcp", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				
				case LogTypes.Log:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.log", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.Trace:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.trc", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.RTE:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.rte", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.User:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.use", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				
				case LogTypes.MDS:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.mds", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.LBM:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.lbm", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.SRLab:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.srlab", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.RDS:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.rds", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.ROM:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.rom", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				
				case LogTypes.Orders:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.ord", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.Positions:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.pos", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				case LogTypes.Trades:
					_filePath = String.Format("ROC_{0:G}{1:G3}{2:G9}.tde", DateTime.Today.Year, DateTime.Today.DayOfYear, count);
					break;
				default:
					break;
			}

			if (_filePath != "" && _filePath != null)
			{
				_filePath = _logDirectory + @"\" + _filePath;
			}
		}

		#endregion

		#region - Public Methods -

		public void Out(string msg)
		{
			writeLog(msg, true);
		}
		public void Out(string msg, bool withTimeStamp)
		{
			writeLog(msg, withTimeStamp);
		}

		private void writeLog(string msg, bool withTimeStamp)
		{
			if (_textWriter == null)
			{
				lock (_syncObject)
				{
					SetPathByType(_fileCount);
					_fileCount = _fileCount + 1;

					FileInfo fileInfo;
					while (File.Exists(_filePath))
					{
						fileInfo = new FileInfo(_filePath);
						if (fileInfo.Length < _fileSizeLimit)
						{
							// file is still small enough to write.
							break;
						}
						else
						{
							// File already exists with the same count, go to the next one
							SetPathByType(_fileCount);
							_fileCount = _fileCount + 1;
						}
					}

					// new file with the fileCount
					_textWriter = new StreamWriter(_filePath, true);

					_textWriter.WriteLine("**START********************************************");
					_textWriter.WriteLine("");
					_textWriter.WriteLine(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff"));
					_textWriter.WriteLine("");
					_textWriter.WriteLine("***************************************************");

					_fileSizeCurrent = 0;
				}
			}

			if (withTimeStamp)
			{
				msg = String.Concat(_dtHP.Now.ToString("HH:mm:ss.fffffff"), " - ", msg);
			}

			_fileSizeCurrent = _fileSizeCurrent + msg.Length;
			if (_fileSizeCurrent >= _fileSizeLimit)
			{
				// At roll size Next msg will go into a new file
				lock (_textWriter)
				{
					_textWriter.WriteLine(msg);
					_textWriter.Flush();

					// Close this file
					_textWriter.Close();
					_textWriter = null;
				}
			}
			else
			{
				// Normal write
				lock (_textWriter)
				{
					_textWriter.WriteLine(msg);
					_textWriter.Flush();
				}
			}
		}

		#endregion

		#region - Roll Log File By Day -

		private void StartRollTimer()
		{
			// Get Time until end of the day
			_startedTime = new DateTime(DateTime.Now.Year, DateTime.Now.Month, DateTime.Now.Day);
			_endTime = _startedTime.AddDays(1);
			_endTime = _endTime.AddMinutes(1);
			TimeSpan endOfDayInterval = _endTime.Subtract(DateTime.Now);

			if (_rollTimer == null)
			{
				_rollTimer = new System.Windows.Forms.Timer();
				_rollTimer.Interval = Convert.ToInt32(endOfDayInterval.TotalSeconds * 1000);
				_rollTimer.Tick += new EventHandler(rollTimer_Tick);
				_rollTimer.Enabled = true;
			}
		}

		private void StopRollTimer()
		{
			if (_rollTimer != null)
			{
				_rollTimer.Stop();
				_rollTimer.Tick -= new EventHandler(rollTimer_Tick);
				_rollTimer = null;
			}

			if (_textWriter != null)
			{
				// File had already been written too only
				writeLog("***************************************************", false);
				writeLog("", false);
				writeLog(DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss.fff"), false);
				writeLog("", false);
				writeLog("**********************************************END**", false);
			}
		}

		private void rollTimer_Tick(object sender, EventArgs e)
		{
			if (_startedTime.Day == DateTime.Now.Day)
			{
				// Not Roll Time Yet,
				TimeSpan endOfDayInterval = _endTime.Subtract(DateTime.Now);
				_rollTimer.Interval = Convert.ToInt32(endOfDayInterval.TotalSeconds * 1000);
			}
			else
			{
				StopRollTimer();

				if (_textWriter != null)
				{
					lock (_textWriter)
					{
						// Reset File Count for the Next Time Cycle
						_fileCount = 0;

						_textWriter.Flush();
						_textWriter.Close();
						_textWriter = null;
					}
				}

				// Do Clean Up
				switch (_currentType)
				{
					case LogTypes.Orders:
					case LogTypes.Positions:
					case LogTypes.Trades:
						break;
					default:
						CleanOldLogFiles();
						break;
				}

				StartRollTimer();
			}
		}

		#endregion

		#region - Remove Old Log File By Retation -

		public void CleanOldLogFiles()
		{
			string[] files = Directory.GetFiles(_logDirectory);
			foreach (string file in files)
			{
				try
				{
					DateTime time = File.GetLastAccessTime(file);
					TimeSpan diff = DateTime.Now.Subtract(time);
					if (diff.Days > RetensionDays)
					{
						File.Delete(file);
					}
				}
				catch (Exception ex)
				{
					writeLog(ex.Message, true);
					writeLog(ex.StackTrace, true);
				}
			}
		}

		#endregion

		#region - Properties -

		public int RetensionDays
		{
			get
			{
				return _retensionDays;
			}
			set
			{
				_retensionDays = value;
			}
		}

		#endregion

		#region - IDisposable Members -

		public void Dispose()
		{
			StopRollTimer();

			lock (_textWriter)
			{
				// Reset File Count for the Next Time Cycle
				_fileCount = 0;

				_textWriter.Flush();
				_textWriter.Close();
				_textWriter = null;
			}
		}

		#endregion
	}
}
