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

namespace TimeZoneInfoEx
{
	/// <summary>
	/// A class representing a given timezone.
	/// This is the major entry point into accessing the key
	/// timezone functions.
	/// </summary>
	/// <remarks>
	/// A zone object can only be created using the Parse method.
	/// </remarks>
	public class Zone
	{
		#region Private Constructor

		private Zone()
		{
			_zoneRules = new List<ZoneRule>();
		}

		#endregion // Private Constructor

		#region Public Members

		/// <summary>
		/// The name of the zone
		/// </summary>
		public string Name
		{
			get
			{
				return _name;
			}
			set
			{
				_name = value;
			}
		}
		private string _name;

		/// <summary>
		/// The list of rules for the current zone
		/// </summary>
		public List<ZoneRule> ZoneRules
		{
			get
			{
				return _zoneRules;
			}
		}
		private List<ZoneRule> _zoneRules;

		/// <summary>
		/// The time NOW in this zone
		/// </summary>
		public DateTime Now
		{
			get
			{
				// Just convert the utcnow to local time
				return ConvertToLocal(DateTime.UtcNow);
			}
		}

		#endregion // Public Members

		#region Public Static Methods

		/// <summary>
		/// Creates a zone based on a text string
		/// </summary>
		/// <param name="line">A given rule line from the zone file</param>
		/// <param name="defaultZone">The default zone to use if not specified in the line</param>
		/// <remarks>
		/// The format of each line is:
		/// Zone	NAME		GMTOFF	RULES	FORMAT	[UNTIL]
		/// 
		/// Zones can appear in sequential lines in the file
		/// and in this case the zone information from the previous line
		/// is carried on.
		/// 
		/// Note: The format of the file seems pretty inconsistent
		/// so this function is not as elegant as it could be.
		/// </remarks>
		public static Zone Parse(string line, Zone defaultZone)
		{
			// Create the zone we will be returning
			Zone zone = null;

			// Split up the line
			string[] arr = line.Split('\t', ' ');

			if (arr[0].Length > 0)
			{
				// The first token of the string is not empty.
				// Therefore it must be a new zone
				Debug.Assert(arr[0].StartsWith("Zone"), "Unexpected value when creating Zone", arr[0]);

				// Create a new zone
				zone = new Zone();
				zone.Name = arr[1];
			}
			else
			{
				// The first token of the string is empty
				// Therefore this is another rule of the last zone
				zone = defaultZone;
			}

			// Create a new ZoneRule for storing these details
			ZoneRule zoneRule = new ZoneRule();
			zone.ZoneRules.Add(zoneRule);
			zoneRule.ZoneName = arr[1];

			// For some reason this is hack is needed where
			// array 2 and 3 can be empty
			int arrayOffset = 0;
			if (arr[2].Length == 0)
			{
				arrayOffset++;
			}
			if (arr[2 + arrayOffset].Length == 0)
			{
				arrayOffset++;
			}

			zoneRule.GmtOffset = Database.ConvertToTimeSpan(arr[2 + arrayOffset]);

			if (arr[3 + arrayOffset].Length == 0)
			{
				arrayOffset++;
			}

			zoneRule.RuleName = arr[3 + arrayOffset];

			// Another hack for an inconsistent format
			for (int i = 0; i < 5; i++)
			{
				if (arr.Length >= (5 + arrayOffset) && arr[4 + arrayOffset].Length == 0)
				{
					arrayOffset++;
				}
			}

			zoneRule.Format = arr[4 + arrayOffset];

			// Another hack for an inconsistent format
			if (arr.Length >= (6 + arrayOffset) && arr[5 + arrayOffset].Length == 0)
			{
				arrayOffset++;
			}

			// Handle until date
			if (arr.Length <= 5 + arrayOffset)
			{
				// There are no more values in the array.
				// Let's set the Until date as forever
				zoneRule.Until = DateTime.MaxValue;
			}
			else
			{
				// There is a Until value set
				int year = Convert.ToInt32(arr[5 + arrayOffset]);
				int month = 1;
				int day = 1;
				int hour = 0;
				int min = 0;
				int sec = 0;

				// Parse the month
				if (arr.Length >= 7 + arrayOffset)
				{
					month = Database.ConvertToMonth(arr[6 + arrayOffset]);
				}

				// Parse the day (or day pattern)
				if (arr.Length >= 8 + arrayOffset)
				{
					// Another hack for an inconsistent format
					if (arr[7 + arrayOffset].Length == 0)
					{
						arrayOffset++;
					}

					// If the value is not a number so special
					// string parsing
					if (!Int32.TryParse(arr[7 + arrayOffset], out day))
					{
						day = Database.ConvertToDateTime(arr[7 + arrayOffset], year, month).Day;
					}
				}

				// Parse the time
				if (arr.Length >= 9 + arrayOffset)
				{
					// Another hack for an inconsistent format
					if (arr[8 + arrayOffset].Length == 0)
					{
						arrayOffset++;
					}
					TimeSpan ts = Database.ConvertToTimeSpan(arr[8 + arrayOffset]);
					hour = ts.Hours;
					min = ts.Minutes;
					sec = ts.Seconds;
				}

				zoneRule.Until = new DateTime(year, month, day, hour, min, sec, DateTimeKind.Local);
			}

			return zone;
		}

