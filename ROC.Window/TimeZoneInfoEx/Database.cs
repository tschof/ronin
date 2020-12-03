/*
 * ZoneInfo .NET API
 * Developed by Mark Rodrigues
 * Published under the Microsoft Public License (Ms-PL)
 * For the latest version and to log issues please go to:
 * http://www.codeplex.com/zoneinfo
 */
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;

namespace TimeZoneInfoEx
{
	/// <summary>
	/// Loads timezone database files and exposes the Zones and Rules
	/// loaded.
	/// It also holds a number of datetime functions useful for rule / zone handling
	/// </summary>
	/// <remarks>
	/// The database still doesn't support a number of features such as:
	/// - Handling of Formats for and variations during the zones.
	/// eg NZST and NZDT
	/// </remarks>
	public static class Database
	{
		#region Constructor

		/// <summary>
		/// Initialises internal dictionaries
		/// </summary>
		static Database()
		{
			_zones = new Dictionary<string, Zone>();
			_rules = new Dictionary<string, List<Rule>>();
			_links = new Dictionary<string, string>();
		}

		#endregion //  Constructor

		#region Public Members

		/// <summary>
		/// The dictionary of all loaded rules indexed
		/// by the rule name.
		/// </summary>
		public static Dictionary<string, List<Rule>> Rules
		{
			get
			{
				return _rules;
			}
		}
		private static Dictionary<string, List<Rule>> _rules;

		/// <summary>
		/// The dictionary of all loaded rules indexed
		/// by the rule name.
		/// </summary>
		public static Dictionary<string, Zone> Zones
		{
			get
			{
				return _zones;
			}
		}
		private static Dictionary<string, Zone> _zones;

		#endregion // Public Members

		#region Public Methods

		/// <summary>
		/// Returns the zone which matches the specified
		/// zoneName.
		/// </summary>
		/// <param name="zoneName">The name of the zone, eg "Australia/Melbourne"</param>
		/// <returns>The zone matching the zoneName</returns>
		public static Zone GetZone(string zoneName)
		{
			if (_zones.ContainsKey(zoneName))
			{
				return _zones[zoneName];
			}
			else
			{
				return _zones[_links[zoneName]];
			}
		}

		/// <summary>
		/// Get the list of all active zones
		/// </summary>
		/// <returns>A sorted string array of zone names</returns>
		/// <remarks>Deprecated zones (those with links) are not listed</remarks>
		public static string[] GetZoneNames()
		{
			// Create the array to return
			string[] array = new string[_zones.Keys.Count];
			int i = 0;

			// For each item in the zone list insert
			// it into the array
			foreach (string zoneName in _zones.Keys)
			{
				array[i] = zoneName;
				i++;
			}

			// Sort the list
			Array.Sort<string>(array);

			return array;
		}

		/// <summary>
		/// Loads the specified zone file.
		/// </summary>
		/// <param name="filename">The filepath of the zonefile.</param>
		/// <remarks>The 'Link' element in the zone files is not yet supported.</remarks>
		public static void LoadFile(string filename)
		{
			// Save the default zone
			Zone lastZone = null;

			// Open the file
			StreamReader sr = new StreamReader(filename);

			while (!sr.EndOfStream)
			{
				string s = sr.ReadLine();

				// Ignore comments
				if (s.Trim().StartsWith("#"))
					continue;

				// Ignore empty lines
				if (string.IsNullOrEmpty(s.Trim()))
					continue;

				if (s.StartsWith("Rule"))
				{
					// We have a rule so create it and add it
					Rule newRule = new Rule(s);
					if (!_rules.ContainsKey(newRule.Name))
					{
						_rules.Add(newRule.Name, new List<Rule>());
					}
					_rules[newRule.Name].Add(newRule);
				}
				else if (s.StartsWith("Link"))
				{
					// Clean-up the string
					string[] arr = StripTrailingComments(s).Trim().Replace(' ', '\t').Replace("\t\t", "\t").Split('\t');

					Debug.Assert(arr.Length >= 3 && arr.Length <= 4, "Link entry invalid with " + arr.Length.ToString() + " entries");
					if (arr.Length == 3)
					{
						// This is well formed
						_links.Add(arr[2], arr[1]);
					}
					else if (arr.Length == 4)
					{
						// This seems to be a standard variation on the 3 item link
						Debug.Assert(arr[2].Length == 0);
						_links.Add(arr[3], arr[1]);
					}
				}
				else
				{
					// We have a zone
					Zone newZone = Zone.Parse(StripTrailingComments(s), lastZone);
					if (!_zones.ContainsKey(newZone.Name))
					{
						_zones.Add(newZone.Name, newZone);
					}

					// Save this zone to reuse if the next one is empty
					lastZone = newZone;
				}
			}
		}

