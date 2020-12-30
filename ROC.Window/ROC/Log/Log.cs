using System.IO;
using System.Runtime.CompilerServices;

namespace Log
{
	public static class Log
	{
		// GLOBAL.HLog.ROC.Out(string.Format("Origional {0} = {1}", row["Key"], Configuration.User.Default.OrderAvgPriceResolution));

		// Define <logger> entries in log4net.xml with these names.  The log
		// methods (Debug, Info, Error) will route messages to the applicable
		// logger based on the 'target' parameter.
		// All messages without an associated logger are routed to General.
		public enum Target : int { General = 0, Benchmark, TCP, Trace, RTE, User, MDS, LBM, SRLab, RDS, ROM, Orders, Positions, Trades };

		private static log4net.ILog[] _loggers = null;

		// Start logging to the given log folder.
		public static void Start(string logFolderPath)
		{
			try {
				// Use log4net.xml in the log folder to define logging.
				string config = Path.Combine(logFolderPath, "log4net.xml");
				FileInfo fi = new FileInfo(config);

				if (fi.Exists) {
					// The XML file uses %property{LogFolder}.  Set that property before configurating log4net.
					log4net.GlobalContext.Properties["LogFolder"] = logFolderPath;

					// Configure logging; create loggers, appenders, etc.
					log4net.Config.XmlConfigurator.Configure(fi);

					// Assign the static public properties of Log to log4net loggers of the same name, if
					// defined in the XML file.  If no specific logger is defined, the property is assigned
					// to the root logger, with appender named "General" in the XML file.
					string[] targets = System.Enum.GetNames(typeof(Target));
					int targetCount = targets.GetLength(0);
					var rootLogger = log4net.LogManager.GetLogger("General");

					_loggers = new log4net.ILog[targetCount];
					for (int i = 0; i < targetCount; ++i)
						_loggers[i] = log4net.LogManager.Exists(targets[i]) ?? rootLogger;
				} else {
					writeLoggerFail(logFolderPath, $"No application logging; can't find log4net.xml in {logFolderPath}.");
				}
			} catch (System.Exception x) {
				writeLoggerFail(logFolderPath, "No application logging: " + x.Message);
			}
		}

		// Supply only 'target' and 'message' when calling Debug().
		// The other params are automatically populated.
		public static void Debug(Target target, object message, 
				[CallerFilePath] string sourceFilePath = "",
				[CallerMemberName] string memberName = "",
				[CallerLineNumber] int sourceLineNumber = 0)
		{
			try {
				string sourceFileName = System.IO.Path.GetFileName(sourceFilePath);
				_loggers[(int)target]?.Debug($"{memberName}@{sourceFileName}:{sourceLineNumber} | {message}");
			} catch {
				_loggers[(int)target] = null;
			}
		}

		public static void Info(Target target, object message)
		{
			try {
				_loggers[(int)target]?.Info(message);
			} catch {
				_loggers[(int)target] = null;
			}
		}

		// Supply only 'target' and 'message' when calling Error().
		// The other params are automatically populated.
		public static void Error(Target target, object message,
				[CallerFilePath] string sourceFilePath = "",
				[CallerMemberName] string memberName = "",
				[CallerLineNumber] int sourceLineNumber = 0)
		{
			try {
				string sourceFileName = Path.GetFileName(sourceFilePath);
				_loggers[(int)target]?.Error($"{memberName}@{sourceFileName}:{sourceLineNumber} | {message}");
			} catch {
				_loggers[(int)target] = null;
			}
		}

		// Supply only 'target', 'message', and 'x' when calling Error().
		// The other params are automatically populated.
		public static void Error(Target target, object message, System.Exception x,
				[CallerFilePath] string sourceFilePath = "",
				[CallerMemberName] string memberName = "",
				[CallerLineNumber] int sourceLineNumber = 0)
		{
			try {
				string sourceFileName = Path.GetFileName(sourceFilePath);
				_loggers[(int)target]?.Error($"{memberName}@{sourceFileName}:{sourceLineNumber} | {message}", x);
			} catch {
				_loggers[(int)target] = null;
			}
		}

		// Stop all logging for the application.
		public static void Stop()
		{
			log4net.LogManager.Shutdown();
		}

		// Called when logging can't be configured or started for the application.
		private static void writeLoggerFail(string folder, string text)
		{
			try {
				string where = Directory.Exists(folder) ? folder : ".";
				using (StreamWriter writer = new StreamWriter(Path.Combine(where, "log_fail.log"))) {
					writer.WriteLine(text);
					writer.Close();
				}
			} catch {
			}
		}
	}
}