		#endregion // Public Static Methods

		#region Public Methods

		/// <summary>
		/// Returns the format string for the zone for the given date
		/// </summary>
		/// <param name="dt">A local date time</param>
		/// <returns>A format string associated with the date in the given zone.</returns>
		/// <example>In Australia/Melbourne the format strings are
		/// AEST or AEDT if in daylight savings</example>
		public string GetFormat(DateTime dt)
		{
			// Get the Zone Rule for the datetime
			ZoneRule zrule = GetZoneRule(dt);

			// Check to see if there are any rules for this zone
			if (Database.Rules.ContainsKey(zrule.RuleName))
			{
				// There are matching rules.
				// Now get the exact rule
				DateTime cutoverDt = DateTime.MinValue;
				Rule rule = GetRule(dt, Database.Rules[zrule.RuleName], out cutoverDt);

				if (rule != null)
				{
					// Modify the format with the rule letter
					string letter = rule.Letters;
					if (letter == "-")
					{
						letter = "";
					}
					return zrule.Format.Replace("%s", letter);
				}
			}

			// We couldn't find an approprirate rule.
			// Just return the format of the zone
			return zrule.Format;
		}

		/// <summary>
		/// Returns the utc offset for the datetime specified
		/// </summary>
		/// <param name="dt">The local time time</param>
		/// <returns>The utc for the local datetime in the current zone</returns>
		public TimeSpan GetUtcOffset(DateTime dt)
		{
			// Get the Zone Rule for the datetime
			ZoneRule zrule = GetZoneRule(dt);

			// Check to see if there are any rules for this zone
			if (Database.Rules.ContainsKey(zrule.RuleName))
			{
				// There are matching rules.
				// Now get the exact rule
				DateTime cutoverDt = DateTime.MinValue;
				Rule rule = GetRule(dt, Database.Rules[zrule.RuleName], out cutoverDt);

				if (rule != null)
				{
					// Add the gmtoffset with the saving time
					return zrule.GmtOffset + rule.Save;
				}
			}

			// We couldn't find an approprirate rule.
			// Just return the standard offset for the zone
			return zrule.GmtOffset;
		}