		/// <summary>
		/// Load all the zone files
		/// </summary>
		/// <param name="directory">The directory containing the zone files</param>
		/// <remarks>This method just iterates through all the files
		/// and calls LoadFile on each, ignoring the .tab, .sh and factory files for now.</remarks>
		public static void LoadFiles(string directory)
		{
			string[] files = Directory.GetFiles(directory);

			foreach (string file in files)
			{
				// Ignore the .tab and .sh files
				// Also the factory and leapsecond files
				// This is pretty crude but does the job
				if (file.EndsWith(".tab") || file.EndsWith(".sh")
					|| file.EndsWith("\\factory") || file.EndsWith("\\leapseconds")
				)
					continue;

				// Now load each of the files
				LoadFile(file);
			}
		}

		/// <summary>
		/// Converts a month string to the corresponding month number
		/// </summary>
		/// <param name="monthStr">A 3 character representation of the month</param>
		/// <returns>An index starting from 1 of the corresponding month</returns>
		/// <exception cref="ArgumentException">If the month string does not match</exception>
		public static int ConvertToMonth(string monthStr)
		{
			int val;

			switch (monthStr)
			{
				case "Jan":
					val = 1;
					break;
				case "Feb":
					val = 2;
					break;
				case "Mar":
					val = 3;
					break;
				case "Apr":
					val = 4;
					break;
				case "May":
					val = 5;
					break;
				case "Jun":
					val = 6;
					break;
				case "Jul":
					val = 7;
					break;
				case "Aug":
					val = 8;
					break;
				case "Sep":
					val = 9;
					break;
				case "Oct":
					val = 10;
					break;
				case "Nov":
					val = 11;
					break;
				case "Dec":
					val = 12;
					break;
				default:
					throw new ArgumentException("The value is an invalid month", monthStr);
			}

			return val;
		}

		/// <summary>
		/// Converts a given time string into a timespan.
		/// </summary>
		/// <param name="time">A string representation of a time.
		/// This may be just minutes (mm), hours and minutes (hh:mm)
		/// or hours, minutes and seconds (hh:mm:ss)</param>
		/// <returns>A timespan respresenting the input value</returns>
		/// <remarks>This function doesn't do anything sensible with the
		/// 's' and 'u' suffix on the time.</remarks>
		public static TimeSpan ConvertToTimeSpan(string time)
		{
			// Strip a trailing s or u
			// TODO: Figure out what s and u means :)
			if (time.EndsWith("s") || time.EndsWith("u"))
			{
				time = time.Substring(0, time.Length - 1);
			}

			// Convert the time
			string[] array = time.Split(':');

			Debug.Assert(array.Length > 0 && array.Length < 4, "Invalid time value ", time);

			// Get the hour and the sign(+/-) of the hour
			int hour = Convert.ToInt32(array[0]);
			int min = 0;
			int sec = 0;
			int sign = Math.Sign(hour);

			if (array.Length > 1)
			{
				min = Convert.ToInt32(array[1]) * ((sign == 0) ? 1 : sign);
			}

			if (array.Length > 2)
			{
				sec = Convert.ToInt32(array[2]) * ((sign == 0) ? 1 : sign);
			}

			return new TimeSpan(hour, min, sec);
		}

