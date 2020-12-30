using System.IO;
using System.Runtime.CompilerServices;

namespace Common
{
	public static class Log
	{
		// Define <logger> entries in log4net.xml with these names.  The log
		// methods (Debug, Info, Error) will route messages to the applicable
		// logger based on the 'target' parameter.
		// All messages without an associated logger are routed to General.
		//public enum Target : int { General = 0, Benchmark, TCP, Trace, RTE, User, MDS, LBM, SRLab, RDS, ROC, ROM, BMK, Orders, Positions, Trades };
		public enum Target : int { General = 0, ROC, ROM };

		private static log4net.ILog[] _loggers = null;

		// Log all targets to the same file.
		public static bool StartSingle(string logFolder)
		{
			try {
				// Use log4net.xml in the log folder to define logging.
				string config = Path.Combine(logFolder, "log4net.xml");
				FileInfo fi = new FileInfo(config);

				if (fi.Exists) {
					// Configure logging; create loggers, appenders, etc.
					log4net.GlobalContext.Properties["LogPath"] = Path.Combine(logFolder, $"general_{System.DateTime.Today:yyyyMMdd}.log");
					log4net.Config.XmlConfigurator.Configure(fi);

					string[] targets = System.Enum.GetNames(typeof(Target));
					int targetCount = targets.GetLength(0);

					_loggers = new log4net.ILog[targetCount];
					_loggers[0] = log4net.LogManager.GetLogger("general");

					if (_loggers[0] != null) {
						for (int i = 1; i < targetCount; ++i)
							_loggers[i] = _loggers[0]; // All targets log to the same place.
						return true;
					}
				} else {
					writeLoggerFail(logFolder, $"No application logging; can't find log4net.xml in {logFolder}.");
				}
			} catch (System.Exception x) {
				writeLoggerFail(logFolder, "No application logging; " + x.Message);
			}
			return false;
		}

		// Start logging each target to its own file in the log folder.
		public static void StartMany(string logFolder)
		{
			if (StartSingle(logFolder))
				Builder.AddTargetLoggers(System.Enum.GetNames(typeof(Target)), logFolder, _loggers[0]);
		}

		// Supply only 'target' and 'message' when calling Debug().
		// The other params are automatically populated.
		public static void Debug(Target target, string message,
				[CallerFilePath] string sourceFilePath = "",
				[CallerMemberName] string memberName = "",
				[CallerLineNumber] int sourceLineNumber = 0)
		{
			if (_loggers != null) {
				try {
					string sourceFileName = Path.GetFileName(sourceFilePath);
					_loggers[(int)target]?.Debug($"{sourceFileName}:{sourceLineNumber} {memberName}() | {message}");
				} catch {
					if (target != Target.General)
						_loggers[(int)target] = null;
				}
			}
		}

		public static void Info(string message)
		{
			Info(Target.General, message);
		}

		public static void Info(Target target, string message)
		{
			if (_loggers != null) {
				try {
					_loggers[(int)target]?.Info(message);
				} catch {
					if (target != Target.General)
						_loggers[(int)target] = null;
				}
			}
		}

		// Supply only 'target' and 'message' when calling Error().
		// The other params are automatically populated.
		public static void Error(Target target, string message,
				[CallerFilePath] string sourceFilePath = "",
				[CallerMemberName] string memberName = "",
				[CallerLineNumber] int sourceLineNumber = 0)
		{
			if (_loggers != null) {
				try {
					string sourceFileName = Path.GetFileName(sourceFilePath);
					_loggers[(int)target]?.Error($"{sourceFileName}:{sourceLineNumber} {memberName}() | {message}");
				} catch {
					if (target != Target.General)
						_loggers[(int)target] = null;
				}
			}
		}

		// Supply only 'target', 'message', and 'exception' when calling Error().
		// The other params are automatically populated.
		public static void Error(Target target, string message, System.Exception exception,
				[CallerFilePath] string sourceFilePath = "",
				[CallerMemberName] string memberName = "",
				[CallerLineNumber] int sourceLineNumber = 0)
		{
			if (_loggers != null) {
				try {
					string sourceFileName = Path.GetFileName(sourceFilePath);
					_loggers[(int)target]?.Error($"{sourceFileName}:{sourceLineNumber} {memberName}() | {message}", exception);
				} catch {
					_loggers[(int)target] = null;
				}
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
				using (StreamWriter writer = new StreamWriter(Path.Combine(where, "log_fail.log"), false)) {
					writer.WriteLine(text);
					writer.Close();
				}
			} catch {
			}
		}

		private static class Builder
		{
			internal static void AddTargetLoggers(string[] targets, string logFolder, log4net.ILog templateLogger)
			{
				log4net.Appender.IAppender template = findTemplateAppender(templateLogger);

				if (template != null) {
					int targetCount = targets.GetLength(0);

					for (int i = 1; i < targetCount; ++i) {
						try {
							string path = Path.Combine(logFolder, targets[i]);
							if (!Directory.Exists(path))
								Directory.CreateDirectory(path);
							_loggers[i] = createLogger(targets[i], path, template);
						} catch {
						}
					}
				} else {
					writeLoggerFail(logFolder, $"No appenders defined in log4net.xml.");
				}
			}

			private static log4net.Appender.IAppender findTemplateAppender(log4net.ILog templateLogger)
			{
				var log = log4net.LogManager.GetLogger("general");
				if (log != null) {
					var logger = (log4net.Repository.Hierarchy.Logger)log.Logger;
					if ((logger.Appenders != null) && (logger.Appenders.Count > 0))
						return logger.Appenders[0];
				}
				return null;
			}

			private static log4net.ILog createLogger(string loggerName, string logFolder, log4net.Appender.IAppender templateAppender)
			{
				log4net.ILog log = log4net.LogManager.GetLogger(loggerName); // Finds or creates the named logger.
				log4net.Repository.Hierarchy.Logger logger = (log4net.Repository.Hierarchy.Logger)log.Logger;
				logger.AddAppender(cloneRollingFileAppender(templateAppender, loggerName, logFolder));
				logger.Repository.Configured = true;
				return log;
			}

			private static log4net.Appender.RollingFileAppender cloneRollingFileAppender(log4net.Appender.IAppender template, string newAppenderName, string logFolder)
			{
				log4net.Appender.RollingFileAppender from = template as log4net.Appender.RollingFileAppender;
				if (from == null)
					return null;

				return new log4net.Appender.RollingFileAppender() {
					AppendToFile = from.AppendToFile,
					CountDirection = from.CountDirection,
					DatePattern = from.DatePattern,
					DateTimeStrategy = from.DateTimeStrategy,
					ErrorHandler = from.ErrorHandler,
					File = Path.Combine(logFolder, string.Format("{0}_{1:yyyyMMdd}.log", newAppenderName.ToLower(), System.DateTime.Today)),
					ImmediateFlush = from.ImmediateFlush,
					Layout = from.Layout,
					LockingModel = from.LockingModel,
					MaxFileSize = from.MaxFileSize,
					MaximumFileSize = from.MaximumFileSize,
					MaxSizeRollBackups = from.MaxSizeRollBackups,
					Name = newAppenderName,
					PreserveLogFileNameExtension = from.PreserveLogFileNameExtension,
					RollingStyle = log4net.Appender.RollingFileAppender.RollingMode.Size,
					SecurityContext = from.SecurityContext,
					StaticLogFileName = from.StaticLogFileName,
					Threshold = from.Threshold
				};
			}
		}
	}
}
