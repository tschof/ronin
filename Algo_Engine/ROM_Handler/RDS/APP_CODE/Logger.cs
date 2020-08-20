/*********************************************************************************
 *
 *  Copyright (c) 2006 by RONIN CAPITAL, LLC
 *   All Rights Reserved.
 *
 *********************************************************************************
 *
 * History
 * date     user       comment
 * ------- --------    -----------------------------------------------------------
 * 7/27/06  wxu        Initial version
 *********************************************************************************/

using System;
using System.Collections;
using System.IO;
using System.Threading;
using System.Configuration;
using System.Diagnostics;

public class RDSStatusCode
{
    public static int OK = 0;
    public static int INVALID_USER = 1;
    public static int INTERNAL_ERROR = 2;
    public static int TPOS_TIMEOUT = 3;
}

public class Logger
{
    private string filename;
    private StreamWriter stream;
    private bool dbg = false;
    private static Logger me = null;
    private static EventLog eLog;

    private Logger()
    {
        string source = "RDS";
        string elogPath = ConfigurationManager.AppSettings["elog"];
        if (elogPath.Length == 0)
            elogPath = "RDS";
        if (!EventLog.SourceExists(source))
            EventLog.CreateEventSource(source, elogPath);
        eLog = new EventLog();
        eLog.Source = source;

        try
        {
            string dir = ConfigurationManager.AppSettings["logpath"];
            dbg = ConfigurationManager.AppSettings["debug"].Equals("true");
            filename = dir + "\\RDS." + DateTime.Today.ToString("yyyyMMdd") + ".log";
            stream = new StreamWriter(filename, true);
        }
        catch (Exception ex)
        {
            stream = null;
            dbg = false;
            eLog.WriteEntry("Logger ctor: " + ex.ToString(), EventLogEntryType.Error);
        }
    }

    public static Logger instance()
    {
        if (me == null)
            me = new Logger();
        return me;
    }

    public void close()
    {
        try
        {
            if (stream != null)
                stream.Close();
        }
        catch (Exception) { }
    }

    public void logEvent(EventLogEntryType type, string msg)
    {
        eLog.WriteEntry(msg, type);
    }

    public void logDebug(string msg)
    {
        try
        {
            if (dbg && stream != null)
            {
                stream.WriteLine(DateTime.Now + " T_" + Thread.CurrentThread.ManagedThreadId + ", DBG: " + msg);
                stream.Flush();
            }
        }
        catch (Exception) { }
    }

    public void logError(string msg)
    {
        try
        {
            if (stream != null)
            {
                stream.WriteLine(DateTime.Now + " T_" + Thread.CurrentThread.ManagedThreadId + ", ERR: " + msg);
                stream.Flush();
            }
            else
                eLog.WriteEntry(msg, EventLogEntryType.Error);
        }
        catch (Exception) { }
    }

    public void logInfo(string msg)
    {
        try
        {
            if (stream != null)
            {
                stream.WriteLine(DateTime.Now + " T_" + Thread.CurrentThread.ManagedThreadId + ", INFO: " + msg);
                stream.Flush();
            }
        }
        catch (Exception) { }
    }
}
