using System;
using System.Collections.Generic;
using System.Text;
using System.Data;
using System.IO;

namespace MDS
{
	public static class HelperScheduler
	{
		private static DataTable _scheduleTable;
		public static DataTable ScheduleTable
		{
			get
			{
				if (_scheduleTable == null)
				{
					_scheduleTable = new DataTable("Schedule");
					_scheduleTable.Columns.Add("Source");
					_scheduleTable.Columns.Add("DayOfWeek");
					_scheduleTable.Columns.Add("Time");
					
					GetScheduleTable();
				}
				return _scheduleTable;
			}
		}

		public static void GetScheduleTable()
		{
			GetScheduleTable("SourceSchedules.xml");
		}
		public static void GetScheduleTable(string xml)
		{
			try
			{
				string path = System.IO.Path.GetDirectoryName(System.Windows.Forms.Application.ExecutablePath);
				path = path + Configuration.Path.Default.SchedulePath.Replace(@"..", "");

				if (Directory.Exists(path) && File.Exists(path + xml))
				{
					ScheduleTable.Rows.Clear();
					ScheduleTable.ReadXml(path + xml);
				}
			}
			catch (Exception ex)
			{
				GLOBAL.AddToExecption(ex);
			}
		}
	}
}