		/// <summary>
		/// Converts the current datetime to UTC
		/// </summary>
		/// <param name="dt">A datetime to convert.
		/// If it is not of time DateTimeKind.Utc then it is assumed
		/// to be of type DateTimeKind.Local</param>
		/// <returns>A datetime of type DateTimeKind.Utc</returns>
		public DateTime ConvertToUtc(DateTime dt)
		{
			// The the kind is UTC already then use this.
			// Else just assume it is Local even if not specified
			if (dt.Kind == DateTimeKind.Utc)
			{
				return dt;
			}

			// Now just subtract the starhnd offset
			return new DateTime((dt - GetUtcOffset(dt)).Ticks, DateTimeKind.Utc);
		}

		/// <summary>
		/// Converts the current datetime to local
		/// </summary>
		/// <param name="dt">A datetime to convert.
		/// If it is not of time DateTimeKind.Local then it is assumed
		/// to be of type DateTimeKind.Utc</param>
		/// <returns>A datetime of type DateTimeKind.Local</returns>
		public DateTime ConvertToLocal(DateTime dt)
		{
			// The the kind is Local already then use this.
			// Else just assume it is UTC even if not specified
			if (dt.Kind == DateTimeKind.Local)
			{
				return dt;
			}

			// Now just add the standard offset
			return new DateTime((dt + GetUtcOffset(dt)).Ticks, DateTimeKind.Utc);
		}

		/// <summary>
		/// Returns the dates in which the day light savings cutover
		/// during a given date window.
		/// </summary>
		/// <param name="from">The from date which is assumed to be
		/// of DateTimeKind.Local</param>
		/// <param name="to">The end date which is assumed to be
		/// of DateTimeKind.Local</param>
		/// <returns>An ordered array of local datetimes where day light
		/// savings cut-over</returns>
		public List<DateTime> GetCutoverWindows(DateTime from, DateTime to)
		{
			List<DateTime> dates = new List<DateTime>();

			// Keep looking for the last date from the to date and keep
			// working backwards
			DateTime tmpDt = to;
			while (tmpDt >= from)
			{
				// The cutover time of this rule
				DateTime cutoverDt = DateTime.MinValue;
				ZoneRule zrule = GetZoneRule(tmpDt);
				if (!Database.Rules.ContainsKey(zrule.RuleName)
					|| GetRule(tmpDt, Database.Rules[zrule.RuleName], out cutoverDt) == null)
				{
					// We couldn't find a rule
					break;
				}

				if (cutoverDt >= from)
				{
					// This cutover date is in range.
					// Insert it at the front so the return list
					// is in chronological order
					dates.Insert(0, cutoverDt);
				}

				tmpDt = cutoverDt.AddMinutes(-1);
			}

			return dates;
		}

		#endregion // Public Methods

		#region Private Methods

		private ZoneRule GetZoneRule(DateTime dt)
		{
			foreach (ZoneRule zrule in ZoneRules)
			{
				// If the datetime is before the rule expires then use this one
				if (dt <= zrule.Until)
				{
					return zrule;
				}
			}

			throw new ArgumentException("No rule found for given date", dt.ToString());
		}

		private Rule GetRule(DateTime dt, List<Rule> rules, out DateTime cutoverDt)
		{
			// Keep a handle on the best match
			Rule bestRule = null;
			cutoverDt = DateTime.MinValue;

			// Iterate through all the rules
			foreach (Rule rule in rules)
			{
				if (dt.Year >= rule.From && dt.Year <= rule.To + 1)
				{
					// This rule may be in the range
					DateTime newDt = DateTime.MinValue;

					// Try the year before
					if (rule.TryGetCutoverDateTime(dt.Year - 1, out newDt) && newDt > cutoverDt && newDt < dt)
					{
						// We have a new winner
						cutoverDt = newDt;
						bestRule = rule;
					}

					// Try the current year
					if (rule.TryGetCutoverDateTime(dt.Year, out newDt) && newDt > cutoverDt && newDt < dt)
					{
						// We have a new winner
						cutoverDt = newDt;
						bestRule = rule;
					}
				}
			}

			// Return the right one
			return bestRule;
		}

		#endregion // Private Methods
	}
}