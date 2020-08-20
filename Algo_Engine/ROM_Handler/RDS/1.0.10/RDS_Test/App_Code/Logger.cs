using System;
using System.Data;
using System.Configuration;
using System.Web;
using System.Web.Security;
using System.Web.UI;
using System.Web.UI.WebControls;
using System.Web.UI.WebControls.WebParts;
using System.Web.UI.HtmlControls;

using System.Diagnostics;
using System.IO;
using System.Threading;

public class RDSStatusCode
{
    public static int OK = 0;
    public static int INVALID_USER = 1;
    public static int INTERNAL_ERROR = 2;
    public static int TPOS_TIMEOUT = 3;
}

public class Logger
{
    private bool dbg;
    private static EventLog eLog;
    private string filename;
    private static Logger me;
    private StreamWriter stream;

	public Logger()
	{
        try
        {
            String source = "RDS";
            String logName = ConfigurationManager.AppSettings["elog"];
            if (logName.Length == 0)
            {
                logName = "RDS";
            }
            if (!EventLog.SourceExists(source))
            {
                EventLog.CreateEventSource(source, logName);
            }
            eLog = new EventLog();
            eLog.Source = source;
        }
        catch (Exception exception)
        {
            if (ConfigurationManager.AppSettings["DebugMode"] == "1")
            {
                Debug.Assert(false, exception.StackTrace);
            }
        }

        try
        {
            string str3 = ConfigurationManager.AppSettings["logpath"];
            this.dbg = ConfigurationManager.AppSettings["debug"].Equals("true");
            this.filename = str3 + @"\RDS." + DateTime.Today.ToString("yyyyMMdd") + ".log.txt";
            this.stream = new StreamWriter(this.filename, true);
        }
        catch (Exception exception)
        {
            this.stream = null;
            this.dbg = false;
            eLog.WriteEntry("Logger ctor: " + exception.ToString(), EventLogEntryType.Error);
        }
	}

    public void close()
    {
        try
        {
            if (this.stream != null)
            {
                this.stream.Close();
            }
        }
        catch (Exception)
        {
        }
    }

    public static Logger instance()
    {
        if (me == null)
        {
            me = new Logger();
        }
        return me;
    }

    public void logDebug(string msg)
    {
        try
        {
            if (this.dbg && (this.stream != null))
            {
                this.stream.WriteLine(string.Concat(new object[] { DateTime.Now, " T_", Thread.CurrentThread.ManagedThreadId, ", DBG: ", msg }));
                this.stream.Flush();
            }
        }
        catch (Exception)
        {
        }
    }

    public void logError(string msg)
    {
        try
        {
            if (this.stream != null)
            {
                this.stream.WriteLine(string.Concat(new object[] { DateTime.Now, " T_", Thread.CurrentThread.ManagedThreadId, ", ERR: ", msg }));
                this.stream.Flush();
            }
            else
            {
                eLog.WriteEntry(msg, EventLogEntryType.Error);
            }
        }
        catch (Exception)
        {
        }
    }

    public void logEvent(EventLogEntryType type, string msg)
    {
        eLog.WriteEntry(msg, type);
    }

    public void logInfo(string msg)
    {
        try
        {
            if (this.stream != null)
            {
                this.stream.WriteLine(string.Concat(new object[] { DateTime.Now, " T_", Thread.CurrentThread.ManagedThreadId, ", INFO: ", msg }));
                this.stream.Flush();
            }
        }
        catch (Exception)
        {
        }
    }
}
