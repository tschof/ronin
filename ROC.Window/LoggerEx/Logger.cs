#if OLD
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
		private const int LOG_RETENTION_DAYS = 7;
		private const int FILE_SIZE_LIMIT = 50;

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
			initializeLogger("", type, LOG_RETENTION_DAYS, FILE_SIZE_LIMIT);
		}
		public Logger(LogTypes type, int retensionDays)
		{
			initializeLogger("", type, retensionDays, 50);
		}
		public Logger(string path, LogTypes type)
		{
			initializeLogger(path, type, LOG_RETENTION_DAYS, FILE_SIZE_LIMIT);
		}
		public Logger(string path, LogTypes type, long fileSizeLimit)
		{
			initializeLogger(path, type, LOG_RETENTION_DAYS, fileSizeLimit);
		}
		public Logger(string path, LogTypes type, int retensionDays)
		{
			initializeLogger(path, type, retensionDays, FILE_SIZE_LIMIT);
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

			_logDirectory = Sumo.Common.GetApplicationFolder(path, "Logs");

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

		private void SetPathByType(int count)
		{
			string suffix = _currentType.ToString().ToLower();
			string fileName = string.Format("ROC_{0:yyyyMMdd}{1:G9}.{2}", DateTime.Today, count, suffix);
			_filePath = System.IO.Path.Combine(_logDirectory, fileName);
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
#endif // OLD