		/// <summary>
		/// Converts a given day pattern token into a given date.
		/// </summary>
		/// <param name="token">A value like "lastSun" or "Sun>=1"
		/// that appears throughout rules</param>
		/// <param name="year">The year for the given date</param>
		/// <param name="month">The month for the given date</param>
		/// <returns>A Datetime object in the given month and year
		/// matching the pattern with a type of DateTimeKind.Local</returns>
		/// <exception cref="ArgumentException"></exception>
		/// <remarks>There may be a number of tokens which are not yet
		/// handled.</remarks>
		public static DateTime ConvertToDateTime(string token, int year, int month)
		{
			DateTime dt = new DateTime(year, month, 1, 0, 0, 0, DateTimeKind.Local);

			int dayOfMonth = 1;
			if (Int32.TryParse(token, out dayOfMonth))
			{
				// The token we have is a straight number
				dt = dt.AddDays(dayOfMonth - 1);

				// Make sure we haven't gone over the month
				if (dt.Month != month)
				{
					dt = dt.AddDays(-1 * dt.Day);
				}
			}
			else
			{
				switch (token)
				{
					case "Mon>=1":
						dt = FirstDayOccurance(dt, DayOfWeek.Monday);
						break;
					case "Thu>=1":
						dt = FirstDayOccurance(dt, DayOfWeek.Thursday);
						break;
					case "Fri>=1":
						dt = FirstDayOccurance(dt, DayOfWeek.Friday);
						break;
					case "Sun>=1":
						dt = FirstDayOccurance(dt, DayOfWeek.Sunday);
						break;
					case "Thu>=8":
						dt = FirstDayOccurance(dt.AddDays(7), DayOfWeek.Thursday);
						break;
					case "Sun>=8":
						dt = FirstDayOccurance(dt.AddDays(7), DayOfWeek.Sunday);
						break;
					case "Sun>=9":
						dt = FirstDayOccurance(dt.AddDays(8), DayOfWeek.Sunday);
						break;
					case "Sun>=15":
						dt = FirstDayOccurance(dt.AddDays(14), DayOfWeek.Sunday);
						break;
					case "Fri>=26":
						dt = FirstDayOccurance(dt.AddDays(25), DayOfWeek.Friday);
						break;
					case "lastThu":
						dt = LastDayOccurance(dt.AddMonths(1).AddDays(-1), DayOfWeek.Thursday);
						break;
					case "lastFri":
						dt = LastDayOccurance(dt.AddMonths(1).AddDays(-1), DayOfWeek.Friday);
						break;
					case "lastSat":
						dt = LastDayOccurance(dt.AddMonths(1).AddDays(-1), DayOfWeek.Saturday);
						break;
					case "lastSun":
						dt = LastDayOccurance(dt.AddMonths(1).AddDays(-1), DayOfWeek.Sunday);
						break;
					default:
						throw new ArgumentException("The value is an invalid day of month token", token);
				}
			}

			return dt;
		}

		#endregion // Public Methods

		#region Private Methods

		private static string StripTrailingComments(string s)
		{
			int pos = s.IndexOf('#');
			if (pos >= 0)
			{
				s = s.Substring(0, pos).TrimEnd();
			}
			return s;
		}

		private static DateTime LastDayOccurance(DateTime dt, DayOfWeek dow)
		{
			// Best case is that we are there already
			if (dow == dt.DayOfWeek)
				return dt;

			int currDOW = ConvertToInt32(dt.DayOfWeek);
			int expDOW = ConvertToInt32(dow);

			if (currDOW > expDOW)
			{
				dt = dt.AddDays(expDOW - currDOW);
			}
			else
			{
				dt = dt.AddDays((expDOW - currDOW) - 7);
			}

			return dt;
		}

		private static DateTime FirstDayOccurance(DateTime dt, DayOfWeek dow)
		{
			// Best case is that we are there already
			if (dow == dt.DayOfWeek)
				return dt;

			int currDOW = ConvertToInt32(dt.DayOfWeek);
			int expDOW = ConvertToInt32(dow);

			if (currDOW > expDOW)
			{
				dt = dt.AddDays(7 - (currDOW - expDOW));
			}
			else
			{
				dt = dt.AddDays(expDOW - currDOW);
			}

			return dt;
		}

		private static int ConvertToInt32(DayOfWeek dow)
		{
			int val;

			switch (dow)
			{
				case DayOfWeek.Monday:
					val = 1;
					break;
				case DayOfWeek.Tuesday:
					val = 2;
					break;
				case DayOfWeek.Wednesday:
					val = 3;
					break;
				case DayOfWeek.Thursday:
					val = 4;
					break;
				case DayOfWeek.Friday:
					val = 5;
					break;
				case DayOfWeek.Saturday:
					val = 6;
					break;
				case DayOfWeek.Sunday:
				default:
					val = 7;
					break;
			}

			return val;
		}

		#endregion // Private Methods

		#region Private Members

		private static Dictionary<string, string> _links;

		#endregion // Private Members
	}
}
