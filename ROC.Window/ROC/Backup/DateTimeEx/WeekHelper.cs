using System;
using System.Collections.Generic;
using System.Text;

namespace DateTimeEx
{
	public static class WeekHelper
	{
		public static DateTime GetMonthlyExpirationDay(DateTime date)
		{
			DateTime result = new DateTime(date.Year, date.Month, 1);
			switch (result.DayOfWeek)
			{
				case DayOfWeek.Monday:
					result = result.AddDays(5 + 7 + 7);
					break;
				case DayOfWeek.Tuesday:
					result = result.AddDays(4 + 7 + 7);
					break;
				case DayOfWeek.Wednesday:
					result = result.AddDays(3 + 7 + 7);
					break;
				case DayOfWeek.Thursday:
					result = result.AddDays(2 + 7 + 7);
					break;
				case DayOfWeek.Friday:
					result = result.AddDays(1 + 7 + 7);
					break;
				case DayOfWeek.Saturday:
					result = result.AddDays(7 + 7 + 7);
					break;
				case DayOfWeek.Sunday:
					result = result.AddDays(6 + 7 + 7);
					break;
			}

			return result;
		}
		public static DateTime GetWeekStart(DateTime date)
		{
			DateTime weekStart;
			int monday = 1;
			int crtDay = (int)date.DayOfWeek;
			if (date.DayOfWeek == DayOfWeek.Sunday)
			{
				crtDay = 7;
			}
			int difference = crtDay - monday;
			weekStart = date.AddDays(-difference);
			return weekStart;
		}
		public static DateTime GetWeekStop(DateTime date)
		{
			DateTime weekStart;
			int sunday = 7;
			int crtDay = (int)date.DayOfWeek;
			if (date.DayOfWeek == DayOfWeek.Sunday)
			{
				crtDay = 7;
			}
			int difference = sunday - crtDay;
			weekStart = date.AddDays(difference);
			return weekStart;
		}
		public static DateTime GetFirstDayOfMonth(DateTime date)
		{
			return new DateTime(date.Year, date.Month, 1);
		}
		public static DateTime GetLastDayOfMonth(DateTime date)
		{
			return date.AddMonths(1).AddDays(-date.Day);
		}
		public static bool IsThridWeek(DateTime date)
		{
			DateTime endOfThridWeek = GetMonthlyExpirationDay(date);
			DateTime startOfThridWeek = endOfThridWeek.AddDays(-6);
			if (date >= startOfThridWeek && date < endOfThridWeek)
			{
				return true;
			}
			return false;
		}
		public static bool IsEndOfMonth(DateTime date)
		{
			DateTime endOfMonthDay = date.AddMonths(1).AddDays(-date.Day);
			if (endOfMonthDay == date)
			{
				return true;
			}
			return false;
		}
		public static void GetWeekInterval(int year, int weekNo, out DateTime weekStart, out DateTime weekStop)
		{
			GetFirstWeekOfYear(year, out weekStart, out weekStop);
			if (weekNo == 1)
			{
				return;
			}
			weekNo--;
			int daysToAdd = weekNo * 7;
			DateTime dt = weekStart.AddDays(daysToAdd);
			GetWeekInterval(dt, out weekStart, out weekStop);
		}
		public static List<KeyValuePair<DateTime, DateTime>> GetWeekSeries(DateTime toDate)
		{
			//gets week series from beginning of the year
			DateTime dtStartYear = new DateTime(toDate.Year, 1, 1);
			List<KeyValuePair<DateTime, DateTime>> list = GetWeekSeries(dtStartYear, toDate);
			if (list.Count > 0)
			{
				KeyValuePair<DateTime, DateTime> week = list[0];
				list[0] = new KeyValuePair<DateTime, DateTime>(dtStartYear, week.Value);
			}
			return list;
		}
		public static List<KeyValuePair<DateTime, DateTime>> GetWeekSeries(DateTime fromDate, DateTime toDate)
		{
			if (fromDate > toDate)
			{
				return null;
			}
			List<KeyValuePair<DateTime, DateTime>> list = new List<KeyValuePair<DateTime, DateTime>>(100);
			DateTime weekStart, weekStop;
			toDate = GetWeekStop(toDate);
			while (fromDate <= toDate)
			{
				GetWeekInterval(fromDate, out weekStart, out weekStop);
				list.Add(new KeyValuePair<DateTime, DateTime>(weekStart, weekStop));
				fromDate = fromDate.AddDays(7);
			}
			return list;
		}
		public static void GetFirstWeekOfYear(int year, out DateTime weekStart, out DateTime weekStop)
		{
			DateTime date = new DateTime(year, 1, 1);
			GetWeekInterval(date, out weekStart, out weekStop);
		}
		public static void GetWeekInterval(DateTime date, out DateTime dtWeekStart, out DateTime dtWeekStop)
		{
			dtWeekStart = GetWeekStart(date);
			dtWeekStop = GetWeekStop(date);
		}
	}
}
